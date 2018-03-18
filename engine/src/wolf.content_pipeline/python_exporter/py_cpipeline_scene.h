/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : py_cpipeline_scene.h
	Description		 : The python exporter for w_cpipeline_scene classe
	Comment          :
*/

#ifdef __PYTHON__

#ifndef __PY_CPIPELINE_SCENE_H__
#define __PY_CPIPELINE_SCENE_H__

namespace pyWolf
{
    static void py_cpipeline_scene_export()
    {
        using namespace boost::python;
        using namespace wolf::system;
        
        //export w_cpipeline_scene class
        //class_<w_cpipeline_scene>("w_cpipeline_scene", init<>())
            //;
    }
}

#endif//__PY_CPIPELINE_SCENE_H__

#endif//__PYTHON__
