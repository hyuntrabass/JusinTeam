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

	_bool Picking_InWorld(_vec4 vPoint1, _vec4 vPoint2, _vec4 vPoint3, _Inout_ _vec3* pPickPos);
	_bool Picking_InLocal(_vec4 vPoint1, _vec4 vPoint2, _vec4 vPoint3, _Inout_ _vec4* pPickPos);
	_bool Picking_InLocal(_vec4 vPoint1, _vec4 vPoint2, _vec4 vPoint3, _vec4 vNormal, _Inout_ _vec4* pPickPos);

private:
	HWND m_hWnd{};
	_uint m_iWinSizeX{}, m_iWinSizeY{};
	class CGameInstance* m_pGameInstance{ nullptr };

	_vec4 m_vRayPos_World{};
	_vec4 m_vRayDir_World{};

	_vec4 m_vRayPos_Local{};
	_vec4 m_vRayDir_Local{};

public:
	static CPicking* Create(HWND hWnd, _uint iWinSizeX, _uint iWinSizeY);
	virtual void Free() override;
};

END