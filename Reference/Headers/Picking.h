#pragma once
#include "Base.h"

BEGIN(Engine);

class CPicking final : public CBase
{
private:
	CPicking(HWND hWnd, _uint iWinSizeX, _uint iWinSizeY);
	virtual ~CPicking() = default;

public:
	void Tick();

public:
	void TransformRay_ToLocal(_fmatrix WorldMatrix);

	_bool Picking_InWorld(_fvector vPoint1, _fvector vPoint2, _fvector vPoint3, _Inout_ _float3* pPickPos);
	_bool Picking_InLocal(_fvector vPoint1, _fvector vPoint2, _fvector vPoint3, _Inout_ _float4* pPickPos);
	_bool Picking_InLocal(_fvector vPoint1, _fvector vPoint2, _fvector vPoint3, _gvector vNormal, _Inout_ _float4* pPickPos);

private:
	HWND m_hWnd{};
	_uint m_iWinSizeX{}, m_iWinSizeY{};
	class CGameInstance* m_pGameInstance{ nullptr };

	_float4 m_vRayPos_World{};
	_float4 m_vRayDir_World{};

	_float4 m_vRayPos_Local{};
	_float4 m_vRayDir_Local{};

public:
	static CPicking* Create(HWND hWnd, _uint iWinSizeX, _uint iWinSizeY);
	virtual void Free() override;
};

END