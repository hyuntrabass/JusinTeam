#include "VIBuffer_Instancing_Mesh.h"

CVIBuffer_Instancing_Mesh::CVIBuffer_Instancing_Mesh(_dev pDevice, _context pContext)
	: CVIBuffer_Model_Instancing(pDevice, pContext)
{
}

CVIBuffer_Instancing_Mesh::CVIBuffer_Instancing_Mesh(const CVIBuffer_Instancing_Mesh& rhs)
	: CVIBuffer_Model_Instancing(rhs)
{
}

//HRESULT CVIBuffer_Instancing_Mesh::Init_Prototype(vector<VTXSTATICMESH> m_Vertices, vector<_ulong> m_Indices, _uint iNumInstance)
HRESULT CVIBuffer_Instancing_Mesh::Init_Prototype(const string& strFilePath, _uint iNumInstance)
{



	m_iNumVertexBuffers = 2;
	m_iVertexStride = sizeof(VTXSTATICMESH);
	m_iNumVertices = m_Vertices.size();

	m_iIndexStride = 2;
	m_iNumInstances = iNumInstance;
	m_iIndexCountPerInstance = 6;
	m_iNumIndices = m_iIndexCountPerInstance * m_iNumInstances;

	m_iInstanceStride = sizeof(VTXMESHINSTANCING);

	m_eIndexFormat = DXGI_FORMAT_R16_UINT;
	m_ePrimitiveTopology = D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST;


#pragma region VERTEX_BUFFER

	ZeroMemory(&m_BufferDesc, sizeof(m_BufferDesc));
	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iVertexStride;

	ZeroMemory(&m_InitialData, sizeof(m_InitialData));

	VTXSTATICMESH* pVertices = new VTXSTATICMESH[m_iNumIndices];
	ZeroMemory(pVertices, sizeof(VTXSTATICMESH) * m_iNumVertices);

	
	for (_uint i = 0; i < m_iNumVertices; i++)
	{
		pVertices[i].vPosition = m_Vertices[i].vPosition;
		pVertices[i].vNormal = m_Vertices[i].vNormal;
		pVertices[i].vTexcoord = m_Vertices[i].vTexcoord;
	}


	m_InitialData.pSysMem = pVertices;

	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;
	Safe_Delete_Array(pVertices);
#pragma endregion


#pragma region INDEX_BUFFER
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = /*m_iIndexStride*/0;

	ZeroMemory(&m_InitialData, sizeof m_InitialData);

	_ulong* pIndices = new _ulong[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_ulong) * m_iNumIndices);

	_uint iNumIndices = 0;

	for (size_t i = 0; i < m_iNumIndices; i++)
	{
		pIndices[iNumIndices++] = m_Indices[i];
	}

	m_InitialData.pSysMem = pIndices;

	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pIndices);
#pragma endregion


#pragma region INSTANCE_BUFFER
	ZeroMemory(&m_InstancingBufferDesc, sizeof m_InstancingBufferDesc);
	m_InstancingBufferDesc.ByteWidth = m_iInstanceStride * m_iNumInstances;
	m_InstancingBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_InstancingBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_InstancingBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_InstancingBufferDesc.MiscFlags = 0;
	m_InstancingBufferDesc.StructureByteStride = m_iInstanceStride;

	ZeroMemory(&m_InstancingInitialData, sizeof m_InstancingInitialData);

	VTXMESHINSTANCING* pVertexInstance = new VTXMESHINSTANCING[m_iNumInstances];
	ZeroMemory(pVertexInstance, sizeof(VTXMESHINSTANCING) * m_iNumInstances);

	for (size_t i = 0; i < m_iNumInstances; i++)
	{
		pVertexInstance[i].vRight = _float4(1.f, 0.f, 0.f, 0.f);
		pVertexInstance[i].vUp = _float4(0.f, 1.f, 0.f, 0.f);
		pVertexInstance[i].vLook = _float4(0.f, 0.f, 1.f, 0.f);
		pVertexInstance[i].vPos = _float4(rand() % 10, 5.f, rand() % 10, 1.f);
	}
	m_InstancingInitialData.pSysMem = pVertexInstance;
#pragma endregion

	return S_OK;
}

HRESULT CVIBuffer_Instancing_Mesh::Init(void* pArg)
{
	_mat mWorldMatrix = *(_mat*)pArg;

	VTXMESHINSTANCING* pVertexInstance = reinterpret_cast<VTXMESHINSTANCING*>(const_cast<void*>(m_InstancingInitialData.pSysMem));
	
	if (pArg)
	{

		for (size_t i = 0; i < m_iNumInstances; i++)
		{

			pVertexInstance[i].vRight = _vec4(mWorldMatrix._11, mWorldMatrix._12, mWorldMatrix._13, mWorldMatrix._14);
			pVertexInstance[i].vUp = _vec4(mWorldMatrix._21, mWorldMatrix._22, mWorldMatrix._23, mWorldMatrix._24);
			pVertexInstance[i].vLook = _vec4(mWorldMatrix._31, mWorldMatrix._32, mWorldMatrix._33, mWorldMatrix._34);
			pVertexInstance[i].vPos = _vec4(mWorldMatrix._41, mWorldMatrix._42, mWorldMatrix._43, mWorldMatrix._44);
		}
	}

	if (FAILED(m_pDevice->CreateBuffer(&m_InstancingBufferDesc, &m_InstancingInitialData, &m_pVBInstance)))
	{
		Safe_Delete_Array(m_InstancingInitialData.pSysMem);
		return E_FAIL;
	}

	return S_OK;
}
//
//CVIBuffer_Instancing_Mesh* CVIBuffer_Instancing_Mesh::Create(_dev pDevice, _context pContext, vector<VTXSTATICMESH> m_Vertices, vector<_ulong> m_Indices, _uint iNumInstance)
//{
//	CVIBuffer_Instancing_Mesh* pInstance = new CVIBuffer_Instancing_Mesh(pDevice, pContext);
//
//	if (FAILED(pInstance->Init_Prototype(m_Vertices, m_Indices, iNumInstance)))
//	{
//		MSG_BOX("Failed to Create : CVIBuffer_Instancing_Mesh");
//		Safe_Release(pInstance);
//	}
//
//	return pInstance;
//}

CVIBuffer_Instancing_Mesh* CVIBuffer_Instancing_Mesh::Create(_dev pDevice, _context pContext, const string& strFilePath,  _uint iNumInstance)
{
	CVIBuffer_Instancing_Mesh* pInstance = new CVIBuffer_Instancing_Mesh(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype(strFilePath, iNumInstance)))
	{
		MSG_BOX("Failed to Create : CVIBuffer_Instancing_Mesh");
		Safe_Release(pInstance);
	}

	return pInstance;
}


CComponent* CVIBuffer_Instancing_Mesh::Clone(void* pArg)
{
	CVIBuffer_Instancing_Mesh* pInstance = new CVIBuffer_Instancing_Mesh(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CVIBuffer_Instancing_Mesh");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Instancing_Mesh::Free()
{
	if (not m_hasCloned)
	{
		Safe_Delete_Array(m_InstancingInitialData.pSysMem);
	}
	__super::Free();
}
