#include "pch.h"
#include "scene.h"
#include "w_graphics\w_textures\ScreenGrab.h"

using namespace std;
using namespace wolf::graphics;
using namespace DirectX;


static const UINT width = 1024;
static const UINT height = 1024;


static glm::vec3 base_a, base_b;

scene::scene()
{
	_super::set_app_name(L"01-Shapes_DX11_X.Win32");
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
	// TODO: load your game assets here
	auto _gDevice = this->get_graphics_device();
	auto _device = _gDevice->device.Get();
	auto _context = _gDevice->context.Get();

	auto _weights = glm::i32vec3(1, 5, 1);

	std::unique_ptr<w_texture_2D> _texture2D;
	auto _hr = w_texture_2D::load_texture_2D_from_file(_device, _texture2D, L"C:\\Test\\t.dds", true, true, _weights, 0Ui64, DXGI_FORMAT_R8G8B8A8_UNORM);
	if (_hr != S_OK)
	{
		return;
	}
	DirectX::SaveDDSTextureToFile(_context, _texture2D->get_resource(), L"c:\\test\\encoded.dds");


	//load encoded texture
	std::unique_ptr<w_texture_2D> _encoded_texture2D;
	_hr = w_texture_2D::load_texture_2D_from_file(_device, _encoded_texture2D, L"C:\\Test\\encoded.dds", true, false, glm::i32vec3(), 0Ui64, DXGI_FORMAT_R8G8B8A8_UNORM);
	if (_hr != S_OK)
	{
		return;
	}
	w_texture_2D* _decoded_texture2D = nullptr;
	w_texture_2D::decode_texture_2D(_device, _texture2D->get_encode_base_a(), _texture2D->get_encode_base_b(), _encoded_texture2D.get(), &_decoded_texture2D);

	DirectX::SaveDDSTextureToFile(_context, _decoded_texture2D->get_resource(), L"c:\\Test\\decoded.dds");

	//auto _got_it = w_texture_2D::compare(_device, _source.get(), _decoded_texture2D, 10);

	//logger.write(_got_it ? "OK" : "NO");

	UNIQUE_RELEASE(_texture2D);
	UNIQUE_RELEASE(_encoded_texture2D);
	SAFE_RELEASE(_decoded_texture2D);
	
	_super::load();
}

void scene::update(const wolf::system::w_game_time& pGameTime)
{
	_super::update(pGameTime);
}

void scene::render(const wolf::system::w_game_time& pGameTime)
{

	_super::render(pGameTime);
}

HRESULT scene::on_msg_proc(HWND pHWND, UINT pMsg, WPARAM pWParam, LPARAM pLParam)
{
	return S_FALSE;
}

ULONG scene::release()
{
	if (this->is_released()) return 0;


	return _super::release();
}
