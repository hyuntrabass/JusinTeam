#include "Renderer.h"
#include "BlendObject.h"
#include "OrthographicObject.h"
#include "VIBuffer_Rect.h"
#include "Shader.h"

CRenderer::CRenderer(_dev pDevice, _context pContext)
	: CComponent(pDevice, pContext)
{
}

HRESULT CRenderer::Init_Prototype()
{
	_uint iNumViewPorts{ 1 };

	D3D11_VIEWPORT ViewportDesc{};

	m_pContext->RSGetViewports(&iNumViewPorts, &ViewportDesc);

#pragma region For_MRT_GameObject

	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Diffuse"), static_cast<_uint>(ViewportDesc.Width), static_cast<_uint>(ViewportDesc.Height), DXGI_FORMAT_R8G8B8A8_UNORM, _float4(1.f, 1.f, 1.f, 0.f))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Normal"), static_cast<_uint>(ViewportDesc.Width), static_cast<_uint>(ViewportDesc.Height), DXGI_FORMAT_R16G16B16A16_UNORM, _float4(1.f, 1.f, 1.f, 1.f))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Depth"), static_cast<_uint>(ViewportDesc.Width), static_cast<_uint>(ViewportDesc.Height), DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(1.f, 1.f, 0.f, 1.f))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Object_Specular"), static_cast<_uint>(ViewportDesc.Width), static_cast<_uint>(ViewportDesc.Height), DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
	{
		return E_FAIL;
	}

	// 원명
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Velocity"), static_cast<_uint>(ViewportDesc.Width), static_cast<_uint>(ViewportDesc.Height), DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
	{
		return E_FAIL;
	}
	//

#pragma endregion

#pragma region For_MRT_Refraction

	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Refraction_Diffuse"), static_cast<_uint>(ViewportDesc.Width), static_cast<_uint>(ViewportDesc.Height), DXGI_FORMAT_R8G8B8A8_UNORM, _float4(1.f, 1.f, 1.f, 0.f))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Refraction_Normal"), static_cast<_uint>(ViewportDesc.Width), static_cast<_uint>(ViewportDesc.Height), DXGI_FORMAT_R16G16B16A16_UNORM, _float4(1.f, 1.f, 1.f, 1.f))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Refraction_Depth"), static_cast<_uint>(ViewportDesc.Width), static_cast<_uint>(ViewportDesc.Height), DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(1.f, 1.f, 0.f, 1.f))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Refraction_Shade"), static_cast<_uint>(ViewportDesc.Width), static_cast<_uint>(ViewportDesc.Height), DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 1.f))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Refraction_Specular"), static_cast<_uint>(ViewportDesc.Width), static_cast<_uint>(ViewportDesc.Height), DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Refraction_Final"), static_cast<_uint>(ViewportDesc.Width), static_cast<_uint>(ViewportDesc.Height), DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
	{
		return E_FAIL;
	}

#pragma endregion

#pragma region For_MRT_Reflection

	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Reflection_Diffuse"), static_cast<_uint>(ViewportDesc.Width), static_cast<_uint>(ViewportDesc.Height), DXGI_FORMAT_R8G8B8A8_UNORM, _float4(1.f, 1.f, 1.f, 0.f))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Reflection_Normal"), static_cast<_uint>(ViewportDesc.Width), static_cast<_uint>(ViewportDesc.Height), DXGI_FORMAT_R16G16B16A16_UNORM, _float4(1.f, 1.f, 1.f, 1.f))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Reflection_Depth"), static_cast<_uint>(ViewportDesc.Width), static_cast<_uint>(ViewportDesc.Height), DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(1.f, 1.f, 0.f, 1.f))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Reflection_Shade"), static_cast<_uint>(ViewportDesc.Width), static_cast<_uint>(ViewportDesc.Height), DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 1.f))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Reflection_Specular"), static_cast<_uint>(ViewportDesc.Width), static_cast<_uint>(ViewportDesc.Height), DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Reflection_Final"), static_cast<_uint>(ViewportDesc.Width), static_cast<_uint>(ViewportDesc.Height), DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
	{
		return E_FAIL;
	}

#pragma endregion

#pragma region For_MRT_Water

	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Water_Depth"), static_cast<_uint>(ViewportDesc.Width), static_cast<_uint>(ViewportDesc.Height), DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(1.f, 1.f, 0.f, 1.f))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Water"), static_cast<_uint>(ViewportDesc.Width), static_cast<_uint>(ViewportDesc.Height), DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
	{
		return E_FAIL;
	}

#pragma endregion


#pragma region For_MRT_Light

	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Shade"), static_cast<_uint>(ViewportDesc.Width), static_cast<_uint>(ViewportDesc.Height), DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 1.f))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Specular"), static_cast<_uint>(ViewportDesc.Width), static_cast<_uint>(ViewportDesc.Height), DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
	{
		return E_FAIL;
	}

#pragma endregion


	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Bloom"), static_cast<_uint>(ViewportDesc.Width), static_cast<_uint>(ViewportDesc.Height), DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_BlurTest"), static_cast<_uint>(ViewportDesc.Width), static_cast<_uint>(ViewportDesc.Height), DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_SSAOTEST"), static_cast<_uint>(ViewportDesc.Width), static_cast<_uint>(ViewportDesc.Height), DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_SSAOBlur"), static_cast<_uint>(ViewportDesc.Width), static_cast<_uint>(ViewportDesc.Height), DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
	{
		return E_FAIL;
	}

	//if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_LightDepth"), static_cast<_uint>(ViewportDesc.Width), static_cast<_uint>(ViewportDesc.Height), DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(1.f, 1.f, 1.f, 1.f))))
	//{
	//	return E_FAIL;
	//}
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_LightDepth"), D3D11_REQ_TEXTURE2D_U_OR_V_DIMENSION, D3D11_REQ_TEXTURE2D_U_OR_V_DIMENSION / 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 
		_float4(1.f, 1.f, 1.f, 1.f))))
	{
		return E_FAIL;
	}

#pragma region For_MRT_HDR

	if(FAILED(m_pGameInstance->Add_RenderTarget(L"Target_HDR", static_cast<_uint>(ViewportDesc.Width), static_cast<_uint>(ViewportDesc.Height), DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f,0.f,0.f,0.f))))
		return E_FAIL;

#pragma endregion

#pragma region MRT_GameObjects

	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Diffuse"))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Normal"))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Depth"))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Object_Specular"))))
	{
		return E_FAIL;
	}

	// 원명
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Velocity"))))
	{
		return E_FAIL;
	}

#pragma endregion

#pragma region MRT_Refraction

	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Refraction"), TEXT("Target_Refraction_Diffuse"))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Refraction"), TEXT("Target_Refraction_Normal"))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Refraction"), TEXT("Target_Refraction_Depth"))))
	{
		return E_FAIL;
	}

#pragma endregion

#pragma region MRT_Reflection

	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Reflection"), TEXT("Target_Reflection_Diffuse"))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Reflection"), TEXT("Target_Reflection_Normal"))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Reflection"), TEXT("Target_Reflection_Depth"))))
	{
		return E_FAIL;
	}

#pragma endregion

#pragma region MRT_Water

	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Water"), TEXT("Target_Water_Depth"))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Water"), TEXT("Target_Water"))))
	{
		return E_FAIL;
	}

#pragma endregion


#pragma region MRT_Lights

	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Lights"), TEXT("Target_Shade"))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Lights"), TEXT("Target_Specular"))))
	{
		return E_FAIL;
	}

#pragma endregion

	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Shadow"), TEXT("Target_LightDepth"))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Blur"), TEXT("Target_Bloom"))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_BlurTest"), TEXT("Target_BlurTest"))))
	{
		return E_FAIL;
	}

#pragma region MRT_SSAO

	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_SSAOTEST"), TEXT("Target_SSAOTEST"))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_SSAOBlur"), TEXT("Target_SSAOBlur"))))
	{
		return E_FAIL;
	}
#pragma endregion

#pragma region MRT_HDR

	if (FAILED(m_pGameInstance->Add_MRT(L"MRT_HDR", L"Target_HDR")))
		return E_FAIL;

#pragma endregion

#pragma region VIBuffer & Shader Ready


	m_pVIBuffer = CVIBuffer_Rect::Create(m_pDevice, m_pContext);
	if (not m_pVIBuffer)
	{
		return E_FAIL;
	}

	m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Deferred.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements);
	if (not m_pShader)
	{
		return E_FAIL;
	}


	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());
	m_WorldMatrix._11 = ViewportDesc.Width;
	m_WorldMatrix._22 = ViewportDesc.Height;
	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(ViewportDesc.Width, ViewportDesc.Height, 0.f, 1.f));

#pragma endregion


	if (FAILED(Ready_ShadowDSV()))
	{
		return E_FAIL;
	}

#pragma region SSAO할려고 랜덤값 생성

	random_device RandomDevice;
	mt19937_64 RandomNumber;
	RandomNumber = mt19937_64(RandomDevice());

	uniform_real_distribution<float> RandomX = uniform_real_distribution<float>(-1.f, 1.f);
	uniform_real_distribution<float> RandomY = uniform_real_distribution<float>(-1.f, 1.f);
	uniform_real_distribution<float> RandomZ = uniform_real_distribution<float>(-1.f, 1.f);

	for (size_t i = 0; i < 16; i++)
		m_vRandom[i] = _vec3(RandomX(RandomNumber), RandomY(RandomNumber), RandomZ(RandomNumber));

#pragma endregion

#ifdef _DEBUGTEST
	if (FAILED(m_pGameInstance->Ready_Debug_RT(TEXT("Target_Diffuse"), _float2(50.f, 50.f), _float2(100.f, 100.f))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Ready_Debug_RT(TEXT("Target_Normal"), _float2(50.f, 150.f), _float2(100.f, 100.f))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Ready_Debug_RT(TEXT("Target_Depth"), _float2(50.f, 250.f), _float2(100.f, 100.f))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Ready_Debug_RT(TEXT("Target_Shade"), _float2(150.f, 50.f), _float2(100.f, 100.f))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Ready_Debug_RT(TEXT("Target_Specular"), _float2(150.f, 150.f), _float2(100.f, 100.f))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Ready_Debug_RT(TEXT("Target_LightDepth"), _float2(ViewportDesc.Width - 50.f, 50.f), _float2(100.f, 100.f))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Ready_Debug_RT(TEXT("Target_Bloom"), _float2(ViewportDesc.Width - 50.f, 150.f), _float2(100.f, 100.f))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Ready_Debug_RT(TEXT("Target_BlurTest"), _float2(ViewportDesc.Width - 50.f, 250.f), _float2(100.f, 100.f))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Ready_Debug_RT(TEXT("Target_SSAOTEST"), _float2(ViewportDesc.Width - 150.f, 50.f), _float2(100.f, 100.f))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Ready_Debug_RT(TEXT("Target_SSAOBlur"), _float2(ViewportDesc.Width - 150.f, 150.f), _float2(100.f, 100.f))))
	{
		return E_FAIL;
	}
#endif // _DEBUG

#pragma region Compute_Shader 생성

	fWidth = ViewportDesc.Width;
	fHeight = ViewportDesc.Height;

	D3D11_BUFFER_DESC DSBDesc{};

	DSBDesc.ByteWidth = sizeof(THPARAM);
	DSBDesc.Usage = D3D11_USAGE_DYNAMIC;
	DSBDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	DSBDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	if (FAILED(m_pDevice->CreateBuffer(&DSBDesc, nullptr, &m_pDSBuffer)))
		return E_FAIL;

	ID3DBlob* errorBlob = nullptr;

	if (FAILED(D3DCompileFromFile(L"../Bin/ShaderFiles/Shader_DownScale.hlsl", nullptr, nullptr, "ThresholdAndDowsample", "cs_5_0", 0, 0, &m_pDSBlob, &errorBlob))) {
		if (nullptr != errorBlob) {
			OutputDebugStringA((char*)errorBlob->GetBufferPointer());
			Safe_Release(errorBlob);
		}
		return E_FAIL;
	}

	if (FAILED(m_pDevice->CreateComputeShader(m_pDSBlob->GetBufferPointer(), m_pDSBlob->GetBufferSize(), nullptr, &m_pDSCShader)))
		return E_FAIL;

	m_BLParam.radius = GAUSSIAN_RADIUS;
	m_BLParam.direction = 0;

	_float sigma = 10.f;
	_float sigmaRcp = 1.f / sigma;
	_float twoSigmaSq = 2 * sigma * sigma;

	_float fSum = 0.f;
	for (size_t i = 0; i < GAUSSIAN_RADIUS; i++)
	{
		m_BLParam.coefficients[i] = (1.f / sigma) * expf(-static_cast<_float>(i * i) / twoSigmaSq);

		fSum += 2 * m_BLParam.coefficients[i];
	}

	fSum -= m_BLParam.coefficients[0];

	_float NormalizationFactor = 1.f / fSum;
	for (size_t i = 0; i < GAUSSIAN_RADIUS; i++)
		m_BLParam.coefficients[i] *= NormalizationFactor;

	D3D11_BUFFER_DESC BlurBDesc{};
	BlurBDesc.ByteWidth = sizeof(BLURPARAM);
	BlurBDesc.Usage = D3D11_USAGE_DYNAMIC;
	BlurBDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	BlurBDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	if (FAILED(m_pDevice->CreateBuffer(&BlurBDesc, nullptr, &m_pBlurBuffer)))
		return E_FAIL;

	if (FAILED(D3DCompileFromFile(L"../Bin/ShaderFiles/Shader_Blur.hlsl", nullptr, nullptr, "Blur", "cs_5_0", 0, 0, &m_pBlurBlob, &errorBlob))) {
		if (nullptr != errorBlob) {
			OutputDebugStringA((char*)errorBlob->GetBufferPointer());
			Safe_Release(errorBlob);
		}
		return E_FAIL;
	}


	if (FAILED(m_pDevice->CreateComputeShader(m_pBlurBlob->GetBufferPointer(), m_pBlurBlob->GetBufferSize(), nullptr, &m_pBlurCShader)))
		return E_FAIL;

	D3D11_TEXTURE2D_DESC TexDesc{};

	TexDesc.Width = fWidth / 2;
	TexDesc.Height = fHeight / 2;
	TexDesc.MipLevels = 1;
	TexDesc.ArraySize = 1;
	TexDesc.Format = DXGI_FORMAT_R16G16B16A16_UNORM;
	TexDesc.SampleDesc.Count = 1;
	TexDesc.Usage = D3D11_USAGE_DEFAULT;
	TexDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	TexDesc.CPUAccessFlags = 0;
	TexDesc.MiscFlags = 0;

	if (FAILED(m_pDevice->CreateTexture2D(&TexDesc, nullptr, &m_pDSTexture)))
		return E_FAIL;

	if (FAILED(m_pDevice->CreateTexture2D(&TexDesc, nullptr, &m_pBlurTexture)))
		return E_FAIL;

	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc{};

	SRVDesc.Format = TexDesc.Format;
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	SRVDesc.Texture2D.MostDetailedMip = 0;
	SRVDesc.Texture2D.MipLevels = 1;

	if (FAILED(m_pDevice->CreateShaderResourceView(m_pDSTexture, &SRVDesc, &m_pDSSRV)))
		return E_FAIL;

	if (FAILED(m_pDevice->CreateShaderResourceView(m_pBlurTexture, &SRVDesc, &m_pBlurSRV)))
		return E_FAIL;

	D3D11_UNORDERED_ACCESS_VIEW_DESC UAVDesc{};

	UAVDesc.Format = TexDesc.Format;
	UAVDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
	UAVDesc.Texture2D.MipSlice = 0;

	if (FAILED(m_pDevice->CreateUnorderedAccessView(m_pDSTexture, &UAVDesc, &m_pDSUAV)))
		return E_FAIL;

	if (FAILED(m_pDevice->CreateUnorderedAccessView(m_pBlurTexture, &UAVDesc, &m_pBlurUAV)))
		return E_FAIL;

	{
		////struct CBuffer {
		////	_uint iDomain;
		////	_uint iGroupSize;
		////};

		////CBuffer cb;
		////ID3D11Buffer* pCBuffer = nullptr;

		////D3D11_BUFFER_DESC bd{};
		////bd.Usage = D3D11_USAGE_DYNAMIC;
		////bd.ByteWidth = sizeof(CBuffer);
		////bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		////bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		////if (FAILED(m_pDevice->CreateBuffer(&bd, nullptr, &pCBuffer)))
		////	return E_FAIL;

		////cb.iDomain = fWidth * fHeight;
		////cb.iGroupSize = 32 * 32;

		////D3D11_MAPPED_SUBRESOURCE msr;
		////if (FAILED(m_pContext->Map(pCBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr)))
		////	return E_FAIL;

		////memcpy(msr.pData, &cb, sizeof(cb));

		////m_pContext->Unmap(pCBuffer, 0);


		//D3D11_BUFFER_DESC Desc{};

		//Desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
		//Desc.StructureByteStride = sizeof(_float);
		//Desc.ByteWidth = ViewportDesc.Width * Desc.StructureByteStride;
		//Desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

		//if (FAILED(m_pDevice->CreateBuffer(&Desc, nullptr, &m_pBuffer)))
		//	return E_FAIL;

		//if (FAILED(m_pDevice->CreateBuffer(&Desc, nullptr, &m_pBuffer2)))
		//	return E_FAIL;

		//D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc{};

		//SRVDesc.Format = DXGI_FORMAT_UNKNOWN;
		//SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
		//SRVDesc.Buffer.NumElements = ViewportDesc.Width;
		//if (FAILED(m_pDevice->CreateShaderResourceView(m_pBuffer, &SRVDesc, &m_pSRV)))
		//	return E_FAIL;

		//if (FAILED(m_pDevice->CreateShaderResourceView(m_pBuffer2, &SRVDesc, &m_pSRV2)))
		//	return E_FAIL;

		//D3D11_UNORDERED_ACCESS_VIEW_DESC UAVDesc{};

		//UAVDesc.Format = DXGI_FORMAT_UNKNOWN;
		//UAVDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		//UAVDesc.Buffer.NumElements = ViewportDesc.Width;
		//if (FAILED(m_pDevice->CreateUnorderedAccessView(m_pBuffer, &UAVDesc, &m_pUAV)))
		//	return E_FAIL;

		//if (FAILED(m_pDevice->CreateUnorderedAccessView(m_pBuffer2, &UAVDesc, &m_pUAV2)))
		//	return E_FAIL;

		//ID3DBlob* pCsBlob = nullptr;
		//ID3DBlob* pCsBlob2 = nullptr;

		//if (FAILED(D3DCompileFromFile(L"../Bin/ShaderFiles/Shader_Compute.hlsl", nullptr, nullptr, "DownScaleFirstPass", "cs_5_0", 0, 0, &pCsBlob, nullptr)))
		//	return E_FAIL;

		//if (FAILED(D3DCompileFromFile(L"../Bin/ShaderFiles/Shader_Compute.hlsl", nullptr, nullptr, "DownScaleSecondPass", "cs_5_0", 0, 0, &pCsBlob2, nullptr)))
		//	return E_FAIL;

		//if (FAILED(m_pDevice->CreateComputeShader(pCsBlob->GetBufferPointer(), pCsBlob->GetBufferSize(), nullptr, &m_pComputeShader)))
		//	return E_FAIL;

		//if (FAILED(m_pDevice->CreateComputeShader(pCsBlob2->GetBufferPointer(), pCsBlob2->GetBufferSize(), nullptr, &m_pComputeShader2)))
		//	return E_FAIL;
	}

#pragma endregion



	return S_OK;
}

HRESULT CRenderer::Init(void* pArg)
{
	return S_OK;
}

HRESULT CRenderer::Add_RenderGroup(RenderGroup eRenderGroup, CGameObject* pRenderObject)
{
	if (!pRenderObject)
	{
		MSG_BOX("pRenderObject is NULL");
		return E_FAIL;
	}

	m_RenderObjects[ToIndex(eRenderGroup)].push_back(pRenderObject);

	Safe_AddRef(pRenderObject);

	return S_OK;
}

HRESULT CRenderer::Draw_RenderGroup()
{
	if (FAILED(Render_Priority()))
	{
		MSG_BOX("Failed to Render : Priority");
		return E_FAIL;
	}
	if (FAILED(Render_Shadow()))
	{
		MSG_BOX("Failed to Render : Shadow");
		return E_FAIL;
	}
	if (FAILED(Render_NonBlend()))
	{
		MSG_BOX("Failed to Render : NonBlend");
		return E_FAIL;
	}
	if (FAILED(Render_Refraction()))
	{
		MSG_BOX("Failed to Render : Refraction");
		return E_FAIL;
	}
	if (FAILED(Render_Reflection()))
	{
		MSG_BOX("Failed to Render : Reflection");
		return E_FAIL;
	}
	if (FAILED(Render_Water()))
	{
		MSG_BOX("Failed to Render : Water");
		return E_FAIL;
	}
	if (FAILED(Render_LightAcc()))
	{
		MSG_BOX("Failed to Render : LightAcc");
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Begin_MRT(L"MRT_HDR")))
		return E_FAIL;

	if (FAILED(Render_Deferred()))
	{
		MSG_BOX("Failed to Render : Deferred");
		return E_FAIL;
	}
	if (FAILED(Render_NonLight()))
	{
		MSG_BOX("Failed to Render : NonLight");
		return E_FAIL;
	}
	if (FAILED(Render_Blend()))
	{
		MSG_BOX("Failed to Render : Blend");
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	if (FAILED(Render_HDR()))
		return E_FAIL;

	if (FAILED(Render_Blur()))
	{
		MSG_BOX("Failed to Render : Blur");
		return E_FAIL;
	}
	if (FAILED(Render_BlendBlur()))
	{
		MSG_BOX("Failed to Render : BlenderBlur");
		return E_FAIL;
	}

	if (FAILED(Render_UI()))
	{
		MSG_BOX("Failed to Render : UI");
		return E_FAIL;
	}

#ifdef _DEBUGTEST
	if (not m_pGameInstance->IsSkipDebugRendering())
	{
		if (FAILED(Render_Debug()))
		{
			MSG_BOX("Failed to Render Debug");
			return E_FAIL;
		}
	}
#endif // _DEBUG


	return S_OK;
}

#ifdef _DEBUGTEST
HRESULT CRenderer::Add_DebugComponent(CComponent* pDebugComponent)
{
	if (m_pGameInstance->IsSkipDebugRendering())
	{
		return S_OK;
	}

	m_DebugComponents.push_back(pDebugComponent);
	Safe_AddRef(pDebugComponent);

	return S_OK;
}
#endif // _DEBUG


HRESULT CRenderer::Ready_ShadowDSV()
{
	if (nullptr == m_pDevice)
		return E_FAIL;

	{
		ID3D11Texture2D* pDepthStencilTexture = nullptr;

		D3D11_TEXTURE2D_DESC	TextureDesc;
		ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

		TextureDesc.Width = D3D11_REQ_TEXTURE2D_U_OR_V_DIMENSION;
		TextureDesc.Height = D3D11_REQ_TEXTURE2D_U_OR_V_DIMENSION / 2;
		TextureDesc.MipLevels = 1;
		TextureDesc.ArraySize = 1;
		TextureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

		TextureDesc.SampleDesc.Quality = 0;
		TextureDesc.SampleDesc.Count = 1;

		TextureDesc.Usage = D3D11_USAGE_DEFAULT;
		TextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL
			/*| D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE*/;
		TextureDesc.CPUAccessFlags = 0;
		TextureDesc.MiscFlags = 0;

		if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr,
											  &pDepthStencilTexture)))
			return E_FAIL;

		/* RenderTarget */
		/* ShaderResource */
		/* DepthStencil */

		if (FAILED(m_pDevice->CreateDepthStencilView(pDepthStencilTexture, nullptr,
													 &m_pShadowDSV)))
			return E_FAIL;

		Safe_Release(pDepthStencilTexture);
	}

	{
		//ID3D11Texture2D* pDepthStencilTexture = nullptr;

		//D3D11_TEXTURE2D_DESC	TextureDesc;
		//ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

		//TextureDesc.Width = 1280;
		//TextureDesc.Height = 720;
		//TextureDesc.MipLevels = 1;
		//TextureDesc.ArraySize = 1;
		//TextureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

		//TextureDesc.SampleDesc.Quality = 0;
		//TextureDesc.SampleDesc.Count = 1;

		//TextureDesc.Usage = D3D11_USAGE_DEFAULT;
		//TextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL
		//	/*| D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE*/;
		//TextureDesc.CPUAccessFlags = 0;
		//TextureDesc.MiscFlags = 0;

		//if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr,
		//									  &pDepthStencilTexture)))
		//	return E_FAIL;

		///* RenderTarget */
		///* ShaderResource */
		///* DepthStencil */

		//if (FAILED(m_pDevice->CreateDepthStencilView(pDepthStencilTexture, nullptr,
		//											 &m_pBlurDSV)))
		//	return E_FAIL;

		//Safe_Release(pDepthStencilTexture);
	}

	return S_OK;
}

HRESULT CRenderer::Render_Priority()
{
	for (auto& pGameObject : m_RenderObjects[RG_Priority])
	{
		if (pGameObject)
		{
			if (FAILED(pGameObject->Render()))
			{
				MSG_BOX("Failed to Render");
			}
		}

		Safe_Release(pGameObject);
	}

	m_RenderObjects[RG_Priority].clear();

	return S_OK;
}

HRESULT CRenderer::Render_Shadow()
{
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Shadow"), m_pShadowDSV)))
	{
		return E_FAIL;
	}

	_uint iNumViewPorts{ 1 };

	D3D11_VIEWPORT OldViewportDesc{};

	m_pContext->RSGetViewports(&iNumViewPorts, &OldViewportDesc);

	D3D11_VIEWPORT TempViewPortDesc{};
	TempViewPortDesc.TopLeftX = 0;
	TempViewPortDesc.TopLeftY = 0;
	TempViewPortDesc.Width = static_cast<_float>(D3D11_REQ_TEXTURE2D_U_OR_V_DIMENSION);
	TempViewPortDesc.Height = static_cast<_float>(D3D11_REQ_TEXTURE2D_U_OR_V_DIMENSION * 0.5f);
	TempViewPortDesc.MinDepth = 0.f;
	TempViewPortDesc.MaxDepth = 1.f;

	m_pContext->RSSetViewports(iNumViewPorts, &TempViewPortDesc);

	for (auto& pGameObject : m_RenderObjects[RG_Shadow])
	{
		if (pGameObject)
		{
			if (FAILED(pGameObject->Render_Shadow()))
			{
				MSG_BOX("Failed to Render");
			}
		}

		Safe_Release(pGameObject);
	}

	m_RenderObjects[RG_Shadow].clear();

	if (FAILED(m_pGameInstance->End_MRT()))
	{
		return E_FAIL;
	}

	m_pContext->RSSetViewports(iNumViewPorts, &OldViewportDesc);

	return S_OK;
}

HRESULT CRenderer::Render_NonBlend()
{
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_GameObjects"))))
	{
		return E_FAIL;
	}

	for (auto& pGameObject : m_RenderObjects[RG_NonBlend])
	{
		if (pGameObject)
		{
			if (FAILED(pGameObject->Render()))
			{
				MSG_BOX("Failed to Render");
			}
		}

		Safe_Release(pGameObject);
	}

	m_RenderObjects[RG_NonBlend].clear();

	if (FAILED(m_pGameInstance->End_MRT()))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CRenderer::Render_Refraction()
{
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Refraction"))))
		return E_FAIL;

	for (auto& pGameObject : m_RenderObjects[RG_Water_Refraction])
	{
		if (pGameObject)
		{
			if (FAILED(pGameObject->Render_Refract()))
				MSG_BOX("Failed to Refraction Render");
		}

		Safe_Release(pGameObject);
	}

	m_RenderObjects[RG_Water_Refraction].clear();

	if (FAILED(m_pGameInstance->End_MRT()))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CRenderer::Render_Reflection()
{
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Reflection"))))
		return E_FAIL;

	for (auto& pGameObject : m_RenderObjects[RG_Water_Reflection])
	{
		if (pGameObject)
		{
			if (FAILED(pGameObject->Render_Reflection()))
				MSG_BOX("Failed to Reflection Render");
		}

		Safe_Release(pGameObject);
	}

	m_RenderObjects[RG_Water_Reflection].clear();

	if (FAILED(m_pGameInstance->End_MRT()))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CRenderer::Render_Water()
{
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Water"))))
		return E_FAIL;

	for (auto& pGameObject : m_RenderObjects[RG_Water])
	{
		if (pGameObject)
		{
			if (FAILED(pGameObject->Render()))
				MSG_BOX("Failed to Water Render");
		}

		Safe_Release(pGameObject);
	}

	m_RenderObjects[RG_Water].clear();

	if (FAILED(m_pGameInstance->End_MRT()))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CRenderer::Render_LightAcc()
{
#pragma region MRT_Lights

	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Lights"))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Bind_ShaderResourceView(m_pShader, "g_NormalTexture", TEXT("Target_Normal"))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Bind_ShaderResourceView(m_pShader, "g_DepthTexture", TEXT("Target_Depth"))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Bind_ShaderResourceView(m_pShader, "g_ObjectSpecTexture", TEXT("Target_Object_Specular"))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShader->Bind_RawValue("g_vCamPosition", &m_pGameInstance->Get_CameraPos(), sizeof _float4)))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShader->Bind_RawValue("g_vCamNF", &m_pGameInstance->Get_CameraNF(), sizeof _float2)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", m_WorldMatrix)))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", m_ViewMatrix)))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", m_ProjMatrix)))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShader->Bind_RawValue("g_vRandom", m_vRandom, sizeof(_vec3) * 50)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrixInv", m_pGameInstance->Get_Transform_Inversed(TransformType::View))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrixInv", m_pGameInstance->Get_Transform_Inversed(TransformType::Proj))))
	{
		return E_FAIL;
	}


	if (FAILED(m_pGameInstance->Render_Lights(m_pGameInstance->Get_CurrentLevelIndex(), m_pShader, m_pVIBuffer)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->End_MRT()))
	{
		return E_FAIL;
	}

#pragma endregion

#pragma region SSAO(Still Test...)

	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_SSAOTEST"))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Bind_ShaderResourceView(m_pShader, "g_NormalTexture", TEXT("Target_Normal"))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Bind_ShaderResourceView(m_pShader, "g_DepthTexture", TEXT("Target_Depth"))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShader->Begin(DefPass_SSAO)))
	{
		return E_FAIL;
	}
	if (FAILED(m_pVIBuffer->Render()))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->End_MRT()))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_SSAOBlur"))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Bind_ShaderResourceView(m_pShader, "g_BlurTexture", TEXT("Target_SSAOTEST"))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShader->Begin(DefPass_Blur)))
	{
		return E_FAIL;
	}
	if (FAILED(m_pVIBuffer->Render()))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->End_MRT()))
	{
		return E_FAIL;
	}

#pragma endregion

	return S_OK;
}

HRESULT CRenderer::Render_Deferred()
{
	if (FAILED(m_pGameInstance->Bind_ShaderResourceView(m_pShader, "g_DiffuseTexture", TEXT("Target_Diffuse"))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Bind_ShaderResourceView(m_pShader, "g_NormalTexture", TEXT("Target_Normal"))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Bind_ShaderResourceView(m_pShader, "g_ShadeTexture", TEXT("Target_Shade"))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Bind_ShaderResourceView(m_pShader, "g_SpecularTexture", TEXT("Target_Specular"))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Bind_ShaderResourceView(m_pShader, "g_DepthTexture", TEXT("Target_Depth"))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Bind_ShaderResourceView(m_pShader, "g_LightDepthTexture", TEXT("Target_LightDepth"))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Bind_ShaderResourceView(m_pShader, "g_SSAOTexture", TEXT("Target_SSAOBlur"))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Bind_ShaderResourceView(m_pShader, "g_VelocityTexture", TEXT("Target_Velocity"))))
		return E_FAIL;

	_uint iNumViewPorts{ 1 };

	D3D11_VIEWPORT ViewportDesc{};

	m_pContext->RSGetViewports(&iNumViewPorts, &ViewportDesc);


	if (FAILED(m_pShader->Bind_RawValue("g_fScreenWidth", &ViewportDesc.Width, sizeof _float)))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShader->Bind_RawValue("g_fScreenHeight", &ViewportDesc.Height, sizeof _float)))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShader->Bind_RawValue("g_fHellStart", &m_pGameInstance->Get_HellHeight(), sizeof _float)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShader->Bind_RawValue("g_vCamNF", &m_pGameInstance->Get_CameraNF(), sizeof _float2)))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShader->Bind_RawValue("g_vFogNF", &m_pGameInstance->Get_FogNF(), sizeof _float2)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", m_WorldMatrix)))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", m_ViewMatrix)))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", m_ProjMatrix)))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrixInv", m_pGameInstance->Get_Transform_Inversed(TransformType::View))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrixInv", m_pGameInstance->Get_Transform_Inversed(TransformType::Proj))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Bind_Light_ViewProjMatrix(m_pGameInstance->Get_CurrentLevelIndex(), TEXT("Light_Main"), m_pShader, "g_LightViewMatrix", "g_LightProjMatrix")))
	{
		//return E_FAIL;
	}

	if (m_pGameInstance->Key_Down(DIK_F2))
		m_TurnOnSSAO = !m_TurnOnSSAO;

	// 원명
	if (FAILED(m_pShader->Bind_RawValue("TurnOnSSAO", &m_TurnOnSSAO, sizeof(_bool))))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_RawValue("g_SSAO", &m_SSAO, sizeof(SSAO_DESC))))
		return E_FAIL;

	//if (FAILED(m_pShader->Bind_RawValue("TurnOnThunder", &m_Thunder, sizeof(_bool))))
	//	return E_FAIL;

	if (FAILED(m_pShader->Begin(DefPass_Deferred)))
	{
		return E_FAIL;
	}
	if (FAILED(m_pVIBuffer->Render()))
	{
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CRenderer::Render_Blur()
{
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Blur"))))
	{
		return E_FAIL;
	}

	for (auto& pGameObject : m_RenderObjects[RG_Blur])
	{
		if (pGameObject)
		{
			if (FAILED(pGameObject->Render()))
			{
				MSG_BOX("Failed to Render");
			}
		}

		Safe_Release(pGameObject);
	}

	m_RenderObjects[RG_Blur].clear();

	if (FAILED(m_pGameInstance->End_MRT()))
	{
		return E_FAIL;
	}

	//if (FAILED(m_pGameInstance->Bind_ShaderResourceView(m_pShader, "g_BlurTexture", TEXT("Target_Bloom"))))
	//{
	//	return E_FAIL;
	//}

	//if (FAILED(m_pShader->Begin(DefPass_Blur)))
	//{
	//	return E_FAIL;
	//}
	//if (FAILED(m_pVIBuffer->Render()))
	//{
	//	return E_FAIL;
	//}

	return S_OK;
}

HRESULT CRenderer::Render_NonLight()
{

	for (auto& pGameObject : m_RenderObjects[RG_NonLight])
	{
		if (pGameObject)
		{
			if (FAILED(pGameObject->Render()))
			{
				MSG_BOX("Failed to Render");
			}
		}

		Safe_Release(pGameObject);
	}

	m_RenderObjects[RG_NonLight].clear();

	return S_OK;
}

HRESULT CRenderer::Render_Blend()
{

	m_RenderObjects[RG_Blend].sort([](CGameObject* pSrc, CGameObject* pDst)
	{
		return dynamic_cast<CBlendObject*>(pSrc)->Get_CamDistance() > dynamic_cast<CBlendObject*>(pDst)->Get_CamDistance();
	});

	for (auto& pGameObject : m_RenderObjects[RG_Blend])
	{
		if (pGameObject)
		{
			if (FAILED(pGameObject->Render()))
			{
				MSG_BOX("Failed to Render");
			}
		}

		Safe_Release(pGameObject);
	}

	m_RenderObjects[RG_Blend].clear();

	return S_OK;
}

HRESULT CRenderer::Render_BlendBlur()
{
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Blur"))))
	{
		return E_FAIL;
	}

	m_RenderObjects[RG_BlendBlur].sort([](CGameObject* pSrc, CGameObject* pDst)
	{
		return dynamic_cast<CBlendObject*>(pSrc)->Get_CamDistance() > dynamic_cast<CBlendObject*>(pDst)->Get_CamDistance();
	});

	for (auto& pGameObject : m_RenderObjects[RG_BlendBlur])
	{
		if (pGameObject)
		{
			if (FAILED(pGameObject->Render()))
			{
				MSG_BOX("Failed to Render");
			}
		}

		Safe_Release(pGameObject);
	}

	m_RenderObjects[RG_BlendBlur].clear();

	if (FAILED(m_pGameInstance->End_MRT()))
	{
		return E_FAIL;
	}

	/*if (FAILED(m_pGameInstance->Bind_ShaderResourceView(m_pShader, "g_BlurTexture", TEXT("Target_Bloom"))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShader->Begin(DefPass_Blur)))
	{
		return E_FAIL;
	}
	if (FAILED(m_pVIBuffer->Render()))
	{
		return E_FAIL;
	}*/

	return S_OK;
}

HRESULT CRenderer::Render_HDR()
{
	// 원명
	{
		constexpr ID3D11ShaderResourceView* NULL_SRV = nullptr;
		constexpr ID3D11UnorderedAccessView* NULL_UAV = nullptr;
		constexpr _uint iNo_Offset = -1;

		THPARAM thParam = { 0.5f };
		D3D11_MAPPED_SUBRESOURCE ms;
		m_pContext->Map(m_pDSBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &ms);
		memcpy(ms.pData, &thParam, sizeof(THPARAM));
		m_pContext->Unmap(m_pDSBuffer, 0);

		// 다운 스케일링
		ID3D11ShaderResourceView* pSRV = nullptr;

		pSRV = m_pGameInstance->Get_SRV(L"Target_HDR");

		m_pContext->CSSetShader(m_pDSCShader, nullptr, 0);

		m_pContext->CSSetShaderResources(0, 1, &pSRV);

		m_pContext->CSSetUnorderedAccessViews(0, 1, &m_pDSUAV, &iNo_Offset);

		m_pContext->CSSetConstantBuffers(0, 1, &m_pDSBuffer);

		m_pContext->Dispatch(fWidth / 16, fHeight / 16, 1);

		m_pContext->CSSetShaderResources(0, 1, &NULL_SRV);
		m_pContext->CSSetUnorderedAccessViews(0, 1, &NULL_UAV, &iNo_Offset);

		ID3D11ShaderResourceView* pSRVs[2] = { m_pDSSRV, m_pBlurSRV };
		ID3D11UnorderedAccessView* pUAVs[2] = { m_pBlurUAV, m_pDSUAV };

		for (size_t i = 0; i < 2; i++)
		{
			m_BLParam.direction = i;
			D3D11_MAPPED_SUBRESOURCE Bms;
			m_pContext->Map(m_pBlurBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &Bms);
			memcpy(Bms.pData, &m_BLParam, sizeof(BLURPARAM));
			m_pContext->Unmap(m_pBlurBuffer, 0);

			// 블러
			m_pContext->CSSetShader(m_pBlurCShader, nullptr, 0);

			m_pContext->CSSetShaderResources(0, 1, &pSRVs[i]);

			m_pContext->CSSetUnorderedAccessViews(0, 1, &pUAVs[i], &iNo_Offset);

			m_pContext->CSSetConstantBuffers(0, 1, &m_pBlurBuffer);

			m_pContext->Dispatch(fWidth / 16.f, fHeight / 16.f, 1);

			m_pContext->CSSetShaderResources(0, 1, &NULL_SRV);
			m_pContext->CSSetUnorderedAccessViews(0, 1, &NULL_UAV, &iNo_Offset);
		}

	}

	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", m_WorldMatrix)))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", m_ViewMatrix)))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", m_ProjMatrix)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Bind_ShaderResourceView(m_pShader, "g_HDRTexture", TEXT("Target_HDR"))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShader->Bind_ShaderResourceView("g_TestBlurTexture", m_pDSSRV)))
		return E_FAIL;

	if (m_pGameInstance->Key_Down(DIK_F3))
		m_TurnOnToneMap = !m_TurnOnToneMap;


	if (m_pGameInstance->Key_Down(DIK_F4))
		m_TurnOnBlur = !m_TurnOnBlur;
	

	if (FAILED(m_pShader->Bind_RawValue("TurnOnToneMap", &m_TurnOnToneMap, sizeof(_bool))))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_RawValue("TurnOnBlur", &m_TurnOnBlur, sizeof(_bool))))
		return E_FAIL;

	if (FAILED(m_pShader->Begin(DefPass_HDR)))
	{
		return E_FAIL;
	}
	if (FAILED(m_pVIBuffer->Render()))
	{
		return E_FAIL;
	}
	

	return S_OK;
}

HRESULT CRenderer::Render_UI()
{
	m_RenderObjects[RG_UI].sort([](CGameObject* pSrc, CGameObject* pDst)
	{
		return dynamic_cast<COrthographicObject*>(pSrc)->Get_Depth() > dynamic_cast<COrthographicObject*>(pDst)->Get_Depth();
	});

	for (auto& pGameObject : m_RenderObjects[RG_UI])
	{
		if (pGameObject)
		{
			if (FAILED(pGameObject->Render()))
			{
				MSG_BOX("Failed to Render");
			}
		}

		Safe_Release(pGameObject);
	}

	m_RenderObjects[RG_UI].clear();

	return S_OK;
}

#ifdef _DEBUGTEST
HRESULT CRenderer::Render_Debug()
{
	for (auto& pComponent : m_DebugComponents)
	{
		pComponent->Render();
		Safe_Release(pComponent);
	}
	m_DebugComponents.clear();

	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", m_ViewMatrix)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", m_ProjMatrix)))
	{
		return E_FAIL;
	}
	
	
	if (FAILED(m_pGameInstance->Render_Debug_RT(TEXT("MRT_GameObjects"), m_pShader, m_pVIBuffer)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Render_Debug_RT(TEXT("MRT_Lights"), m_pShader, m_pVIBuffer)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Render_Debug_RT(TEXT("MRT_Shadow"), m_pShader, m_pVIBuffer)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Render_Debug_RT(TEXT("MRT_Blur"), m_pShader, m_pVIBuffer)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Render_Debug_RT(TEXT("MRT_BlurTest"), m_pShader, m_pVIBuffer)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Render_Debug_RT(TEXT("MRT_SSAOTEST"), m_pShader, m_pVIBuffer)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Render_Debug_RT(TEXT("MRT_SSAOBlur"), m_pShader, m_pVIBuffer)))
	{
		return E_FAIL;
	}
	
	return S_OK;
}
#endif // _DEBUG


CRenderer* CRenderer::Create(_dev pDevice, _context pContext)
{
	CRenderer* pInstance = new CRenderer(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CRenderer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CRenderer::Clone(void* pArg)
{
	AddRef();

	return this;
}

void CRenderer::Free()
{
	__super::Free();

	Safe_Release(m_pDSBuffer);
	Safe_Release(m_pDSBlob);
	Safe_Release(m_pDSCShader);
	Safe_Release(m_pDSTexture);
	Safe_Release(m_pDSUAV);
	Safe_Release(m_pDSSRV);

	Safe_Release(m_pBlurBuffer);
	Safe_Release(m_pBlurBlob);
	Safe_Release(m_pBlurCShader);
	Safe_Release(m_pBlurTexture);
	Safe_Release(m_pBlurUAV);
	Safe_Release(m_pBlurSRV);

	Safe_Release(m_pShader);
	Safe_Release(m_pVIBuffer);
	Safe_Release(m_pShadowDSV);

	for (auto& ObjectList : m_RenderObjects)
	{
		for (auto& pGameObject : ObjectList)
		{
			Safe_Release(pGameObject);
		}
		ObjectList.clear();
	}

#ifdef _DEBUGTEST
	for (auto& pComponent : m_DebugComponents)
	{
		Safe_Release(pComponent);
	}
	m_DebugComponents.clear();
#endif // _DEBUG


}