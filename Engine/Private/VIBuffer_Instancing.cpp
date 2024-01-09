#include "VIBuffer_Instancing.h"

CVIBuffer_Instancing::CVIBuffer_Instancing(_dev pDevice, _context pContext)
	: CVIBuffer(pDevice, pContext)
{
}

CVIBuffer_Instancing::CVIBuffer_Instancing(const CVIBuffer_Instancing& rhs)
	: CVIBuffer(rhs)
	, m_InstancingBufferDesc(rhs.m_InstancingBufferDesc)
	, m_InstancingInitialData(rhs.m_InstancingInitialData)
	, m_iNumInstances(rhs.m_iNumInstances)
	, m_iIndexCountPerInstance(rhs.m_iIndexCountPerInstance)
	, m_iInstanceStride(rhs.m_iInstanceStride)
{
}

HRESULT CVIBuffer_Instancing::Init_Prototype()
{
	return S_OK;
}

HRESULT CVIBuffer_Instancing::Init(void* pArg)
{
	return S_OK;
}

void CVIBuffer_Instancing::Update(_float fTimeDelta, _int iNumUse)
{
	if (iNumUse == -1)
	{
		iNumUse = m_iNumInstances;
	}
	D3D11_MAPPED_SUBRESOURCE SubResource{};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	for (size_t i = 0; i < m_iNumInstances; i++)
	{
		VTXINSTANCING* pVertex = &reinterpret_cast<VTXINSTANCING*>(SubResource.pData)[i];
		//pVertex->vPos.y += pVertex->fSpeed * fTimeDelta;
		XMStoreFloat4(&pVertex->vPos, XMLoadFloat4(&pVertex->vPos) + XMLoadFloat4(&pVertex->vDirection) * pVertex->fSpeed * fTimeDelta);
		pVertex->vLifeTime.x += fTimeDelta;

		if (i >= iNumUse)
		{
			pVertex->vPos = _float4(-1000.f, -1000.f, -1000.f, 1.f);
			continue;
		}

		if (pVertex->vLifeTime.x > pVertex->vLifeTime.y)
		{
			if (m_isLoop)
			{
				pVertex->vLifeTime.x = 0.f;
				pVertex->vPos = pVertex->vOriginPos;
			}
			else
			{
				pVertex->vPos = _float4(0.f, -100.f, 0.f, 1.f);
			}
		}
	}

	m_pContext->Unmap(m_pVBInstance, 0);
}

HRESULT CVIBuffer_Instancing::Render()
{
	ID3D11Buffer* pVertexBuffer[] =
	{
		m_pVB,
		m_pVBInstance,
	};

	_uint iVertexStrides[] =
	{
		m_iVertexStride,
		m_iInstanceStride,
	};

	_uint iOffsets[] =
	{
		0,
		0,
	};

	m_pContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pVertexBuffer, iVertexStrides, iOffsets);

	m_pContext->IASetIndexBuffer(m_pIB, m_eIndexFormat, 0);

	m_pContext->IASetPrimitiveTopology(m_ePrimitiveTopology);

	m_pContext->DrawIndexedInstanced(m_iIndexCountPerInstance, m_iNumInstances, 0, 0, 0);

	return S_OK;
}

void CVIBuffer_Instancing::Free()
{
	__super::Free();

	Safe_Release(m_pVBInstance);
}
