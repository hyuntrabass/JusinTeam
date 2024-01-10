#pragma once
#include "Base.h"

BEGIN(Engine);

class CPicking final : public CBase
{
private:
	CPicking(_dev pDevice, _context pContext, HWND hWnd, _uint iWinSizeX, _uint iWinSizeY);
	virtual ~CPicking() = default;

public:
	void Tick();

public:
	void TransformRay_ToLocal(_fmatrix WorldMatrix);

<<<<<<< HEAD
	_bool Picking_InWorld(_fvector vPoint1, _fvector vPoint2, _fvector vPoint3, _Inout_ _float3* pPickPos);
	_bool Picking_InLocal(_fvector vPoint1, _fvector vPoint2, _fvector vPoint3, _Inout_ _float4* pPickPos);
	_bool Picking_InLocal(_fvector vPoint1, _fvector vPoint2, _fvector vPoint3, _gvector vNormal, _Inout_ _float4* pPickPos);
	_float4 PickingDepth(_float x, _float y);
	HRESULT Ready_Texture2D();
=======
	_bool Picking_InWorld(_vec4 vPoint1, _vec4 vPoint2, _vec4 vPoint3, _Inout_ _vec3* pPickPos);
	_bool Picking_InLocal(_vec4 vPoint1, _vec4 vPoint2, _vec4 vPoint3, _Inout_ _vec4* pPickPos);
	_bool Picking_InLocal(_vec4 vPoint1, _vec4 vPoint2, _vec4 vPoint3, _vec4 vNormal, _Inout_ _vec4* pPickPos);
>>>>>>> master

private:
	_dev m_pDevice{ nullptr };
	_context m_pContext{ nullptr };
	HWND m_hWnd{};
	_uint m_iWinSizeX{}, m_iWinSizeY{};
	class CGameInstance* m_pGameInstance{ nullptr };

	_vec4 m_vRayPos_World{};
	_vec4 m_vRayDir_World{};

	_vec4 m_vRayPos_Local{};
	_vec4 m_vRayDir_Local{};

	ID3D11Texture2D* m_pTexture{ nullptr };

public:
	static CPicking* Create(_dev pDevice, _context pContext, HWND hWnd, _uint iWinSizeX, _uint iWinSizeY);
	virtual void Free() override;
};

END