#include <boost/python.hpp>
#include <w_logger.h>
#include <w_content_manager.h>

using namespace boost::python;

// Initialize Log
bool initialize()
{
    return logger.initialize(L"PyWolfContentPipeline", L"c:\\log\\");
}

// Load collada file and merge it with extra data then convert to .WScene file with message pack 
int collada_to_wolf_scene(const char* pColladaFilePath, const char* pExtra)
{
    using namespace wolf::content_pipeline;
    using namespace wolf::system::convert;

    std::wstring _path = string_to_wstring(pColladaFilePath);
    
    w_content_manager _content_manager;
    auto _scene = _content_manager.load<w_scene>(_path.c_str());
    _path.clear();

    std::vector<w_model*> _models;
    _scene->get_all_models(_models);
    
    int _return = _models[0]->get_instnaces_count();

    //release all
    _models.clear();
    SAFE_RELEASE(_scene);

    return _return;
}

BOOST_PYTHON_MODULE(PyWolfContentPipeline)
{
    def("initialize", initialize);
    def("collada_to_wolf_scene", collada_to_wolf_scene);
}
