#include "w_system_pch.h"
#include "w_image.h"
#include <turbojpeg.h>
#include <png.h>

namespace wolf
{
	namespace system
	{
		struct w_image_pimp
		{
#pragma region png

			static W_RESULT is_png_file(_Inout_ std::istream& pStream)
			{
				const int _png_paging_size = 8;
				png_byte _header[_png_paging_size];

				pStream.read((char*)_header, _png_paging_size);

				if (!pStream.good()) return W_INVALID_FILE_ATTRIBUTES;

				//check for png
				auto _hr = png_sig_cmp(_header, 0, _png_paging_size);
				//seek to first of stream
				pStream.seekg(0, std::ios::beg);

				return _hr == 0 ? W_PASSED : W_FAILED;
			}

			static W_RESULT is_png_file(_In_z_ const char* pFilePath)
			{
				std::ifstream _file(pFilePath, std::ios::binary);
				if (!_file)
				{
					//file is exist but it might be corrupted
					return W_INVALID_FILE_ATTRIBUTES;
				}
				auto _hr = is_png_file(_file);
				_file.close();

				return _hr == 0 ? W_PASSED : W_FAILED;
			}

			static uint8_t* read_png_from_stream(
				_Inout_ std::istream& pStream,
				_Out_ int& pWidth,
				_Out_ int& pHeight,
				_Out_ uint8_t& pColorType,
				_Out_ uint8_t& pBitDepth,
				_Out_ int& pNumberOfPasses,
				_Out_ int& pState,
				_In_ const w_png_pixel_format& pPixelFormat)
			{
				pState = 0;

				const int _png_paging_size = 8;
				png_byte _header[_png_paging_size];
				pStream.read((char*)_header, _png_paging_size);
				if (!pStream.good())
				{
					pState = -2;
					return nullptr;
				}

				//check for png
				if (png_sig_cmp(_header, 0, _png_paging_size))
				{
					pState = 1;
					return nullptr;
				}

				//initialize stuff
				auto _png_ptr = png_create_read_struct(
					PNG_LIBPNG_VER_STRING,
					NULL,
					NULL,
					NULL);
				if (!_png_ptr)
				{
					pState = -2;
					return nullptr;
				}

				auto _info_ptr = png_create_info_struct(_png_ptr);
				if (!_info_ptr)
				{
					pState = -2;
					return nullptr;
				}

				if (setjmp(png_jmpbuf(_png_ptr)))
				{
					pState = -2;
					png_destroy_read_struct(&_png_ptr, &_info_ptr, (png_infopp)0);
					return nullptr;
				}

				png_set_read_fn(_png_ptr, (void*)&pStream, png_user_read_data);//png_init_io(_png_ptr, _file);
				png_set_sig_bytes(_png_ptr, _png_paging_size);
				png_read_info(_png_ptr, _info_ptr);

				pWidth = (int)png_get_image_width(_png_ptr, _info_ptr);
				pHeight = (int)png_get_image_height(_png_ptr, _info_ptr);
				pColorType = png_get_color_type(_png_ptr, _info_ptr);
				pBitDepth = png_get_bit_depth(_png_ptr, _info_ptr);
				pNumberOfPasses = png_set_interlace_handling(_png_ptr);

				//check bit depth
				if (pBitDepth == 16)
				{
					png_set_strip_16(_png_ptr);
				}

				if (pColorType == PNG_COLOR_TYPE_PALETTE)
				{
					png_set_palette_to_rgb(_png_ptr);
				}

				// PNG_COLOR_TYPE_GRAY_ALPHA is always 8 or 16 bit depth.
				if (pColorType == PNG_COLOR_TYPE_GRAY && pBitDepth < 8)
				{
					png_set_expand_gray_1_2_4_to_8(_png_ptr);
				}

				if (png_get_valid(_png_ptr, _info_ptr, PNG_INFO_tRNS))
				{
					png_set_tRNS_to_alpha(_png_ptr);
				}

				// These color_type don't have an alpha channel then fill it with 0xff.
				if (pColorType == PNG_COLOR_TYPE_RGB ||
					pColorType == PNG_COLOR_TYPE_GRAY ||
					pColorType == PNG_COLOR_TYPE_PALETTE)
				{
					png_set_filler(_png_ptr, 0xFF, PNG_FILLER_AFTER);
				}

				if (pColorType == PNG_COLOR_TYPE_GRAY || pColorType == PNG_COLOR_TYPE_GRAY_ALPHA)
				{
					png_set_gray_to_rgb(_png_ptr);
				}

				png_read_update_info(_png_ptr, _info_ptr);

				//now data must be rgba
				auto _comp = 4;
				if (pPixelFormat == w_png_pixel_format::RGB_PNG ||
					pPixelFormat == w_png_pixel_format::BGR_PNG)
				{
					_comp = 3;
				}

				//allocate memory
				auto _pixels = (uint8_t*)malloc(_comp * pWidth * pHeight * sizeof(uint8_t));
				auto _bytes_per_row = png_get_rowbytes(_png_ptr, _info_ptr);
				auto _raw_data = (uint8_t*)malloc(_bytes_per_row * sizeof(uint8_t));

				//pixels counter
				uint32_t _k = 0;

				//read single row at a time and then convert it to desired pixel format
				switch (pPixelFormat)
				{
				case w_png_pixel_format::RGB_PNG:
					for (auto i = 0; i < pHeight; ++i)
					{
						png_read_row(_png_ptr, (png_bytep)_raw_data, NULL);

						const auto _row_offset = i * pWidth;

						uint32_t _byte_index = 0;
						for (auto j = 0; j < pWidth; ++j)
						{
							const uint32_t _r = _raw_data[_byte_index++];
							const uint32_t _g = _raw_data[_byte_index++];
							const uint32_t _b = _raw_data[_byte_index++];
							const uint32_t _a = _raw_data[_byte_index++];//ignored

							_pixels[_k] = _r;
							_pixels[_k + 1] = _g;
							_pixels[_k + 2] = _b;

							_k += _comp;
						}
					}
					break;
				case w_png_pixel_format::BGR_PNG:
					for (auto i = 0; i < pHeight; ++i)
					{
						png_read_row(_png_ptr, (png_bytep)_raw_data, NULL);

						const auto _row_offset = i * pWidth;

						uint32_t _byte_index = 0;
						for (auto j = 0; j < pWidth; ++j)
						{
							const uint32_t _r = _raw_data[_byte_index++];
							const uint32_t _g = _raw_data[_byte_index++];
							const uint32_t _b = _raw_data[_byte_index++];
							const uint32_t _a = _raw_data[_byte_index++];//ignored

							_pixels[_k] = _b;
							_pixels[_k + 1] = _g;
							_pixels[_k + 2] = _r;

							_k += _comp;
						}
					}
					break;
				case w_png_pixel_format::RGBA_PNG:
					for (auto i = 0; i < pHeight; ++i)
					{
						png_read_row(_png_ptr, (png_bytep)_raw_data, NULL);

						const auto _row_offset = i * pWidth;

						uint32_t _byte_index = 0;
						for (auto j = 0; j < pWidth; ++j)
						{
							const uint32_t _r = _raw_data[_byte_index++];
							const uint32_t _g = _raw_data[_byte_index++];
							const uint32_t _b = _raw_data[_byte_index++];
							const uint32_t _a = _raw_data[_byte_index++];//ignored

							_pixels[_k] = _r;
							_pixels[_k + 1] = _g;
							_pixels[_k + 2] = _b;
							_pixels[_k + 3] = _a;

							_k += _comp;
						}
					}
					break;
				case w_png_pixel_format::BGRA_PNG:
					for (auto i = 0; i < pHeight; ++i)
					{
						png_read_row(_png_ptr, (png_bytep)_raw_data, NULL);

						const auto _row_offset = i * pWidth;

						uint32_t _byte_index = 0;
						for (auto j = 0; j < pWidth; ++j)
						{
							const uint32_t _r = _raw_data[_byte_index++];
							const uint32_t _g = _raw_data[_byte_index++];
							const uint32_t _b = _raw_data[_byte_index++];
							const uint32_t _a = _raw_data[_byte_index++];//ignored

							_pixels[_k] = _b;
							_pixels[_k + 1] = _g;
							_pixels[_k + 2] = _r;
							_pixels[_k + 3] = _a;

							_k += _comp;
						}
					}
					break;
				};

				png_destroy_read_struct(&_png_ptr, &_info_ptr, (png_infopp)0);
				free(_raw_data);

				return _pixels;
			}

			static uint8_t* read_png_from_file(
				_In_z_ const char* pFilePath,
				_Out_ int& pWidth,
				_Out_ int& pHeight,
				_Out_ uint8_t& pColorType,
				_Out_ uint8_t& pBitDepth,
				_Out_ int& pNumberOfPasses,
				_Out_ int& pState,
				_In_ const w_png_pixel_format& pPixelFormat)
			{
				pState = 0;

				std::ifstream _file(pFilePath, std::ios::binary);
				if (!_file)
				{
					pState = -1;
					return nullptr;
				}
				auto _pixels = read_png_from_stream(
					_file,
					pWidth,
					pHeight,
					pColorType,
					pBitDepth,
					pNumberOfPasses,
					pState,
					pPixelFormat);
				_file.close();

				return _pixels;
			}

			//TODO: WRITE PNG FILEs
#pragma region Needs to check
			/*
				write png to file
				pState indicates the state
				 0 means succeeded
				-1 means file could not be opened for writing
				-2 means internal function error
			*/
			//			inline void write_png_to_file(
			//				_In_z_ const char* pFilePath,
			//				_In_ const uint8_t* pPixels, 
			//				_In_ const int& pWidth,
			//				_In_ const int& pHeight,
			//				_In_ const uint8_t& pBitDepth,
			//				_Out_ int& pState)
			//			{
			//				pState = 0;
			//
			//				//create file
			//#if defined(__WIN32) || defined(__UWP)
			//				FILE* _file;
			//				fopen_s(&_file, pFilePath, "wb");
			//
			//#else
			//				FILE* _file = fopen(pFilePath, "wb");
			//#endif
			//				if (!_file)
			//				{
			//					pState = -1;
			//					return;
			//				}
			//
			//				//initialize stuff
			//				auto _png_ptr = png_create_write_struct(
			//					PNG_LIBPNG_VER_STRING, 
			//					NULL, 
			//					NULL, 
			//					NULL);
			//				if (!_png_ptr)
			//				{
			//					pState = -2;
			//					fclose(_file);
			//					return;
			//				}
			//
			//				auto _info_ptr = png_create_info_struct(_png_ptr);
			//				if (!_info_ptr)
			//				{
			//					pState = -2;
			//					png_destroy_write_struct(&_png_ptr, &_info_ptr);
			//					fclose(_file);
			//					return;
			//				}
			//
			//				if (setjmp(png_jmpbuf(_png_ptr)))
			//				{
			//					pState = -2;
			//					png_destroy_write_struct(&_png_ptr, &_info_ptr);
			//					fclose(_file);
			//					return;
			//				}
			//				png_init_io(_png_ptr, _file);
			//
			//				//write header
			//				if (setjmp(png_jmpbuf(_png_ptr)))
			//				{
			//					pState = -2;
			//					png_destroy_write_struct(&_png_ptr, &_info_ptr);
			//					fclose(_file);
			//					return;
			//				}
			//
			//				png_set_IHDR(
			//					_png_ptr, 
			//					_info_ptr, 
			//					pWidth, 
			//					pHeight,
			//					pBitDepth, 
			//					PNG_COLOR_TYPE_RGBA,
			//					PNG_INTERLACE_NONE,
			//					PNG_COMPRESSION_TYPE_BASE, 
			//					PNG_FILTER_TYPE_BASE);
			//
			//				png_write_info(_png_ptr, _info_ptr);
			//				
			//				//write bytes
			//				if (setjmp(png_jmpbuf(_png_ptr)))
			//				{
			//					pState = -2;
			//					png_destroy_write_struct(&_png_ptr, &_info_ptr);
			//					fclose(_file);
			//					return;
			//				}
			//				png_write_image(_png_ptr, (png_bytep*)pPixels);
			//
			//				//end write
			//				if (setjmp(png_jmpbuf(_png_ptr)))
			//				{
			//					pState = -2;
			//					png_destroy_write_struct(&_png_ptr, &_info_ptr);
			//					fclose(_file);
			//					return;
			//				}
			//				png_write_end(_png_ptr, NULL);
			//				png_destroy_write_struct(&_png_ptr, &_info_ptr);
			//				fclose(_file);
			//			}


						/*
							write png to stream
							pState indicates the state
							 0 means succeeded
							-1 means file could not be opened for writing
							-2 means internal function error
						*/
						//inline void write_png_to_stream(
						//	_Inout_ std::ostream& pStream,
						//	_In_ const uint8_t* pPixels,
						//	_In_ const int& pWidth,
						//	_In_ const int& pHeight,
						//	_In_ const uint8_t& pBitDepth,
						//	_Out_ int& pState)
						//{
						//	pState = 0;

						//	//initialize stuff
						//	auto _png_ptr = png_create_write_struct(
						//		PNG_LIBPNG_VER_STRING,
						//		NULL,
						//		NULL,
						//		NULL);
						//	if (!_png_ptr)
						//	{
						//		pState = -2;
						//		return;
						//	}

						//	auto _info_ptr = png_create_info_struct(_png_ptr);
						//	if (!_info_ptr)
						//	{
						//		pState = -2;
						//		png_destroy_write_struct(&_png_ptr, &_info_ptr);
						//		return;
						//	}

						//	if (setjmp(png_jmpbuf(_png_ptr)))
						//	{
						//		pState = -2;
						//		png_destroy_write_struct(&_png_ptr, &_info_ptr);
						//		return;
						//	}

						//	png_set_write_fn(_png_ptr, (void*)&pStream, png_user_write_data, NULL);//png_init_io(_png_ptr, _file);

						//	//write header
						//	if (setjmp(png_jmpbuf(_png_ptr)))
						//	{
						//		pState = -2;
						//		png_destroy_write_struct(&_png_ptr, &_info_ptr);
						//		return;
						//	}

						//	png_set_IHDR(
						//		_png_ptr,
						//		_info_ptr,
						//		pWidth,
						//		pHeight,
						//		pBitDepth,
						//		PNG_COLOR_TYPE_RGBA,
						//		PNG_INTERLACE_NONE,
						//		PNG_COMPRESSION_TYPE_BASE,
						//		PNG_FILTER_TYPE_BASE);

						//	png_write_info(_png_ptr, _info_ptr);

						//	//write bytes
						//	if (setjmp(png_jmpbuf(_png_ptr)))
						//	{
						//		pState = -2;
						//		png_destroy_write_struct(&_png_ptr, &_info_ptr);
						//		return;
						//	}
						//	png_write_image(_png_ptr, (png_bytep*)pPixels);

						//	//end write
						//	if (setjmp(png_jmpbuf(_png_ptr)))
						//	{
						//		pState = -2;
						//		png_destroy_write_struct(&_png_ptr, &_info_ptr);
						//		return;
						//	}
						//	png_write_end(_png_ptr, NULL);
						//	png_destroy_write_struct(&_png_ptr, &_info_ptr);
						//}

#pragma endregion

#pragma endregion

#pragma region jpeg

			static uint8_t* read_jpeg_from_stream(
				_In_z_ std::istream& pStream,
				_Out_ int& pWidth,
				_Out_ int& pHeight,
				_Out_ int& pSubSample,
				_Out_ int& pColorSpace,
				_Out_ int& pNumberOfPasses,
				_Out_ int& pState,
				_In_ const TJPF& pPixelFormat = TJPF_RGB)
			{
				pState = 0;

				if (pPixelFormat == TJPF_UNKNOWN)
				{
					pState = -2;
					return nullptr;
				}

				//find size of file
				pStream.seekg(0, std::ios::end);
				unsigned long _jpeg_buffer_len = static_cast<unsigned long>(pStream.tellg());
				if (_jpeg_buffer_len <= 0)
				{
					pState = -1;
					return nullptr;
				}

				//go to begin 
				pStream.seekg(0, std::ios::beg);
				auto _jpeg_buffer = (uint8_t*)tjAlloc(_jpeg_buffer_len);
				if (!_jpeg_buffer)
				{
					pState = -2;
					return nullptr;
				}

				pStream.read((char*)_jpeg_buffer, _jpeg_buffer_len);
				if (!pStream.good())
				{
					pState = -1;
					return nullptr;
				}

				auto _tj_instance = tjInitDecompress();
				if (!_tj_instance)
				{
					pState = -2;
					return nullptr;
				}

				if (tjDecompressHeader3(
					_tj_instance,
					_jpeg_buffer,
					_jpeg_buffer_len,
					&pWidth,
					&pHeight,
					&pSubSample,
					&pColorSpace))
				{
					pState = 1;
					free(_jpeg_buffer);
					return nullptr;
				}

				auto _comp = 4;
				switch (pPixelFormat)
				{
				case TJPF_RGB:
				case TJPF_BGR:
					_comp = 3;
					break;
				}

				//TODO: should be alligned_malloc
				auto _pixels = (uint8_t*)malloc(_comp * pWidth * pHeight * sizeof(uint8_t));
				auto _hr = tjDecompress2(
					_tj_instance,
					_jpeg_buffer,
					_jpeg_buffer_len,
					_pixels,
					pWidth,
					_comp * pWidth,
					pHeight,
					pPixelFormat,
					0);

				tjFree(_jpeg_buffer);
				tjDestroy(_tj_instance);

				if (_hr)
				{
					pState = -2;
					return nullptr;
				}

				return _pixels;
			}

			static uint8_t* read_jpeg_from_file(
				_In_z_ const char* pFilePath,
				_Out_ int& pWidth,
				_Out_ int& pHeight,
				_Out_ int& pSubSample,
				_Out_ int& pColorSpace,
				_Out_ int& pNumberOfPasses,
				_Out_ int& pState,
				_In_ const TJPF& pPixelFormat = TJPF_RGB)
			{
				pState = 0;

				std::ifstream _file(pFilePath, std::ios::binary);
				if (!_file)
				{
					pState = -1;
					return nullptr;
				}
				auto _pixels = read_jpeg_from_stream(
					_file,
					pWidth,
					pHeight,
					pSubSample,
					pColorSpace,
					pNumberOfPasses,
					pState,
					pPixelFormat);
				_file.close();

				return _pixels;
			}

#pragma endregion

		private:
			static  void png_user_read_data(
				png_structp pPngPtr,
				png_bytep pData,
				png_size_t pLength)
			{
				//cast istream
				png_voidp _io = png_get_io_ptr(pPngPtr);
				((std::istream*)_io)->read((char*)pData, pLength);
			}

			static  void png_user_write_data(
				png_structp pPngPtr,
				png_bytep pData,
				png_size_t pLength)
			{
				//cast istream
				png_voidp _io = png_get_io_ptr(pPngPtr);
				((std::ostream*)_io)->write((char*)pData, pLength);
			}

			//static void png_user_flush_data(png_structp png_ptr)
//{

//}
		};
	}
}

using namespace wolf::system;

w_image_pimp* w_image::_pimp = new (std::nothrow) w_image_pimp();

W_RESULT w_image::is_png_file(_Inout_ std::istream& pStream)
{
	if (!w_image::_pimp)
	{
		wolf::logger.error("memory not allocated for w_image_pimp");
		return W_FAILED;
	}
	return _pimp->is_png_file(pStream);
}

W_RESULT w_image::is_png_file(_In_z_ const char* pFilePath)
{
	if (!w_image::_pimp)
	{
		wolf::logger.error("memory not allocated for w_image_pimp");
		return W_FAILED;
	}
	return _pimp->is_png_file(pFilePath);
}

uint8_t* w_image::read_png_from_stream(
	_Inout_ std::istream& pStream,
	_Out_ int& pWidth,
	_Out_ int& pHeight,
	_Out_ uint8_t& pColorType,
	_Out_ uint8_t& pBitDepth,
	_Out_ int& pNumberOfPasses,
	_Out_ int& pState,
	_In_ const w_png_pixel_format& pPixelFormat)
{
	if (!w_image::_pimp)
	{
		wolf::logger.error("memory not allocated for w_image_pimp");
		return nullptr;
	}
	return _pimp->read_png_from_stream(
		pStream,
		pWidth,
		pHeight,
		pColorType,
		pBitDepth,
		pNumberOfPasses,
		pState,
		pPixelFormat);
}

uint8_t* w_image::read_png_from_file(
	_In_z_ const char* pFilePath,
	_Out_ int& pWidth,
	_Out_ int& pHeight,
	_Out_ uint8_t& pColorType,
	_Out_ uint8_t& pBitDepth,
	_Out_ int& pNumberOfPasses,
	_Out_ int& pState,
	_In_ const w_png_pixel_format& pPixelFormat)
{
	if (!w_image::_pimp)
	{
		wolf::logger.error("memory not allocated for w_image_pimp");
		return nullptr;
	}
	return _pimp->read_png_from_file(
		pFilePath,
		pWidth,
		pHeight,
		pColorType,
		pBitDepth,
		pNumberOfPasses,
		pState,
		pPixelFormat);
}

uint8_t* w_image::read_jpeg_from_stream(
	_In_z_ std::istream& pStream,
	_Out_ int& pWidth,
	_Out_ int& pHeight,
	_Out_ int& pSubSample,
	_Out_ int& pColorSpace,
	_Out_ int& pNumberOfPasses,
	_Out_ int& pState,
	_In_ const w_jpeg_pixel_format& pPixelFormat)
{
	if (!w_image::_pimp)
	{
		wolf::logger.error("memory not allocated for w_image_pimp");
		return nullptr;
	}
	return _pimp->read_jpeg_from_stream(
		pStream,
		pWidth,
		pHeight,
		pSubSample,
		pColorSpace,
		pNumberOfPasses,
		pState,
		(TJPF)pPixelFormat);
}

uint8_t* w_image::read_jpeg_from_file(
	_In_z_ const char* pFilePath,
	_Out_ int& pWidth,
	_Out_ int& pHeight,
	_Out_ int& pSubSample,
	_Out_ int& pColorSpace,
	_Out_ int& pNumberOfPasses,
	_Out_ int& pState,
	_In_ const w_jpeg_pixel_format& pPixelFormat)
{
	if (!w_image::_pimp)
	{
		wolf::logger.error("memory not allocated for w_image_pimp");
		return nullptr;
	}
	return _pimp->read_jpeg_from_file(
		pFilePath,
		pWidth,
		pHeight,
		pSubSample,
		pColorSpace,
		pNumberOfPasses,
		pState,
		(TJPF)pPixelFormat);
}
