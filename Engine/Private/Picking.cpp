#include "Picking.h"
#include "GameInstance.h"

CPicking::CPicking(_dev pDevice, _context pContext, HWND hWnd, _uint iWinSizeX, _uint iWinSizeY)
	:m_pDevice(pDevice)
	, m_pContext(pContext)
	, m_hWnd(hWnd)
	, m_iWinSizeX(iWinSizeX)
	, m_iWinSizeY(iWinSizeY)
	, m_pGameInstance(CGameInstance::Get_Instance())
{
	Safe_AddRef(m_pGameInstance);
}

void CPicking::Tick()
{
	// 뷰포트 상의 커서 위치
	POINT ptCursor{};
	GetCursorPos(&ptCursor);
	ScreenToClient(m_hWnd, &ptCursor);

	// 투영 스페이스 상의 위치
	_vector vCursorPos
	{
		XMVectorSet
		(
			ptCursor.x / (m_iWinSizeX * 0.5f) - 1.f,
			ptCursor.y / (m_iWinSizeY * -0.5f) + 1.f,
			0.f,
			1.f
		)
	};

	// 뷰 스페이스 상의 위치
	_matrix ProjMatrix_Inverse{ m_pGameInstance->Get_Transform_Inversed(TransformType::Proj) };
	vCursorPos = XMVector4Transform(vCursorPos, ProjMatrix_Inverse);

	_vector vRayPos{ XMVectorSet(0.f, 0.f, 0.f, 1.f) };
	_vector vRayDir{ XMVectorSet(vCursorPos.m128_f32[0], vCursorPos.m128_f32[1], 1.f, 0.f) };
	//_vector vRayDir{ vCursorPos - vRayPos }; 

	// 월드 스페이스 상의 위치
	_matrix ViewMat_Inverse{ m_pGameInstance->Get_Transform_Inversed(TransformType::View) };
	XMStoreFloat4(&m_vRayPos_World, XMVector4Transform(vRayPos, ViewMat_Inverse));
	XMStoreFloat4(&m_vRayDir_World, XMVector4Normalize(XMVector4Transform(vRayDir, ViewMat_Inverse)));
}

void CPicking::TransformRay_ToLocal(_fmatrix WorldMatrix)
{
	_matrix InversedWorld{ XMMatrixInverse(nullptr, WorldMatrix) };

	XMStoreFloat4(&m_vRayPos_Local, XMVector4Transform(XMLoadFloat4(&m_vRayPos_World), InversedWorld));
	XMStoreFloat4(&m_vRayDir_Local, XMVector4Normalize(XMVector4Transform(XMLoadFloat4(&m_vRayDir_World), InversedWorld)));
}

_bool CPicking::Picking_InWorld(_fvector vPoint1, _fvector vPoint2, _fvector vPoint3, _Inout_ _float3* pPickPos)
{
	_float fDist{};
	_vector vRayPos{ XMLoadFloat4(&m_vRayPos_World) };
	_vector vRayDir{ XMLoadFloat4(&m_vRayDir_World) };
	if (TriangleTests::Intersects(vRayPos, vRayDir, vPoint1, vPoint2, vPoint3, fDist))
	{
		XMStoreFloat3(pPickPos, vRayPos + (vRayDir * fDist));
		return true;
	}
	else
	{
		return false;
	}
}

_float4 CPicking::PickingDepth(_float x, _float y)
{
	ID3D11Texture2D* pTexture = nullptr;

	pTexture = m_pGameInstance->Get_Texture2D(L"Target_Depth");

	if (nullptr == pTexture) {
		MSG_BOX("Get Target_Depth FAILED");
		return _float4(0.f, 0.f, 0.f, 0.f);
	}

	D3D11_BOX m_Box;
	m_Box.left = x;
	m_Box.top = y;
	m_Box.right = x + 1;
	m_Box.bottom = y + 1;
	m_Box.front = 0;
	m_Box.back = 1;

	m_pContext->CopySubresourceRegion(m_pTexture, 0, 0, 0, 0, pTexture, 0, &m_Box);

	D3D11_MAPPED_SUBRESOURCE MappedResource = {};

	if (FAILED(m_pContext->Map(m_pTexture, 0, D3D11_MAP_READ, 0, &MappedResource)))
		return _float4(0.f, 0.f, 0.f, 0.f);


	if (nullptr == MappedResource.pData) {
		m_pContext->Unmap(m_pTexture, 0);
		return _float4(0.f, 0.f, 0.f, 0.f);

	}

	_float4 PickPos = ((_float4*)MappedResource.pData)[0];

	m_pContext->Unmap(m_pTexture, 0);

	_float fX = (_float(x) / m_iWinSizeX) * 2.f - 1.f;
	_float fY = (_float(y) / m_iWinSizeY) * -2.f + 1.f;

	_vector vWorldPos = XMVectorSet(fX, fY, PickPos.x, 1.f);

	vWorldPos = vWorldPos * PickPos.y * m_pGameInstance->Get_CameraNF().y;

	vWorldPos = XMVector4Transform(vWorldPos, m_pGameInstance->Get_Transform_Inversed(TransformType::Proj));

	vWorldPos = XMVector4Transform(vWorldPos, m_pGameInstance->Get_Transform_Inversed(TransformType::View));

	_float4 WorldPos = {};

	XMStoreFloat4(&WorldPos, vWorldPos);

	return WorldPos;
}

HRESULT CPicking::Ready_Texture2D()
{
	D3D11_TEXTURE2D_DESC Desc{};
	Desc.Width = 1;
	Desc.Height = 1;
	Desc.MipLevels = 1;
	Desc.ArraySize = 1;
	Desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;

	Desc.SampleDesc.Quality = 0;
	Desc.SampleDesc.Count = 1;

	Desc.Usage = D3D11_USAGE_STAGING;
	Desc.BindFlags = 0;
	Desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
	Desc.MiscFlags = 0;

	if (FAILED(m_pDevice->CreateTexture2D(&Desc, nullptr, &m_pTexture)))
	{
		return E_FAIL;
	}

	return E_NOTIMPL;
}

_bool CPicking::Picking_InLocal(_fvector vPoint1, _fvector vPoint2, _fvector vPoint3, _Inout_ _float4* pPickPos)
{
	_float fDist{};
	_vector vRayPos{ XMLoadFloat4(&m_vRayPos_Local) };
	_vector vRayDir{ XMLoadFloat4(&m_vRayDir_Local) };
	if (TriangleTests::Intersects(vRayPos, vRayDir, vPoint1, vPoint2, vPoint3, fDist))
	{
		XMStoreFloat4(pPickPos, vRayPos + (vRayDir * fDist));
		return true;
	}
	else
	{
		return false;
	}
}

_bool CPicking::Picking_InLocal(_fvector vPoint1, _fvector vPoint2, _fvector vPoint3, _gvector vNormal, _Inout_ _float4* pPickPos)
{
	if (XMVector4Dot(vNormal, XMLoadFloat4(&m_vRayDir_Local)).m128_f32[0] > 0)
	{
		return false;
	}

	_float fDist{};
	_vector vRayPos{ XMLoadFloat4(&m_vRayPos_Local) };
	_vector vRayDir{ XMLoadFloat4(&m_vRayDir_Local) };
	if (TriangleTests::Intersects(vRayPos, vRayDir, vPoint1, vPoint2, vPoint3, fDist))
	{
		XMStoreFloat4(pPickPos, vRayPos + (vRayDir * fDist));
		return true;
	}
	else
	{
		return false;
	}
}

CPicking* CPicking::Create(_dev pDevice, _context pContext, HWND hWnd, _uint iWinSizeX, _uint iWinSizeY)
{
	return new CPicking(pDevice, pContext, hWnd, iWinSizeX, iWinSizeY);
}

void CPicking::Free()
{
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pTexture);
}
