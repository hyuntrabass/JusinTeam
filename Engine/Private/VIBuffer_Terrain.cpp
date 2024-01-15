#include "VIBuffer_Terrain.h"

CVIBuffer_Terrain::CVIBuffer_Terrain(_dev pDevice, _context pContext)
	: CVIBuffer(pDevice, pContext)
{
}

CVIBuffer_Terrain::CVIBuffer_Terrain(const CVIBuffer_Terrain& rhs)
	: CVIBuffer(rhs)
{
}

HRESULT CVIBuffer_Terrain::Init_Prototype()
{
	return S_OK;
}

HRESULT CVIBuffer_Terrain::Init(void* pArg)
{
	m_pTerrain = *(Info*)pArg;
	
	m_iNumVerticesX = m_pTerrain.iNumVerticesX;
	m_iNumVerticesZ = m_pTerrain.iNumVerticesZ;

	m_iNumVertexBuffers = 1;
	m_iVertexStride = sizeof VTXNORTEX;
	m_iNumVertices = m_iNumVerticesX * m_iNumVerticesZ;

	m_iIndexStride = 4;
	m_iNumIndices = (m_iNumVerticesX - 1) * (m_iNumVerticesZ - 1) * 2 * 3;

	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

#pragma region Vertex Buffer
	VTXNORTEX* pVertices = new VTXNORTEX[m_iNumVertices];

	for (_uint z = 0; z < m_iNumVerticesZ; z++)
	{
		for (_uint x = 0; x < m_iNumVerticesX; x++)
		{
			_uint iIndex = z * m_iNumVerticesX + x;

			pVertices[iIndex].vPosition = _float3(static_cast<_float>(x), 0.f, static_cast<_float>(z));
			pVertices[iIndex].vNormal = _float3(0.f, 0.f, 0.f);
			pVertices[iIndex].vTexcoord = _float2(x / (m_iNumVerticesX - 1.f), z / (m_iNumVerticesZ - 1.f));
		}
	}
#pragma endregion

#pragma region Index Buffer
	_ulong* pIndices = new _ulong[m_iNumIndices];

	_uint iNumIndices{};

	for (_uint z = 0; z < m_iNumVerticesZ - 1; z++)
	{
		for (_uint x = 0; x < m_iNumVerticesX - 1; x++)
		{
			_uint iIndex = z * m_iNumVerticesX + x;

			_uint iIndices[4] =
			{
				iIndex + m_iNumVerticesX,
				iIndex + m_iNumVerticesX + 1,
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
	m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iVertexStride;

	m_InitialData.pSysMem = pVertices;
	if (FAILED(__super::Create_Buffer(&m_pVB)))
	{
		return E_FAIL;
	}

	m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
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

HRESULT CVIBuffer_Terrain::ModifyTerrainVertexBuffer( _uint iNumVerticesX, _uint iNumVerticesZ)
{
	m_iNumVertexBuffers = 1;
	m_iVertexStride = sizeof VTXNORTEX;
	m_iNumVertices = iNumVerticesX * iNumVerticesZ;
	m_iIndexStride = 4;
	m_iNumIndices = (iNumVerticesX - 1) * (iNumVerticesZ - 1) * 2 * 3;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

#pragma region VERTEX_BUFFER

	D3D11_MAPPED_SUBRESOURCE MappedVertex;
	m_pContext->Map(m_pVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedVertex);
	VTXNORTEX* pVertices = (VTXNORTEX*)MappedVertex.pData;

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

	m_pContext->Unmap(m_pVB, 0);
#pragma endregion

#pragma region INDEX_BUFFER
	D3D11_MAPPED_SUBRESOURCE MappedIndex;
	m_pContext->Map(m_pIB, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedIndex);
	_ulong* pIndices = (_ulong*)MappedIndex.pData;

	_uint iNumIndices = 0;
	for (size_t i = 0; i < iNumVerticesZ - 1; ++i)
	{
		for (size_t j = 0; j < iNumVerticesX - 1; ++j)
		{
			_uint	iIndex = (_uint)i * iNumVerticesX + (_uint)j;
			_uint	iIndices[4] = {
				iIndex + iNumVerticesX,
				iIndex + iNumVerticesX + 1,
				iIndex + 1,
				iIndex
			};


			_vector	vSour, vDest, vNormal;

			pIndices[iNumIndices++] = iIndices[0];
			pIndices[iNumIndices++] = iIndices[1];
			pIndices[iNumIndices++] = iIndices[2];

			vSour = XMLoadFloat3(&pVertices[iIndices[1]].vPosition) - XMLoadFloat3(&pVertices[iIndices[0]].vPosition);
			vDest = XMLoadFloat3(&pVertices[iIndices[2]].vPosition) - XMLoadFloat3(&pVertices[iIndices[1]].vPosition);
			vNormal = XMVector3Normalize(XMVector3Cross(vSour, vDest));

			XMStoreFloat3(&pVertices[iIndices[0]].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[iIndices[0]].vNormal)) + vNormal);
			XMStoreFloat3(&pVertices[iIndices[1]].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[iIndices[0]].vNormal)) + vNormal);
			XMStoreFloat3(&pVertices[iIndices[2]].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[iIndices[0]].vNormal)) + vNormal);

			pIndices[iNumIndices++] = iIndices[0];
			pIndices[iNumIndices++] = iIndices[2];
			pIndices[iNumIndices++] = iIndices[3];

			vSour = XMLoadFloat3(&pVertices[iIndices[2]].vPosition) - XMLoadFloat3(&pVertices[iIndices[0]].vPosition);
			vDest = XMLoadFloat3(&pVertices[iIndices[3]].vPosition) - XMLoadFloat3(&pVertices[iIndices[2]].vPosition);
			vNormal = XMVector3Normalize(XMVector3Cross(vSour, vDest));

			XMStoreFloat3(&pVertices[iIndices[0]].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[iIndices[0]].vNormal)) + vNormal);
			XMStoreFloat3(&pVertices[iIndices[2]].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[iIndices[2]].vNormal)) + vNormal);
			XMStoreFloat3(&pVertices[iIndices[3]].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[iIndices[3]].vNormal)) + vNormal);
		}
	}

	for (size_t i = 0; i < m_iNumVertices; i++)
	{
		XMStoreFloat3(&pVertices[i].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[i].vNormal)));
	}

	m_pContext->Unmap(m_pIB, 0);

	return S_OK;
}
CVIBuffer_Terrain* CVIBuffer_Terrain::Create(_dev pDevice, _context pContext)
{
	CVIBuffer_Terrain* pInstance = new CVIBuffer_Terrain(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
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
