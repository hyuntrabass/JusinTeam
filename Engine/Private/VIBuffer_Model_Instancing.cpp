#include "VIBuffer_Model_Instancing.h"
#include "GameInstance.h"
#include "Mesh.h"
#include "Texture.h"

CVIBuffer_Model_Instancing::CVIBuffer_Model_Instancing(_dev pDevice, _context pContext)
	: CComponent(pDevice, pContext)
{
}

CVIBuffer_Model_Instancing::CVIBuffer_Model_Instancing(const CVIBuffer_Model_Instancing& rhs)
	: CComponent(rhs)
	, m_iNumMeshes(rhs.m_iNumMeshes)
	, m_iNumMaterials(rhs.m_iNumMaterials)
	, m_Materials(rhs.m_Materials)
	, m_PivotMatrix(rhs.m_PivotMatrix)

	, m_InstancingBufferDesc(rhs.m_InstancingBufferDesc)
	, m_InstancingInitialData(rhs.m_InstancingInitialData)
	, m_iNumInstances(rhs.m_iNumInstances)
	, m_iIndexCountPerInstance(rhs.m_iIndexCountPerInstance)
	, m_iInstanceStride(rhs.m_iInstanceStride)
{
}

HRESULT CVIBuffer_Model_Instancing::Init_Prototype(const string& strFilePath, const _bool& isCOLMesh, _fmatrix PivotMatrix, _uint iNumInstance)
{
	strcpy_s(m_szFilePath, MAX_PATH, strFilePath.c_str());
	XMStoreFloat4x4(&m_PivotMatrix, PivotMatrix);
	ModelType eType{};
	_char szExt[MAX_PATH]{};
	if (!isCOLMesh)
	{
		_splitpath_s(strFilePath.c_str(), nullptr, 0, nullptr, 0, nullptr, 0, szExt, MAX_PATH);
		eType = ModelType::Static;
	}
	else
	{
		eType = ModelType::Collision;
	}

	ifstream ModelFile(strFilePath.c_str(), ios::binary);
	if (ModelFile.is_open())
	{

		if (FAILED(Read_Meshes(ModelFile, PivotMatrix, iNumInstance)))
		{
			return E_FAIL;
		}

		if (FAILED(Read_Materials(ModelFile, strFilePath)))
		{
			return E_FAIL;
		}
	}
	else
	{
		MSG_BOX("Failed to Open File!");
		return E_FAIL;
	}


	return S_OK;
}

HRESULT CVIBuffer_Model_Instancing::Init(void* pArg)
{
	if (pArg != nullptr)
	{
		m_pOwnerTransform = reinterpret_cast<CTransform*>(pArg);
		Safe_AddRef(m_pOwnerTransform);
	}

	return S_OK;
}

void CVIBuffer_Model_Instancing::Update(_float fTimeDelta, _mat WorldMatrix)
{
	//D3D11_MAPPED_SUBRESOURCE SubResource{};

	//m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	//list<VTXMESHINSTANCING> VertexList{};

	//for (size_t i = 0; i < m_iNumInstances; i++)
	//{
	//	VTXMESHINSTANCING* pVertex = &reinterpret_cast<VTXMESHINSTANCING*>(SubResource.pData)[i];

	//	pVertex->vRight = _vec4(WorldMatrix._11, WorldMatrix._12, WorldMatrix._13, WorldMatrix._14);
	//	pVertex->vUp = _vec4(WorldMatrix._21, WorldMatrix._22, WorldMatrix._23, WorldMatrix._24);
	//	pVertex->vLook = _vec4(WorldMatrix._31, WorldMatrix._32, WorldMatrix._33, WorldMatrix._34);
	//	pVertex->vPos = _vec4(WorldMatrix._41, WorldMatrix._42, WorldMatrix._43, WorldMatrix._44);
	//	
	//	VertexList.push_back(*pVertex);
	//	reinterpret_cast<VTXMESHINSTANCING*>(SubResource.pData)[i] = VertexList.front();
	//}

	//m_pContext->Unmap(m_pVBInstance, 0);

	//VertexList.pop_front();
	
}

HRESULT CVIBuffer_Model_Instancing::Render()
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

HRESULT CVIBuffer_Model_Instancing::Bind_Material(CShader* pShader, const _char* pVariableName, _uint iMeshIndex, TextureType eTextureType)
{
	_uint iMatIndex = m_InstancingMeshes[iMeshIndex]->Get_MatIndex();

	if (iMatIndex >= m_iNumMaterials)
	{
		return E_FAIL;
	}

	CTexture* pMaterial = m_Materials[iMatIndex].pMaterials[ToIndex(eTextureType)];
	if (!pMaterial)
	{
		return E_FAIL;
	}

	return pMaterial->Bind_ShaderResource(pShader, pVariableName);
}
void CVIBuffer_Model_Instancing::Apply_TransformToActor(_fmatrix WorldMatrix)
{
	for (auto& pMesh : m_InstancingMeshes)
	{
		pMesh->Apply_TransformToActor(WorldMatrix);
	}
}

_bool CVIBuffer_Model_Instancing::Intersect_RayModel(_fmatrix WorldMatrix, _vec4* pPickPos)
{
	for (auto& pMesh : m_InstancingMeshes)
	{
		if (pMesh->Intersect_RayMesh(WorldMatrix, pPickPos))
		{
			XMStoreFloat4(pPickPos, XMVector4Transform(XMLoadFloat4(pPickPos), WorldMatrix));
			return true;
		}
	}

	return false;
}

HRESULT CVIBuffer_Model_Instancing::Read_Meshes(ifstream& File, _fmatrix PivotMatrix, _uint iNumInstance)
{
	File.read(reinterpret_cast<_char*>(&m_iNumMeshes), sizeof _uint);
	m_InstancingMeshes.reserve(m_iNumMeshes);

	for (size_t i = 0; i < m_iNumMeshes; i++)
	{
		CVIBuffer_Instancing_Mesh* pInstancingMesh = CVIBuffer_Instancing_Mesh::Create(m_pDevice, m_pContext, File, PivotMatrix, iNumInstance);
		if (!pInstancingMesh)
		{
			MSG_BOX("Failed to Read InstancingMeshs!");
			return E_FAIL;
		}
		m_InstancingMeshes.push_back(pInstancingMesh);
	}

	return S_OK;
}


HRESULT CVIBuffer_Model_Instancing::Read_Materials(ifstream& File, const string& strFilePath)
{
	_char szMatFilePath[MAX_PATH]{};
	_char szFullPath[MAX_PATH]{};

	_splitpath_s(strFilePath.c_str(), nullptr, 0, szMatFilePath, MAX_PATH, nullptr, 0, nullptr, 0);
	strcat_s(szMatFilePath, "../Texture/");

	File.read(reinterpret_cast<_char*>(&m_iNumMaterials), sizeof _uint);

	for (size_t i = 0; i < m_iNumMaterials; i++)
	{
		Model_Material Material{};
		_uint iNameSize{};
		_char* pFileName{};

		for (size_t j = 0; j < ToIndex(TextureType::End); j++)
		{
			File.read(reinterpret_cast<_char*>(&iNameSize), sizeof _uint);
			if (iNameSize == 1)
			{
				continue;
			}

			pFileName = new _char[iNameSize];
			File.read(pFileName, iNameSize);
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

		m_Materials.push_back(Material);
	}

	return S_OK;
}


CVIBuffer_Model_Instancing* CVIBuffer_Model_Instancing::Create(_dev pDevice, _context pContext, const string& strFilePath, const _bool& isCOLMesh, _fmatrix PivotMatrix, _uint iNumInstance)
{
	CVIBuffer_Model_Instancing* pInstance = new CVIBuffer_Model_Instancing(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype(strFilePath, isCOLMesh, PivotMatrix, iNumInstance)))
	{
		MSG_BOX("Failed to Create : CModel");
	}

	return pInstance;
}

CComponent* CVIBuffer_Model_Instancing::Clone(void* pArg)
{
	CVIBuffer_Model_Instancing* pInstance = new CVIBuffer_Model_Instancing(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CModel");
	}

	return pInstance;
}


void CVIBuffer_Model_Instancing::Free()
{
	__super::Free();

	Safe_Release(m_pVBInstance);
}

