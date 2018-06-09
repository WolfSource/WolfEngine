#include "simplygon.h"

using namespace wolf;
using namespace SimplygonSDK;

ISimplygonSDK* simplygon::iSimplygonSDK = NULL;
bool simplygon::is_initialized = false;

class progress_observer : public SimplygonSDK::robserver
{
public:
	virtual void Execute(
		SimplygonSDK::IObject * pIObject,
		SimplygonSDK::rid pEventId,
		void* pEventParameterBlock,
		unsigned int EventParameterBlockSize)
	{
		// only care for progress events
		if (pEventId == SimplygonSDK::SG_EVENT_PROGRESS)
		{
			// get the progress in percent
			int _val = *((int*)pEventParameterBlock);
			// tell the process to continue
			// this is required by the progress event
			*((int*)pEventParameterBlock) = 1;
			// output the progress update

			wolf::logger.write("Simplygon's job procceded: %{}" , _val);
		}
	}
}; //_progress_observer;

// Error handling class
class simplygon_error_handler : public SimplygonSDK::rerrorhandler
{
public:
	virtual void HandleError(
		SimplygonSDK::IObject* pIObject,
		_In_z_ const char* pInterfaceName,
		_In_z_ const char* pMethodName,
		SimplygonSDK::rid pErrorType,
		_In_z_ const char* pErrorText
	)
	{
		char _str[1024];

#ifdef _WIN32
		sprintf_s(
#else
		sprintf(
#endif
			_str, "A SimplygonSDK error occured!\n\tInterface: %s (%p)\n\tMethod: %s\n\tError Type: %d\n\tError Description: %s\n",
			pInterfaceName, pIObject, pMethodName, pErrorType, pErrorText);

		wolf::logger.write(_str);
	}
} _simplygon_error_handler;

W_RESULT simplygon::initialize(_In_z_ const std::wstring& pSimplygonSDKPath)
{
	AddSearchPath(pSimplygonSDKPath.c_str());

	int _init_val = SimplygonSDK::Initialize(&simplygon::iSimplygonSDK);
	if (_init_val != SimplygonSDK::SG_ERROR_NOERROR)
	{
		const wchar_t* _e_code = SimplygonSDK::GetError(_init_val);
		V(W_FAILED, 
			w_log_type::W_ERROR, 
			L"Failed to initialize simplygon.\n\tError: {}\n\tError string:{}\n.trace info: {}", 
			_init_val, 
			_e_code,
			L"simplygon::initialize");

		simplygon::is_initialized = false;
		return W_FAILED;
	}

	simplygon::is_initialized = true;
	return W_PASSED;
}

W_RESULT simplygon::generate_lod(
		_Inout_ SimplygonSDK::CountedPointer<SimplygonSDK::IScene>& pOutScene,
		_In_ SimplygonSDK::CountedPointer<SimplygonSDK::IScene>& pInScene,
		_In_z_ const SimplygonSDK::TangentSpaceMethod& pTangentSpaceMethod)
{
	const char* _trace_info = "simplygon::generate_lod";

	bool _error = false;

	CountedPointer<IReductionProcessor> _processor = NULL;
	CountedPointer<IReductionSettings> _settings = NULL;
	CountedPointer<IRepairSettings> _repair_settings = NULL;
	CountedPointer<INormalCalculationSettings> _normal_settings = NULL;

	using namespace simplygon;

	iSimplygonSDK->SetGlobalSetting("Remesh", pTangentSpaceMethod);

	auto _name_str = pInScene->GetName().GetText();
	if (!pInScene)
	{
		_error = true;
		V(W_FAILED, 
			w_log_type::W_ERROR, 
			"getting object scene for model: {}. trace info: {}", 
			_name_str,
			_trace_info);
		goto exit_func;
	}
	//Create a copy of the original scene on which we will run the reduction
	pOutScene = pInScene->NewCopy();
	if (!pOutScene)
	{
		_error = true;
		V(W_FAILED,
			w_log_type::W_ERROR,
			"getting deep copy from object scene for model: {}. trace info: {}",
			_name_str,
			_trace_info);
		goto exit_func;
	}

	// Create the reduction-processor, and set which scene to reduce
	_processor = iSimplygonSDK->CreateReductionProcessor();
	if (!_processor)
	{
		_error = true;
		V(W_FAILED, 
			w_log_type::W_ERROR, 
			"error on CreateReductionProcessor for model: {}. trace info: {}",
			_name_str, 
			_trace_info);
		goto exit_func;
	}

	_processor->SetScene(pOutScene);

	//Set Simplygon setting
	_settings = _processor->GetReductionSettings();
	if (!_settings)
	{
		_error = true;
		V(W_FAILED, 
			w_log_type::W_ERROR, 
			"error on GetReductionSettings for model: {}. trace info: {}",
			_name_str, 
			_trace_info);
		goto exit_func;
	}
	_settings->SetDataCreationPreferences(SG_DATACREATIONPREFERENCES_PREFER_OPTIMIZED_RESULT);
	_settings->SetEdgeSetImportance(1.0f);
	_settings->SetGeometryImportance(1.0f);
	_settings->SetGroupImportance(1.0f);
	_settings->SetKeepSymmetry(false);
	_settings->SetMaterialImportance(1.0f);
	_settings->SetMaxDeviation(1.0f);
	_settings->SetOnScreenSize(300);
	_settings->SetReductionHeuristics(SG_REDUCTIONHEURISTICS_CONSISTENT); //Choose between "fast" and "consistent" processing. Fast will look as good, but may cause inconsistent 
	_settings->SetReductionTargets(SG_REDUCTIONTARGET_TRIANGLERATIO); //Choose between "fast" and "consistent" processing. Fast will look as good, but may cause inconsistent 
	_settings->SetSkinningImportance(1.0f);
	_settings->SetSymmetryAxis(0);
	_settings->SetSymmetryOffset(0.0f);
	_settings->SetTextureImportance(1.0f);
	_settings->SetTriangleCount(1000.0f);
	_settings->SetTriangleRatio(1.0f);
	_settings->SetUseAutomaticSymmetryDetection(false); //Auto-detect the symmetry plane, if one exists. Can, if required, be set manually instead.
	_settings->SetUseVertexWeights(true); //Drastically increases the quality of the LODs normals, at the cost of extra processing time.
	_settings->SetVertexColorImportance(1.0f);

	_repair_settings = _processor->GetRepairSettings();
	if (!_repair_settings)
	{
		_error = true;
		V(W_FAILED,
			w_log_type::W_ERROR,
			"error on GetRepairSettings for model: {}. trace info: {}",
			_name_str,
			_trace_info);
		goto exit_func;
	}
	_repair_settings->SetTjuncDist(0.0f);
	_repair_settings->SetUseTJunctionRemover(false);
	_repair_settings->SetWeldDist(0.0f);
	_repair_settings->SetWeldOnlyObjectBoundary(true);

	_normal_settings = _processor->GetNormalCalculationSettings();
	if (!_normal_settings)
	{
		_error = true;
		V(W_FAILED, 
			w_log_type::W_ERROR, 
			"error on GetNormalCalculationSettings for model: {}. trace info: {}",
			_name_str, 
			_trace_info);
		goto exit_func;
	}

	_normal_settings->SetHardEdgeAngleInRadians(120.0f);
	_normal_settings->SetRepairInvalidNormals(false);
	_normal_settings->SetReplaceNormals(false);

	//add progress observer
	//_processor->AddObserver(&_progress_observer, SG_EVENT_PROGRESS);

	//Run the simplygon process
	_processor->RunProcessing();

exit_func:

	//release all 
	if (_processor) _processor->Release();
	if (_settings) _settings->Release();
	if (_repair_settings) _repair_settings->Release();
	if (_normal_settings) _normal_settings->Release();

	//if error happended, release out scene
	if (_error)
	{
		if (pOutScene) pOutScene->Release();
		return W_FAILED;
	}

	return W_PASSED;
}

W_RESULT simplygon::obj_writer(
	_In_z_ const std::wstring& pObjPath,
	_In_ CountedPointer<IScene>& pScene)
{
	const char* _trace_info = "simplygon::obj_writer";

	bool _error = false;
	CountedPointer<IWavefrontExporter> _obj_writer = NULL;

	auto _out_path = wolf::system::convert::wstring_to_string(pObjPath).c_str();

	//Create an .obj exporter to save our result
	_obj_writer = iSimplygonSDK->CreateWavefrontExporter();
	if (!_obj_writer)
	{
		_error = true;
		V(W_FAILED, 
			w_log_type::W_ERROR,
			"CreateWavefrontExporter for model {}. trace info: {}",
			_out_path, 
			_trace_info);
		goto exit_func;
	}

	// Do the actual exporting
	_obj_writer->SetExportFilePath(_out_path);
	_obj_writer->SetScene(pScene);
	if (!_obj_writer->RunExport())
	{
		_error = true;
		V(W_FAILED, 
			w_log_type::W_ERROR, 
			"Running export for model {}. trace info: {}",
			_out_path, 
			_trace_info);
		goto exit_func;
	}

exit_func:
	//release
	if (_obj_writer) _obj_writer->Release();

	return _error ? W_FAILED : W_PASSED;
}

W_RESULT simplygon::obj_reader(
	_In_z_ const std::wstring& pObjPath,
	_Inout_ CountedPointer<IScene>& pScene)
{
	const char* _trace_info = "simplygon::obj_reader";

	bool _error = false;
	CountedPointer<IWavefrontImporter> _obj_reader = NULL;

	// Load input geometry from file
	auto _obj_path = wolf::system::convert::wstring_to_string(pObjPath);
	_obj_reader = iSimplygonSDK->CreateWavefrontImporter();
	if (!_obj_reader)
	{
		_error = true;
		V(W_FAILED, 
			w_log_type::W_ERROR, 
			"Creating object importer for model {}. trace info: {}",
			_obj_path, 
			_trace_info);
		goto exit_func;
	}
	_obj_reader->SetExtractGroups(false); //This makes the .obj reader import into a single geometry object instead of multiple
	_obj_reader->SetImportFilePath(_obj_path.c_str());
	if (!_obj_reader->RunImport())
	{
		_error = true;
		V(W_FAILED, 
			w_log_type::W_ERROR, 
			"Running import for model {}. trace info: {}",
			_obj_path, 
			_trace_info);
		goto exit_func;
	}

	// Get geometry and materials from importer
	pScene = _obj_reader->GetScene();
	if (!pScene)
	{
		_error = true;
		V(W_FAILED, 
			w_log_type::W_ERROR, 
			"Getting object scene for model {}. trace info: {}",
			_obj_path, 
			_trace_info);
		goto exit_func;
	}

exit_func:

	if (_obj_reader) _obj_reader->Release();

	return _error ? W_FAILED : W_PASSED;
}

//W_RESULT simplygon::create_obj_scene_from_data(
//	_In_ std::vector<wolf::content_pipeline::w_vertex_struct>& pVerticesData,
//	_In_ std::vector<uint32_t>& pIndicesData,
//	_Inout_ CountedPointer<IScene>& pScene)
//{
//	const std::string _trace_info = "simplygon::create_obj_scene_from_data";
//
//	using namespace wolf::content_pipeline;
//
//	bool _error = false;
//
//	spGeometryData _geom = NULL;
//	spRealArray _vertices = NULL, _normals = NULL, _tex_coords = NULL;
//	spRidArray _vertex_ids = NULL;
//	CountedPointer<ISceneMesh> _scene_mesh = NULL;
//	
//	// Create the Geometry. All geometrydata will be loaded into this object
//	_geom = iSimplygonSDK->CreateGeometryData();
//
//	// Array with vertex-coordinates. Will contain 3 real-values for each vertex in the geometry.
//	_vertices = _geom->GetCoords();
//
//	// Array with normal-data. Will contain 3 real-values for each vertex's normal in the geometry.
//	//_geom->AddNormals();
//	//_normals = _geom->GetNormals();
//
//	// Must add texture channel before adding data to it. 
//	//_geom->AddTexCoords(0);
//	//_tex_coords = _geom->GetTexCoords(0);
//
//	// indices
//	_vertex_ids = _geom->GetVertexIds();
//
//	std::array<float, 3> _pos_floats;
//	std::array<float, 3> _nor_floats;
//	std::array<float, 2> _uv_floats;
//
//	std::vector<std::array<float, 3>> _pos;
//	std::vector<std::array<float, 3>> _nor;
//	std::vector<std::array<float, 2>> _uv;
//	std::vector<int> _v_id;
//
//	for (size_t i = 0; i < pIndicesData.size(); ++i)
//	{
//		auto _index = pIndicesData[i];
//		auto _vertex = &(pVerticesData[_index]);
//		if (!_vertex)
//		{
//			V(W_FAILED, "vertex does not have memory", _trace_info, 3);
//			continue;
//		}
//
//		_pos_floats[0] = _vertex->position[0];
//		_pos_floats[1] = _vertex->position[1];
//		_pos_floats[2] = _vertex->position[2];
//		auto _iter_pos = std::find(_pos.begin(), _pos.end(), _pos_floats);
//		if (_iter_pos == _pos.end())
//		{
//			_pos.push_back(_pos_floats);
//			_v_id.push_back((int)_pos.size());
//		}
//		else
//		{
//			_v_id.push_back((int)(std::distance(_pos.begin(), _iter_pos) + 1));
//		}
//	}
//
//	auto _vertex_count = _pos.size();
//	auto _triangle_count = pVerticesData.size() / 3;
//	
//	_geom->SetVertexCount(_vertex_count);
//	_geom->SetTriangleCount(_triangle_count);
//
//	for (int i = 0; i < _vertex_count; ++i)
//	{
//		_vertices->SetTuple(i, &_pos[i][0]);
//	}
//	for (int i = 0; i < _v_id.size(); ++i)
//	{
//		_vertex_ids->SetItem(i, _v_id[i]);
//	}
//
//	_scene_mesh = iSimplygonSDK->CreateSceneMesh();
//	if (!_scene_mesh)
//	{
//		_error = true;
//		V(W_FAILED, "could not create scene mesh", _trace_info, 3);
//		goto exit_func;
//	}
//	_scene_mesh->SetGeometry(_geom);
//	pScene->GetRootNode()->AddChild(_scene_mesh);
//	
//	//test
//	if (obj_writer(L"C:\\Wolf\\A.obj", pScene) == W_FAILED)
//	{
//		logger.error("EEEEEEE");
//	}
//
//
//exit_func:
//	//release all 
//	_pos.clear();
//	_nor.clear();
//	_uv.clear();
//
//	if (_vertices) _vertices->Release();
//	if (_normals) _normals->Release();
//	if (_tex_coords) _tex_coords->Release();
//	if (_vertex_ids) _vertex_ids->Release();
//
//	if (_error)
//	{
//		if (_geom) _geom->Release();
//		if (_scene_mesh) _scene_mesh->Release();
//		if (pScene) pScene->Release();
//
//		return W_FAILED;
//	}
//
//	return W_PASSED;
//}

//W_RESULT simplygon::retrieve_data_from_obj_scene(
//	_Inout_ std::vector<wolf::content_pipeline::w_vertex_struct>& pVerticesData,
//	_Inout_ std::vector<uint32_t>& pIndicesData,
//	_In_ CountedPointer<IScene>& pScene)
//{
//	const std::string _trace_info = "simplygon::retrieve_data_from_obj_scene";
//
//	bool _error = false;
//
//	if (!pScene)
//	{
//		V(W_FAILED, "pScene is NULL", _trace_info, 3);
//		return W_FAILED;
//	}
//
//	spRealArray _vertices = NULL, _normals = NULL, _tex_coords = NULL;
//	spRidArray _vertex_ids = NULL;
//
//	auto _geom = pScene->GetCombinedGeometry();
//
//	//vertex positions
//	_vertices = _geom->GetCoords();
//
//	//vertex normals
//	_normals = _geom->GetNormals();
//
//	//uvs
//	_tex_coords = _geom->GetTexCoords(0);
//
//	//indices
//	_vertex_ids = _geom->GetVertexIds();
//
//	//get vertices and indices
//	auto _triangle_count = _geom->GetTriangleCount();
//	auto _num_vertices = _triangle_count * 3;
//	pVerticesData.resize(_num_vertices);
//	pIndicesData.resize(_num_vertices);
//	size_t _index = 0;
//	for (size_t i = 0; i < _triangle_count; ++i)
//	{
//		auto _vertex = &pVerticesData[_index];
//		if (!_vertex)
//		{
//			V(W_FAILED, "vertex does not have memory", _trace_info, 3);
//			continue;
//		}
//		for (size_t j = 0; j < 3; ++j)
//		{
//			auto _v_i = _vertices->GetItem(i * 3 + j);
//			_vertex->position[i] = _vertices->GetItem(_v_i);
//		}
//
//		pIndicesData[i] = _index++;
//	}
//
//exit_func:
//	//release all 
//	if (_vertices) _vertices->Release();
//	if (_normals) _normals->Release();
//	if (_tex_coords) _tex_coords->Release();
//	if (_vertex_ids) _vertex_ids->Release();
//
//	return _error ? W_FAILED : W_PASSED;
//}

void simplygon::release()
{
	// Deinitialize the SDK
	SimplygonSDK::Deinitialize();
}

