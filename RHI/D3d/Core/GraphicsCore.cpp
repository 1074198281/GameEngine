#include "GraphicsCore.h"
#include "D3dGraphicsCoreManager.h"

D3dGraphicsCore::CD3dGraphicsCore::CD3dGraphicsCore()
{

}

D3dGraphicsCore::CD3dGraphicsCore::~CD3dGraphicsCore()
{

}

void D3dGraphicsCore::CD3dGraphicsCore::setCoreHWND(HWND hwnd)
{
	g_hWnd = hwnd;
}

void D3dGraphicsCore::CD3dGraphicsCore::InitializeGraphics()
{
	Initialize(false);
}