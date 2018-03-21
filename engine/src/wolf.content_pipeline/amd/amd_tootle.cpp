#include "w_cpipeline_pch.h"
#include "amd_tootle.h"
#include <w_timer.h>

using namespace amd;
using namespace wolf::system;

W_RESULT tootle::apply(
    _In_ const std::vector<wolf::content_pipeline::w_vertex_struct>& pVerticesStruct,
    _In_ const std::vector<float>& pVerticesPosition,
    _Inout_ std::vector<uint32_t>& pIndicesData)
{
    TootleSettings _settings;
    _settings.nClustering = 0;
    _settings.nCacheSize = TOOTLE_DEFAULT_VCACHE_SIZE;
    _settings.eWinding = TOOTLE_CW;
    _settings.algorithmChoice = TOOTLE_FAST_OPTIMIZE;
    _settings.eVCacheOptimizer = TOOTLE_VCACHE_AUTO;             // the auto selection as the default to optimize vertex cache
    _settings.bOptimizeVertexMemory = true;                           // default value is to optimize the vertex memory
    _settings.bMeasureOverdraw = true;                           // default is to measure overdraw


    // *****************************************************************
    //   Prepare the mesh and initialize stats variables
    // *****************************************************************

    const float INVALID_TIME = -1;
    const float* pViewpoints = NULL;
    unsigned int nViewpoints = 0;

    size_t        indices_size = pIndicesData.size();
    unsigned int  nFaces = (unsigned int)indices_size / 3;
    unsigned int  nVertices = (unsigned int)pVerticesPosition.size() / 3;
    float*        pfVB = (float*)&pVerticesPosition[0];
    unsigned int* pnIB = (unsigned int*)&pIndicesData[0];
    unsigned int  nStride = 3 * sizeof(float);

    TootleStats _stats;

    // initialize the timing variables
    _stats.fOptimizeVCacheTime = INVALID_TIME;
    _stats.fClusterMeshTime = INVALID_TIME;
    _stats.fVCacheClustersTime = INVALID_TIME;
    _stats.fOptimizeVCacheAndClusterMeshTime = INVALID_TIME;
    _stats.fOptimizeOverdrawTime = INVALID_TIME;
    _stats.fTootleOptimizeTime = INVALID_TIME;
    _stats.fTootleFastOptimizeTime = INVALID_TIME;
    _stats.fMeasureOverdrawTime = INVALID_TIME;
    _stats.fOptimizeVertexMemoryTime = INVALID_TIME;

    TootleResult _result = TootleInit();
    if (_result != TOOTLE_OK)
    {
        DisplayTootleErrorMessage(_result);
        return W_FAILED;
    }

    // measure input VCache efficiency
    _result = TootleMeasureCacheEfficiency(pnIB, nFaces, _settings.nCacheSize, &_stats.fVCacheIn);

    if (_result != TOOTLE_OK)
    {
        DisplayTootleErrorMessage(_result);
        return W_FAILED;
    }

    if (_settings.bMeasureOverdraw)
    {
        // measure input overdraw.  Note that we assume counter-clockwise vertex winding.
        _result = TootleMeasureOverdraw(
            pfVB, 
            pnIB, 
            nVertices, 
            nFaces, 
            nStride, 
            pViewpoints, 
            nViewpoints, 
            _settings.eWinding,
            &_stats.fOverdrawIn, 
            &_stats.fMaxOverdrawIn);

        if (_result != TOOTLE_OK)
        {
            DisplayTootleErrorMessage(_result);
            return W_FAILED;
        }
    }

    // allocate an array to hold the cluster ID for each face
    std::vector<unsigned int> faceClusters;
    faceClusters.resize(nFaces + 1);
    unsigned int nNumClusters;

    w_timer timer;
    timer.start();

    // **********************************************************************************************************************
    //   Optimize the mesh:
    //
    // The following cases show five examples for developers on how to use the library functions in Tootle.
    // 1. If you are interested in optimizing vertex cache only, see the TOOTLE_VCACHE_ONLY case.
    // 2. If you are interested to optimize vertex cache and overdraw, see either TOOTLE_CLUSTER_VCACHE_OVERDRAW
    //     or TOOTLE_OPTIMIZE cases.  The former uses three separate function calls while the latter uses a single
    //     utility function.
    // 3. To use the algorithm from SIGGRAPH 2007 (v2.0), see TOOTLE_FAST_VCACHECLUSTER_OVERDRAW or TOOTLE_FAST_OPTIMIZE
    //     cases.  The former uses two separate function calls while the latter uses a single utility function.
    //
    // Note the algorithm from SIGGRAPH 2007 (v2.0) is very fast but produces less quality results especially for the
    //  overdraw optimization.  During our experiments with some medium size models, we saw an improvement of 1000x in
    //  running time (from 20+ minutes to less than 1 second) for using v2.0 calls vs v1.2 calls.  The resulting vertex
    //  cache optimization is very similar while the overdraw optimization drops from 3.8x better to 2.5x improvement over
    //  the input mesh.
    //  Developers should always run the overdraw optimization using the fast algorithm from SIGGRAPH initially.
    //  If they require a better result, then re-run the overdraw optimization using the old v1.2 path (TOOTLE_OVERDRAW_AUTO).
    //  Passing TOOTLE_OVERDRAW_AUTO to the algorithm will let the algorithm choose between Direct3D or raytracing path
    //  depending on the total number of clusters (less than 200 clusters, it will use Direct3D path otherwise it will
    //  use raytracing path since the raytracing path will be faster than the Direct3D path at that point).
    //
    // Tips: use v2.0 for fast optimization, and v1.2 to further improve the result by mix-matching the calls.
    // **********************************************************************************************************************

    switch (_settings.algorithmChoice)
    {
    case TOOTLE_VCACHE_ONLY:
        // *******************************************************************************************************************
        // Perform Vertex Cache Optimization ONLY
        // *******************************************************************************************************************

        _stats.nClusters = 1;

        // Optimize vertex cache
        _result = TootleOptimizeVCache(
            pnIB, 
            nFaces, 
            nVertices, 
            _settings.nCacheSize,
            pnIB, 
            NULL, 
            _settings.eVCacheOptimizer);

        if (_result != TOOTLE_OK)
        {
            DisplayTootleErrorMessage(_result);
            return W_FAILED;
        }

        _stats.fOptimizeVCacheTime = timer.get_seconds();
        break;

    case TOOTLE_CLUSTER_VCACHE_OVERDRAW:
        // *******************************************************************************************************************
        // An example of calling clustermesh, vcacheclusters and optimize overdraw individually.
        // This case demonstrate mix-matching v1.2 clustering with v2.0 overdraw optimization.
        // *******************************************************************************************************************

        // Cluster the mesh, and sort faces by cluster.
        _result = TootleClusterMesh(pfVB, pnIB, nVertices, nFaces, nStride, _settings.nClustering, pnIB, &faceClusters[0], NULL);

        if (_result != TOOTLE_OK)
        {
            DisplayTootleErrorMessage(_result);
            return W_FAILED;
        }

        _stats.fClusterMeshTime = timer.get_seconds();
        timer.reset();

        // The last entry of of faceClusters store the total number of clusters.
        _stats.nClusters = faceClusters[nFaces];

        // Perform vertex cache optimization on the clustered mesh.
        _result = TootleVCacheClusters(
            pnIB, 
            nFaces, 
            nVertices, 
            _settings.nCacheSize, 
            &faceClusters[0],
            pnIB, 
            NULL, 
            _settings.eVCacheOptimizer);

        if (_result != TOOTLE_OK)
        {
            DisplayTootleErrorMessage(_result);
            return W_FAILED;
        }

        _stats.fVCacheClustersTime = timer.get_seconds();
        timer.reset();

        // Optimize the draw order (using v1.2 path: TOOTLE_OVERDRAW_AUTO, the default path is from v2.0--SIGGRAPH version).
        _result = TootleOptimizeOverdraw(
            pfVB, 
            pnIB, 
            nVertices, 
            nFaces, 
            nStride, 
            pViewpoints, 
            nViewpoints, 
            _settings.eWinding,
            &faceClusters[0], pnIB, NULL, TOOTLE_OVERDRAW_AUTO);

        if (_result != TOOTLE_OK)
        {
            DisplayTootleErrorMessage(_result);
            return W_FAILED;
        }

        _stats.fOptimizeOverdrawTime = timer.get_seconds();
        break;

    case TOOTLE_FAST_VCACHECLUSTER_OVERDRAW:
        // *******************************************************************************************************************
        // An example of calling v2.0 optimize vertex cache and clustering mesh with v1.2 overdraw optimization.
        // *******************************************************************************************************************

        // Optimize vertex cache and create cluster
        // The algorithm from SIGGRAPH combine the vertex cache optimization and clustering mesh into a single step
        _result = TootleFastOptimizeVCacheAndClusterMesh(
            pnIB, 
            nFaces, 
            nVertices, 
            _settings.nCacheSize, 
            pnIB,
            &faceClusters[0], 
            &nNumClusters, 
            TOOTLE_DEFAULT_ALPHA);

        if (_result != TOOTLE_OK)
        {
            // an error detected
            DisplayTootleErrorMessage(_result);
            return W_FAILED;
        }

        _stats.fOptimizeVCacheAndClusterMeshTime = timer.get_seconds();
        timer.reset();

        _stats.nClusters = nNumClusters;

        // In this example, we use TOOTLE_OVERDRAW_AUTO to show that we can mix-match the clustering and
        //  vcache computation from the new library with the overdraw optimization from the old library.
        //  TOOTLE_OVERDRAW_AUTO will choose between using Direct3D or CPU raytracing path.  This path is
        //  much slower than TOOTLE_OVERDRAW_FAST but usually produce 2x better results.
        _result = TootleOptimizeOverdraw(
            pfVB, 
            pnIB, 
            nVertices, 
            nFaces, 
            nStride, 
            NULL, 
            0,
            _settings.eWinding, 
            &faceClusters[0], 
            pnIB, 
            NULL, 
            TOOTLE_OVERDRAW_AUTO);

        if (_result != TOOTLE_OK)
        {
            // an error detected
            DisplayTootleErrorMessage(_result);
            return W_FAILED;
        }

        _stats.fOptimizeOverdrawTime = timer.get_seconds();

        break;

    case TOOTLE_OPTIMIZE:
        // *******************************************************************************************************************
        // An example of using a single utility function to perform v1.2 optimizations.
        // *******************************************************************************************************************

        // This function will compute the entire optimization (cluster mesh, vcache per cluster, and optimize overdraw).
        // It will use TOOTLE_OVERDRAW_FAST as the default overdraw optimization
        _result = TootleOptimize(
            pfVB, 
            pnIB, 
            nVertices, 
            nFaces, 
            nStride, 
            _settings.nCacheSize,
            pViewpoints, 
            nViewpoints, 
            _settings.eWinding, 
            pnIB, 
            &nNumClusters, 
            _settings.eVCacheOptimizer);

        if (_result != TOOTLE_OK)
        {
            DisplayTootleErrorMessage(_result);
            return W_FAILED;
        }

        _stats.fTootleOptimizeTime = timer.get_seconds();

        _stats.nClusters = nNumClusters;
        break;

    case TOOTLE_FAST_OPTIMIZE:
        // *******************************************************************************************************************
        // An example of using a single utility function to perform v2.0 optimizations.
        // *******************************************************************************************************************

        // This function will compute the entire optimization (optimize vertex cache, cluster mesh, and optimize overdraw).
        // It will use TOOTLE_OVERDRAW_FAST as the default overdraw optimization
        _result = TootleFastOptimize(
            pfVB, 
            pnIB, 
            nVertices, 
            nFaces, 
            nStride, 
            _settings.nCacheSize,
            _settings.eWinding, 
            pnIB, 
            &nNumClusters, 
            TOOTLE_DEFAULT_ALPHA);

        if (_result != TOOTLE_OK)
        {
            DisplayTootleErrorMessage(_result);
            return W_FAILED;
        }

        _stats.fTootleFastOptimizeTime = timer.get_seconds();

        _stats.nClusters = nNumClusters;

        break;

    default:
        // wrong algorithm choice
        break;
    }

    // measure output VCache efficiency
    _result = TootleMeasureCacheEfficiency(
        pnIB, 
        nFaces, 
        _settings.nCacheSize, 
        &_stats.fVCacheOut);

    if (_result != TOOTLE_OK)
    {
        DisplayTootleErrorMessage(_result);
        return W_FAILED;
    }

    if (_settings.bMeasureOverdraw)
    {
        // measure output overdraw
        timer.reset();
        _result = TootleMeasureOverdraw(
            pfVB, 
            pnIB, 
            nVertices, 
            nFaces, 
            nStride, 
            pViewpoints, 
            nViewpoints, 
            _settings.eWinding,
            &_stats.fOverdrawOut, 
            &_stats.fMaxOverdrawOut);
        _stats.fMeasureOverdrawTime = timer.get_seconds();

        if (_result != TOOTLE_OK)
        {
            DisplayTootleErrorMessage(_result);
            return W_FAILED;
        }
    }

    //-----------------------------------------------------------------------------------------------------
    // PERFORM VERTEX MEMORY OPTIMIZATION (rearrange memory layout for vertices based on the final indices
    //  to exploit vertex cache prefetch).
    //  We want to optimize the vertex memory locations based on the final optimized index buffer that will
    //  be in the output file.
    //  Thus, in this sample code, we recompute a copy of the indices that point to the original vertices
    //  (pnIBTmp) to be passed into the function TootleOptimizeVertexMemory.  If we use the array pnIB, we
    //  will optimize for the wrong result since the array pnIB is based on the rehashed vertex location created
    //  by the function ObjLoader.
    //-----------------------------------------------------------------------------------------------------
    timer.reset();

    std::vector<unsigned int> pnVertexRemapping;
    unsigned int nReferencedVertices = 0;          // The actual total number of vertices referenced by the indices

    if (_settings.bOptimizeVertexMemory)
    {
        std::vector<unsigned int> pnIBTmp;
        pnIBTmp.resize(nFaces * 3);

        // compute the indices to be optimized for (the original pointed by the obj file).
        for (unsigned int i = 0; i < indices_size; i += 3)
        {
            for (int j = 0; j < 3; j++)
            {
                const auto& rVertex = pVerticesStruct[pnIB[i + j]];
                pnIBTmp[i + j] = rVertex.vertex_index - 1; // index is off by 1

                // compute the max vertices
                if (rVertex.vertex_index > nReferencedVertices)
                {
                    nReferencedVertices = rVertex.vertex_index;
                }
            }
        }

        pnVertexRemapping.resize(nReferencedVertices);

        // For this sample code, we are just going to use vertexRemapping array _result.  This is to support general obj
        //  file input and output.
        //  In fact, we are sending the wrong vertex buffer here (it should be based on the original file instead of the
        //  rehashed vertices).  But, it is ok because we do not request the reordered vertex buffer as an output.
        _result = TootleOptimizeVertexMemory(pfVB, &pnIBTmp[0], nReferencedVertices, nFaces, nStride, NULL, NULL,
            &pnVertexRemapping[0]);

        if (_result != TOOTLE_OK)
        {
            DisplayTootleErrorMessage(_result);
            return W_FAILED;
        }

        _stats.fOptimizeVertexMemoryTime = timer.get_seconds();
    }

    // clean up tootle
    TootleCleanup();

    // print tootle statistics to stdout and stderr
    // display the current test case
    
    std::string _print;
    
    PrintAlgorithm(_print, _settings.eVCacheOptimizer, _settings.algorithmChoice, _settings.nCacheSize, _stats.nClusters);
    PrintStats(_print, &_stats);

#ifdef DEBUG
    //logger.write("[AMD TOOTLE Debugger] Flag: DEBUG - Message :\n" + _print);
#endif

    _print.clear();

    return W_PASSED;
}

void tootle::DisplayTootleErrorMessage(_In_ const TootleResult& pResult)
{
    using namespace wolf;

    switch (pResult)
    {
    case NA_TOOTLE_RESULT:
        logger.error("[AMD TOOTLE Debugger] Flag: ERROR - Message : NA_TOOTLE_RESULT");
        break;

    case TOOTLE_OK:
        break;

    case TOOTLE_INVALID_ARGS:
        logger.error("[AMD TOOTLE Debugger] Flag: ERROR - Message : TOOTLE_INVALID_ARGS");
        break;

    case TOOTLE_OUT_OF_MEMORY:
        logger.error("[AMD TOOTLE Debugger] Flag: ERROR - Message : TOOTLE_OUT_OF_MEMORY");
        break;

    case TOOTLE_3D_API_ERROR:
        logger.error("[AMD TOOTLE Debugger] Flag: ERROR - Message : TOOTLE_3D_API_ERROR");
        break;

    case TOOTLE_INTERNAL_ERROR:
        logger.error("[AMD TOOTLE Debugger] Flag: ERROR - Message : TOOTLE_INTERNAL_ERROR");
        break;

    case TOOTLE_NOT_INITIALIZED:
        logger.error("[AMD TOOTLE Debugger] Flag: ERROR - Message : TOOTLE_NOT_INITIALIZED");
        break;
    }
}

//=================================================================================================================================
/// Displays the tootle algorithm selection.
///
/// \param fp               The file pointer
/// \param eVCacheOptimizer The vertex cache optimizer enum.
/// \param eAlgorithmChoice The algorithm choice enum.
/// \param nCacheSize       The hardware vertex cache size.
/// \param nClusters        Total number of clusters generated.
//=================================================================================================================================
void tootle::PrintAlgorithm(
    _Inout_ std::string& pPrint,
    _In_ const TootleVCacheOptimizer& pVCacheOptimizer,
    _In_ const TootleAlgorithm& pAlgorithmChoice,
    _In_ const unsigned int& pCacheSize,
    _In_ const unsigned int& pClusters)
{
    pPrint += "#Tootle Settings\n";
    pPrint += "#Vertex cache size     : %" + std::to_string(pCacheSize) + "\n";

    switch (pAlgorithmChoice)
    {
    case TOOTLE_VCACHE_ONLY:
        PrintVCacheOptimizer(pPrint, pVCacheOptimizer, pCacheSize);
        break;

    case TOOTLE_CLUSTER_VCACHE_OVERDRAW:
        PrintVCacheOptimizer(pPrint, pVCacheOptimizer, pCacheSize);
        pPrint += "#Algorithm             : TootleClusterMesh, TootleVCacheClusters and TootleOptimizeOverdraw\n";
        PrintOverdrawOptimizer(pPrint, TOOTLE_OVERDRAW_AUTO, pClusters);
        break;

    case TOOTLE_FAST_VCACHECLUSTER_OVERDRAW:
        pPrint += "#Algorithm             : TootleFastOptimizeVCacheAndClusterMesh and TootleOptimizeOverdraw\n";
        PrintOverdrawOptimizer(pPrint, TOOTLE_OVERDRAW_AUTO, pClusters);
        break;

    case TOOTLE_OPTIMIZE:
        PrintVCacheOptimizer(pPrint, pVCacheOptimizer, pCacheSize);
        pPrint += "#Algorithm             : TootleOptimize\n";
        PrintOverdrawOptimizer(pPrint, TOOTLE_OVERDRAW_FAST, pClusters);
        break;

    case TOOTLE_FAST_OPTIMIZE:
        pPrint += "#Algorithm             : TootleFastOptimize\n";
        PrintOverdrawOptimizer(pPrint, TOOTLE_OVERDRAW_FAST, pClusters);
        break;

    default:
        pPrint += "#Algorithm             : Error input\n";
        break;
    }
}

//=================================================================================================================================
/// Displays the vertex cache optimizer selection.
///
/// \param fp               The file pointer.
/// \param eVCacheOptimizer The vertex cache optimizer enum.
/// \param nCacheSize       The hardware vertex cache size.
//=================================================================================================================================
void tootle::PrintVCacheOptimizer(
    _Inout_ std::string& pPrint,
    _In_ const TootleVCacheOptimizer& pVCacheOptimizer, 
    _In_ const unsigned int& pCacheSize)
{
    switch (pVCacheOptimizer)
    {
    case TOOTLE_VCACHE_AUTO:
        if (pCacheSize <= 6)
        {
            pPrint += "#Vertex Cache Optimizer: AUTO (LStrips)\n";
        }
        else
        {
            pPrint += "#Vertex Cache Optimizer: AUTO (Tipsy)\n";
        }

        break;

    case TOOTLE_VCACHE_DIRECT3D:
        pPrint += "#Vertex Cache Optimizer: Direct3D\n";
        break;

    case TOOTLE_VCACHE_LSTRIPS:
        pPrint += "#Vertex Cache Optimizer: LStrips (a custom algorithm to create a list like triangle strips)\n";
        break;

    case TOOTLE_VCACHE_TIPSY:
        pPrint += "#Vertex Cache Optimizer: Tipsy (an algorithm from SIGGRAPH 2007)\n";
        break;

    case NA_TOOTLE_VCACHE_OPTIMIZER:
    default:
        pPrint += "#Vertex Cache Optimizer: Error input\n";
        break;
    }
}

//=================================================================================================================================
/// Displays the overdraw optimizer selection.
///
/// \param fp                 The file pointer.
/// \param eOverdrawOptimizer The overdraw optimizer enum.
/// \param nClusters          The number of clusters generated.
//=================================================================================================================================
void tootle::PrintOverdrawOptimizer(
    _Inout_ std::string& pPrint,
    _In_ const TootleOverdrawOptimizer& pOverdrawOptimizer,
    _In_ const unsigned int& pClusters)
{
    switch (pOverdrawOptimizer)
    {
    case TOOTLE_OVERDRAW_AUTO:
#ifdef _SOFTWARE_ONLY_VERSION
        pPrint += "#Overdraw Optimizer    : TOOTLE_OVERDRAW_AUTO (Software renderer)\n";
#else

        if (pClusters > 225)
        {
            pPrint += "#Overdraw Optimizer    : TOOTLE_OVERDRAW_AUTO (Software renderer)\n";
        }
        else
        {
            pPrint += "#Overdraw Optimizer    : TOOTLE_OVERDRAW_AUTO (Direct3D renderer)\n";
        }

#endif
        break;

    case TOOTLE_OVERDRAW_DIRECT3D:
        pPrint += "#Overdraw Optimizer    : TOOTLE_OVERDRAW_DIRECT3D (Direct3D renderer)\n";
        break;

    case TOOTLE_OVERDRAW_RAYTRACE:
        pPrint += "#Overdraw Optimizer    : TOOTLE_OVERDRAW_RAYTRACE (Software renderer)\n";
        break;

    case TOOTLE_OVERDRAW_FAST:
        pPrint += "#Overdraw Optimizer    : TOOTLE_OVERDRAW_FAST (SIGGRAPH 2007 version)\n";
        break;

    case NA_TOOTLE_OVERDRAW_OPTIMIZER:
    default:
        pPrint += "#Overdraw Optimizer    : Error input\n";
        break;
    }
}


//=================================================================================================================================
/// A helper function to print formatted TOOTLE statistics
/// \param f      A file to print the statistics to
/// \param pStats The statistics to be printed
//=================================================================================================================================
void tootle::PrintStats(_Inout_ std::string& pPrint, _In_ const TootleStats* pStats)
{
    if (!pStats) return;

    pPrint += "#Tootle Stats\n";
    pPrint += "#Clusters         : " + std::to_string(pStats->nClusters) + "\n";
    pPrint += "#CacheIn/Out      : " + std::to_string(pStats->fVCacheIn / pStats->fVCacheOut) +
        " (" + std::to_string(pStats->fVCacheIn) + " / " + std::to_string(pStats->fVCacheOut) + " )\n";

    if (pStats->fMeasureOverdrawTime >= 0)
    {
        pPrint += "#OverdrawIn/Out      : " + std::to_string(pStats->fOverdrawIn / pStats->fOverdrawOut) +
            " (" + std::to_string(pStats->fOverdrawIn) + " / " + std::to_string(pStats->fOverdrawOut) + " )\n";

        pPrint += "#OverdrawMaxIn/Out      : " + std::to_string(pStats->fMaxOverdrawIn / pStats->fMaxOverdrawOut) +
            " (" + std::to_string(pStats->fMaxOverdrawIn) + " / " + std::to_string(pStats->fMaxOverdrawOut) + " )\n";
    }

    pPrint += "\n#Tootle Timings\n";

    // print out the timing result if appropriate.
    if (pStats->fOptimizeVCacheTime >= 0)
    {
        pPrint += "#OptimizeVCache               = " + std::to_string(pStats->fOptimizeVCacheTime) + " seconds\n";
    }

    if (pStats->fClusterMeshTime >= 0)
    {
        pPrint += "#ClusterMesh               = " + std::to_string(pStats->fClusterMeshTime) + " seconds\n";
    }

    if (pStats->fVCacheClustersTime >= 0)
    {
        pPrint += "#VCacheClusters               = " + std::to_string(pStats->fVCacheClustersTime) + " seconds\n";
    }

    if (pStats->fOptimizeVCacheAndClusterMeshTime >= 0)
    {
        pPrint += "#OptimizeVCacheAndClusterMesh               = " + std::to_string(pStats->fOptimizeVCacheAndClusterMeshTime) + " seconds\n";
    }

    if (pStats->fOptimizeOverdrawTime >= 0)
    {
        pPrint += "#OptimizeOverdraw               = " + std::to_string(pStats->fOptimizeOverdrawTime) + " seconds\n";
    }

    if (pStats->fTootleOptimizeTime >= 0)
    {
        pPrint += "#TootleOptimize               = " + std::to_string(pStats->fTootleOptimizeTime) + " seconds\n";
    }

    if (pStats->fTootleFastOptimizeTime >= 0)
    {
        pPrint += "#TootleFastOptimize               = " + std::to_string(pStats->fTootleFastOptimizeTime) + " seconds\n";
    }

    if (pStats->fMeasureOverdrawTime >= 0)
    {
        pPrint += "#MeasureOverdraw               = " + std::to_string(pStats->fMeasureOverdrawTime) + " seconds\n";
    }

    if (pStats->fOptimizeVertexMemoryTime >= 0)
    {
        pPrint += "#OptimizeVertexMemory               = " + std::to_string(pStats->fOptimizeVertexMemoryTime) + " seconds\n";
    }
}
