/*
	Project			 : Wolf Engine (http://WolfSource.io). Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : https://github.com/PooyaEimandar/WolfEngine - Please direct any bug to contact@WolfSource.io
	Name			 : Scene.h
	Description		 : The main class of your game
	Comment          :
*/

#ifndef __SCENE_H__
#define __SCENE_H__

#include <w_game.h>
#include <w_graphics/w_sprite_batch.h>
#include <w_graphics/w_fonts/w_sprite_font.h>
#include <w_graphics/w_direct2D/w_rectangle.h>
#include <w_graphics/w_direct2D/w_ellipse.h>
#include <w_graphics/w_geometries/w_quad.h>
#include <w_graphics/w_direct2D/w_geometry.h>
#include <w_graphics/w_textures/w_render_target_2D.h>
#include <w_gui/w_widgets_resource_manager.h>
#include <w_renderable/w_renderable_scene.h>

class CppClass
{
public:
	int a;
	int getNum()
	{
		return a;
	}
	void setNum(int b)
	{
		a = b;
	}
};

class scene sealed : public wolf::framework::w_game
{
public:
	scene();
	virtual ~scene();

	/*
		Allows the game to perform any initialization and it needs to before starting to run.
		Calling Game::Initialize() will enumerate through any components and initialize them as well.
		The parameter pOutputWindowsInfo represents the information of output window(s) of this game.
	*/
	void initialize(std::map<int, std::vector<w_window_info>> pOutputWindowsInfo) override;

	//The function "Load()" will be called once per game and is the place to load all of your game assets.
	void load() override;

	//This is the place where allows the game to run logic such as updating the world, checking camera, collisions, physics, input, playing audio and etc.
	void update(const wolf::system::w_game_time& pGameTime) override;
	
	//Begin render on all graphics devices
	void begin_render(const wolf::system::w_game_time& pGameTime) override;

	//This is called when the game should draw itself.
	void render(const wolf::system::w_game_time& pGameTime) override;

	//End render on all graphics devices
	void end_render(const wolf::system::w_game_time& pGameTime) override;

	//This is called when the window game should resized. pIndex is the index of window.
	void on_window_resized(UINT pIndex) override;

	//This is called when the we lost graphics device.
	void on_device_lost() override;

	HRESULT on_msg_proc(HWND pHWND, UINT pMessage, WPARAM pWparam, LPARAM pLparam) override;

	//Release will be called once per game and is the place to unload assets and release all resources
	ULONG release() override;

private:

	void _on_connect(const boost::system::error_code& pErrorCode);
	void _on_sent(const boost::system::error_code& pErrorCode, const size_t pBytesTransferred);
	void _on_received(const boost::system::error_code& pErrorCode, const size_t pBytesTransferred, std::string _pReceivedMessage);

	//std::unique_ptr<Wolf::Graphics::W_SpriteFont>						spriteFont;

	//std::unique_ptr<Wolf::Graphics::Direct2D::Shapes::W_Ellipse>		ellipse;
	//std::unique_ptr<Wolf::Graphics::Direct2D::Shapes::W_Rectangle>		rectangle;
	//
	//std::unique_ptr<Wolf::Graphics::Direct2D::Shapes::W_Geometry>		geoCloud0;
	//std::unique_ptr<Wolf::Graphics::Direct2D::Shapes::W_Geometry>		geoCloud1;

	//std::unique_ptr<Wolf::Graphics::W_Quad>								quad;
	//std::unique_ptr<Wolf::Graphics::W_RenderTarget2D>					renderTarget2D;

	//std::unique_ptr<Wolf::Graphics::W_Texture2D>						texture2D;

	std::unique_ptr<wolf::gui::w_widgets_resource_manager>								_widget_resource_manager;
	std::unique_ptr<wolf::gui::w_widget>												_widget;

};

#endif

