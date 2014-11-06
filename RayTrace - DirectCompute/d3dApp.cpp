#include "d3dApp.h"
#include "Direct3D.h"

LRESULT CALLBACK
MainWndProc(HWND p_hwnd, UINT p_msg, WPARAM p_wParam, LPARAM p_lParam)
{
	static Direct3D* dd = 0;

	switch( p_msg )
	{
		case WM_CREATE:
		{
			// Get the 'this' pointer we passed to CreateWindow via the lpParam parameter.
			CREATESTRUCT* cs = (CREATESTRUCT*)p_lParam;
			dd = (Direct3D*)cs->lpCreateParams;
			return 0;
		}
	}
	// Don't start processing messages until after WM_CREATE.
	if( dd )
		return dd->msgProc(p_msg, p_wParam, p_lParam);
	else
		return DefWindowProc(p_hwnd, p_msg, p_wParam, p_lParam);
}

D3DApp::D3DApp(HINSTANCE p_hInstance)
{
	m_hAppInst   = p_hInstance;
	m_hMainWnd   = 0;
	m_AppPaused  = false;
	m_Minimized  = false;
	m_Maximized  = false;
	m_Resizing   = false;

	m_FrameStats = (LPCWSTR)"";
 
	m_pDevice          = 0;
	m_pSwapChain          = 0;
	//m_pDepthStencilBuffer = 0;
	//m_pRenderTargetView   = 0;
	//m_pDepthStencilView   = 0;

	m_MainWndTitle = (LPCWSTR)"RayTracing - Direct Compute";
	m_DriverType  = D3D_DRIVER_TYPE_HARDWARE;
	m_ClearColor[0] = 0.0f; m_ClearColor[1] = 0.0f; m_ClearColor[2] = 1.0f; m_ClearColor[3]  = 1.0f;
	m_ClientWidth    = 800;
	m_ClientHeight   = 800;

	m_4xMsaaQuality = 0;
	m_pDeviceContext = 0;

	m_winTitle = L"";
}

D3DApp::~D3DApp()
{
	//ReleaseCOM(m_pRenderTargetView);
	//ReleaseCOM(m_pDepthStencilView);
	ReleaseCOM(m_pSwapChain);
	//ReleaseCOM(m_pDepthStencilBuffer);
	ReleaseCOM(m_pDeviceContext);
	ReleaseCOM(m_pDevice);
}

HINSTANCE D3DApp::getAppInst()
{
	return m_hAppInst;
}

HWND D3DApp::getMainWnd()
{
	return m_hMainWnd;
}

int D3DApp::run()
{
	MSG msg = {0};
 
	m_Timer.reset();

	while(msg.message != WM_QUIT)
	{
		// If there are Window messages then process them.
		if(PeekMessage( &msg, 0, 0, 0, PM_REMOVE ))
		{
            TranslateMessage( &msg );
            DispatchMessage( &msg );
		}
		// Otherwise, do animation/game stuff.
		else
        {	
			m_Timer.tick();

			if( !m_AppPaused )
			{
				updateScene(m_Timer.getDeltaTime());
				drawScene();
			}
			else
				Sleep(100);
        }
    }
	return (int)msg.wParam;
}

void D3DApp::initApp()
{
	initMainWindow();
	initDirect3D();
}

void D3DApp::updateScene(float p_dt)
{
	// Code computes the average frames per second, and also the 
	// average time it takes to render one frame.

	static int frameCnt = 0;
	static float t_base = 0.0f;

	frameCnt++;

	// Compute averages over one second period.
	if( (m_Timer.getGameTime() - t_base) >= 1.0f )
	{
		float fps = (float)frameCnt; // fps = frameCnt / 1
		float mspf = 1000.0f / fps;

		std::wostringstream outs;   
		outs.precision(6);
		outs << (LPCTSTR)m_MainWndTitle.c_str() << L"    "
			<< L"FPS: " << fps << L"    " 
			<< L"Frame Time: " << mspf << L" (ms)";


		m_winTitle = outs.str();
		//SetWindowTextW(m_hMainWnd, m_winTitle.c_str());

		// Reset for next average.
		frameCnt = 0;
		t_base  += 1.0f;
	}
}

void D3DApp::drawScene()
{
	//m_pDeviceContext->ClearRenderTargetView(m_pRenderTargetView, m_ClearColor);
	//m_pDeviceContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0);
}

LRESULT D3DApp::msgProc(UINT p_msg, WPARAM wParam, LPARAM lParam)
{
	switch( p_msg )
	{
	// WM_ACTIVATE is sent when the window is activated or deactivated.  
	// We pause the game when the window is deactivated and unpause it 
	// when it becomes active.  
	case WM_ACTIVATE:
		if( LOWORD(wParam) == WA_INACTIVE )
		{
			m_AppPaused = true;
			m_Timer.stop();
		}
		else
		{
			m_AppPaused = false;
			m_Timer.start();
		}
		return 0;

	// WM_SIZE is sent when the user resizes the window.  
	case WM_SIZE:
		// Save the new client area dimensions.
		m_ClientWidth  = LOWORD(lParam);
		m_ClientHeight = HIWORD(lParam);
		if( m_pDevice )
		{
			if( wParam == SIZE_MINIMIZED )
			{
				m_AppPaused = true;
				m_Minimized = true;
				m_Maximized = false;
			}
			else if( wParam == SIZE_MAXIMIZED )
			{
				m_AppPaused = false;
				m_Minimized = false;
				m_Maximized = true;
				//onResize();
			}
			else if( wParam == SIZE_RESTORED )
			{
				
				// Restoring from minimized state?
				if( m_Minimized )
				{
					m_AppPaused = false;
					m_Minimized = false;
					//onResize();
				}

				// Restoring from maximized state?
				else if( m_Maximized )
				{
					m_AppPaused = false;
					m_Maximized = false;
					//onResize();
				}
				else if( m_Resizing )
				{
					// If user is dragging the resize bars, we do not resize 
					// the buffers here because as the user continuously 
					// drags the resize bars, a stream of WM_SIZE messages are
					// sent to the window, and it would be pointless (and slow)
					// to resize for each WM_SIZE message received from dragging
					// the resize bars.  So instead, we reset after the user is 
					// done resizing the window and releases the resize bars, which 
					// sends a WM_EXITSIZEMOVE message.
				}
				else // API call such as SetWindowPos or m_pSwapChain->SetFullscreenState.
				{
					//onResize();
				}
			}
		}
		return 0;

	// WM_EXITSIZEMOVE is sent when the user grabs the resize bars.
	case WM_ENTERSIZEMOVE:
		m_AppPaused = true;
		m_Resizing  = true;
		m_Timer.stop();
		return 0;

	// WM_EXITSIZEMOVE is sent when the user releases the resize bars.
	// Here we reset everything based on the new window dimensions.
	case WM_EXITSIZEMOVE:
		m_AppPaused = false;
		m_Resizing  = false;
		m_Timer.start();
		//onResize();
		return 0;
 
	// WM_DESTROY is sent when the window is being destroyed.
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	// The WM_MENUCHAR message is sent when a menu is active and the user presses 
	// a key that does not correspond to any mnemonic or accelerator key. 
	case WM_MENUCHAR:
        // Don't beep when we alt-enter.
        return MAKELRESULT(0, MNC_CLOSE);
 
	// Catch this message so to prevent the window from becoming too small.
	case WM_GETMINMAXINFO:
		((MINMAXINFO*)lParam)->ptMinTrackSize.x = 200;
		((MINMAXINFO*)lParam)->ptMinTrackSize.y = 200; 
		return 0;
	}

	return DefWindowProc(m_hMainWnd, p_msg, wParam, lParam);
}


void D3DApp::initMainWindow()
{
	WNDCLASS wc;
	wc.style         = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc   = MainWndProc; 
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = m_hAppInst;
	wc.hIcon         = LoadIcon(0, IDI_APPLICATION);
	wc.hCursor       = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
	wc.lpszMenuName  = 0;
	wc.lpszClassName = (LPCTSTR)"D3DWndClassName";

	if( !RegisterClass(&wc) )
	{
		MessageBox(0, (LPCTSTR)"RegisterClass FAILED", 0, 0);
		PostQuitMessage(0);
	}

	// Compute window rectangle dimensions based on requested client area dimensions.
	RECT R = { 0, 0, m_ClientWidth, m_ClientHeight };
    AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);
	int width  = R.right - R.left;
	int height = R.bottom - R.top;

	m_hMainWnd = CreateWindow((LPCSTR)"D3DWndClassName",(LPCTSTR)m_MainWndTitle.c_str(), 
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, width, height, 0, 0, m_hAppInst, this); 
	if( !m_hMainWnd )
	{
		MessageBox(0, (LPCTSTR)"CreateWindow FAILED", 0, 0);
		PostQuitMessage(0);
	}

	ShowWindow(m_hMainWnd, SW_SHOW);
	UpdateWindow(m_hMainWnd);
}

void D3DApp::initDirect3D()
{
	// Create the device.
	UINT createDeviceFlags = 0;

	// Fill out a DXGI_SWAP_CHAIN_DESC to describe our swap chain.
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory( &sd, sizeof(sd) );
	sd.BufferCount  = 1;
	sd.BufferDesc.Width  = m_ClientWidth;
	sd.BufferDesc.Height = m_ClientHeight;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage  = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_UNORDERED_ACCESS;
	sd.OutputWindow = m_hMainWnd;
	//sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	//sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	sd.SampleDesc.Count   = 1;
	sd.SampleDesc.Quality = 0;

	sd.Windowed     = true;

	D3D_FEATURE_LEVEL featureLevelsToTry[] = {
		D3D_FEATURE_LEVEL_11_0
	};
	D3D_FEATURE_LEVEL initiatedFeatureLevel;
	HRESULT hr = D3D11CreateDeviceAndSwapChain(
									NULL,                 // default adapter
									m_DriverType,
									NULL,                 // no software device
									createDeviceFlags, 
									featureLevelsToTry,
									ARRAYSIZE(featureLevelsToTry),  // default feature level array
									D3D11_SDK_VERSION,
									&sd,
									&m_pSwapChain,
									&m_pDevice,
									&initiatedFeatureLevel,
									&m_pDeviceContext);

	if( FAILED(hr) )
	{
		MessageBox(0, (LPCTSTR)"D3D11CreateDevice or Swapchain Failed.", 0, 0);
		PostQuitMessage(0);
	}

	if( featureLevelsToTry[0] != D3D_FEATURE_LEVEL_11_0 )
	{
		MessageBox(0, (LPCTSTR)"Direct3D Feature Level 11 unsupported.", 0, 0);
		PostQuitMessage(0);
	}

	// The remaining steps that need to be carried out for d3d creation
	// also need to be executed every time the window is resized.  So
	// just call the onResize method here to avoid code duplication.

	// ## Ray tracing ##
	m_pComputeWrap = new ComputeWrap(m_pDevice, m_pDeviceContext);

	std::wstring fileNameWString = L"RayCasting.fx";
	LPCWSTR* fileNameLPC;
	fileNameLPC = new LPCWSTR(((LPCWSTR)fileNameWString.c_str()));
	m_pRayCasting = m_pComputeWrap->createComputeShader(fileNameLPC, NULL, "main", NULL); 
	m_pRayCBuffer = m_pComputeWrap->createConstantBuffer(sizeof(RayCBufferStruct), NULL, 0);
	m_pRaySBuffer = m_pComputeWrap->createBuffer(STRUCTURED_BUFFER, sizeof(float) * 8, m_ClientHeight*m_ClientWidth, true, true, NULL, false, 0 ); // Rays origin and direction (7 floats)
	
	fileNameWString = L"Intersection.fx";
	fileNameLPC = new LPCWSTR(((LPCWSTR)fileNameWString.c_str()));
	m_pIntersection			= m_pComputeWrap->createComputeShader(fileNameLPC, NULL, "main", NULL);
	m_pIntersectionCBuffer	= m_pComputeWrap->createConstantBuffer(sizeof(IntersectionCBufferStruct), NULL, 0);
	m_pIntersectionSBuffer	= m_pComputeWrap->createBuffer(STRUCTURED_BUFFER, sizeof(Surface), m_ClientHeight*m_ClientWidth, true, true, NULL, false, 0 ); // Returns a point in world space with normal and color. (12 floats)

	fileNameWString = L"ColorStage.fx";
	fileNameLPC = new LPCWSTR(((LPCWSTR)fileNameWString.c_str()));
	m_pColorStage	= m_pComputeWrap->createComputeShader(fileNameLPC, NULL, "main", NULL);
	m_pColorCBuffer = m_pComputeWrap->createConstantBuffer(sizeof(ColorCBufferStruct), NULL, 0);
	m_pColorSBuffer = m_pComputeWrap->createBuffer(STRUCTURED_BUFFER, sizeof(float) * 4, m_ClientHeight*m_ClientWidth, true, true, NULL, false, 0 ); // Returns color ( 4 floats )

	ID3D11Texture2D* pBackBuffer;
	m_pSwapChain->GetBuffer(0, __uuidof( ID3D11Texture2D ) ,(LPVOID*)(&pBackBuffer));
	m_pDevice->CreateUnorderedAccessView( pBackBuffer, NULL, &m_pBackBufferUAV );
	m_pGPUTimer = new D3D11Timer(m_pDevice, m_pDeviceContext);
}