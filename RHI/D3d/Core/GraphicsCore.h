#pragma once

#include "Common/D3dCommonDef.h"


namespace D3dGraphicsCore {
	
	class CD3dGraphicsCore {
	public:
		CD3dGraphicsCore();
		~CD3dGraphicsCore();

	public:
		void setCoreHWND(HWND hwnd);
		void InitializeGraphics();
	};

}