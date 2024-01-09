#include "Model.h"
#include "Mesh.h"
#include "Texture.h"
#include "Bone.h"
#include "Animation.h"

CModel::CModel(_dev pDevice, _context pContext)
	: CComponent(pDevice, pContext)
{
}

CModel::CModel(const CModel& rhs)
	: CComponent(rhs)
	, m_iNumMeshes(rhs.m_iNumMeshes)
	, m_iNumMaterials(rhs.m_iNumMaterials)
	, m_Materials(rhs.m_Materials)
	, m_PivotMatrix(rhs.m_PivotMatrix)
	, m_iNumAnimations(rhs.m_iNumAnimations)
{
	for (auto& pPrototypeBone : rhs.m_Bones)
	{
		CBone* pBone = pPrototypeBone->Clone();

		m_Bones.push_back(pBone);
	}

	for (auto& pPrototypeAnimation : rhs.m_Animations)
	{
		CAnimation* pAnimation = pPrototypeAnimation->Clone();

		m_Animations.push_back(pAnimation);
	}

	for (auto& pPrototypeMesh : rhs.m_Meshes)
	{
		CMesh* pMesh = reinterpret_cast<CMesh*>(pPrototypeMesh->Clone());

		m_Meshes.push_back(pMesh);
	}

	for (auto& Material : m_Materials)
	{
		for (auto& pTexture : Material.pMaterials)
		{
			Safe_AddRef(pTexture);
		}
	}
}

const _uint& CModel::Get_NumMeshes() const
{
	return m_iNumMeshes;
}

const _bool& CModel::IsAnimationFinished(_uint iAnimIndex) const
{
	return m_Animations[iAnimIndex]->IsFinished();
}

const _uint& CModel::Get_CurrentAnimationIndex() const
{
	return m_AnimDesc.iAnimIndex;
}

const _float& CModel::Get_CurrentAnimPos() const
{
	return m_Animations[m_AnimDesc.iAnimIndex]->Get_CurrentAnimPos();
}

const _float44* CModel::Get_BoneMatrix(const _char* pBoneName) const
{
	auto iter = find_if(m_Bones.begin(), m_Bones.end(), [&pBoneName](CBone* pBone) 
	{
		if (!strcmp(pBone->Get_BoneName(), pBoneName))
		{
			return true;
		}
		return false;
	});

	if (iter == m_Bones.end())
	{
		MSG_BOX("Can't Find Bone");
		return nullptr;
	}

	return (*iter)->Get_CombinedMatrix();
}

_matrix CModel::Get_PivotMatrix()
{
	return XMLoadFloat4x4(&m_PivotMatrix);
}

void CModel::Set_Animation(ANIM_DESC Animation_Desc)
{
	if (m_AnimDesc.iAnimIndex != Animation_Desc.iAnimIndex or
		Animation_Desc.bRestartAnimation)
	{
		m_isAnimChanged = true;

		for (auto& pAnim : m_Animations)
		{
			pAnim->ResetFinished();
		}

		if (m_AnimDesc.iAnimIndex >= m_iNumAnimations)
		{
			m_AnimDesc.iAnimIndex = m_iNumAnimations - 1;
		}
	}
	
	m_AnimDesc = Animation_Desc;
}

HRESULT CModel::Init_Prototype(const string& strFilePath, const _bool& isCOLMesh, _fmatrix PivotMatrix)
{
	XMStoreFloat4x4(&m_PivotMatrix, PivotMatrix);
	ModelType eType{};
	_char szExt[MAX_PATH]{};
	if (!isCOLMesh)
	{
		_splitpath_s(strFilePath.c_str(), nullptr, 0, nullptr, 0, nullptr, 0, szExt, MAX_PATH);
		if (!strcmp(szExt, ".hyuntraanimmesh"))
		{
			eType = ModelType::Anim;
		}
		else
		{
			eType = ModelType::Static;
		}
	}
	else
	{
		eType = ModelType::Collision;
	}

	ifstream ModelFile(strFilePath.c_str(), ios::binary);
	if (ModelFile.is_open())
	{
		if (eType == ModelType::Anim)
		{
			if (FAILED(Read_Bones(ModelFile)))
			{
				return E_FAIL;
			}
		}

		if (FAILED(Read_Meshes(ModelFile, eType, PivotMatrix)))
		{
			return E_FAIL;
		}

		if (FAILED(Read_Materials(ModelFile, strFilePath)))
		{
			return E_FAIL;
		}

		if (eType == ModelType::Anim)
		{
			if (FAILED(Read_Animations(ModelFile)))
			{
				return E_FAIL;
			}
		}


		ModelFile.close();
	}
	else
	{
		MSG_BOX("Failed to Open File!");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CModel::Init(void* pArg)
{
	return S_OK;
}

void CModel::Play_Animation(_float fTimeDelta)
{
	m_Animations[m_AnimDesc.iAnimIndex]->Update_TransformationMatrix(m_Bones, fTimeDelta * m_AnimDesc.fAnimSpeedRatio, m_isAnimChanged, m_AnimDesc.isLoop, m_AnimDesc.bSkipInterpolation, m_AnimDesc.fInterpolationTime, m_AnimDesc.fDurationRatio);

	for (auto& pBone : m_Bones)
	{
		pBone->Update_CombinedMatrix(m_Bones);
	}
}

HRESULT CModel::Bind_BoneMatrices(_uint iMeshIndex, CShader* pShader, const _char* pVariableName)
{
	return m_Meshes[iMeshIndex]->Bind_BoneMatrices(pShader, m_Bones, pVariableName, XMLoadFloat4x4(&m_PivotMatrix));
}

HRESULT CModel::Bind_Material(CShader* pShader, const _char* pVariableName, _uint iMeshIndex, TextureType eTextureType)
{
	_uint iMatIndex = m_Meshes[iMeshIndex]->Get_MatIndex();

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

void CModel::Apply_TransformToActor(_fmatrix WorldMatrix)
{
	for (auto& pMesh : m_Meshes)
	{
		pMesh->Apply_TransformToActor(WorldMatrix);
	}
}

HRESULT CModel::Render(_uint iMeshIndex)
{
	m_Meshes[iMeshIndex]->Render();

	return S_OK;
}

_bool CModel::Intersect_RayModel(_fmatrix WorldMatrix, _float4* pPickPos)
{
	for (auto& pMesh : m_Meshes)
	{
		if (pMesh->Intersect_RayMesh(WorldMatrix, pPickPos))
		{
			XMStoreFloat4(pPickPos, XMVector4Transform(XMLoadFloat4(pPickPos), WorldMatrix));
			return true;
		}
	}

	return false;
}

HRESULT CModel::Read_Bones(ifstream& File)
{
	_uint iNumBones{};
	File.read(reinterpret_cast<_char*>(&iNumBones), sizeof _uint);

	for (size_t i = 0; i < iNumBones; i++)
	{
		CBone* pBone = CBone::Create(File);
		if (!pBone)
		{
			MSG_BOX("Failed to Read Bone!");
			return E_FAIL;
		}
		m_Bones.push_back(pBone);
	}

	return S_OK;
}

HRESULT CModel::Read_Meshes(ifstream& File, const ModelType& eType, _fmatrix PivotMatrix)
{
	File.read(reinterpret_cast<_char*>(&m_iNumMeshes), sizeof _uint);
	m_Meshes.reserve(m_iNumMeshes);

	for (size_t i = 0; i < m_iNumMeshes; i++)
	{
		CMesh* pMesh = CMesh::Create(m_pDevice, m_pContext, eType, File, PivotMatrix);
		if (!pMesh)
		{
			MSG_BOX("Failed to Read Meshes!");
			return E_FAIL;
		}
		m_Meshes.push_back(pMesh);
	}

	return S_OK;
}

HRESULT CModel::Read_Animations(ifstream& File)
{
	File.read(reinterpret_cast<_char*>(&m_iNumAnimations), sizeof _uint);

	for (size_t i = 0; i < m_iNumAnimations; i++)
	{
		CAnimation* pAnimation = CAnimation::Create(File);
		if (!pAnimation)
		{
			MSG_BOX("Failed to Read Animations!");
			return E_FAIL;
		}
		m_Animations.push_back(pAnimation);
	}

	return S_OK;
}

HRESULT CModel::Read_Materials(ifstream& File, const string& strFilePath)
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

CModel* CModel::Create(_dev pDevice, _context pContext, const string& strFilePath, const _bool& isCOLMesh, _fmatrix PivotMatrix)
{
	CModel* pInstance = new CModel(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype(strFilePath, isCOLMesh, PivotMatrix)))
	{
		MSG_BOX("Failed to Create : CModel");
	}

	return pInstance;
}

CComponent* CModel::Clone(void* pArg)
{
	CModel* pInstance = new CModel(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CModel");
	}

	return pInstance;
}

void CModel::Free()
{
	__super::Free();

	for (auto& pBone : m_Bones)
	{
		Safe_Release(pBone);
	}
	m_Bones.clear();

	for (auto& pAinmation : m_Animations)
	{
		Safe_Release(pAinmation);
	}
	m_Animations.clear();

	for (auto& pMesh : m_Meshes)
	{
		Safe_Release(pMesh);
	}
	m_Meshes.clear();

	for (auto& Material : m_Materials)
	{
		for (auto& pTexture : Material.pMaterials)
		{
			Safe_Release(pTexture);
		}
	}
	m_Materials.clear();
}
