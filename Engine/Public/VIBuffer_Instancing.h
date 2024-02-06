#pragma once
#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Instancing abstract : public CVIBuffer
{
//public:
	//typedef struct tagMeshDesc
	//{
	//	_float3		vMinPosition, vMaxPosition;
	//	_float2		vScaleRange;
	//	_mat		mWorldMatrix;
	//}MESH_DESC;

protected:
	CVIBuffer_Instancing(_dev pDevice, _context pContext);
	CVIBuffer_Instancing(const CVIBuffer_Instancing& rhs);
	virtual ~CVIBuffer_Instancing() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	virtual void Update(_float fTimeDelta, _mat WorldMatrix, _int iNumUse = -1, _bool bApplyGravity = false, _vec3 vGravityDir = _vec3(), _float fAppearRatio = 0.f, _float fDissolveRatio = 0.8f);
	virtual HRESULT Render() override;

protected:
	D3D11_BUFFER_DESC m_InstancingBufferDesc{};
	D3D11_SUBRESOURCE_DATA m_InstancingInitialData{};

	ID3D11Buffer* m_pVBInstance{ nullptr };

	_uint m_iNumInstances{};
	_uint m_iIndexCountPerInstance{};
	_uint m_iInstanceStride{};


	_bool m_isLoop{};

public:
	virtual CComponent* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END