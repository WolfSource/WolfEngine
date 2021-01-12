/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (https://PooyaEimandar.github.io) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/WolfEngine/Wolf.Engine/issues
	Website			 : https://WolfEngine.App
	Name			 : w_point.h
	Description		 : Global color structure
	Comment          :
*/

#pragma once

typedef struct w_point
{
    long x;
    long y;
} w_point;

typedef struct w_point_f
{
    float x;
    float y;
} w_point_f;

typedef struct w_point_t
{
    unsigned int x;
    unsigned int y;
} w_point_t;
