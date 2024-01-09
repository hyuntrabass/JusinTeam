#include "VIBuffer_Terrain.h"

CVIBuffer_Terrain::CVIBuffer_Terrain(_dev pDevice, _context pContext)
	: CVIBuffer(pDevice, pContext)
{
}

CVIBuffer_Terrain::CVIBuffer_Terrain(const CVIBuffer_Terrain& rhs)
	: CVIBuffer(rhs)
{
}

HRESULT CVIBuffer_Terrain::Init_Prototype(_uint iNumVerticesX, _uint iNumVerticesZ)
{
	m_iNumVertexBuffers = 1;
	m_iVertexStride = sizeof VTXNORTEX;
	m_iNumVertices = iNumVerticesX * iNumVerticesZ;

	m_iIndexStride = 4;
	m_iNumIndices = (iNumVerticesX - 1) * (iNumVerticesZ - 1) * 2 * 3;
	
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

#pragma region Vertex Buffer
	VTXNORTEX* pVertices = new VTXNORTEX[m_iNumVertices];

	for (_uint z = 0; z < iNumVerticesZ; z++)
	{
		for (_uint x = 0; x < iNumVerticesX; x++)
		{
			_uint iIndex = z * iNumVerticesX + x;

			pVertices[iIndex].vPosition = _float3(static_cast<_float>(x), 0.f, static_cast<_float>(z));
			pVertices[iIndex].vNormal = _float3(0.f, 0.f, 0.f);
			pVertices[iIndex].vTexcoord = _float2(x / (iNumVerticesX - 1.f), z / (iNumVerticesZ - 1.f));
		}
	}
#pragma endregion

#pragma region Index Buffer
	_ulong* pIndices = new _ulong[m_iNumIndices];

	_uint iNumIndices{};

	for (_uint z = 0; z < iNumVerticesZ - 1; z++)
	{
		for (_uint x = 0; x < iNumVerticesX - 1; x++)
		{
			_uint iIndex = z * iNumVerticesX + x;

			_uint iIndices[4] =
			{
				iIndex + iNumVerticesX,
				iIndex + iNumVerticesX + 1,
				iIndex + 1,
				iIndex
			};

			_vector vSrc{}, vDst{}, vNor{};

			pIndices[iNumIndices++] = iIndices[0];
			pIndices[iNumIndices++] = iIndices[1];
			pIndices[iNumIndices++] = iIndices[2];

			vSrc = XMLoadFloat3(&pVertices[iIndices[1]].vPosition) - XMLoadFloat3(&pVertices[iIndices[0]].vPosition);
			vDst = XMLoadFloat3(&pVertices[iIndices[2]].vPosition) - XMLoadFloat3(&pVertices[iIndices[1]].vPosition);
			vNor = XMVector3Normalize(XMVector3Cross(vSrc, vDst));

			XMStoreFloat3(&pVertices[iIndices[0]].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[iIndices[0]].vNormal) + vNor));
			XMStoreFloat3(&pVertices[iIndices[1]].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[iIndices[1]].vNormal) + vNor));
			XMStoreFloat3(&pVertices[iIndices[2]].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[iIndices[2]].vNormal) + vNor));

			pIndices[iNumIndices++] = iIndices[0];
			pIndices[iNumIndices++] = iIndices[2];
			pIndices[iNumIndices++] = iIndices[3];

			vSrc = XMLoadFloat3(&pVertices[iIndices[2]].vPosition) - XMLoadFloat3(&pVertices[iIndices[0]].vPosition);
			vDst = XMLoadFloat3(&pVertices[iIndices[3]].vPosition) - XMLoadFloat3(&pVertices[iIndices[2]].vPosition);
			vNor = XMVector3Normalize(XMVector3Cross(vSrc, vDst));

			XMStoreFloat3(&pVertices[iIndices[0]].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[iIndices[0]].vNormal) + vNor));
			XMStoreFloat3(&pVertices[iIndices[2]].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[iIndices[2]].vNormal) + vNor));
			XMStoreFloat3(&pVertices[iIndices[3]].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[iIndices[3]].vNormal) + vNor));
		}
	}
#pragma endregion

	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iVertexStride;

	m_InitialData.pSysMem = pVertices;
	if (FAILED(__super::Create_Buffer(&m_pVB)))
	{
		return E_FAIL;
	}

	m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = 0;

	m_InitialData.pSysMem = pIndices;
	if (FAILED(__super::Create_Buffer(&m_pIB)))
	{
		return E_FAIL;
	}

	Safe_Delete_Array(pVertices);
	Safe_Delete_Array(pIndices);

	return S_OK;
}

HRESULT CVIBuffer_Terrain::Init(void* pArg)
{
	return S_OK;
}

CVIBuffer_Terrain* CVIBuffer_Terrain::Create(_dev pDevice, _context pContext, _uint iNumVerticesX, _uint iNumVerticesZ)
{
	CVIBuffer_Terrain* pInstance = new CVIBuffer_Terrain(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype(iNumVerticesX,iNumVerticesZ)))
	{
		MSG_BOX("Failed to Create : CVIBuffer_Terrain");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CVIBuffer_Terrain::Clone(void* pArg)
{
	CVIBuffer_Terrain* pInstance = new CVIBuffer_Terrain(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CVIBuffer_Terrain");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Terrain::Free()
{
	__super::Free();
}
