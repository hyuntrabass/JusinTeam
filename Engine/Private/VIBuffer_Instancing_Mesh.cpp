#include "VIBuffer_Instancing_Mesh.h"
#include "Texture.h"

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
	strcpy_s(m_szFilePath, MAX_PATH, strFilePath.c_str());
	ModelType eType{};
	_char szExt[MAX_PATH]{};
	_uint m_iNumMeshes{};
	_uint m_iMatIndex{};
	_char m_szName[MAX_PATH]{};
	_uint m_iNumMaterials{};

	ifstream ModelFile(strFilePath.c_str(), ios::binary);
	if (ModelFile.is_open())
	{

		ModelFile.read(reinterpret_cast<_char*>(&m_iNumMeshes), sizeof _uint);

		for (size_t i = 0; i < m_iNumMeshes; i++)
		{

			ModelFile.read(reinterpret_cast<_char*>(&m_iMatIndex), sizeof _uint);

			_uint iNameSize{};
			ModelFile.read(reinterpret_cast<_char*>(&iNameSize), sizeof _uint);
			if (iNameSize >= sizeof(m_szName))
			{
				MSG_BOX("Name Is Too Long!");
			}
			ModelFile.read(m_szName, iNameSize);
			ModelFile.read(reinterpret_cast<_char*>(&m_iNumVertices), sizeof _uint);
			_uint iNumFaces{};
			ModelFile.read(reinterpret_cast<_char*>(&iNumFaces), sizeof _uint);

			m_iNumVertexBuffers = 1;
			m_iIndexStride = 4;
			m_iNumIndices = iNumFaces * 3;
			m_eIndexFormat = DXGI_FORMAT_R32_UINT;
			m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

			m_iVertexStride = sizeof VTXSTATICMESH;
			ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
			m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
			m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
			m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			m_BufferDesc.CPUAccessFlags = 0;
			m_BufferDesc.MiscFlags = 0;
			m_BufferDesc.StructureByteStride = m_iVertexStride;

			ZeroMemory(&m_InitialData, sizeof m_InitialData);

			VTXSTATICMESH* pVertices = new VTXSTATICMESH[m_iNumVertices]{};

			for (size_t i = 0; i < m_iNumVertices; i++)
			{
				ModelFile.read(reinterpret_cast<_char*>(&pVertices[i].vPosition), sizeof _float3);
				XMStoreFloat3(&pVertices[i].vPosition, XMLoadFloat3(&pVertices[i].vPosition));
				ModelFile.read(reinterpret_cast<_char*>(&pVertices[i].vNormal), sizeof _float3);
				XMStoreFloat3(&pVertices[i].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[i].vNormal)));
				ModelFile.read(reinterpret_cast<_char*>(&pVertices[i].vTexcoord), sizeof _float2);
				ModelFile.read(reinterpret_cast<_char*>(&pVertices[i].vTangent), sizeof _float3);
			}


			m_InitialData.pSysMem = pVertices;

			if (FAILED(__super::Create_Buffer(&m_pVB)))
			{
				return E_FAIL;
			}

			Safe_Delete_Array(pVertices);


#pragma region Index
			ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
			m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
			m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
			m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
			m_BufferDesc.CPUAccessFlags = 0;
			m_BufferDesc.MiscFlags = 0;
			m_BufferDesc.StructureByteStride = 0;

			ZeroMemory(&m_InitialData, sizeof m_InitialData);

			_ulong* pIndices = new _ulong[m_iNumIndices]{};

			_ulong dwIndex{};
			for (size_t i = 0; i < iNumFaces; i++)
			{
				ModelFile.read(reinterpret_cast<_char*>(&pIndices[dwIndex]), sizeof _uint);
				dwIndex++;

				ModelFile.read(reinterpret_cast<_char*>(&pIndices[dwIndex]), sizeof _uint);
				dwIndex++;

				ModelFile.read(reinterpret_cast<_char*>(&pIndices[dwIndex]), sizeof _uint);
				dwIndex++;

			}

			m_InitialData.pSysMem = pIndices;

			if (FAILED(__super::Create_Buffer(&m_pIB)))
			{
				return E_FAIL;
			}
			Safe_Delete_Array(pIndices);

		}
	}

	_char szMatFilePath[MAX_PATH]{};
	_char szFullPath[MAX_PATH]{};

	_splitpath_s(strFilePath.c_str(), nullptr, 0, szMatFilePath, MAX_PATH, nullptr, 0, nullptr, 0);
	strcat_s(szMatFilePath, "../Texture/");

	ModelFile.read(reinterpret_cast<_char*>(&m_iNumMaterials), sizeof _uint);

	for (size_t i = 0; i < m_iNumMaterials; i++)
	{
		Model_Material Material{};
		_uint iNameSize{};
		_char* pFileName{};

		for (size_t j = 0; j < ToIndex(TextureType::End); j++)
		{
			ModelFile.read(reinterpret_cast<_char*>(&iNameSize), sizeof _uint);
			if (iNameSize == 1)
			{
				continue;
			}

			pFileName = new _char[iNameSize];
			ModelFile.read(pFileName, iNameSize);
			if (strlen(szMatFilePath) + strlen(pFileName) >= sizeof(szFullPath))
			{
				MSG_BOX("Name Is Too Long!");
			}
			strcpy_s(szFullPath, szMatFilePath);
			strcat_s(szFullPath, pFileName);
			Safe_Delete_Array(pFileName);

			_tchar szTexturePath[MAX_PATH]{};
			MultiByteToWideChar(CP_ACP, 0, szFullPath, static_cast<_int>(strlen(szFullPath)), szTexturePath, MAX_PATH);

			Material.pMaterials[j] = CTexture::Create(m_pDevice, m_pContext, szTexturePath);
			if (!Material.pMaterials[j])
			{
				MSG_BOX("Failed to Create Texture from Model!");
				return E_FAIL;
			}
		}

	}
	ModelFile.close();



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

//CVIBuffer_Instancing_Mesh* CVIBuffer_Instancing_Mesh::Create(_dev pDevice, _context pContext, vector<VTXSTATICMESH> m_Vertices, vector<_ulong> m_Indices, _uint iNumInstance)
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
