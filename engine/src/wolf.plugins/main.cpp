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
int collada_to_wolf_scene(const char* pColladaFilePath, const char* pWolfSceneFilePath, const char* pExtraData)
{
    using namespace wolf::content_pipeline;
    using namespace wolf::system::convert;

    w_content_manager _content_manager;

    auto _path = string_to_wstring(pColladaFilePath);
    auto _scene = _content_manager.load<w_scene>(_path.c_str());
    _path.clear();

    _path = string_to_wstring(pWolfSceneFilePath);

    w_scene_pack _scene_pack;
    auto _hr = _scene->get_scene_pack(_scene_pack);

    if (_hr == S_OK)
    {
        std::vector<w_scene_pack> _scenes_pack;
        _scenes_pack.push_back(_scene_pack);

        _hr = _content_manager.save_wolf_scenes_to_file(_scenes_pack, _path);
        if (_hr == S_FALSE)
        {
            logger.error(L"Error on saving wolf for scene: " + _scene->get_scene_name());
        }


        _scenes_pack.clear();
        _hr = _content_manager.load_wolf_scenes_from_file(_scenes_pack, _path);
        if (_hr == S_FALSE)
        {
            logger.error(L"Error on loading wolf for scene");
        }

    }

    _path.clear();
    SAFE_RELEASE(_scene);

    return 1;
}

BOOST_PYTHON_MODULE(PyWolfContentPipeline)
{
    def("initialize", initialize);
    def("collada_to_wolf_scene", collada_to_wolf_scene);
}
