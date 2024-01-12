#pragma once
#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer abstract : public CComponent
{
protected:
	CVIBuffer(_dev pDevice, _context pContext);
	CVIBuffer(const CVIBuffer& rhs);
	virtual ~CVIBuffer() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	virtual HRESULT Render() override;

protected:
	ID3D11Buffer* m_pVB{ nullptr };
	ID3D11Buffer* m_pIB{ nullptr };

	_uint m_iNumVertexBuffers{};

	_uint m_iVertexStride{};
	_uint m_iNumVertices{};

	_uint m_iIndexStride{};
	_uint m_iNumIndices{};

	DXGI_FORMAT m_eIndexFormat{};
	D3D11_PRIMITIVE_TOPOLOGY m_ePrimitiveTopology{};

	D3D11_BUFFER_DESC m_BufferDesc{};
	D3D11_SUBRESOURCE_DATA m_InitialData{};

protected:
	HRESULT Create_Buffer(ID3D11Buffer** ppBuffer);

public:
	virtual void Free() override;
};

END