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
    struct w_vec2
    {
        float get_x()                          { return this->_v.x;        }
        void  set_x(_In_ const float& pValue)  { this->_v.x = pValue;      }
        
        float get_y()                          { return this->_v.y;        }
        void  set_y(_In_ const float& pValue)  { this->_v.y = pValue;      }
        
    private:
        glm::vec2 _v = glm::vec2();
    };
    
    struct w_vec3
    {
        float get_x()                          { return this->_v.x;        }
        void  set_x(_In_ const float& pValue)  { this->_v.x = pValue;      }
        
        float get_y()                          { return this->_v.y;        }
        void  set_y(_In_ const float& pValue)  { this->_v.y = pValue;      }
        
        float get_z()                          { return this->_v.z;        }
        void  set_z(_In_ const float& pValue)  { this->_v.z = pValue;      }
        
    private:
        glm::vec3 _v = glm::vec3();
    };
    
    struct w_vec4
    {
        float get_x()                          { return this->_v.x;        }
        void  set_x(_In_ const float& pValue)  { this->_v.x = pValue;      }
        
        float get_y()                          { return this->_v.y;        }
        void  set_y(_In_ const float& pValue)  { this->_v.y = pValue;      }
        
        float get_z()                          { return this->_v.z;        }
        void  set_z(_In_ const float& pValue)  { this->_v.z = pValue;      }
        
        float get_w()                          { return this->_v.w;        }
        void  set_w(_In_ const float& pValue)  { this->_v.w = pValue;      }
        
    private:
        glm::vec4 _v = glm::vec4();
    };
    
    struct w_mat4x4
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
        glm::mat4x4 _m = glm::mat4(1);
    };

    struct w_mat3x3
    {
        float get_00()                          { return this->_m[0][0];    }
        void  set_00(_In_ const float& pValue)  { this->_m[0][0] = pValue;  }
        
        float get_01()                          { return this->_m[0][1];    }
        void  set_01(_In_ const float& pValue)  { this->_m[0][1] = pValue;  }
        
        float get_02()                          { return this->_m[0][2];    }
        void  set_02(_In_ const float& pValue)  { this->_m[0][2] = pValue;  }
        
        float get_10()                          { return this->_m[1][0];    }
        void  set_10(_In_ const float& pValue)  { this->_m[1][0] = pValue;  }
        
        float get_11()                          { return this->_m[1][1];    }
        void  set_11(_In_ const float& pValue)  { this->_m[1][1] = pValue;  }
        
        float get_12()                          { return this->_m[1][2];    }
        void  set_12(_In_ const float& pValue)  { this->_m[1][2] = pValue;  }
        
        float get_20()                          { return this->_m[2][0];    }
        void  set_20(_In_ const float& pValue)  { this->_m[2][0] = pValue;  }
        
        float get_21()                          { return this->_m[2][1];    }
        void  set_21(_In_ const float& pValue)  { this->_m[2][1] = pValue;  }
        
        float get_22()                          { return this->_m[2][2];    }
        void  set_22(_In_ const float& pValue)  { this->_m[2][2] = pValue;  }
        
    private:
        glm::mat3x3 _m = glm::mat3x3(1);
    };
    
    static w_mat4x4 py_rotate(_In_ w_vec3 pValue)
    {
        auto _rotate = glm::rotate(glm::vec3(pValue.get_x(), pValue.get_y(), pValue.get_z()));
        
        w_mat4x4 _m;
        _m.set_00(_rotate[0][0]);
        _m.set_01(_rotate[0][1]);
        _m.set_02(_rotate[0][2]);
        _m.set_03(_rotate[0][3]);
        
        _m.set_10(_rotate[1][0]);
        _m.set_11(_rotate[1][1]);
        _m.set_12(_rotate[1][2]);
        _m.set_13(_rotate[1][3]);
        
        _m.set_20(_rotate[2][0]);
        _m.set_21(_rotate[2][1]);
        _m.set_22(_rotate[2][2]);
        _m.set_23(_rotate[2][3]);
        
        _m.set_30(_rotate[3][0]);
        _m.set_31(_rotate[3][1]);
        _m.set_32(_rotate[3][2]);
        _m.set_33(_rotate[3][3]);
        
        return _m;
    }

    static w_mat4x4 py_scale(_In_ w_vec3 pValue)
    {
        auto _scale = glm::scale(glm::vec3(pValue.get_x(), pValue.get_y(), pValue.get_z()));
        
        w_mat4x4 _m;
        _m.set_00(_scale[0][0]);
        _m.set_01(_scale[0][1]);
        _m.set_02(_scale[0][2]);
        _m.set_03(_scale[0][3]);
        
        _m.set_10(_scale[1][0]);
        _m.set_11(_scale[1][1]);
        _m.set_12(_scale[1][2]);
        _m.set_13(_scale[1][3]);
        
        _m.set_20(_scale[2][0]);
        _m.set_21(_scale[2][1]);
        _m.set_22(_scale[2][2]);
        _m.set_23(_scale[2][3]);
        
        _m.set_30(_scale[3][0]);
        _m.set_31(_scale[3][1]);
        _m.set_32(_scale[3][2]);
        _m.set_33(_scale[3][3]);
        
        return _m;
    }
    
    static w_mat4x4 py_lookAtRH(_In_ w_vec3 pEye, _In_ w_vec3 pLookAt, _In_ w_vec3 pUp)
    {
        auto _eye = glm::vec3(pEye.get_x(), pEye.get_y(), pEye.get_z());
        auto _look_at = glm::vec3(pLookAt.get_x(), pLookAt.get_y(), pLookAt.get_z());
        auto _up = glm::vec3(pUp.get_x(), pUp.get_y(), pUp.get_z());
        
        auto _mat = glm::lookAtRH(_eye, _look_at, _up);
        
        w_mat4x4 _m;
        _m.set_00(_mat[0][0]);
        _m.set_01(_mat[0][1]);
        _m.set_02(_mat[0][2]);
        _m.set_03(_mat[0][3]);
        
        _m.set_10(_mat[1][0]);
        _m.set_11(_mat[1][1]);
        _m.set_12(_mat[1][2]);
        _m.set_13(_mat[1][3]);
        
        _m.set_20(_mat[2][0]);
        _m.set_21(_mat[2][1]);
        _m.set_22(_mat[2][2]);
        _m.set_23(_mat[2][3]);
        
        _m.set_30(_mat[3][0]);
        _m.set_31(_mat[3][1]);
        _m.set_32(_mat[3][2]);
        _m.set_33(_mat[3][3]);
        
        return _m;
    }
    
    static w_mat4x4 py_lookAtLH(_In_ w_vec3 pEye, _In_ w_vec3 pLookAt, _In_ w_vec3 pUp)
    {
        auto _eye = glm::vec3(pEye.get_x(), pEye.get_y(), pEye.get_z());
        auto _look_at = glm::vec3(pLookAt.get_x(), pLookAt.get_y(), pLookAt.get_z());
        auto _up = glm::vec3(pUp.get_x(), pUp.get_y(), pUp.get_z());
        
        auto _mat = glm::lookAtLH(_eye, _look_at, _up);
        
        w_mat4x4 _m;
        _m.set_00(_mat[0][0]);
        _m.set_01(_mat[0][1]);
        _m.set_02(_mat[0][2]);
        _m.set_03(_mat[0][3]);
        
        _m.set_10(_mat[1][0]);
        _m.set_11(_mat[1][1]);
        _m.set_12(_mat[1][2]);
        _m.set_13(_mat[1][3]);
        
        _m.set_20(_mat[2][0]);
        _m.set_21(_mat[2][1]);
        _m.set_22(_mat[2][2]);
        _m.set_23(_mat[2][3]);
        
        _m.set_30(_mat[3][0]);
        _m.set_31(_mat[3][1]);
        _m.set_32(_mat[3][2]);
        _m.set_33(_mat[3][3]);
        
        return _m;
    }
    
    static w_mat4x4 py_perspectiveRH(_In_ float pFOV, _In_ float pAspectRatio, _In_ float pzNear, _In_ float pzFar)
    {
        auto _mat = glm::perspectiveRH(pFOV, pAspectRatio, pzNear, pzFar);
        
        w_mat4x4 _m;
        _m.set_00(_mat[0][0]);
        _m.set_01(_mat[0][1]);
        _m.set_02(_mat[0][2]);
        _m.set_03(_mat[0][3]);
        
        _m.set_10(_mat[1][0]);
        _m.set_11(_mat[1][1]);
        _m.set_12(_mat[1][2]);
        _m.set_13(_mat[1][3]);
        
        _m.set_20(_mat[2][0]);
        _m.set_21(_mat[2][1]);
        _m.set_22(_mat[2][2]);
        _m.set_23(_mat[2][3]);
        
        _m.set_30(_mat[3][0]);
        _m.set_31(_mat[3][1]);
        _m.set_32(_mat[3][2]);
        _m.set_33(_mat[3][3]);
        
        return _m;
    }
    
    static w_mat4x4 py_perspectiveLH(_In_ float pFOV, _In_ float pAspectRatio, _In_ float pzNear, _In_ float pzFar)
    {
        auto _mat = glm::perspectiveLH(pFOV, pAspectRatio, pzNear, pzFar);
        
        w_mat4x4 _m;
        _m.set_00(_mat[0][0]);
        _m.set_01(_mat[0][1]);
        _m.set_02(_mat[0][2]);
        _m.set_03(_mat[0][3]);
        
        _m.set_10(_mat[1][0]);
        _m.set_11(_mat[1][1]);
        _m.set_12(_mat[1][2]);
        _m.set_13(_mat[1][3]);
        
        _m.set_20(_mat[2][0]);
        _m.set_21(_mat[2][1]);
        _m.set_22(_mat[2][2]);
        _m.set_23(_mat[2][3]);
        
        _m.set_30(_mat[3][0]);
        _m.set_31(_mat[3][1]);
        _m.set_32(_mat[3][2]);
        _m.set_33(_mat[3][3]);
        
        return _m;
    }
    
    static w_vec2 py_multiply_vec2(_In_ w_vec2 pV1, _In_ w_vec2 pV2)
    {
        glm::vec2 _v1 = glm::vec2(pV1.get_x(), pV1.get_y());
        glm::vec2 _v2 = glm::vec2(pV2.get_x(), pV2.get_y());
        glm::vec2 _r = _v1 * _v2;
        w_vec2 _m;
        _m.set_x(_r.x);
        _m.set_y(_r.y);
        return _m;
    }
    
    static w_vec3 py_multiply_vec3(_In_ w_vec3 pV1, _In_ w_vec3 pV2)
    {
        glm::vec3 _v1 = glm::vec3(pV1.get_x(), pV1.get_y(), pV1.get_z());
        glm::vec3 _v2 = glm::vec3(pV2.get_x(), pV2.get_y(), pV2.get_z());
        glm::vec3 _r = _v1 * _v2;
        w_vec3 _m;
        _m.set_x(_r.x);
        _m.set_y(_r.y);
        _m.set_z(_r.z);
        return _m;
    }
    
    static w_vec4 py_multiply_vec4(_In_ w_vec4 pV1, _In_ w_vec4 pV2)
    {
        glm::vec4 _v1 = glm::vec4(pV1.get_x(), pV1.get_y(), pV1.get_z(), pV1.get_w());
        glm::vec4 _v2 = glm::vec4(pV2.get_x(), pV2.get_y(), pV2.get_z(), pV2.get_w());
        glm::vec4 _r = _v1 * _v2;
        
        w_vec4 _m;
        _m.set_x(_r.x);
        _m.set_y(_r.y);
        _m.set_z(_r.z);
        _m.set_w(_r.w);
        return _m;
    }
    
    static w_mat3x3 py_multiply_mat3x3(_In_ w_mat3x3 pM1, _In_ w_mat3x3 pM2)
    {
        glm::mat3x3 _m1 = glm::mat3x3(pM1.get_00(), pM1.get_01(), pM1.get_02(),
                                      pM1.get_10(), pM1.get_11(), pM1.get_12(),
                                      pM1.get_20(), pM1.get_21(), pM1.get_22());
        
        glm::mat3x3 _m2 = glm::mat3x3(pM2.get_00(), pM2.get_01(), pM2.get_02(),
                                      pM2.get_10(), pM2.get_11(), pM2.get_12(),
                                      pM2.get_20(), pM2.get_21(), pM2.get_22());
        
        glm::mat3x3 _r = _m1 * _m2;
        
        w_mat3x3 _m;
        _m.set_00(_r[0][0]);
        _m.set_01(_r[0][1]);
        _m.set_02(_r[0][2]);
        
        _m.set_10(_r[1][0]);
        _m.set_11(_r[1][1]);
        _m.set_12(_r[1][2]);
        
        _m.set_20(_r[2][0]);
        _m.set_21(_r[2][1]);
        _m.set_22(_r[2][2]);
        
        return _m;
    }
    
    static w_mat4x4 py_multiply_mat4x4(_In_ w_mat4x4 pM1, _In_ w_mat4x4 pM2)
    {
        glm::mat4x4 _m1 = glm::mat4x4(pM1.get_00(), pM1.get_01(), pM1.get_02(), pM1.get_03(),
                                      pM1.get_10(), pM1.get_11(), pM1.get_12(), pM1.get_13(),
                                      pM1.get_20(), pM1.get_21(), pM1.get_22(), pM1.get_23(),
                                      pM1.get_30(), pM1.get_31(), pM1.get_32(), pM1.get_33());
        
        glm::mat4x4 _m2 = glm::mat4x4(pM2.get_00(), pM2.get_01(), pM2.get_02(), pM2.get_03(),
                                      pM2.get_10(), pM2.get_11(), pM2.get_12(), pM2.get_13(),
                                      pM2.get_20(), pM2.get_21(), pM2.get_22(), pM2.get_23(),
                                      pM2.get_30(), pM2.get_31(), pM2.get_32(), pM2.get_33());
        
        glm::mat4x4 _r = _m1 * _m2;
        
        w_mat4x4 _m;
        _m.set_00(_r[0][0]);
        _m.set_01(_r[0][1]);
        _m.set_02(_r[0][2]);
        _m.set_03(_r[0][3]);
        
        _m.set_10(_r[1][0]);
        _m.set_11(_r[1][1]);
        _m.set_12(_r[1][2]);
        _m.set_13(_r[1][3]);
        
        _m.set_20(_r[2][0]);
        _m.set_21(_r[2][1]);
        _m.set_22(_r[2][2]);
        _m.set_23(_r[2][3]);
        
        _m.set_30(_r[3][0]);
        _m.set_31(_r[3][1]);
        _m.set_32(_r[3][2]);
        _m.set_33(_r[3][3]);
        
        return _m;
    }
    
	static void py_glm_export()
	{
		using namespace boost::python;
		using namespace wolf::system;

        //export w_vec2 struct
        class_<w_vec2>("vec2", init<>())
        .add_property("x", &w_vec2::get_x, &w_vec2::set_x, "X")
        .add_property("y", &w_vec2::get_y, &w_vec2::set_y, "Y")
        ;
        
        //export w_vec3 struct
        class_<w_vec3>("vec3", init<>())
        .add_property("x", &w_vec3::get_x, &w_vec3::set_x, "X")
        .add_property("y", &w_vec3::get_y, &w_vec3::set_y, "Y")
        .add_property("z", &w_vec3::get_z, &w_vec3::set_z, "Z")
        ;
        
        //export w_vec4 struct
        class_<w_vec4>("vec4", init<>())
        .add_property("x", &w_vec4::get_x, &w_vec4::set_x, "X")
        .add_property("y", &w_vec4::get_y, &w_vec4::set_y, "Y")
        .add_property("z", &w_vec4::get_z, &w_vec4::set_z, "Z")
        .add_property("w", &w_vec4::get_w, &w_vec4::set_w, "W")
        ;
        
        //export w_mat3x3 struct
        class_<w_mat3x3>("mat3x3", init<>())
        .add_property("m00", &w_mat3x3::get_00, &w_mat3x3::set_00, "matrix element [0][0]")
        .add_property("m01", &w_mat3x3::get_01, &w_mat3x3::set_01, "matrix element [0][1]")
        .add_property("m02", &w_mat3x3::get_02, &w_mat3x3::set_02, "matrix element [0][2]")
        .add_property("m10", &w_mat3x3::get_10, &w_mat3x3::set_10, "matrix element [1][0]")
        .add_property("m11", &w_mat3x3::get_11, &w_mat3x3::set_11, "matrix element [1][1]")
        .add_property("m12", &w_mat3x3::get_12, &w_mat3x3::set_12, "matrix element [1][2]")
        .add_property("m20", &w_mat3x3::get_20, &w_mat3x3::set_20, "matrix element [2][0]")
        .add_property("m21", &w_mat3x3::get_21, &w_mat3x3::set_21, "matrix element [2][1]")
        .add_property("m22", &w_mat3x3::get_22, &w_mat3x3::set_22, "matrix element [2][2]")
        ;
        
        //export w_mat4 struct
        class_<w_mat4x4>("mat4x4", init<>())
        .add_property("m00", &w_mat4x4::get_00, &w_mat4x4::set_00, "matrix element [0][0]")
        .add_property("m01", &w_mat4x4::get_01, &w_mat4x4::set_01, "matrix element [0][1]")
        .add_property("m02", &w_mat4x4::get_02, &w_mat4x4::set_02, "matrix element [0][2]")
        .add_property("m03", &w_mat4x4::get_03, &w_mat4x4::set_03, "matrix element [0][3]")
        .add_property("m10", &w_mat4x4::get_10, &w_mat4x4::set_10, "matrix element [1][0]")
        .add_property("m11", &w_mat4x4::get_11, &w_mat4x4::set_11, "matrix element [1][1]")
        .add_property("m12", &w_mat4x4::get_12, &w_mat4x4::set_12, "matrix element [1][2]")
        .add_property("m13", &w_mat4x4::get_13, &w_mat4x4::set_13, "matrix element [1][3]")
        .add_property("m20", &w_mat4x4::get_20, &w_mat4x4::set_20, "matrix element [2][0]")
        .add_property("m21", &w_mat4x4::get_21, &w_mat4x4::set_21, "matrix element [2][1]")
        .add_property("m22", &w_mat4x4::get_22, &w_mat4x4::set_22, "matrix element [2][2]")
        .add_property("m23", &w_mat4x4::get_23, &w_mat4x4::set_23, "matrix element [2][3]")
        .add_property("m30", &w_mat4x4::get_30, &w_mat4x4::set_30, "matrix element [3][0]")
        .add_property("m31", &w_mat4x4::get_31, &w_mat4x4::set_31, "matrix element [3][1]")
        .add_property("m32", &w_mat4x4::get_32, &w_mat4x4::set_32, "matrix element [3][2]")
        .add_property("m33", &w_mat4x4::get_33, &w_mat4x4::set_33, "matrix element [3][3]")
        ;
        
        def("rotate", &pyWolf::py_rotate, "perform rotation");
        def("scale", &pyWolf::py_scale, "perform scale");
        def("lookAtRH", &pyWolf::py_lookAtRH, "perform lookAtRH");
        def("lookAtLH", &pyWolf::py_lookAtLH, "perform lookAtLH");
        def("perspectiveRH", &pyWolf::py_perspectiveRH, "perform perspectiveRH");
        def("perspectiveLH", &pyWolf::py_perspectiveLH, "perform perspectiveLH");
        def("multiplyVectors", &pyWolf::py_multiply_vec2, "perform multiply two 2D vectors");
        def("multiplyVectors", &pyWolf::py_multiply_vec3, "perform multiply two 3D vectors");
        def("multiplyVectors", &pyWolf::py_multiply_vec4, "perform multiply two 4D vectors");
        def("multiplyMatrices", &pyWolf::py_multiply_mat3x3, "perform multiply two 3x3 matrices");
        def("multiplyMatrices", &pyWolf::py_multiply_mat4x4, "perform multiply two 4x4 matrices");
	}
}

#endif//__PY_GLM_H__

#endif //__PYTHON__
