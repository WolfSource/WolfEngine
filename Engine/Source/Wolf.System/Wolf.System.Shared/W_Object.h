/*
	Project			 : Wolf Engine (http://WolfStudio.co). Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : https://github.com/PooyaEimandar/WolfEngine - Please direct any bug to hello@WolfStudio.co or tweet @PooyaEimandar on twitter
	Name			 : W_Object.h
	Description		 : Base object class of Wolf clases
	Comment          :
*/

#pragma once

#ifdef __WOLF_DLL__
#define API __declspec(dllexport)
#else
#define API __declspec(dllimport)
#endif


#include <string>
#include "W_IReleasable.h"

namespace Wolf
{
	namespace System
	{
		class W_Object : public W_IReleasable
		{
		public:

			W_Object() : isReleased(false), loaded(false) {}
			virtual ~W_Object() { Release(); }

			virtual bool IsReleased() { return this->isReleased; }
			virtual ULONG Release() { return InternalRelease(); }

			void* Tag;
			std::string Name;

		protected:
			bool loaded;

		private:
			//Prevent copying
			W_Object(W_Object const&);
			W_Object& operator= (W_Object const&);

			ULONG InternalRelease()
			{
				if (this->isReleased) return 0;
				this->isReleased = true;
				this->Tag = nullptr;

#ifdef _DEBUG
				auto msg = L"\t\t[WOLF DEBUG] : " + std::wstring(this->Name.begin(), this->Name.end()) + L" Released\r\n";
				OutputDebugStringW(msg.c_str());
				msg.clear();
#endif
				return 1;
			};

			bool isReleased;
		};
	}
}