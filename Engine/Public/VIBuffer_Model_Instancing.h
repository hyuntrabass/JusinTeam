#pragma once
#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Model_Instancing abstract : public CVIBuffer
{
protected:
	CVIBuffer_Model_Instancing(_dev pDevice, _context pContext);
	CVIBuffer_Model_Instancing(const CVIBuffer_Model_Instancing& rhs);
	virtual ~CVIBuffer_Model_Instancing() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	virtual void Update(_float fTimeDelta, _mat WorldMatrix);
		virtual HRESULT Render() override;

protected:
	D3D11_BUFFER_DESC m_InstancingBufferDesc{};
	D3D11_SUBRESOURCE_DATA m_InstancingInitialData{};

	ID3D11Buffer* m_pVBInstance{ nullptr };

	_uint m_iNumInstances{};
	_uint m_iIndexCountPerInstance{};
	_uint m_iInstanceStride{};

public:
	virtual CComponent* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END