/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_geometry_sink_callback_data.h
	Description		 : Responsible for drawing direct2D rectangle
	Comment          :
*/

#ifndef __W_GEOMETRY_SINK_CALLBACK_DATA_H__
#define __W_GEOMETRY_SINK_CALLBACK_DATA_H__

#include "w_graphics_device_manager.h"

namespace wolf
{
	namespace graphics
	{
		namespace direct2D
		{
			class w_geometry_sink_callback_data : public ID2D1SimplifiedGeometrySink
			{
			public:
				w_geometry_sink_callback_data()
					: _ref(1)
				{
				}

				STDMETHOD_(ULONG, AddRef)(THIS)
				{
					return InterlockedIncrement(reinterpret_cast<LONG volatile *>(&this->_ref));
				}
				
				STDMETHOD_(ULONG, Release)(THIS)
				{
					ULONG _c_ref = static_cast<ULONG>(InterlockedDecrement(reinterpret_cast<LONG volatile *>(&this->_ref)));

					if (0 == _c_ref)
					{
						delete this;
					}

					return _c_ref;
				}

				STDMETHOD(QueryInterface)(THIS_ REFIID pIID, void** pPVObject)
				{
					HRESULT hr = S_OK;

					if (__uuidof(IUnknown) == pIID)
					{
						*pPVObject = static_cast<IUnknown*>(this);
						AddRef();
					}
					else if (__uuidof(ID2D1SimplifiedGeometrySink) == pIID)
					{
						*pPVObject = static_cast<ID2D1SimplifiedGeometrySink*>(this);
						AddRef();
					}
					else
					{
						*pPVObject = NULL;
						hr = E_NOINTERFACE;
					}

					return hr;
				}

				STDMETHOD_(void, AddBeziers)(const D2D1_BEZIER_SEGMENT* pBeziers, UINT pBeziersCount)
				{
					//logger.write("Hello");
				}

				STDMETHOD_(void, AddLines)(const D2D1_POINT_2F* pPoints, UINT pPointsCount)
				{
					// Customize this method to meet your specific data needs.
					//logger.write("Retrieving geometry data from a derived ID2D1SimplifiedGeometrySink object:");
					//for (UINT i = 0; i < pPointsCount; ++i)
					//{
					//	logger.write(std::to_string(pPoints[i].x) + "," + std::to_string(pPoints[i].y));
					//}
				}

				STDMETHOD_(void, BeginFigure)(D2D1_POINT_2F startPoint, D2D1_FIGURE_BEGIN figureBegin)
				{
				}

				STDMETHOD_(void, EndFigure)(D2D1_FIGURE_END figureEnd)
				{
				}

				STDMETHOD_(void, SetFillMode)(D2D1_FILL_MODE fillMode)
				{
				}

				STDMETHOD_(void, SetSegmentFlags)(D2D1_PATH_SEGMENT vertexFlags)
				{
				}

				STDMETHOD(Close)()
				{
					return S_OK;
				}

			private:
				UINT													_ref;
			};
		}
	}
}

#endif