#include "ComputeWrap.h"


ComputeWrap::ComputeWrap(ID3D11Device* p_pDevice, ID3D11DeviceContext* p_pDeviceContext)
{
	m_pDevice = p_pDevice;
	m_pDeviceContext = p_pDeviceContext;
}

ComputeWrap::~ComputeWrap(){}

ComputeShader* ComputeWrap::createComputeShader(LPCWSTR* p_pShaderFile, char* p_pBlobFileAppendix, char* p_pFunctionName, D3D10_SHADER_MACRO* p_pDefines)
{
	ComputeShader* cs = new ComputeShader();

	if(cs && !cs->init(
		p_pShaderFile,
		p_pBlobFileAppendix,
		p_pFunctionName,
		p_pDefines,
		m_pDevice,
		m_pDeviceContext))
	{
		SAFE_DELETE(cs);
	}

	return cs;
}

ID3D11Buffer* ComputeWrap::createConstantBuffer(UINT p_uSize, VOID* p_pInitData, char* p_pDebugName)
{
	ID3D11Buffer* pBuffer = NULL;

	// setup creation information
	D3D11_BUFFER_DESC cbDesc;
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.ByteWidth = p_uSize + (16 - p_uSize % 16);
	cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbDesc.MiscFlags = 0;
	cbDesc.StructureByteStride = 0;
	cbDesc.Usage = D3D11_USAGE_DYNAMIC;

    if(p_pInitData)
    {
        D3D11_SUBRESOURCE_DATA InitData;
        InitData.pSysMem = p_pInitData;
        m_pDevice->CreateBuffer(&cbDesc, &InitData, &pBuffer);
    }
	else
	{
        m_pDevice->CreateBuffer(&cbDesc, NULL, &pBuffer);
	}

	if(p_pDebugName && pBuffer)
	{
		setDebugName(pBuffer, p_pDebugName);
	}

	return pBuffer;
}

ComputeBuffer* ComputeWrap::createBuffer(COMPUTE_BUFFER_TYPE p_uType, UINT p_uElementSize, UINT p_uCount, bool p_bSRV, bool p_bUAV, VOID* p_pInitData, bool p_bCreateStaging, char* p_debugName)
{
	ComputeBuffer* buffer = new ComputeBuffer();
	buffer->m_pDeviceContext = m_pDeviceContext;

	if(p_uType == STRUCTURED_BUFFER)
		buffer->m_pResource = createStructuredBuffer(p_uElementSize, p_uCount, p_bSRV, p_bUAV, p_pInitData);
	else if(p_uType == RAW_BUFFER)
		buffer->m_pResource = createRawBuffer(p_uElementSize * p_uCount, p_pInitData);
	else if(p_uType == CONSTANT_BUFFER)
		buffer->m_pResource = createConstantBuffer(p_uElementSize, p_pInitData, p_debugName);
	

	if(buffer->m_pResource != NULL)
	{
		if(p_bSRV)
			buffer->m_pResourceView = createBufferSRV(buffer->m_pResource);
		if(p_bUAV)
			buffer->m_pUnorderedAccessView = createBufferUAV(buffer->m_pResource);
		if(p_bCreateStaging)
			buffer->m_pStaging = createStagingBuffer(p_uElementSize * p_uCount);
	}

	if(p_debugName)
	{
		if(buffer->m_pResource)				setDebugName(buffer->m_pResource, p_debugName);
		if(buffer->m_pStaging)				setDebugName(buffer->m_pStaging, p_debugName);
		if(buffer->m_pResourceView)			setDebugName(buffer->m_pResourceView, p_debugName);
		if(buffer->m_pUnorderedAccessView)	setDebugName(buffer->m_pUnorderedAccessView, p_debugName);
	}

	return buffer; //return shallow copy
}

ComputeTexture* ComputeWrap::createTexture(DXGI_FORMAT p_dxFormat, UINT p_uWidth, UINT p_uHeight, UINT p_uRowPitch, VOID* p_pInitData, bool p_bCreateStaging, char* p_pDebugName)
{
	ComputeTexture* texture = new ComputeTexture();
	texture->m_pDeviceContext = m_pDeviceContext;

	texture->m_pResource = createTextureResource(p_dxFormat, p_uWidth, p_uHeight, p_uRowPitch, p_pInitData);

	if(texture->m_pResource != NULL)
	{
		texture->m_pResourceView = createTextureSRV(texture->m_pResource);
		texture->m_pUnorderedAccessView = createTextureUAV(texture->m_pResource);
		
		if(p_bCreateStaging)
			texture->m_pStaging = createStagingTexture(texture->m_pResource);
	}

	if(p_pDebugName)
	{
		if(texture->m_pResource)			setDebugName(texture->m_pResource, p_pDebugName);
		if(texture->m_pStaging)				setDebugName(texture->m_pStaging, p_pDebugName);
		if(texture->m_pResourceView)		setDebugName(texture->m_pResourceView, p_pDebugName);
		if(texture->m_pUnorderedAccessView)	setDebugName(texture->m_pUnorderedAccessView, p_pDebugName);
	}

	return texture;
}

ComputeTexture* ComputeWrap::createTexture(wchar_t* p_textureFilename, char* p_pDebugName)
{
	ComputeTexture* texture = new ComputeTexture();
	texture->m_pDeviceContext = m_pDeviceContext;

	//DirectX::TexMetadata t_data;
	//DirectX::GetMetadataFromDDSFile(p_textureFilename, NULL, t_data);

	//DirectX::CreateTexture();

	if(SUCCEEDED(CreateWICTextureFromFile(m_pDevice, m_pDeviceContext, p_textureFilename, (ID3D11Resource**)&texture->m_pResource, &texture->m_pResourceView, NULL)))
	{
		texture->m_pResourceView = createTextureSRV(texture->m_pResource);
		
		if(p_pDebugName)
		{
			if(texture->m_pResource)			setDebugName(texture->m_pResource, p_pDebugName);
			if(texture->m_pStaging)				setDebugName(texture->m_pStaging, p_pDebugName);
			if(texture->m_pResourceView)		setDebugName(texture->m_pResourceView, p_pDebugName);
			if(texture->m_pUnorderedAccessView)	setDebugName(texture->m_pUnorderedAccessView, p_pDebugName);
		}
	}
	return texture;
}

ID3D11Buffer* ComputeWrap::createStructuredBuffer(UINT p_uElementSize, UINT p_uCount, bool p_bSRV, bool p_bUAV, VOID* p_pInitData)
{
	ID3D11Buffer* pBufOut = NULL;

    D3D11_BUFFER_DESC desc;
    ZeroMemory( &desc, sizeof(desc) );
    desc.BindFlags = 0;
	
	if(p_bUAV)	desc.BindFlags |= D3D11_BIND_UNORDERED_ACCESS;
	if(p_bSRV)	desc.BindFlags |= D3D11_BIND_SHADER_RESOURCE;
    
	UINT bufferSize = p_uElementSize * p_uCount;
	desc.ByteWidth = bufferSize < 16 ? 16 : bufferSize;
    desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
    desc.StructureByteStride = p_uElementSize;

    if ( p_pInitData )
    {
        D3D11_SUBRESOURCE_DATA InitData;
        InitData.pSysMem = p_pInitData;
		m_pDevice->CreateBuffer( &desc, &InitData, &pBufOut);
    }
	else
	{
		m_pDevice->CreateBuffer(&desc, NULL, &pBufOut);
	}

	return pBufOut;
}

ID3D11Buffer* ComputeWrap::createRawBuffer(UINT p_uSize, VOID* p_pInitData)
{
	ID3D11Buffer* pBufOut = NULL;

    D3D11_BUFFER_DESC desc;
    ZeroMemory(&desc, sizeof(desc));
    desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_INDEX_BUFFER | D3D11_BIND_VERTEX_BUFFER;
    desc.ByteWidth = p_uSize;
    desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;

    if ( p_pInitData )
    {
        D3D11_SUBRESOURCE_DATA InitData;
        InitData.pSysMem = p_pInitData;
        m_pDevice->CreateBuffer(&desc, &InitData, &pBufOut);
    }
	else
	{
        m_pDevice->CreateBuffer(&desc, NULL, &pBufOut);
	}

	return pBufOut;
}

ID3D11ShaderResourceView* ComputeWrap::createBufferSRV(ID3D11Buffer* p_pBuffer)
{
	ID3D11ShaderResourceView* pSRVOut = NULL;

    D3D11_BUFFER_DESC descBuf;
    ZeroMemory(&descBuf, sizeof(descBuf));
    p_pBuffer->GetDesc(&descBuf);

    D3D11_SHADER_RESOURCE_VIEW_DESC desc;
    ZeroMemory(&desc, sizeof(desc));
    desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
    desc.BufferEx.FirstElement = 0;

    if(descBuf.MiscFlags & D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS)
    {
        // This is a Raw Buffer
        desc.Format = DXGI_FORMAT_R32_TYPELESS;
        desc.BufferEx.Flags = D3D11_BUFFEREX_SRV_FLAG_RAW;
        desc.BufferEx.NumElements = descBuf.ByteWidth / 4;
    }
	else if(descBuf.MiscFlags & D3D11_RESOURCE_MISC_BUFFER_STRUCTURED)
    {
        // This is a Structured Buffer
        desc.Format = DXGI_FORMAT_UNKNOWN;
        desc.BufferEx.NumElements = descBuf.ByteWidth / descBuf.StructureByteStride;
    }
	else
	{
		return NULL;
	}

	HRESULT hr;
	hr = m_pDevice->CreateShaderResourceView(p_pBuffer, &desc, &pSRVOut);

	return pSRVOut;
}

ID3D11UnorderedAccessView* ComputeWrap::createBufferUAV(ID3D11Buffer* p_pBuffer)
{
	ID3D11UnorderedAccessView* pUAVOut = NULL;

	D3D11_BUFFER_DESC descBuf;
    ZeroMemory(&descBuf, sizeof(descBuf));
    p_pBuffer->GetDesc(&descBuf);
        
    D3D11_UNORDERED_ACCESS_VIEW_DESC desc;
    ZeroMemory(&desc, sizeof(desc));
    desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
    desc.Buffer.FirstElement = 0;

    if (descBuf.MiscFlags & D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS)
    {
        // This is a Raw Buffer
        desc.Format = DXGI_FORMAT_R32_TYPELESS; // Format must be DXGI_FORMAT_R32_TYPELESS, when creating Raw Unordered Access View
        desc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_RAW;
        desc.Buffer.NumElements = descBuf.ByteWidth / 4; 
    }
	else if(descBuf.MiscFlags & D3D11_RESOURCE_MISC_BUFFER_STRUCTURED)
    {
        // This is a Structured Buffer
        desc.Format = DXGI_FORMAT_UNKNOWN;      // Format must be must be DXGI_FORMAT_UNKNOWN, when creating a View of a Structured Buffer
        desc.Buffer.NumElements = descBuf.ByteWidth / descBuf.StructureByteStride; 
    }
	else
	{
		return NULL;
	}
    
	m_pDevice->CreateUnorderedAccessView(p_pBuffer, &desc, &pUAVOut);

	return pUAVOut;
}

ID3D11Buffer* ComputeWrap::createStagingBuffer(UINT p_uSize)
{
	ID3D11Buffer* debugbuf = NULL;

    D3D11_BUFFER_DESC desc;
    ZeroMemory(&desc, sizeof(desc));
	desc.ByteWidth = p_uSize;
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
    desc.Usage = D3D11_USAGE_STAGING;
    desc.BindFlags = 0;
    desc.MiscFlags = 0;
    
	m_pDevice->CreateBuffer(&desc, NULL, &debugbuf);

    return debugbuf;
}

	//texture functions
ID3D11Texture2D* ComputeWrap::createTextureResource(DXGI_FORMAT p_dxFormat, UINT p_uWidth, UINT p_uHeight, UINT p_uRowPitch, VOID* p_pInitData)
{
	ID3D11Texture2D* pTexture = NULL;

	D3D11_TEXTURE2D_DESC desc;
	desc.Width = p_uWidth;
	desc.Height = p_uHeight;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = p_dxFormat;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = p_pInitData;
	data.SysMemPitch = p_uRowPitch;

	if(FAILED(m_pDevice->CreateTexture2D( &desc, p_pInitData ? &data : NULL, &pTexture )))
	{

	}

	return pTexture;
}
//ID3D11Buffer* ComputeWrap::CreateRawBuffer(UINT uSize, VOID* pInitData);

ID3D11ShaderResourceView* ComputeWrap::createTextureSRV(ID3D11Texture2D* p_pTexture)
{
	ID3D11ShaderResourceView* pSRV = NULL;

	D3D11_TEXTURE2D_DESC td;
	p_pTexture->GetDesc(&td);

	//init view description
	D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc; 
	ZeroMemory( &viewDesc, sizeof(viewDesc) ); 
	
	viewDesc.Format					= td.Format;
	viewDesc.ViewDimension			= D3D11_SRV_DIMENSION_TEXTURE2D;
	viewDesc.Texture2D.MipLevels	= td.MipLevels;

	if(FAILED(m_pDevice->CreateShaderResourceView(p_pTexture, &viewDesc, &pSRV)))
	{
		//MessageBox(0, "Unable to create shader resource view", "Error!", 0);
	}

	return pSRV;
}

ID3D11UnorderedAccessView* ComputeWrap::createTextureUAV(ID3D11Texture2D* p_pTexture)
{
	ID3D11UnorderedAccessView* pUAV = NULL;

	m_pDevice->CreateUnorderedAccessView( p_pTexture, NULL, &pUAV );
	p_pTexture->Release();

	return pUAV;
}

ID3D11Texture2D* ComputeWrap::createStagingTexture(ID3D11Texture2D* p_pTexture)
{
	ID3D11Texture2D* pStagingTex = NULL;

    D3D11_TEXTURE2D_DESC desc;
	p_pTexture->GetDesc(&desc);
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
    desc.Usage = D3D11_USAGE_STAGING;
    desc.BindFlags = 0;
    desc.MiscFlags = 0;
    
	m_pDevice->CreateTexture2D(&desc, NULL, &pStagingTex);

    return pStagingTex;
}
	
void ComputeWrap::setDebugName(ID3D11DeviceChild* p_object, char* p_debugName)
{
	#if defined( DEBUG ) || defined( _DEBUG )
	// Only works if device is created with the D3D debug layer, or when attached to PIX for Windows
	p_object->SetPrivateData( WKPDID_D3DDebugObjectName, (UINT)strlen(p_debugName), p_debugName );
	#endif
}