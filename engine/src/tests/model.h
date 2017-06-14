/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/WolfSource/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : model.h
	Description		 : The model
	Comment          :
*/

#ifndef __MODEL_H__
#define __MODEL_H__

#include <w_object.h>
#include <w_render_export.h>

class model : public wolf::system::w_object
{
public:
    W_EXP model();
    virtual ~model();

    void load();

    //Release will be called once per game and is the place to unload assets and release all resources
    ULONG release() override;

private:
    typedef wolf::system::w_object      _super;

};

#endif
