/*
    Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
    Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
    Website			 : http://WolfSource.io
    Name			 : py_glm.h
    Description		 : The python exporter for glm 
    Comment          :
*/

#ifdef __PYTHON__

#ifndef __PY_GLM_H__
#define __PY_GLM_H__

#include <glm_extention.h>
#include <boost/python.hpp>

namespace pyWolf
{
    struct w_mat4
    {
        float get_00()                          { return this->_m[0][0];    }
        void  set_00(_In_ const float& pValue)  { this->_m[0][0] = pValue;  }

        float get_01()                          { return this->_m[0][1];    }
        void  set_01(_In_ const float& pValue)  { this->_m[0][1] = pValue;  }

        float get_02()                          { return this->_m[0][2];    }
        void  set_02(_In_ const float& pValue)  { this->_m[0][2] = pValue;  }

        float get_03()                          { return this->_m[0][3];    }
        void  set_03(_In_ const float& pValue)  { this->_m[0][3] = pValue;  }

        float get_10()                          { return this->_m[1][0];    }
        void  set_10(_In_ const float& pValue)  { this->_m[1][0] = pValue;  }

        float get_11()                          { return this->_m[1][1];    }
        void  set_11(_In_ const float& pValue)  { this->_m[1][1] = pValue;  }

        float get_12()                          { return this->_m[1][2];    }
        void  set_12(_In_ const float& pValue)  { this->_m[1][2] = pValue;  }

        float get_13()                          { return this->_m[1][3];    }
        void  set_13(_In_ const float& pValue)  { this->_m[1][3] = pValue;  }

        float get_20()                          { return this->_m[2][0];    }
        void  set_20(_In_ const float& pValue)  { this->_m[2][0] = pValue;  }

        float get_21()                          { return this->_m[2][1];    }
        void  set_21(_In_ const float& pValue)  { this->_m[2][1] = pValue;  }

        float get_22()                          { return this->_m[2][2];    }
        void  set_22(_In_ const float& pValue)  { this->_m[2][2] = pValue;  }

        float get_23()                          { return this->_m[2][3];    }
        void  set_23(_In_ const float& pValue)  { this->_m[2][3] = pValue;  }

        float get_30()                          { return this->_m[3][0];    }
        void  set_30(_In_ const float& pValue)  { this->_m[3][0] = pValue;  }

        float get_31()                          { return this->_m[3][1];    }
        void  set_31(_In_ const float& pValue)  { this->_m[3][1] = pValue;  }

        float get_32()                          { return this->_m[3][2];    }
        void  set_32(_In_ const float& pValue)  { this->_m[3][2] = pValue;  }

        float get_33()                          { return this->_m[3][3];    }
        void  set_33(_In_ const float& pValue)  { this->_m[3][3] = pValue;  }
        
    private:
        glm::mat4 _m = glm::mat4(1);
    };

    static boost::python::list scale(float pX, float pY, float pZ)
    {
			auto _scale = glm::scale(glm::vec3(pX, pY, pZ));

			boost::python::list _l;
			_l.append(_scale[0][0]); _l.append(_scale[0][1]); _l.append(_scale[0][2]); _l.append(_scale[0][3]);
			_l.append(_scale[1][0]); _l.append(_scale[1][1]); _l.append(_scale[1][2]); _l.append(_scale[1][3]);
			_l.append(_scale[2][0]); _l.append(_scale[2][1]); _l.append(_scale[2][2]); _l.append(_scale[2][3]);
			_l.append(_scale[3][0]); _l.append(_scale[3][1]); _l.append(_scale[3][2]); _l.append(_scale[3][3]);
			return _l;
    }

	static void py_glm_export()
	{
		using namespace boost::python;
		using namespace wolf::system;

        //export py_glm class
        class_<w_mat4>("w_mat4", init<>())
        .add_property("m00", &w_mat4::get_00, &w_mat4::set_00, "matrix element [0][0]")
        .add_property("m01", &w_mat4::get_01, &w_mat4::set_01, "matrix element [0][1]")
        .add_property("m02", &w_mat4::get_02, &w_mat4::set_02, "matrix element [0][2]")
        .add_property("m03", &w_mat4::get_03, &w_mat4::set_03, "matrix element [0][3]")
        .add_property("m10", &w_mat4::get_10, &w_mat4::set_10, "matrix element [1][0]")
        .add_property("m11", &w_mat4::get_11, &w_mat4::set_11, "matrix element [1][1]")
        .add_property("m12", &w_mat4::get_12, &w_mat4::set_12, "matrix element [1][2]")
        .add_property("m13", &w_mat4::get_13, &w_mat4::set_13, "matrix element [1][3]")
        .add_property("m20", &w_mat4::get_10, &w_mat4::set_10, "matrix element [2][0]")
        .add_property("m21", &w_mat4::get_11, &w_mat4::set_11, "matrix element [2][1]")
        .add_property("m22", &w_mat4::get_12, &w_mat4::set_12, "matrix element [2][2]")
        .add_property("m23", &w_mat4::get_13, &w_mat4::set_13, "matrix element [2][3]")
        .add_property("m30", &w_mat4::get_10, &w_mat4::set_10, "matrix element [3][0]")
        .add_property("m31", &w_mat4::get_11, &w_mat4::set_11, "matrix element [3][1]")
        .add_property("m32", &w_mat4::get_12, &w_mat4::set_12, "matrix element [3][2]")
        .add_property("m33", &w_mat4::get_13, &w_mat4::set_13, "matrix element [3][3]")
        ;
        
        def("scale", &pyWolf::scale, "perform scale");
	}
}

#endif//__PY_GLM_H__

#endif //__PYTHON__
