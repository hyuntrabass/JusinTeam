#include "Picking.h"
#include "GameInstance.h"

CPicking::CPicking(HWND hWnd, _uint iWinSizeX, _uint iWinSizeY)
	: m_hWnd(hWnd)
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

CPicking* CPicking::Create(HWND hWnd, _uint iWinSizeX, _uint iWinSizeY)
{
	return new CPicking(hWnd, iWinSizeX, iWinSizeY);
}

void CPicking::Free()
{
	Safe_Release(m_pGameInstance);
}
