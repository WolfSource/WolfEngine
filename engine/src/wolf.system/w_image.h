/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (https://PooyaEimandar.github.io) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/WolfEngine/Wolf.Engine/issues
	Website			 : https://WolfEngine.App
	Name			 : w_image.h
	Description		 : image encoder & decoder struct "https://github.com/dwd/rapidxml"
	Comment          :
*/

#pragma once

#include "w_std.h"

namespace wolf::system
{
	enum w_png_pixel_format
	{
		//The red, green, and blue components in the image are stored in 3 bytes.
		RGB_PNG = 0,
		//The blue, green, and red components in the image are stored in 3 bytes.
		BGR_PNG,
		//The red, green, blue and alpha components in the image are stored in 4 bytes.
		RGBA_PNG,
		//The blue, green, red and alpha components in the image are stored in 4 bytes.
		BGRA_PNG
	};

	enum w_jpeg_pixel_format
	{
		//Unknown pixel format
		UNKNOWN_JPEG = -1,
		//The red, green, and blue components in the image are stored in 3 bytes.
		RGB_JPEG = 0,
		//The blue, green, and red components in the image are stored in 3 bytes.
		BGR_JPEG,
		//The red, green, blue and alpha components in the image are stored in 4 bytes.
		RGBX_JPEG,
		//The blue, green, red and alpha components in the image are stored in 4 bytes.
		BGRX_JPEG,
		//The blue, green, red and alpha components in the image are stored in 4 bytes.
		XBGR_JPEG,
		//The alpha, blue, green and red components in the image are stored in 4 bytes.
		XRGB_JPEG,
		//Grayscale pixel format.  Each 1-byte pixel represents a luminance * (brightness) level from 0 to 255.
		GRAY_JPEG,
		//This is the same as JPEG_RGBX, except that when decompressing, the X component is guaranteed to be 0xFF.
		RGBA_JPEG,
		//This is the same as JPEG_BGRX, except that when decompressing, the X component is guaranteed to be 0xFF.
		BGRA_JPEG,
		//This is the same as JPEG_XBGR, except that when decompressing, the X component is guaranteed to be 0xFF.
		ABGR_JPEG,
		//This is the same as JPEG_XRGB, except that when decompressing, the X component is guaranteed to be 0xFF.
		ARGB_JPEG,
		/*
			Unlike RGB, which is an additive color model used
			primarily for display, CMYK (Cyan/Magenta/Yellow/Key) is a subtractive
			color model used primarily for printing.  In the CMYK color model, the
			value of each color component typically corresponds to an amount of cyan,
			magenta, yellow, or black ink that is applied to a white background.  In
			order to convert between CMYK and RGB, it is necessary to use a color
			management system (CMS.)  A CMS will attempt to map colors within the
			printer's gamut to perceptually similar colors in the display's gamut and
			vice versa, but the mapping is typically not 1:1 or reversible, nor can it
			be defined with a simple formula.  Thus, such a conversion is out of scope
			for a codec library.  However, the TurboJPEG API allows for compressing
			CMYK pixels into a YCCK JPEG image (see #TJCS_YCCK) and decompressing YCCK
			JPEG images into CMYK pixels.
		 */
		CMYK_JPEG,
	};

	struct w_image_pimp;
	struct w_image
	{
		//is stream contains png data
		WSYS_EXP static W_RESULT is_png_file(_Inout_ std::istream& pStream);
		//is file contains png data
		WSYS_EXP static W_RESULT is_png_file(_In_z_ const char* pFilePath);
		/*
			read png from stream
			pState indicates the state
			 0 means the succeded
			 1 means file is not png
			-1 means the file could not be opened for reading
			-2 means internal function error
		*/
		WSYS_EXP static uint8_t* read_png_from_stream(
			_Inout_ std::istream& pStream,
			_Out_ int& pWidth,
			_Out_ int& pHeight,
			_Out_ uint8_t& pColorType,
			_Out_ uint8_t& pBitDepth,
			_Out_ int& pNumberOfPasses,
			_Out_ int& pState,
			_In_ const w_png_pixel_format& pPixelFormat = w_png_pixel_format::RGBA_PNG);

		/*
			read png from file
			pState indicates the state
			 0 means the succeded
			 1 means file is not png
			-1 means the file could not be opened for reading
			-2 means internal function error
		*/
		WSYS_EXP static uint8_t* read_png_from_file(
			_In_z_ const char* pFilePath,
			_Out_ int& pWidth,
			_Out_ int& pHeight,
			_Out_ uint8_t& pColorType,
			_Out_ uint8_t& pBitDepth,
			_Out_ int& pNumberOfPasses,
			_Out_ int& pState,
			_In_ const w_png_pixel_format& pPixelFormat = w_png_pixel_format::RGBA_PNG);

		/*
			read jpg from stream
			pState indicates the state
			 0 means the succeded
			 1 means file is not jpg
			-1 means the file could not be opened for reading
			-2 means internal function error
		*/
		WSYS_EXP static uint8_t* read_jpeg_from_stream(
			_In_z_ std::istream& pStream,
			_Out_ int& pWidth,
			_Out_ int& pHeight,
			_Out_ int& pSubSample,
			_Out_ int& pColorSpace,
			_Out_ int& pNumberOfPasses,
			_Out_ int& pState,
			_In_ const w_jpeg_pixel_format& pPixelFormat = w_jpeg_pixel_format::RGB_JPEG);

		/*
			read jpg from file
			pState indicates the state
			 0 means the succeded
			 1 means file is not jpg
			-1 means the file could not be opened for reading
			-2 means internal function error
		*/
		WSYS_EXP static uint8_t* read_jpeg_from_file(
			_In_z_ const char* pFilePath,
			_Out_ int& pWidth,
			_Out_ int& pHeight,
			_Out_ int& pSubSample,
			_Out_ int& pColorSpace,
			_Out_ int& pNumberOfPasses,
			_Out_ int& pState,
			_In_ const w_jpeg_pixel_format& pPixelFormat = w_jpeg_pixel_format::RGB_JPEG);


	private:
		static w_image_pimp* _pimp;
	};
}
