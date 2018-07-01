#include "pch.h"
#include "scene.h"
#include <w_aligned_malloc.h>
#include <glm/vec3.hpp>

using namespace wolf;
using namespace wolf::system;
using namespace wolf::render::embree;

scene::scene(_In_z_ const std::wstring& pAppName, _In_z_ const std::wstring& pContentPath) :
	w_scene(pAppName, pContentPath)
{
}

scene::~scene()
{
	//release all resources
	release();
}

W_RESULT scene::initialize(_In_ w_present_info& pPresentInfo)
{
	// TODO: Add your pre-initialization logic here

	return w_scene::initialize(pPresentInfo);
}

void scene::load()
{
    const std::string _trace_info = this->name + "::load";
    
    auto _device = this->_gDevice->get_device();
    
    //add cube
    
    //create a triangulated cube with 12 triangles and 8 vertices
    RTCGeometry _mesh = rtcNewGeometry(_device, RTC_GEOMETRY_TYPE_TRIANGLE);
    
    //create face and vertex color arrays
    auto _face_colors = (glm::vec3*) aligned_malloc(12 * sizeof(glm::vec3), 16);
    auto _vertex_colors = (glm::vec3*) aligned_malloc(8 * sizeof(glm::vec3),16);

    //set vertices and vertex colors
    struct vertex { float x,y,z,r; };
    vertex* _vertices = (vertex*) rtcSetNewGeometryBuffer(_mesh, RTC_BUFFER_TYPE_VERTEX, 0, RTC_FORMAT_FLOAT3, sizeof(vertex), 8);

    _vertex_colors[0] = glm::vec3(0,0,0); _vertices[0].x = -1; _vertices[0].y = -1; _vertices[0].z = -1;
    _vertex_colors[1] = glm::vec3(0,0,1); _vertices[1].x = -1; _vertices[1].y = -1; _vertices[1].z = +1;
    _vertex_colors[2] = glm::vec3(0,1,0); _vertices[2].x = -1; _vertices[2].y = +1; _vertices[2].z = -1;
    _vertex_colors[3] = glm::vec3(0,1,1); _vertices[3].x = -1; _vertices[3].y = +1; _vertices[3].z = +1;
    _vertex_colors[4] = glm::vec3(1,0,0); _vertices[4].x = +1; _vertices[4].y = -1; _vertices[4].z = -1;
    _vertex_colors[5] = glm::vec3(1,0,1); _vertices[5].x = +1; _vertices[5].y = -1; _vertices[5].z = +1;
    _vertex_colors[6] = glm::vec3(1,1,0); _vertices[6].x = +1; _vertices[6].y = +1; _vertices[6].z = -1;
    _vertex_colors[7] = glm::vec3(1,1,1); _vertices[7].x = +1; _vertices[7].y = +1; _vertices[7].z = +1;

    //set triangles and face colors
    struct triangle { int v0, v1, v2; };
    int _tri = 0;
    triangle* _triangles = (triangle*) rtcSetNewGeometryBuffer(_mesh, RTC_BUFFER_TYPE_INDEX, 0, RTC_FORMAT_UINT3, sizeof(triangle), 12);

    // left side
    _face_colors[_tri] = glm::vec3(1,0,0); _triangles[_tri].v0 = 0; _triangles[_tri].v1 = 1; _triangles[_tri].v2 = 2; _tri++;
    _face_colors[_tri] = glm::vec3(1,0,0); _triangles[_tri].v0 = 1; _triangles[_tri].v1 = 3; _triangles[_tri].v2 = 2; _tri++;

    // right side
    _face_colors[_tri] = glm::vec3(0,1,0); _triangles[_tri].v0 = 4; _triangles[_tri].v1 = 6; _triangles[_tri].v2 = 5; _tri++;
    _face_colors[_tri] = glm::vec3(0,1,0); _triangles[_tri].v0 = 5; _triangles[_tri].v1 = 6; _triangles[_tri].v2 = 7; _tri++;

    // bottom side
    _face_colors[_tri] = glm::vec3(0.5f);  _triangles[_tri].v0 = 0; _triangles[_tri].v1 = 4; _triangles[_tri].v2 = 1; _tri++;
    _face_colors[_tri] = glm::vec3(0.5f);  _triangles[_tri].v0 = 1; _triangles[_tri].v1 = 4; _triangles[_tri].v2 = 5; _tri++;

    // top side
    _face_colors[_tri] = glm::vec3(1.0f);  _triangles[_tri].v0 = 2; _triangles[_tri].v1 = 3; _triangles[_tri].v2 = 6; _tri++;
    _face_colors[_tri] = glm::vec3(1.0f);  _triangles[_tri].v0 = 3; _triangles[_tri].v1 = 7; _triangles[_tri].v2 = 6; _tri++;

    // front side
    _face_colors[_tri] = glm::vec3(0,0,1); _triangles[_tri].v0 = 0; _triangles[_tri].v1 = 2; _triangles[_tri].v2 = 4; _tri++;
    _face_colors[_tri] = glm::vec3(0,0,1); _triangles[_tri].v0 = 2; _triangles[_tri].v1 = 6; _triangles[_tri].v2 = 4; _tri++;

    // back side
    _face_colors[_tri] = glm::vec3(1,1,0); _triangles[_tri].v0 = 1; _triangles[_tri].v1 = 5; _triangles[_tri].v2 = 3; _tri++;
    _face_colors[_tri] = glm::vec3(1,1,0); _triangles[_tri].v0 = 3; _triangles[_tri].v1 = 5; _triangles[_tri].v2 = 7; _tri++;

    rtcSetGeometryVertexAttributeCount(_mesh,1);
    rtcSetSharedGeometryBuffer(_mesh, RTC_BUFFER_TYPE_VERTEX_ATTRIBUTE,0,RTC_FORMAT_FLOAT3, _vertex_colors,0, sizeof(glm::vec3), 8);
    
    rtcCommitGeometry(_mesh);
    unsigned int _geom_id = rtcAttachGeometry(this->_rtc_scene, _mesh);
    rtcReleaseGeometry(_mesh);
    
    //add ground
    _mesh = rtcNewGeometry (_device, RTC_GEOMETRY_TYPE_TRIANGLE);
    
    /* set vertices */
    _vertices = (vertex*) rtcSetNewGeometryBuffer(_mesh, RTC_BUFFER_TYPE_VERTEX, 0, RTC_FORMAT_FLOAT3, sizeof(vertex), 4);
    _vertices[0].x = -10; _vertices[0].y = -2; _vertices[0].z = -10;
    _vertices[1].x = -10; _vertices[1].y = -2; _vertices[1].z = +10;
    _vertices[2].x = +10; _vertices[2].y = -2; _vertices[2].z = -10;
    _vertices[3].x = +10; _vertices[3].y = -2; _vertices[3].z = +10;
    
    /* set triangles */
    _triangles = (triangle*) rtcSetNewGeometryBuffer(_mesh, RTC_BUFFER_TYPE_INDEX,0,RTC_FORMAT_UINT3, sizeof(triangle), 2);
    _triangles[0].v0 = 0; _triangles[0].v1 = 1; _triangles[0].v2 = 2;
    _triangles[1].v0 = 1; _triangles[1].v1 = 3; _triangles[1].v2 = 2;
    
    rtcCommitGeometry(_mesh);
    _geom_id = rtcAttachGeometry(this->_rtc_scene, _mesh);
    rtcReleaseGeometry(_mesh);
    
    rtcCommitScene(this->_rtc_scene);
}

void scene::update(_In_ const wolf::system::w_game_time& pGameTime)
{
    if (w_scene::exiting) return;
    const std::string _trace_info = this->name + "::update";


    w_scene::update(pGameTime);
}

W_RESULT scene::render(_In_ const wolf::system::w_game_time& pGameTime)
{
    if (w_scene::exiting) return W_PASSED;

    const std::string _trace_info = this->name + "::render";

    return W_PASSED;
}

void scene::on_resized(_In_ const w_point& pNewSizeOfWindow)
{
}

void scene::on_device_lost()
{
	w_scene::on_device_lost();
}

ULONG scene::release()
{
    if (this->get_is_released()) return 1;

    return w_scene::release();
}

bool scene::_update_gui()
{
//    //Setting Style
//    ImGuiStyle& _style = ImGui::GetStyle();
//    _style.Colors[ImGuiCol_Text].x = 1.0f;
//    _style.Colors[ImGuiCol_Text].y = 1.0f;
//    _style.Colors[ImGuiCol_Text].z = 1.0f;
//    _style.Colors[ImGuiCol_Text].w = 1.0f;
//
//    _style.Colors[ImGuiCol_WindowBg].x = 0.0f;
//    _style.Colors[ImGuiCol_WindowBg].y = 0.4f;
//    _style.Colors[ImGuiCol_WindowBg].z = 1.0f;
//    _style.Colors[ImGuiCol_WindowBg].w = 1.0f;
//
//    ImGuiWindowFlags  _window_flags = 0;;
//    ImGui::SetNextWindowSize(ImVec2(200, 100), ImGuiSetCond_FirstUseEver);
//    bool _is_open = true;
//    if (!ImGui::Begin("Wolf.Engine", &_is_open, _window_flags))
//    {
//        ImGui::End();
//        return false;
//    }
//
//    ImGui::Text("FPS:%d\r\nFrameTime:%f\r\nTotalTime:%f\r\nMouse Position:%d,%d\r\n",
//        sFPS,
//        sElapsedTimeInSec,
//        sTotalTimeTimeInSec,
//        wolf::inputs_manager.mouse.pos_x, wolf::inputs_manager.mouse.pos_y);
//    ImGui::End();
//
    return true;
}
