#include "pch.h"
#include "scene.h"

using namespace std;
using namespace wolf::graphics;
using namespace wolf::graphics::direct2D::shapes;

scene::scene()
{
	_super::set_app_name(L"03-Animated_Geometries_DX11_X.Win32");
}

scene::~scene()
{
	release();
}

void scene::initialize(std::map<int, std::vector<w_window_info>> pOutputWindowsInfo)
{
	// TODO: Add your pre-initialization logic here

	_super::initialize(pOutputWindowsInfo);

	// TODO: Add your post-initialization logic here
}

void scene::load()
{
	auto _gDevice = this->get_graphics_device();

#pragma region Create Sun

	//create sun
	this->_sun = make_unique<w_ellipse>(_gDevice);
	this->_sun->set_geormetry(800, 100, 70, 70);
	this->_sun->set_color(w_color::from_hex(w_color::YELLOW));
	this->_sun->set_border_color(w_color::from_hex(w_color::YELLOW));

#pragma endregion

#pragma region Create cloud

	this->_cloud = make_unique<w_geometry>(_gDevice);
	this->_cloud->set_color(w_color::from_hex(w_color::WHITE));
	this->_cloud->set_border_color(w_color::from_hex(w_color::WHITE));

	this->_cloud->begin();
	{
		this->_cloud->add_arc(
			D2D1::Point2F(40, 220),//Start
			D2D1::Point2F(210, 220),//End
			D2D1::SizeF(70, 70),//Size
			0.0f, //Rotation angle
			D2D1_SWEEP_DIRECTION_CLOCKWISE,
			D2D1_ARC_SIZE_SMALL);

		this->_cloud->add_arc(
			D2D1::Point2F(120, 220),//Start
			D2D1::Point2F(340, 220),//End
			D2D1::SizeF(100, 150),//Size
			0.0f, //Rotation angle
			D2D1_SWEEP_DIRECTION_CLOCKWISE,
			D2D1_ARC_SIZE_SMALL);

		this->_cloud->add_arc(
			D2D1::Point2F(150, 220),//Start
			D2D1::Point2F(400, 220),//End
			D2D1::SizeF(100, 90),//Size
			0.0f, //Rotation angle
			D2D1_SWEEP_DIRECTION_CLOCKWISE,
			D2D1_ARC_SIZE_SMALL);
	}
	this->_cloud->end();

#pragma endregion

#pragma region Create sea and waves

	this->_sea = make_unique<w_rectangle>(_gDevice);
	this->_sea->set_geormetry(0, 300, this->get_window_width(), 400, 0, 0);
	this->_sea->set_color(w_color::from_hex(w_color::BLUE));
	this->_sea->set_border_color(w_color::from_hex(w_color::BLUE));

	//create first wave
	auto _stroke_style = D2D1::StrokeStyleProperties(
		D2D1_CAP_STYLE::D2D1_CAP_STYLE_FLAT,
		D2D1_CAP_STYLE::D2D1_CAP_STYLE_FLAT,
		D2D1_CAP_STYLE::D2D1_CAP_STYLE_FLAT,
		D2D1_LINE_JOIN::D2D1_LINE_JOIN_MITER,
		10.0f,
		D2D1_DASH_STYLE::D2D1_DASH_STYLE_SOLID,
		0);

	std::vector<std::array<D2D1_POINT_2F, 3>> _points;
	for (size_t j = 0, _y = 350; j < 7; j++, _y += 35)
	{
		auto _wave = new w_geometry(_gDevice);
		_wave->set_color(w_color::from_hex(w_color::WHEAT));
		_wave->set_stroke_width(3);
		_wave->set_fill_geometry(false);
		_wave->set_style(_stroke_style);

		auto _start_point = D2D1::Point2F(50, _y);
		
		int _offset;
		if (j % 2 == 0)
		{
			_offset = 150;
			_start_point.x = 50;
		}
		else
		{
			_offset = 110;
			_start_point.x = 150;
		}
		_points.clear();
		for (size_t i = 0; i < 5; i++)
		{
			std::array<D2D1_POINT_2F, 3> _array_points;
			_array_points[0] = D2D1::Point2F(_start_point.x + 50 + i * _offset, _start_point.y - 50);
			_array_points[1] = D2D1::Point2F(_start_point.x + 100 + i * _offset, _start_point.y);
			_array_points[2] = D2D1::Point2F(_start_point.x + 150 + i * _offset, _start_point.y);

			_points.push_back(_array_points);
		}

		_wave->begin();
		{
			_wave->add_beziers(_start_point, _points,
				D2D1_FILL_MODE::D2D1_FILL_MODE_WINDING,
				D2D1_FIGURE_BEGIN::D2D1_FIGURE_BEGIN_FILLED,
				D2D1_FIGURE_END::D2D1_FIGURE_END_OPEN);
		}
		_wave->end();

		this->_waves.push_back(_wave);
	}
	_points.clear();

#pragma endregion

	_photons.resize(4);

	_super::load();
}

void scene::update(const wolf::system::w_game_time& pGameTime)
{
#pragma region update photons

	auto _gDevice = this->get_graphics_device();
	std::vector<std::array<D2D1_POINT_2F, 3>> _points;

	for (size_t i = 0; i < 4; ++i)
	{
		float _dot_offset = fmodf(10 * pGameTime.get_total_seconds(), 2);

		auto _photon = new w_geometry(_gDevice);
		_photon->set_color(w_color::from_hex(w_color::YELLOW));
		_photon->set_stroke_width(4);//3
		_photon->set_fill_geometry(false);
		_photon->set_style(D2D1::StrokeStyleProperties(
			D2D1_CAP_STYLE::D2D1_CAP_STYLE_ROUND,
			D2D1_CAP_STYLE::D2D1_CAP_STYLE_ROUND,
			D2D1_CAP_STYLE::D2D1_CAP_STYLE_ROUND,
			D2D1_LINE_JOIN::D2D1_LINE_JOIN_ROUND,
			10.0f,
			D2D1_DASH_STYLE::D2D1_DASH_STYLE_DOT,//D2D1_DASH_STYLE_SOLID
			_dot_offset));

		auto _start_point = D2D1::Point2F(695, 47);
		std::array<D2D1_POINT_2F, 3> _array_points;
		if (i == 1)
		{
			_start_point.x = 670;
			_start_point.y = 100;
		}
		else if (i == 2)
		{
			_start_point.x = 680;
			_start_point.y = 150;
		}
		else if (i == 3)
		{
			_start_point.x = 723;
			_start_point.y = 185;
		}
		_array_points[0] = D2D1::Point2F(_start_point.x + 50, _start_point.y - 50);
		_array_points[1] = D2D1::Point2F(_start_point.x + 100, _start_point.y);
		_array_points[2] = D2D1::Point2F(_start_point.x + 110, _start_point.y);

		_points.clear();
		_points.push_back(_array_points);

		_photon->begin();
		{
			_photon->add_beziers(_start_point, _points,
				D2D1_FILL_MODE::D2D1_FILL_MODE_WINDING,
				D2D1_FIGURE_BEGIN::D2D1_FIGURE_BEGIN_FILLED,
				D2D1_FIGURE_END::D2D1_FIGURE_END_OPEN);
		}
		_photon->end();

		if (this->_photons[i])
		{
			SAFE_RELEASE(this->_photons[i]);
		}
		this->_photons[i] = _photon;
	}

#pragma endregion

	_super::update(pGameTime);
}

void scene::render(const wolf::system::w_game_time& pGameTime)
{
	auto _gDevice = this->get_graphics_device();

	auto _movement = (float)std::cos(pGameTime.get_total_seconds());
	this->sprite_batch->begin();
	{
		_gDevice->context_2D->SetTransform(D2D1::Matrix3x2F::Identity());
		for (auto _photon : this->_photons)
		{
			_photon->draw();
		}
		this->_sun->draw();

		_gDevice->context_2D->SetTransform(D2D1::Matrix3x2F::Translation(5.0f * _movement, 0));
		this->_cloud->draw();

		_gDevice->context_2D->SetTransform(D2D1::Matrix3x2F::Identity());
		this->_sea->draw();
		for (size_t i = 0; i < this->_waves.size(); ++i)
		{
			float _sign = i % 2 == 0 ? 1 : -1;
			_gDevice->context_2D->SetTransform(D2D1::Matrix3x2F::Translation(_sign * 10.0f * _movement, 0));
			this->_waves.at(i)->draw();
		}
	}
	this->sprite_batch->end();

	_super::render(pGameTime);
}

HRESULT scene::on_msg_proc(HWND pHWND, UINT pMsg, WPARAM pWParam, LPARAM pLParam)
{
	return S_FALSE;
}

ULONG scene::release()
{
	if (this->is_released()) return 0;
	
	UNIQUE_RELEASE(this->_sun);
	for (auto _photon : this->_photons)
	{
		SAFE_RELEASE(_photon);
	}

	UNIQUE_RELEASE(this->_sea);

	UNIQUE_RELEASE(this->_cloud);

	//release waves
	for (auto _wave : this->_waves)
	{
		SAFE_RELEASE(_wave);
	}

	return _super::release();
}
