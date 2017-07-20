/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : pch.h
	Description		 : The main scene of Wolf Engine
	Comment          : Read more information about this sample on http://wolfsource.io/gpunotes/wolfengine/
*/

#ifndef __SCENE_H__
#define __SCENE_H__

#include <w_game.h>

class scene : public wolf::framework::w_game
{
public:
    scene(_In_z_ std::string pRootDirectory, _In_z_ std::string pAppName);
    virtual ~scene();
    
    /*
        Allows the game to perform any initialization and it needs to before starting to run.
        Calling Game::Initialize() will enumerate through any components and initialize them as well.
        The parameter pOutputWindowsInfo represents the information of output window(s) of this game.
     */
    void initialize(_In_ std::map<int, std::vector<w_window_info>> pOutputWindowsInfo) override;
    
    //The function "Load()" will be called once per game and is the place to load all of your game assets.
    void load() override;
    
    //This is the place where allows the game to run logic such as updating the world, checking camera, collisions, physics, input, playing audio and etc.
    void update(_In_ const wolf::system::w_game_time& pGameTime) override;
    
    //This is called when the game should draw itself.
    HRESULT render(_In_ const wolf::system::w_game_time& pGameTime) override;
    
    //This is called when the window game should resized. pIndex is the index of window.
    void on_window_resized(_In_ UINT pIndex) override;
    
    //This is called when the we lost graphics device.
    void on_device_lost() override;
    
#ifdef __WIN32
    HRESULT on_msg_proc(HWND pHWND, UINT pMessage, WPARAM pWparam, LPARAM pLparam) override;
#endif
    //Release will be called once per game and is the place to unload assets and release all resources
    ULONG release() override;
    
private:
    typedef wolf::framework::w_game _super;
    
};
#endif

