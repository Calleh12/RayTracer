#pragma once

#include <d3dcommon.h>
#include <d3d11.h>
#include <d3dcompiler.h>

#include <tchar.h>

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p)      { if (p) { (p)->Release(); (p)=NULL; } }
#endif

#ifndef SAFE_DELETE
#define SAFE_DELETE(p)      { if (p) { delete (p); (p)=NULL; } }
#endif