#ifndef __W_MATH_H__
#define __W_MATH_H__

#include <string>
#include <sstream>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#ifndef RGBA_TO_HEX_COLOR
#define RGBA_TO_HEX_COLOR(r,g,b,a) ((unsigned long)((((a)&0xff)<<24) | (((r)&0xff)<<16) | (((g)&0xff)<<8) | ((b)&0xff)))
#endif

namespace std
{
	//create glm::vec2 from string in the format of "0,0" (x,y)
	inline glm::vec2 atov2(const char* pValue)
	{
		//create glm::vec2 from string in the format of "0,0"
		std::stringstream _sstrream(pValue);

		float _x = 0, _y = 0;
		int i = 0;
		char _c;
		std::string _number;
		while (_sstrream >> _c)
		{
			_number += _c;
			if (_sstrream.peek() == ',')
			{
				_sstrream.ignore();

				if (i == 0)
				{
					_x = (float) std::atof(_number.c_str());
				}

				i++;
				_number = "";
			}
		}

		if (!_number.empty())
		{
			_y = (float) std::atof(_number.c_str());
		}

		_sstrream.clear();

		return glm::vec2(_x, _y);
	}

	//create glm::vec3 from string in the format of "0,0,0" (w,x,y)
	inline glm::vec3 atov3(const char* pValue)
	{
		//create glm::vec3 from string in the format of "0,0,0"
		std::stringstream _sstrream(pValue);

		float _x = 0, _y = 0, _z = 0;
		int i = 0;
		char _c;
		std::string _number;
		while (_sstrream >> _c)
		{
			_number += _c;
			if (_sstrream.peek() == ',')
			{
				_sstrream.ignore();

				if (i == 0)
				{
					_x = (float) std::atof(_number.c_str());
				}
				else if (i == 1)
				{
					_y = (float) std::atof(_number.c_str());
				}

				i++;
				_number = "";
			}
		}

		if (!_number.empty())
		{
			_z = (float) std::atof(_number.c_str());
		}

		_sstrream.clear();

		return glm::vec3(_x, _y, _z);
	}

	//create glm::vec4 from string in the format of "0,0,0,0" (w,x,y,z)
	inline glm::vec4 atov4(const char* pValue)
	{
		std::stringstream _sstrream(pValue);

		float _x = 0, _y = 0, _z = 0, _w = 0;
		int i = 0;
		char _c;
		std::string _number;
		while (_sstrream >> _c)
		{
			_number += _c;
			if (_sstrream.peek() == ',')
			{
				_sstrream.ignore();

				if (i == 0)
				{
					_w = (float) std::atof(_number.c_str());
				}
				else if (i == 1)
				{
					_x = (float) std::atof(_number.c_str());
				}
				else if (i == 2)
				{
					_y = (float) std::atof(_number.c_str());
				}

				i++;
				_number = "";
			}
		}

		if (!_number.empty())
		{
			_z = (float) std::atof(_number.c_str());
		}

		_sstrream.clear();

		return glm::vec4(_w, _x, _y, _z);
	}
}

#endif