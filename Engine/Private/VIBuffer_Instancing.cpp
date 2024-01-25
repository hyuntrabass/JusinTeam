#include "VIBuffer_Instancing.h"
#include "GameInstance.h"

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

void CVIBuffer_Instancing::Update(_float fTimeDelta, _mat WorldMatrix, _int iNumUse, _bool bApplyGravity, _vec3 vGravityDir)
{
	if (iNumUse == -1)
	{
		iNumUse = m_iNumInstances;
	}
	D3D11_MAPPED_SUBRESOURCE SubResource{};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	list<VTXINSTANCING> VertexList{};

	for (size_t i = 0; i < m_iNumInstances; i++)
	{
		VTXINSTANCING* pVertex = &reinterpret_cast<VTXINSTANCING*>(SubResource.pData)[i];

		pVertex->vPrevPos = pVertex->vPos;

		if (pVertex->vLifeTime.x == 0)
		{
			_vec4 Test = _vec4::UnitX;
			pVertex->vPos = _vec4::Transform(pVertex->vPos, WorldMatrix);
		}

		//pVertex->vPos.y += pVertex->fSpeed * fTimeDelta;
		if (bApplyGravity)
		{
			_float fAlpha = (pVertex->vLifeTime.x / pVertex->vLifeTime.y) * 0.7f;
			pVertex->vDirection = _vec4::Lerp(pVertex->vDirection, _vec4(vGravityDir), fAlpha);
		}
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
				pVertex->vPos = _float4(0.f, -10000.f, 0.f, 1.f);
			}
		}
		VertexList.push_back(*pVertex);
	}

	_vec4 vCamPos = CGameInstance::Get_Instance()->Get_CameraPos();

	VertexList.sort([&WorldMatrix, &vCamPos](VTXINSTANCING pSrc, VTXINSTANCING pDst)
	{
		_float fSrcDist = (_vec4::Transform(_vec4(pSrc.vPos), WorldMatrix) - vCamPos).Length();
		_float fDstDist = (_vec4::Transform(_vec4(pDst.vPos), WorldMatrix) - vCamPos).Length();
		return fSrcDist > fDstDist;
	});

	for (size_t i = 0; i < iNumUse; i++)
	{
		reinterpret_cast<VTXINSTANCING*>(SubResource.pData)[i] = VertexList.front();
		VertexList.pop_front();
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
