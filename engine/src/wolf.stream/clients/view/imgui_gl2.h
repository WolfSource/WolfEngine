#if _MSC_VER > 1000
#pragma once
#endif

#ifndef __IMGUI_IMPL_GL2_H__
#define __IMGUI_IMPL_GL2_H__

struct GLFWwindow;

IMGUI_API bool        ImGui_GL2_Init(GLFWwindow* window, bool install_callbacks);
IMGUI_API void        ImGui_GL2_Shutdown();
IMGUI_API void        ImGui_GL2_NewFrame();

IMGUI_API void        ImGui_GL2_InvalidateDeviceObjects();
IMGUI_API bool        ImGui_GL2_CreateDeviceObjects();

IMGUI_API void        ImGui_GL2_MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
IMGUI_API void        ImGui_GL2_ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
IMGUI_API void        ImGui_GL2_KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
IMGUI_API void        ImGui_GL2_CharCallback(GLFWwindow* window, unsigned int c);

#endif