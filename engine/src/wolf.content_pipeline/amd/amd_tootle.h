/*
    Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
    Source			 : https://github.com/PooyaEimandar/Wolf.Engine - Please direct any bug to https://github.com/WolfSource/Wolf.Engine/issues
    Website			 : http://WolfSource.io
    Name			 : amd_tootle.h
    Description		 : wrapper of amd_tootle library https://github.com/GPUOpen-Tools/amd-tootle
    Comment          : 
*/

#ifndef __AMD_TOOTLE_H__
#define __AMD_TOOTLE_H__

#include <vector>
#include <w_cpipeline_structs.h>
#include "amd_tootle/include/tootlelib.h"

namespace amd
{
    //=================================================================================================================================
    /// Enumeration for the choice of test cases for tootle.
    //=================================================================================================================================
    enum TootleAlgorithm
    {
        NA_TOOTLE_ALGORITHM,                // Default invalid choice.
        TOOTLE_VCACHE_ONLY,                 // Only perform vertex cache optimization.
        TOOTLE_CLUSTER_VCACHE_OVERDRAW,     // Call the clustering, optimize vertex cache and overdraw individually.
        TOOTLE_FAST_VCACHECLUSTER_OVERDRAW, // Call the functions to optimize vertex cache and overdraw individually.  This is using
        //  the algorithm from SIGGRAPH 2007.
        TOOTLE_OPTIMIZE,                    // Call a single function to optimize vertex cache, cluster and overdraw.
        TOOTLE_FAST_OPTIMIZE                // Call a single function to optimize vertex cache, cluster and overdraw using
        //  a fast algorithm from SIGGRAPH 2007.
    };

    //=================================================================================================================================
    /// A simple structure to store the settings for this sample app
    //=================================================================================================================================
    struct TootleSettings
    {
        unsigned int          nClustering;
        unsigned int          nCacheSize;
        TootleFaceWinding     eWinding;
        TootleAlgorithm       algorithmChoice;         // five different types of algorithm to test Tootle
        TootleVCacheOptimizer eVCacheOptimizer;        // the choice for vertex cache optimization algorithm, it can be either
        //  TOOTLE_VCACHE_AUTO, TOOTLE_VCACHE_LSTRIPS, TOOTLE_VCACHE_DIRECT3D or TOOTLE_VCACHE_TIPSY.
        bool                  bOptimizeVertexMemory;   // true if you want to optimize vertex memory location, false to skip
        bool                  bMeasureOverdraw;        // true if you want to measure overdraw, false to skip
    };

    //=================================================================================================================================
    /// A simple structure to hold Tootle statistics
    //=================================================================================================================================
    struct TootleStats
    {
        unsigned int nClusters;
        float        fVCacheIn;
        float        fVCacheOut;
        float        fOverdrawIn;
        float        fOverdrawOut;
        float        fMaxOverdrawIn;
        float        fMaxOverdrawOut;
        double       fOptimizeVCacheTime;
        double       fClusterMeshTime;
        double       fOptimizeOverdrawTime;
        double       fVCacheClustersTime;
        double       fOptimizeVCacheAndClusterMeshTime;
        double       fTootleOptimizeTime;
        double       fTootleFastOptimizeTime;
        double       fMeasureOverdrawTime;
        double       fOptimizeVertexMemoryTime;
    };

    class tootle
    {
    public:
        static HRESULT apply(
            _In_ const std::vector<wolf::content_pipeline::w_vertex_data>& pVerticesData,
            _In_ const std::vector<float>& pVerticesPosition,
            _Inout_ std::vector<UINT>& pIndicesData);

    private:
        static void DisplayTootleErrorMessage(_In_ const TootleResult& pResult);

        static void PrintAlgorithm(
            _Inout_ std::string& pPrint,
            _In_ const TootleVCacheOptimizer& pVCacheOptimizer,
            _In_ const TootleAlgorithm& pAlgorithmChoice,
            _In_ const unsigned int& pCacheSize,
            _In_ const unsigned int& pClusters);

        static void PrintVCacheOptimizer(
            _Inout_ std::string& pPrint,
            _In_ const TootleVCacheOptimizer& pVCacheOptimizer, 
            _In_ const unsigned int& pCacheSize);

        static void PrintOverdrawOptimizer(
            _Inout_ std::string& pPrint,
            _In_ const TootleOverdrawOptimizer& pOverdrawOptimizer,
            _In_ const unsigned int& pClusters);

        static void PrintStats(_Inout_ std::string& pPrint, _In_ const TootleStats* pStats);
    };
}

#endif
