#include "Direct3D.h"
#include "TXLoader.h"

int WINAPI WinMain(HINSTANCE p_hInstance, HINSTANCE p_prevInstance,
				   PSTR p_cmdLine, int p_showCmd)
{
	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

	Direct3D theApp(p_hInstance);
	
	theApp.initApp();

	return theApp.run();
}

Direct3D::Direct3D(HINSTANCE p_hInstance)
: D3DApp(p_hInstance) {}

Direct3D::~Direct3D()
{
	ReleaseCOM(m_pDevice);
	ReleaseCOM(m_pDeviceContext);
	ReleaseCOM(m_pSwapChain);
}

void Direct3D::initApp()
{
	m_dispatchSize = 50;

	D3DApp::initApp();
	HRESULT hr = S_OK;
	m_buffer = Buffer();
	m_shader = Shader();

	m_pCamera = new Camera();

	m_pHID = new HID(m_hMainWnd);
	m_pObserver = new Observer(this);
	m_pHID->getObservable()->addSubscriber(m_pObserver);
	m_speed = 10.0f;

	// ## Ray tracing ##


	//Set up world view proj
	m_camPosition = XMFLOAT4(0.0f, 201.0f, -750.f, 0.0f );
	m_camTarget = XMFLOAT4( 0.0f, 0.0f, 0.0f, 0.0f );
	m_camUp = XMFLOAT4( 0.0f, 1.0f, 0.0f, 0.0f );

	float test = 3.14f * 0.25f;
	m_pCamera->perspective(test, (float)m_ClientWidth/m_ClientHeight, 1.0f, 300.0f);
	m_pCamera->view(m_camPosition, m_camTarget, m_camUp);

	//ModelLoader pModelLoader = ModelLoader(m_pDevice, m_pDeviceContext);
	TXLoader pModelLoader = TXLoader(m_pDevice, m_pDeviceContext);

	//optimus = pModelLoader.loadModelFromOBJ("cube50.obj", 100.0f);
	//optimus = pModelLoader.loadModelFromOBJ("WEP-OptimusSword.obj", 100.0f);
	optimus = pModelLoader.loadModelFromTX("Barrel1.tx", 100.0f);
	optimus.m_triangles.at(0).trig.corners[2].w = optimus.m_triangles.size();
	//Triangle* tlist = new Triangle[optimus.m_triangles.size()];

	//for(int i = 0; i < optimus.m_triangles.at(0).trig.corners[2].w; i++)
	//{
	//	optimus.m_triangles.at(i).trig.corners[0].x *= 0.f; 
	//	optimus.m_triangles.at(i).trig.corners[1].x *= 0.f;
	//	optimus.m_triangles.at(i).trig.corners[2].x *= 0.f;
	//}

	m_pOptimusPrimeSBuffer =  m_pComputeWrap->createBuffer(STRUCTURED_BUFFER, sizeof(TriangleMesh), optimus.m_triangles.size(), true, false, &optimus.m_triangles[0], false, 0 );
	//m_pDevice->
	//m_pIntersectionSBuffer	= m_pComputeWrap->createBuffer(STRUCTURED_BUFFER, sizeof(Surface), 800*800, true, true, NULL, false, 0 ); // Returns a point in world space with normal and color. (12 floats)

	//m_pOptimusPrimeSBuffer->

	XMFLOAT4 corners[3] = 
	{
		XMFLOAT4(50.0f, -25.0f, 0.0f, 1.0f),
		XMFLOAT4(-50.0f, -25.0f, 0.0f, 1.0f),
		XMFLOAT4(0.0, 75.0f, 0.0f, 1.0f)
	};
	m_intersectionStruct.triangle = Triangle(corners, 1.0f);
	m_intersectionStruct.sphere = Sphere(XMFLOAT4(0.0f, 220.0f, 50.0f, 0.1f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 20.0f, 0.0f);

	m_intersectionStruct.box = Box(XMFLOAT4(-300.0f, -300.0f, -300.0f, 1.0f), XMFLOAT4(300.0f, 300.0f, 300.0f, 1.0f), 1.0f, 1.0f);
	m_intersectionStruct.mtr = optimus.m_material.getMaterial();
	
	m_colorStruct.lights[0] = SpotLight(XMFLOAT3(0.f, 0.f, 0.f), 0.f, XMFLOAT3(-150.0f, 150.0f, 0.0f), 2000.0f, XMFLOAT4(.2f, 0.01f, .0f, 0.0f),XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f),	XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f));
	m_colorStruct.lights[1] = SpotLight(XMFLOAT3(0.f, 0.f, 0.f), 0.f, XMFLOAT3(0.0f, 150.0f, 0.0f), 2000.0f,	XMFLOAT4(.2f, 0.01f, .0f, 0.0f),	XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f),	XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f));
	m_colorStruct.lights[2] = SpotLight(XMFLOAT3(0.f, 0.f, 0.f), 0.f, XMFLOAT3(150.0f, 150.0f, 0.0f), 2000.0f,  XMFLOAT4(.2f, 0.01f, .0f, 0.0f), XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f),	XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f));
	m_colorStruct.lights[3] = SpotLight(XMFLOAT3(0.f, 0.f, 0.f), 0.f, XMFLOAT3(-150.0f, 0.0f, 0.0f), 2000.0f,	XMFLOAT4(.2f, 0.01f, .0f, 0.0f),	XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f),	XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f));
	m_colorStruct.lights[4] = SpotLight(XMFLOAT3(0.f, 0.f, 0.f), 0.f, XMFLOAT3(0.0f, 100.0f, 0.0f), 2000.0f,	XMFLOAT4(.2f, 0.01f, .0f, 0.0f),	XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f),	XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f));
	m_colorStruct.lights[5] = SpotLight(XMFLOAT3(0.f, 0.f, 0.f), 0.f, XMFLOAT3(150.0f, 0.0f, 0.0f), 2000.0f,	XMFLOAT4(.2f, 0.01f, .0f, 0.0f),	XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f),	XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f));
	m_colorStruct.lights[6] = SpotLight(XMFLOAT3(0.f, 0.f, 0.f), 0.f, XMFLOAT3(-150.0f, -150.0f, 0.0f), 2000.0f,XMFLOAT4(.2f, 0.01f, .0f, 0.0f),XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f),	XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f));
	m_colorStruct.lights[7] = SpotLight(XMFLOAT3(0.f, 0.f, 0.f), 0.f, XMFLOAT3(0.0f, -150.0f, 0.0f), 2000.0f,	XMFLOAT4(.2f, 0.01f, .0f, 0.0f),	 XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f),	XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f));
	m_colorStruct.lights[8] = SpotLight(XMFLOAT3(0.f, 0.f, 0.f), 0.f, XMFLOAT3(150.0f, -150.0f, 0.0f), 2000.0f, XMFLOAT4(.2f, 0.01f, .0f, 0.0f), XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f),	XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f));
	m_colorStruct.lights[9] = SpotLight(XMFLOAT3(0.f, 0.f, 0.f), 0.f, XMFLOAT3(0.0f, 100.0f, 150.0f), 2000.0f,  XMFLOAT4(.2f, 0.01f, .0f, 0.0f), XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f),	XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f));
	
	m_colorStruct.lights[10] = SpotLight(XMFLOAT3(1.f, 1.f, 0.f), 0.99f, XMFLOAT3(0.0f, 0.0f, 150.0f), 2000.0f, XMFLOAT4(0.6f, 0.f, 0.000002f, 0.0f),	XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f),	XMFLOAT4(0.9f, 0.9f, 0.9f, 1.0f));
	m_colorStruct.lights[11] = SpotLight(XMFLOAT3(-1.f, 1.f, 0.f), 0.99f, XMFLOAT3(150.0f, 0.0f, 0.0f), 2000.0f, XMFLOAT4(0.6f, 0.f, 0.000002f, 0.0f),	XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f),	XMFLOAT4(0.9f, 0.9f, 0.9f, 1.0f));
	m_colorStruct.lights[12] = SpotLight(XMFLOAT3(0.f, 0.f, 1.f), 0.99f, XMFLOAT3(0.0f, 0.0f, 100.0f), 2000.0f, XMFLOAT4(0.6f, 0.f, 0.000002f, 0.0f),	 XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f),	XMFLOAT4(0.9f, 0.9f, 0.9f, 1.0f));
	m_colorStruct.lights[13] = SpotLight(XMFLOAT3(0.f, -0.f, 1.f), 0.99f, XMFLOAT3(0.0f, 0.0f, -300.0f), 2000.0f,XMFLOAT4(0.6f, 0.f, 0.000002f, 0.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f),XMFLOAT4(0.9f, 0.9f, 0.9f, 1.0f));
	m_colorStruct.lights[14] = SpotLight(XMFLOAT3(0.f, -0.f, 1.f), 0.99f, XMFLOAT3(-0.0f, 0.0f, -1000.0f), 2000.0f, XMFLOAT4(0.6f, 0.f, 0.000002f, 0.0f), XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f),	XMFLOAT4(0.9f, 0.9f, 0.9f, 1.0f));

	m_movingAngle = 0.005f;
	XMStoreFloat4x4(&m_rotation, XMMatrixRotationRollPitchYaw(m_movingAngle, -m_movingAngle, m_movingAngle));



	//D3D11_SAMPLER_DESC sd;
	//ZeroMemory(&sd, sizeof(sd));
	//sd.Filter                                = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	//sd.AddressU                              = D3D11_TEXTURE_ADDRESS_WRAP;
	//sd.AddressV                              = D3D11_TEXTURE_ADDRESS_WRAP;
	//sd.AddressW                              = D3D11_TEXTURE_ADDRESS_WRAP;
	//sd.ComparisonFunc						 = D3D11_COMPARISON_NEVER;
	//sd.MinLOD                                = 0;
	//sd.MaxLOD                                = D3D11_FLOAT32_MAX;

	//m_pTextureSampler = nullptr;
	//hr = m_pDevice->CreateSamplerState( &sd, &m_pTextureSampler );
}

void Direct3D::updateScene(float p_dt)
{
	D3DApp::updateScene(p_dt);

	std::wostringstream outs;   
	outs.precision(6);
	outs << L" "
		<< L" " ;

	SetWindowTextW(m_hMainWnd,outs.str().c_str());

	m_winTitle = outs.str();

	for(int i = 0; i < 15; i++)
	{
		XMVECTOR temp = XMVectorSet(m_colorStruct.lights[i].position.x, m_colorStruct.lights[i].position.y, m_colorStruct.lights[i].position.z, 1.0f); 
		temp = XMVector4Transform(temp, XMLoadFloat4x4(&m_rotation));
		m_colorStruct.lights[i].position = XMFLOAT4(temp.m128_f32[0], temp.m128_f32[1], temp.m128_f32[2], m_colorStruct.lights[i].position.w);
	}


}

void Direct3D::drawScene()
{
	ID3D11UnorderedAccessView*	m_uavNULL[] = {0, 0, 0, 0, 0 };
	ID3D11ShaderResourceView*	m_srvNULL[] = {0, 0, 0, 0, 0 };

	rays = 0.0f;
	intersection = 0.0f;
	color = 0.0f;

	D3DApp::drawScene();

	////////////////////////////////////
	// RAY STAGE
	////////////////////////////////////
	ID3D11UnorderedAccessView* pUAV[] = { m_pRaySBuffer->getUnorderedAccessView() };
	m_pDeviceContext->CSSetUnorderedAccessViews(0, 1, pUAV, NULL);
	
	D3D11_MAPPED_SUBRESOURCE mappedResource; 
	RayCBufferStruct* cbuffer;
	m_pDeviceContext->Map( m_pRayCBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource );
	cbuffer = (RayCBufferStruct*)mappedResource.pData;

	cbuffer->camPos = XMLoadFloat4(&m_camPosition);
	XMVECTOR determinant = XMMatrixDeterminant(m_pCamera->getProjectionMatrix());
	cbuffer->proj = XMMatrixInverse(&determinant, m_pCamera->getProjectionMatrix());
	determinant = XMMatrixDeterminant(m_pCamera->getViewMatrix());
	cbuffer->view = XMMatrixInverse(&determinant, m_pCamera->getViewMatrix());

	m_pDeviceContext->Unmap(m_pRayCBuffer, 0);

	ID3D11Buffer* res = m_pRayCBuffer;
	m_pDeviceContext->CSSetConstantBuffers(0, 1, &res);

	m_pRayCasting->set();
	m_pGPUTimer->start();
	m_pDeviceContext->Dispatch(m_dispatchSize, m_dispatchSize, 1);
	m_pGPUTimer->stop();
	m_pRayCasting->unset();

	rays += m_pGPUTimer->getTime();

	m_pDeviceContext->CSSetUnorderedAccessViews(0, 1, m_uavNULL, NULL);

	////////////////////////////////////
	// END RAY STAGE
	////////////////////////////////////

	for(int pass = 0; pass < 2; pass++)
	{
		////////////////////////////////////
		// INTERSECTION STAGE
		////////////////////////////////////
	
		ID3D11UnorderedAccessView* pIntersectionUAV[] = { m_pIntersectionSBuffer->getUnorderedAccessView() };
		m_pDeviceContext->CSSetUnorderedAccessViews(0, 1, pIntersectionUAV, NULL);
	
		ID3D11ShaderResourceView* pIntersectionResourceView[] = 
							{	m_pRaySBuffer->getResourceView(),
								m_pOptimusPrimeSBuffer->getResourceView(), 
								optimus.m_material.getTexture()->getResourceView(), 
								optimus.m_material.getNormalTexture()->getResourceView() 
							};
		m_pDeviceContext->CSSetShaderResources(0, 4, pIntersectionResourceView);
		IntersectionCBufferStruct* icbuffer;
	
		m_pDeviceContext->Map( m_pIntersectionCBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource );
		icbuffer = (IntersectionCBufferStruct*)mappedResource.pData;

		icbuffer->triangle	= m_intersectionStruct.triangle;
		icbuffer->sphere	= m_intersectionStruct.sphere;
		icbuffer->box		= m_intersectionStruct.box;

		m_pDeviceContext->Unmap(m_pIntersectionCBuffer, 0);

		res = m_pIntersectionCBuffer;
		m_pDeviceContext->CSSetConstantBuffers(0, 1, &res);

		//m_pDeviceContext->CSSetSamplers(0, 1, &m_pTextureSampler);

		m_pIntersection->set();
		m_pGPUTimer->start();
		m_pDeviceContext->Dispatch(m_dispatchSize, m_dispatchSize, 1);
		m_pGPUTimer->stop();
		m_pIntersection->unset();
		
		intersection += m_pGPUTimer->getTime();

		m_pDeviceContext->CSSetUnorderedAccessViews(0, 1, m_uavNULL, NULL);
		m_pDeviceContext->CSSetShaderResources(0, 4, m_srvNULL);

		////////////////////////////////////
		// END INTERSECTION STAGE
		////////////////////////////////////

		////////////////////////////////////
		// COLOR STAGE
		////////////////////////////////////
		ID3D11UnorderedAccessView* pColorUAV[] = { m_pBackBufferUAV, m_pRaySBuffer->getUnorderedAccessView(), m_pColorSBuffer->getUnorderedAccessView() };
		m_pDeviceContext->CSSetUnorderedAccessViews(0, 3, pColorUAV, NULL);

		//if (pass == 0)
		//	continue;
	
		ID3D11ShaderResourceView* pColorResourceView[] = { m_pIntersectionSBuffer->getResourceView(), m_pOptimusPrimeSBuffer->getResourceView() };
		m_pDeviceContext->CSSetShaderResources(0, 2, pColorResourceView);
		ColorCBufferStruct* ccbuffer;
	
		m_pDeviceContext->Map( m_pColorCBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource );
		ccbuffer = (ColorCBufferStruct*)mappedResource.pData;

		//ccbuffer->camPos = m_camPosition;
		for(int i = 0; i < 15; i++)
		{
			ccbuffer->lights[i] = m_colorStruct.lights[i];
		}
		ccbuffer->triangle	= m_intersectionStruct.triangle;

		if(pass == 0)
			m_intersectionStruct.sphere.radius.w = 10.0f;
		else
			m_intersectionStruct.sphere.radius.w = 0.0f;
			
		ccbuffer->sphere	= m_intersectionStruct.sphere;
		ccbuffer->box		= m_intersectionStruct.box;
		m_pDeviceContext->Unmap(m_pColorCBuffer, 0);

		res = m_pColorCBuffer;
		m_pDeviceContext->CSSetConstantBuffers(0, 1, &res);

		m_pColorStage->set();
		m_pGPUTimer->start();
		m_pDeviceContext->Dispatch(m_dispatchSize, m_dispatchSize, 1);
		m_pGPUTimer->stop();
		m_pColorStage->unset();
		
		color += m_pGPUTimer->getTime();

		m_pDeviceContext->CSSetUnorderedAccessViews(0, 3, m_uavNULL, NULL);
		m_pDeviceContext->CSSetShaderResources(0, 2, m_srvNULL);

		////////////////////////////////////
		// END COLOR STAGE
		////////////////////////////////////
	}
	

	m_pSwapChain->Present(0, 0);
}

// ## Input start ##

void Direct3D::leftMouseClick( XMFLOAT2 p_mousePosition )
{

}
void Direct3D::rightMouseClick( XMFLOAT2 p_mousePosition )
{

}
void Direct3D::mouseMove( XMFLOAT2 p_mousePosition )
{
	//float Rotation = 0;
	//m_camera->rotate(Rotation,0.0f);
	//m_camera->setTarget(m_objects.m_pacman->getPosition());
	//m_camera->update(m_deltaTime);
	m_pCamera->rotate(p_mousePosition.y / (PI*100.0f), p_mousePosition.x / (PI*100.0f), 0); 
	m_pCamera->view(m_camPosition, m_camTarget, m_camUp);
}
void Direct3D::keyEvent( USHORT p_key )
{
	if(p_key == 0x57) // W
	{
		XMStoreFloat4(&m_camPosition, XMLoadFloat4(&m_camPosition) + m_pCamera->getLookAt() * m_speed);
		XMStoreFloat4(&m_camTarget, XMLoadFloat4(&m_camTarget) + m_pCamera->getLookAt() * m_speed);
		XMStoreFloat4(&m_camUp, m_pCamera->getUpVector());
		m_pCamera->view(m_camPosition, m_camTarget, m_camUp);
	}
	if(p_key == 0x41) // A
	{
		XMStoreFloat4(&m_camPosition, XMLoadFloat4(&m_camPosition) - m_pCamera->getRightVector() * m_speed);
		XMStoreFloat4(&m_camTarget, XMLoadFloat4(&m_camTarget) - m_pCamera->getRightVector() * m_speed);
		XMStoreFloat4(&m_camUp, m_pCamera->getUpVector());
		m_pCamera->view(m_camPosition, m_camTarget, m_camUp);
	}
	if(p_key == 0x53) // S
	{
		XMStoreFloat4(&m_camPosition, XMLoadFloat4(&m_camPosition) - m_pCamera->getLookAt() * m_speed);
		XMStoreFloat4(&m_camTarget, XMLoadFloat4(&m_camTarget) - m_pCamera->getLookAt() * m_speed);
		XMStoreFloat4(&m_camUp, m_pCamera->getUpVector());
		m_pCamera->view(m_camPosition, m_camTarget, m_camUp);
	}
	if(p_key == 0x44) // D
	{
		XMStoreFloat4(&m_camPosition, XMLoadFloat4(&m_camPosition) + m_pCamera->getRightVector() * m_speed);
		XMStoreFloat4(&m_camTarget, XMLoadFloat4(&m_camTarget) + m_pCamera->getRightVector() * m_speed);
		XMStoreFloat4(&m_camUp, m_pCamera->getUpVector());
		m_pCamera->view(m_camPosition, m_camTarget, m_camUp);
	}
	if(p_key == 0x20) // Space
	{
		
		XMStoreFloat4(&m_camPosition, XMLoadFloat4(&m_camPosition) + m_pCamera->getUpVector() * m_speed);
		XMStoreFloat4(&m_camTarget, XMLoadFloat4(&m_camTarget) + m_pCamera->getUpVector() * m_speed);
		XMStoreFloat4(&m_camUp, m_pCamera->getUpVector());
		m_pCamera->view(m_camPosition, m_camTarget, m_camUp);		
	}
	if(p_key == 0x11) // ctrl
	{
		XMStoreFloat4(&m_camPosition, XMLoadFloat4(&m_camPosition) - m_pCamera->getUpVector() * m_speed);
		XMStoreFloat4(&m_camTarget, XMLoadFloat4(&m_camTarget) - m_pCamera->getUpVector() * m_speed);
		XMStoreFloat4(&m_camUp, m_pCamera->getUpVector());
		m_pCamera->view(m_camPosition, m_camTarget, m_camUp);		
	}
	if(p_key == 0x1B) //ESC
		PostQuitMessage(0);
	
}

LRESULT Direct3D::msgProc(UINT p_msg, WPARAM p_wParam, LPARAM p_lParam)
{
	m_pHID->update(p_msg, p_lParam);
	return D3DApp::msgProc(p_msg, p_wParam, p_lParam);;
}

// ## Input end ##