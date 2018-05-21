#include <wolf.h>

#include <view/imgui/imgui.h>
#include <view/imgui_gl2.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <stdio.h>
#include <cmath>
#include <string>
#include <chrono>

using namespace wolf;
using namespace wolf::system;

#define MAX_SEARCH_LENGHT 256

static void error_callback(int error, const char* description)
{
	fprintf(stderr, "Error %d: %s\n", error, description);
}

static std::string _content_path = "D:\\github\\PooyaEimandar\\ProjectFalcon\\content\\";
static float sWindowWidth = 1280;
static float sWindowHeight = 720;
static float sUXAnimationSpeed = 1.2f;
typedef enum collapse_states
{
	openned = 0,
	openning,
	collapsing,
	collapsed
};
collapse_states sLeftWidgetCollapseState = collapse_states::openned;
static ImVec4 clear_color = ImVec4(0.184f, 0.184f, 0.184f, 1.00f);
static char sSearch[MAX_SEARCH_LENGHT];

static GLuint sIconTexture = 0;
static ImTextureID sIconTextureID = nullptr;

static GLuint sMap2DTexture = 0;
static ImTextureID sMap2DTextureID = nullptr;

static ImVec2 sLeftWidgetControllerSize;
static float sTotalTime = 0;
static float sDeltaTime = 0;

namespace Colors
{
	static ImVec4 White = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
	static ImVec4 WhiteGray = ImVec4(0.835f, 0.835f, 0.835f, 1.0f);
	static ImVec4 Gray = ImVec4(0.615f, 0.615f, 0.615f, 1.0f);
	static ImVec4 Black = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
	static ImVec4 Transparent = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
	static ImVec4 Orange = ImVec4(1.0f, 0.564f, 0.313f, 1.0f);
	static ImVec4 LightBlue = ImVec4(0.0f, 0.631f, 0.949f, 1.0f);
	static ImVec4 Blue = ImVec4(0.0f, 0.490f, 0.949f, 1.0f);
	static ImVec4 BorderColor = ImVec4(0.4f, 0.392f, 0.388f, 1.0f);
};

struct widget_info
{
	ImVec2 size;
	ImVec2 pos;
};

//forward declaration
bool initialize();
bool load();
void release();
void show_ui();

//Entry point of program 
WOLF_MAIN()
{
	WOLF_INIT(L"FalconViewer");

	if (!initialize())
	{
		release();
		return EXIT_FAILURE;
	}

	//Setup window
	glfwSetErrorCallback(error_callback);
	if (!glfwInit())
	{
		return 1;
	}

	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	GLFWwindow* window = glfwCreateWindow(sWindowWidth, sWindowHeight, "Project Falcon", NULL, NULL);
	
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1); // Enable vsync

	// Setup ImGui binding
	ImGui_GL2_Init(window, true);
	// Setup style
	ImGui::StyleColorsLight();

	if (!load())
	{
		release();
		return EXIT_FAILURE;
	}

	// Main loop
	while (!glfwWindowShouldClose(window))
	{
		auto _start_time = std::chrono::high_resolution_clock::now();

		int display_w, display_h;
		glfwGetFramebufferSize(window, &display_w, &display_h);

		sWindowWidth = static_cast<float>(display_w);
		sWindowHeight = static_cast<float>(display_h);


		// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
		// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
		// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
		// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
		glfwPollEvents();
		ImGui_GL2_NewFrame();

		show_ui();

		// Rendering
		glViewport(0, 0, display_w, display_h);
		glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
		glClear(GL_COLOR_BUFFER_BIT);
		
		//create toolbar
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		//glEnable(GL_BLEND);
		//if (_once)
		//{
		//	_once = false;
		//	glMatrixMode(GL_MODELVIEW);
		//	glTranslatef(0, 0.95, 0);
		//	glScalef(1.0f, 0.05f, 1.0f);
		//}
		//glBegin(GL_QUADS);
		//{
		//	glColor3f(0.835f, 0.835f, 0.835f); glVertex3f(-1.0f, 1.0f, 0.0f);// Top Left
		//	glColor3f(1.0f, 0.608f, 0.0f); glVertex3f(1.0f, 1.0f, 0.0f);// Top Right
		//	glColor3f(1.0f, 0.608f, 0.0f); glVertex3f(1.0f, -1.0f, 0.0f);// Bottom Right
		//	glColor3f(0.835f, 0.835f, 0.835f); glVertex3f(-1.0f, -1.0f, 0.0f);// Bottom Left
		//}
		//glEnd();

		ImGui::Render();
		glfwSwapBuffers(window);

		auto _end_time = std::chrono::high_resolution_clock::now();
		sDeltaTime = std::chrono::duration<double, std::milli>(_end_time - _start_time).count();
		sTotalTime += sDeltaTime;
	}

	// Cleanup
	release();
	ImGui_GL2_Shutdown();
	glfwTerminate();

	//output a message to the log file
	logger.write(L"shutting down Project Falcon");


	return EXIT_SUCCESS;
}

bool initialize()
{
	logger.write(L"initializing Falcon");

#ifdef __WIN32
	sprintf_s(sSearch, "	Search");
#else
	sprintf(sSearch, "	Search");
#endif

	return true;
}

bool load()
{
	// Load Font for ui
	ImGuiIO& _io = ImGui::GetIO();
	//ImFontConfig _font_config = {};
	auto _font = _io.Fonts->AddFontFromFileTTF(
		(_content_path + "fonts\\MyriadPro-Regular.ttf").c_str(),
		16.0f,
		NULL,
		_io.Fonts->GetGlyphRangesDefault());
	if (!_font)
	{
		printf("error loading font");
		return false;
	}

	GLint _last_texture_id;
	//load texture of icons
	int _width = -1, _height = -1, _comp = -1;
	uint8_t* _pixels = stbi_load((_content_path + "/textures/ui/icons.png").c_str(), &_width, &_height, &_comp, STBI_rgb_alpha);
	if (_pixels)
	{
		glGetIntegerv(GL_TEXTURE_BINDING_2D, &_last_texture_id);
		glGenTextures(1, &sIconTexture);
		glBindTexture(GL_TEXTURE_2D, sIconTexture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _width, _height, 0, GL_RGBA, GL_UNSIGNED_BYTE, _pixels);

		sIconTextureID = (void*)(intptr_t)sIconTexture;

		// Restore state
		glBindTexture(GL_TEXTURE_2D, _last_texture_id);

		free(_pixels);
		_pixels = nullptr;
	}
	else
	{
		logger.error("Error on loading icons file");
	}

	//load Map2D
	_pixels = stbi_load((_content_path + "/textures/ui/2DMap.jpg").c_str(), &_width, &_height, &_comp, STBI_rgb);
	if (_pixels)
	{
		glGetIntegerv(GL_TEXTURE_BINDING_2D, &_last_texture_id);
		glGenTextures(1, &sMap2DTexture);
		glBindTexture(GL_TEXTURE_2D, sMap2DTexture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, _width, _height, 0, GL_RGB, GL_UNSIGNED_BYTE, _pixels);

		sMap2DTextureID = (void*)(intptr_t)sMap2DTexture;

		// Restore state
		glBindTexture(GL_TEXTURE_2D, _last_texture_id);

		free(_pixels);
		_pixels = nullptr;
	}
	else
	{
		logger.error("Error on loading icons file");
	}

	return true;
}

void release()
{
	GLuint _ids[] = { sIconTexture , sMap2DTexture };
	glDeleteTextures(2, &_ids[0]);
}


widget_info show_menu(widget_info* pRelatedWidgetInfo)
{
	ImGuiStyle& _style = ImGui::GetStyle();

	widget_info _w_i;
	_w_i.size = ImVec2(sWindowWidth, sWindowHeight / 15.5f);
	_w_i.pos = ImVec2(0, 0);

	float _opacity = 0.5f;
	if (ImGui::IsMouseHoveringRect(_w_i.pos, ImVec2(_w_i.pos.x + _w_i.size.x, _w_i.pos.y + _w_i.size.y), false))
	{
		_opacity = 1.0f;
	}

#pragma region Setting Style
	_style.WindowPadding = ImVec2(0, 0);
	_style.WindowRounding = 0;
	_style.GrabRounding = 0;
	_style.GrabMinSize = 0;
	_style.FramePadding = ImVec2(0, 15);
	_style.ItemSpacing = ImVec2(15, 15);
	_style.WindowBorderSize = 0;
	_style.FrameBorderSize = 0;
	_style.ChildBorderSize = 0;

	//set text color
	_style.Colors[ImGuiCol_Text] = Colors::Black;
	_style.Colors[ImGuiCol_WindowBg] = Colors::WhiteGray;
	_style.Colors[ImGuiCol_WindowBg].w = _opacity;
	_style.Colors[ImGuiCol_HeaderHovered] = Colors::Orange;
	_style.Colors[ImGuiCol_Header] = Colors::Orange;
	_style.Colors[ImGuiCol_Header].w = 0.7f;
	_style.Colors[ImGuiCol_PopupBg] = Colors::WhiteGray;
	_style.Colors[ImGuiCol_PopupBg].w = 0.8f;
	_style.Colors[ImGuiCol_MenuBarBg].w = 0.0f;
	_style.Colors[ImGuiCol_Border].w = 0.0f;

#pragma endregion

	ImGuiWindowFlags _window_flags = 0;
	_window_flags |= ImGuiWindowFlags_NoTitleBar;
	_window_flags |= ImGuiWindowFlags_NoResize;
	_window_flags |= ImGuiWindowFlags_NoMove;
	_window_flags |= ImGuiWindowFlags_NoScrollbar;
	_window_flags |= ImGuiWindowFlags_NoCollapse;
	_window_flags |= ImGuiWindowFlags_MenuBar;

	ImGui::SetNextWindowSize(_w_i.size, ImGuiCond_Always);
	ImGui::SetNextWindowPos(_w_i.pos);
	if (!ImGui::Begin("Menu", 0, _window_flags))
	{
		// Early out if the window is collapsed, as an optimization.
		goto end;
	}
	
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			ImGui::MenuItem("	File_1													 ", NULL, nullptr);
			ImGui::MenuItem("	File_2													 ", NULL, nullptr);
			ImGui::MenuItem("	File_3													 ", NULL, nullptr);
			ImGui::MenuItem("	File_4													 ", NULL, nullptr);
			ImGui::MenuItem("	Exit													 ", NULL, nullptr);
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Edit"))
		{
			ImGui::MenuItem("	Edit_1													 ", NULL, nullptr);
			ImGui::MenuItem("	Edit_2													 ", NULL, nullptr);
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Configuration"))
		{
			ImGui::MenuItem("	Config_1												 ", NULL, nullptr);
			ImGui::MenuItem("	Config_2												 ", NULL, nullptr);
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Help"))
		{
			ImGui::MenuItem("	Help_1													 ", NULL, nullptr);
			ImGui::MenuItem("	Help_2													 ", NULL, nullptr);
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}

	//ImGui::SameLine();
	//ImGui::SetWindowPos(ImVec2(0, -30));
	//if (ImGui::ImageButton(sIconTextureID, ImVec2(32.0f, 32.0f), ImVec2(0, 0.1f), ImVec2(0.133f, 0.233f)))
	//{

	//}
end:
	ImGui::End();

	return _w_i;
}

widget_info show_falcon_icon(widget_info* pRelatedWidgetInfo)
{
	ImGuiStyle& _style = ImGui::GetStyle();

	widget_info _w_i;
	_w_i.size = ImVec2(sWindowWidth / 20, sWindowHeight / 15.5f);
	_w_i.pos = ImVec2(sWindowWidth - _w_i.size.x - 25.0f, -10.0f);

#pragma region Setting Style
	_style.WindowPadding = ImVec2(0, 0);
	_style.WindowRounding = 0;
	_style.GrabRounding = 0;
	_style.GrabMinSize = 0;
	_style.FramePadding = ImVec2(0, 0);
	_style.ItemSpacing = ImVec2(0, 0);
	_style.WindowBorderSize = 0;
	_style.FrameBorderSize = 0;
	_style.ChildBorderSize = 0;

	//set text color
	_style.Colors[ImGuiCol_Text] = Colors::Black;
	_style.Colors[ImGuiCol_WindowBg] = Colors::Transparent;
	_style.Colors[ImGuiCol_Button] = Colors::Transparent;
	_style.Colors[ImGuiCol_ButtonActive] = Colors::Transparent;
	_style.Colors[ImGuiCol_ButtonHovered] = Colors::Transparent;
	_style.Colors[ImGuiCol_ImageButtonActive] = Colors::Blue;
	_style.Colors[ImGuiCol_ImageButtonHovered] = Colors::Orange;

#pragma endregion

	ImGuiWindowFlags _window_flags = 0;
	_window_flags |= ImGuiWindowFlags_NoTitleBar;
	_window_flags |= ImGuiWindowFlags_NoResize;
	_window_flags |= ImGuiWindowFlags_NoMove;
	_window_flags |= ImGuiWindowFlags_NoScrollbar;
	_window_flags |= ImGuiWindowFlags_NoCollapse;
	_window_flags |= ImGuiWindowFlags_MenuBar;

	ImGui::SetNextWindowSize(_w_i.size, ImGuiCond_Always);
	ImGui::SetNextWindowPos(_w_i.pos);
	if (!ImGui::Begin("Falcon", 0, _window_flags))
	{
		// Early out if the window is collapsed, as an optimization.
		goto end;
	}
	
	if(ImGui::ImageButton(sIconTextureID, ImVec2(32.0f, 32.0f), ImVec2(0.133, 0.1f), ImVec2(0.266f, 0.233f), -1, Colors::Transparent, Colors::Black))
	{
		//show about
	}
	ImGui::SameLineEx(ImVec2(17.0f, 15.0f));
	ImGui::Text("alcon");
end:
	ImGui::End();

	return _w_i;
}

widget_info show_left_widget_controller(widget_info* pRelatedWidgetInfo)
{
	widget_info _w_i;
	_w_i.size = ImVec2(sWindowWidth / 40.0f, sWindowHeight / 10.0f);
	_w_i.pos = ImVec2(0, pRelatedWidgetInfo->pos.y + pRelatedWidgetInfo->size.y + 1);

#pragma region Setting Style
	ImGuiStyle& _style = ImGui::GetStyle();
	_style.WindowPadding = ImVec2(0, 0);
	_style.WindowRounding = 0;
	_style.GrabRounding = 0;
	_style.GrabMinSize = 0;
	_style.FramePadding = ImVec2(0, 0);
	_style.ItemSpacing = ImVec2(0, 0);
	_style.WindowBorderSize = 0.0f;
	_style.FrameBorderSize = 0.0f;
	_style.ChildBorderSize = 0;

	//set text color
	_style.Colors[ImGuiCol_WindowBg] = Colors::Orange;
	_style.Colors[ImGuiCol_ButtonActive] = Colors::Orange;
	_style.Colors[ImGuiCol_ImageButtonActive] = Colors::Blue;
	_style.Colors[ImGuiCol_ImageButtonHovered] = Colors::LightBlue;

#pragma endregion

	ImGuiWindowFlags _window_flags = 0;
	_window_flags |= ImGuiWindowFlags_NoTitleBar;
	_window_flags |= ImGuiWindowFlags_NoResize;
	_window_flags |= ImGuiWindowFlags_NoMove;
	_window_flags |= ImGuiWindowFlags_NoScrollbar;
	_window_flags |= ImGuiWindowFlags_NoCollapse;

	ImGui::SetNextWindowSize(_w_i.size, ImGuiCond_Always);
	ImGui::SetNextWindowPos(_w_i.pos);
	if (!ImGui::Begin("Left Controller", 0, _window_flags))
	{
		// Early out if the window is collapsed, as an optimization.
		goto end;
	}

	ImGui::SetWindowPos(ImVec2(_w_i.pos.x, _w_i.pos.y + 16.0f));
	if (ImGui::ImageButton(sIconTextureID, ImVec2(32.0f, 32.0f), ImVec2(0, 0.1f), ImVec2(0.133f, 0.233f)))
	{
		sLeftWidgetCollapseState = collapse_states::openning;
	}
end:
	ImGui::End();

	return _w_i;
}

widget_info show_explorer(widget_info* pRelatedWidgetInfo)
{
	widget_info _w_i;
	_w_i.size = ImVec2(sWindowWidth / 6.095238095238095f, sWindowHeight / 22.5f);
	_w_i.pos = ImVec2(0, pRelatedWidgetInfo->pos.y + pRelatedWidgetInfo->size.y + 1);

	if (sLeftWidgetCollapseState == collapse_states::collapsed)
	{
		show_left_widget_controller(pRelatedWidgetInfo);
		return _w_i;
	}
	else if (sLeftWidgetCollapseState == collapse_states::openning)
	{
		sLeftWidgetControllerSize.x += sUXAnimationSpeed * sDeltaTime;
		sLeftWidgetControllerSize.y = _w_i.size.y;

		if (sLeftWidgetControllerSize.x > _w_i.size.x)
		{
			sLeftWidgetCollapseState = collapse_states::openned;
		}
	}
	else if (sLeftWidgetCollapseState == collapse_states::collapsing)
	{
		sLeftWidgetControllerSize.x -= sUXAnimationSpeed * sDeltaTime;
		sLeftWidgetControllerSize.y = _w_i.size.y;

		if (sLeftWidgetControllerSize.x < _w_i.size.x / 4)
		{
			sLeftWidgetCollapseState = collapse_states::collapsed;
		}
	}
	else
	{
		sLeftWidgetControllerSize = _w_i.size;
	}

#pragma region Setting Style
	ImGuiStyle& _style = ImGui::GetStyle();
	_style.WindowPadding = ImVec2(0, 0);
	_style.WindowRounding = 0;
	_style.GrabRounding = 0;
	_style.GrabMinSize = 0;
	_style.FramePadding = ImVec2(0, 0);
	_style.ItemSpacing = ImVec2(0, 0);
	_style.WindowBorderSize = 0.0f;
	_style.FrameBorderSize = 0.0f;
	_style.ChildBorderSize = 0;

	//set text color
	_style.Colors[ImGuiCol_Text] = Colors::Black;	
	_style.Colors[ImGuiCol_WindowBg] = Colors::Orange;
	_style.Colors[ImGuiCol_Button] = Colors::Orange;
	_style.Colors[ImGuiCol_ButtonHovered] = Colors::Orange;
	_style.Colors[ImGuiCol_ButtonActive] = Colors::Blue;
	_style.Colors[ImGuiCol_ImageButtonHovered] = Colors::LightBlue;
	_style.Colors[ImGuiCol_ImageButtonActive] = Colors::Orange;
#pragma endregion

	ImGuiWindowFlags _window_flags = 0;
	_window_flags |= ImGuiWindowFlags_NoTitleBar;
	_window_flags |= ImGuiWindowFlags_NoResize;
	_window_flags |= ImGuiWindowFlags_NoMove;
	_window_flags |= ImGuiWindowFlags_NoScrollbar;
	_window_flags |= ImGuiWindowFlags_NoCollapse;

	ImGui::SetNextWindowSize(sLeftWidgetControllerSize, ImGuiCond_Always);
	ImGui::SetNextWindowPos(_w_i.pos);
	if (!ImGui::Begin("Explorer", 0, _window_flags))
	{
		// Early out if the window is collapsed, as an optimization.
		goto end;
	}

	ImGui::SetWindowPos(ImVec2(10, _w_i.pos.y + (_w_i.size.y / 2) - 8));
	ImGui::Text("Explorer");
	ImGui::SameLine(0.0f, 0.0f);

	ImVec2 _icon_size(_w_i.size.y - 2.0f, _w_i.size.y - 2.0f);
	ImGui::SetWindowPos(ImVec2(_w_i.pos.x + _w_i.size.x - 2.9f * _icon_size.x, _w_i.pos.y + 1));
	if (ImGui::ImageButton(sIconTextureID, _icon_size, ImVec2(0, 0), ImVec2(0.1, 0.1), -1.0f, ImVec4(0, 0, 0, 0), ImVec4(0.3f, 0.3f, 0.3f, 1)))
	{
		sLeftWidgetCollapseState = collapse_states::collapsing;
	}
end:
	ImGui::End();

	return _w_i;
}

widget_info show_search_widget(widget_info* pRelatedWidgetInfo)
{
	float _y_offset = pRelatedWidgetInfo->pos.y + pRelatedWidgetInfo->size.y + 1;
	widget_info _w_i;
	_w_i.size = ImVec2(sWindowWidth / 6.095238095238095f, sWindowHeight - _y_offset);
	_w_i.pos = ImVec2(0, _y_offset);

	if (sLeftWidgetCollapseState == collapse_states::collapsed)
	{
		return _w_i;
	}
	else if (sLeftWidgetCollapseState == collapse_states::openning)
	{
		//already did
		//sLeftWidgetControllerSize.x += 1.1f * sDeltaTime;
		sLeftWidgetControllerSize.y = _w_i.size.y;
	}
	else if (sLeftWidgetCollapseState == collapse_states::collapsing)
	{
		//already did
		//sLeftWidgetControllerSize.x -= sDeltaTime;
		sLeftWidgetControllerSize.y = _w_i.size.y;
	}
	else
	{
		sLeftWidgetControllerSize = _w_i.size;
	}

#pragma region Setting Style
	ImGuiStyle& _style = ImGui::GetStyle();
	_style.WindowPadding = ImVec2(0, -1);
	_style.WindowRounding = 0;
	_style.GrabRounding = 0;
	_style.GrabMinSize = 0;
	_style.FramePadding = ImVec2(0, 5.0f);
	_style.ItemSpacing = ImVec2(0, 0.0f);
	_style.WindowBorderSize = 1.5f;
	_style.FrameBorderSize = 0.1f;
	_style.ChildBorderSize = 0;

	//set text color
	_style.Colors[ImGuiCol_Text].x = 0.1f;
	_style.Colors[ImGuiCol_Text].y = 0.1f;
	_style.Colors[ImGuiCol_Text].z = 0.1f;
	_style.Colors[ImGuiCol_Text].w = 1.0f;

	_style.Colors[ImGuiCol_WindowBg] = Colors::Gray;
	_style.Colors[ImGuiCol_Header] = Colors::WhiteGray;
	_style.Colors[ImGuiCol_HeaderHovered] = Colors::Orange;
	_style.Colors[ImGuiCol_TextSelectedBg] = Colors::Orange;
	_style.Colors[ImGuiCol_Border] = Colors::BorderColor;
	_style.Colors[ImGuiCol_BorderShadow] = Colors::BorderColor;
	_style.Colors[ImGuiCol_BorderShadow].w = 0.5f;
	_style.Colors[ImGuiCol_Button] = Colors::Transparent;
	_style.Colors[ImGuiCol_ButtonHovered] = Colors::Orange;
	_style.Colors[ImGuiCol_ButtonActive] = Colors::Blue;
	_style.Colors[ImGuiCol_ImageButtonHovered] = Colors::LightBlue;
	_style.Colors[ImGuiCol_ImageButtonActive] = Colors::Orange;
#pragma endregion

	ImGuiWindowFlags _window_flags = 0;
	_window_flags |= ImGuiWindowFlags_NoTitleBar;
	_window_flags |= ImGuiWindowFlags_NoResize;
	_window_flags |= ImGuiWindowFlags_NoMove;
	_window_flags |= ImGuiWindowFlags_NoCollapse;

	ImGui::SetNextWindowSize(sLeftWidgetControllerSize, ImGuiCond_Always);
	ImGui::SetNextWindowPos(_w_i.pos);
	if (!ImGui::Begin("Search", 0, _window_flags))
	{
		// Early out if the window is collapsed, as an optimization.
		goto end;
	}
	
	auto _text_box_width = _w_i.size.x / 1.5f;
	ImGui::SetWindowPos(ImVec2(_w_i.pos.x + 2, _w_i.pos.y + 5));

	auto _serach_color = Colors::Black;
	_serach_color.w = 0.9f;
	ImGui::PushStyleColor(ImGuiCol_Header, _serach_color);
	ImGui::PushItemWidth(_text_box_width);
	if (ImGui::InputText("", sSearch, MAX_SEARCH_LENGHT, ImGuiInputTextFlags_EnterReturnsTrue))
	{
		printf("Searched entered");
	}
	ImGui::PopItemWidth();
	ImGui::PopStyleColor();

	ImGui::SameLineEx(ImVec2(_w_i.pos.x + _text_box_width, -3.0f), 7.0f);
	ImVec2 _icon_size(24.0f, 24.0f);
	ImGui::PushStyleColor(ImGuiCol_Border, Colors::Transparent);
	ImGui::PushStyleColor(ImGuiCol_BorderShadow, Colors::Transparent);
	if (ImGui::ImageButton(sIconTextureID, _icon_size, ImVec2(0.1f, 0.0f), ImVec2(0.2, 0.1), -1.0f, ImVec4(0, 0, 0, 0), Colors::Black))
	{
		printf("Searched called");
	}
	ImGui::SameLine(0.0f, 7.0f);
	if (ImGui::ImageButton(sIconTextureID, _icon_size, ImVec2(0.2f, 0.0f), ImVec2(0.3, 0.1), -1.0f, ImVec4(0, 0, 0, 0), Colors::Black))
	{
		printf("Searched called");
	}
	ImGui::PopStyleColor();
	ImGui::PopStyleColor();

	ImGuiTreeNodeFlags _node_flags =
		ImGuiTreeNodeFlags_OpenOnArrow |
		ImGuiTreeNodeFlags_OpenOnDoubleClick |
		ImGuiTreeNodeFlags_Selected |
		ImGuiTreeNodeFlags_Leaf |
		ImGuiTreeNodeFlags_NoTreePushOnOpen;

	auto _icon_color = Colors::Black;
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, -5));
	if (ImGui::CollapsingHeaderEx(sIconTextureID, ImVec2(0.3f, 0.0f), ImVec2(0.4f, 0.1f), _icon_color, "Equipments", false))
	{
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 5));
		if (ImGui::TreeNode("\t\tArea 110"))
		{
			ImGui::PushStyleColor(ImGuiCol_Header, Colors::Gray);
			for (int i = 0; i < 10; ++i)
			{
				ImGui::TreeNodeEx((void*)(intptr_t)i, _node_flags, "\tEquipments");
			}
			ImGui::PopStyleColor();
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("\t\tArea 120"))
		{
			ImGui::PushStyleColor(ImGuiCol_Header, Colors::Gray);
			for (int i = 0; i < 10; ++i)
			{
				ImGui::TreeNodeEx((void*)(intptr_t)i, _node_flags, "\tEquipments");
			}
			ImGui::PopStyleColor();
			ImGui::TreePop();
		}
		ImGui::PopStyleVar();
	}
	if (ImGui::CollapsingHeaderEx(sIconTextureID, ImVec2(0.4f, 0.0f), ImVec2(0.5f, 0.1f), _icon_color, "Cameras", false))
	{
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 5));
		if (ImGui::TreeNode("\t\tArea 130"))
		{
			ImGui::PushStyleColor(ImGuiCol_Header, Colors::Gray);
			for (int i = 0; i < 10; ++i)
			{
				ImGui::TreeNodeEx((void*)(intptr_t)i, _node_flags, "\tCam");
			}
			ImGui::PopStyleColor();
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("\t\tArea 140"))
		{
			ImGui::PushStyleColor(ImGuiCol_Header, Colors::Gray);
			for (int i = 0; i < 10; ++i)
			{
				ImGui::TreeNodeEx((void*)(intptr_t)i, _node_flags, "\Cam");
			}
			ImGui::PopStyleColor();
			ImGui::TreePop();
		}
		ImGui::PopStyleVar();
	}
	if (ImGui::CollapsingHeaderEx(sIconTextureID, ImVec2(0.5f, 0.0f), ImVec2(0.6f, 0.1f), _icon_color, "P & ID", false))
	{
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 5));
		if (ImGui::TreeNode("\t\tArea 150"))
		{
			ImGui::PushStyleColor(ImGuiCol_Header, Colors::Gray);
			for (int i = 0; i < 10; ++i)
			{
				ImGui::TreeNodeEx((void*)(intptr_t)i, _node_flags, "\tPlan");
			}
			ImGui::PopStyleColor();
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("\t\tArea 160"))
		{
			ImGui::PushStyleColor(ImGuiCol_Header, Colors::Gray);
			for (int i = 0; i < 10; ++i)
			{
				ImGui::TreeNodeEx((void*)(intptr_t)i, _node_flags, "\tPlan");
			}
			ImGui::PopStyleColor();
			ImGui::TreePop();
		}
		ImGui::PopStyleVar();
	}
	ImGui::PopStyleVar();
end:
	ImGui::End();

	return _w_i;
}

static float sTimes[7];
widget_info show_right_widget(widget_info* pRelatedWidgetInfo)
{
	widget_info _w_i;
	_w_i.size = ImVec2(sWindowWidth / 4.923f, sWindowHeight - pRelatedWidgetInfo->size.y);
	_w_i.pos = ImVec2(sWindowWidth - _w_i.size.x, pRelatedWidgetInfo->size.y);

#pragma region Setting Style
	ImGuiStyle& _style = ImGui::GetStyle();
	_style.WindowPadding = ImVec2(0, 0);
	_style.WindowRounding = 0;
	_style.GrabRounding = 0;
	_style.GrabMinSize = 0;
	_style.FramePadding = ImVec2(0, 5.0);
	_style.ItemSpacing = ImVec2(0, 1);
	_style.WindowBorderSize = 1.5f;
	_style.FrameBorderSize = 0.1f;
	_style.ChildBorderSize = 0;

	//set text color
	_style.Colors[ImGuiCol_Text].x = 0.0f;
	_style.Colors[ImGuiCol_Text].y = 0.0f;
	_style.Colors[ImGuiCol_Text].z = 0.0f;
	_style.Colors[ImGuiCol_Text].w = 1.0f;

	_style.Colors[ImGuiCol_WindowBg].x = 0.615f;
	_style.Colors[ImGuiCol_WindowBg].y = 0.615f;
	_style.Colors[ImGuiCol_WindowBg].z = 0.615f;
	_style.Colors[ImGuiCol_WindowBg].w = 1.0f;

	_style.Colors[ImGuiCol_TextSelectedBg].x = 0.9098039215686275f;
	_style.Colors[ImGuiCol_TextSelectedBg].y = 0.4431372549019608f;
	_style.Colors[ImGuiCol_TextSelectedBg].z = 0.3176470588235294f;
	_style.Colors[ImGuiCol_TextSelectedBg].w = 0.6f;

	_style.Colors[ImGuiCol_Border].x = 0.4f;
	_style.Colors[ImGuiCol_Border].y = 0.392f;
	_style.Colors[ImGuiCol_Border].z = 0.388f;
	_style.Colors[ImGuiCol_Border].w = 1.0f;

	_style.Colors[ImGuiCol_BorderShadow].x = 0.4f;
	_style.Colors[ImGuiCol_BorderShadow].y = 0.392f;
	_style.Colors[ImGuiCol_BorderShadow].z = 0.388f;
	_style.Colors[ImGuiCol_BorderShadow].w = 0.5f;

#pragma endregion

	ImGuiWindowFlags _window_flags = 0;
	_window_flags |= ImGuiWindowFlags_NoTitleBar;
	_window_flags |= ImGuiWindowFlags_NoResize;
	_window_flags |= ImGuiWindowFlags_NoMove;
	_window_flags |= ImGuiWindowFlags_NoScrollbar;
	_window_flags |= ImGuiWindowFlags_NoCollapse;

	ImGui::SetNextWindowSize(_w_i.size, ImGuiCond_Always);
	ImGui::SetNextWindowPos(_w_i.pos);
	if (!ImGui::Begin("Properties", 0, _window_flags))
	{
		// Early out if the window is collapsed, as an optimization.
		goto end;
	}
	
	static bool _live = true;
	ImGui::Checkbox("Live", &_live);

	auto _time_now = ImGui::GetTime();
	auto _array_time_len = IM_ARRAYSIZE(sTimes);
	if (_live)
	{
		//shift times
		for (size_t i = 0; i < _array_time_len - 1; ++i)
		{
			sTimes[i] = sTimes[i + 1];
		}
		sTimes[_array_time_len - 1] = _time_now;
	}
	ImGui::PlotLines("Time", sTimes, _array_time_len);

	// Create a dummy array of contiguous float values to plot
	// Tip: If your float aren't contiguous but part of a structure, you can pass a pointer to your first float and the sizeof() of your structure in the Stride parameter.
	static float values[90] = { 0 };
	static int values_offset = 0;
	static float refresh_time = 0.0f;
	if (!_live || refresh_time == 0.0f)
	{
		refresh_time = _time_now;
	}
	float _avg = 0;
	while (refresh_time < ImGui::GetTime()) // Create dummy data at fixed 60 hz rate for the demo
	{
		static float phase = 0.0f;
		values[values_offset] = cosf(phase);
		values_offset = (values_offset + 1) % IM_ARRAYSIZE(values);
		phase += 0.10f*values_offset;
		refresh_time += 1.0f / 60.0f;

		_avg += values[values_offset];
	}

	char _avg_str[32];
	_avg_str[31] = '\0';
	sprintf(_avg_str, "avg %.3f", abs(_avg));
	ImGui::PlotLines("Samples", values, IM_ARRAYSIZE(values), values_offset, _avg_str, -1.0f, 1.0f, ImVec2(0, 80));
	ImGui::PlotHistogram("Temps", sTimes, _array_time_len, 0, NULL, 0.0f, 1.0f, ImVec2(0, 80));

	// Use functions to generate output
	// FIXME: This is rather awkward because current plot API only pass in indices. We probably want an API passing floats and user provide sample rate/count.
	struct Funcs
	{
		static float Sin(void*, int i) { return sinf(i * 0.1f); }
		static float Saw(void*, int i) { return (i & 1) ? 1.0f : -1.0f; }
	};
	static int func_type = 0, display_count = 70;
	ImGui::Separator();
	ImGui::PushItemWidth(100); ImGui::Combo("Range", &func_type, "Data1\0Data2\0"); ImGui::PopItemWidth();
	ImGui::SameLine();
	ImGui::SliderInt("Sample count", &display_count, 1, 400);
	float(*func)(void*, int) = (func_type == 0) ? Funcs::Sin : Funcs::Saw;
	ImGui::PlotLines("Samples", func, NULL, display_count, 0, NULL, -1.0f, 1.0f, ImVec2(0, 70));
	ImGui::PlotHistogram("Histogram", func, NULL, display_count, 0, NULL, -1.0f, 1.0f, ImVec2(0, 70));
	ImGui::Separator();

	// Animate a simple progress bar
	static float progress = 0.0f, progress_dir = 1.0f;
	if (_live)
	{
		progress += progress_dir * 0.4f * ImGui::GetIO().DeltaTime;
		if (progress >= +1.1f) { progress = +1.1f; progress_dir *= -1.0f; }
		if (progress <= -0.1f) { progress = -0.1f; progress_dir *= -1.0f; }
	}

	float progress_saturated = (progress < 0.0f) ? 0.0f : (progress > 1.0f) ? 1.0f : progress;
	char buf[32];
	sprintf(buf, "%d/%d", (int)(progress_saturated * 1753), 1753);
	ImGui::ProgressBar(progress, ImVec2(0.f, 0.f), buf);
	ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
	ImGui::Text("Pressures");
	
	ImVec2 _tex_size(256, 256);
	ImGui::Image(sMap2DTextureID, _tex_size);

end:
	ImGui::End();

	return _w_i;
}

void show_ui()
{
	auto _w_i = show_menu(nullptr);
	show_falcon_icon(nullptr);
	auto _l_w_info = show_explorer(&_w_i);
	show_search_widget(&_l_w_info);
	show_right_widget(&_w_i);
}