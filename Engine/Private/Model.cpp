#include "Model.h"
#include "Mesh.h"
#include "Texture.h"
#include "Bone.h"
#include "Animation.h"
#include "GameInstance.h"

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
	, m_iNumTriggersEffect(rhs.m_iNumTriggersEffect)
	, m_TriggerEffects(rhs.m_TriggerEffects)
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

	strcpy_s(m_szFilePath, MAX_PATH, rhs.m_szFilePath);
}

const _char* CModel::Get_FilePath() const
{
	return m_szFilePath;
}

const _uint& CModel::Get_NumMeshes() const
{
	return m_iNumMeshes;
}

const _uint& CModel::Get_NumAnim() const
{
	return m_iNumAnimations;
}

const _uint CModel::Get_NumBones() const
{
	return m_Bones.size();
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

const _mat* CModel::Get_BoneMatrix(const _char* pBoneName) const
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

vector<VTXSTATICMESH> CModel::Get_StaticMeshVertices()
{
	vector<VTXSTATICMESH> vVertices;
	for (auto iter = m_Meshes.begin(); iter != m_Meshes.end(); iter++)
	{
		vVertices = ((*iter)->Get_VtxStaticInfo());
	}
	return vVertices;
}

vector<_ulong> CModel::Get_StaticMeshIndices()
{
	vector<_ulong> vIndices;
	for (auto iter = m_Meshes.begin(); iter != m_Meshes.end(); iter++)
	{
		vIndices = ((*iter)->Get_InidcesStaticInfo());
	}
	return vIndices;
}

const _uint CModel::Get_NumTriggerEffect() const
{
	return m_iNumTriggersEffect;
}

TRIGGEREFFECT_DESC* CModel::Get_TriggerEffect(_uint iTriggerEffectIndex)
{
	return &m_TriggerEffects[iTriggerEffectIndex];
}

vector<TRIGGEREFFECT_DESC>& CModel::Get_TriggerEffects()
{
	return m_TriggerEffects;
}
void CModel::Add_TriggerEffect(TRIGGEREFFECT_DESC TriggerEffectDesc)
{
	m_iNumTriggersEffect++;
	_mat* pMatrix = new _mat{};
	m_EffectMatrices.push_back(pMatrix);
	m_TriggerEffects.push_back(TriggerEffectDesc);
}

void CModel::Delete_TriggerEffect(_uint iTriggerEffectIndex)
{
	m_iNumTriggersEffect--;
	auto Effect_iter = m_TriggerEffects.begin();
	auto Matrix_iter = m_EffectMatrices.begin();
	for (_uint i = 0; i < iTriggerEffectIndex; i++)
	{
		Effect_iter++;
		Matrix_iter++;
	}
	m_pGameInstance->Delete_Effect((*Matrix_iter));
	m_TriggerEffects.erase(Effect_iter);
	m_EffectMatrices.erase(Matrix_iter);
}

void CModel::Reset_TriggerEffects()
{
	m_iNumTriggersEffect = 0;
	m_TriggerEffects.clear();
	for (auto& pEffectMatrix : m_EffectMatrices)
	{
		m_pGameInstance->Delete_Effect(pEffectMatrix);
		Safe_Delete(pEffectMatrix);
	}
	m_EffectMatrices.clear();
}

_uint CModel::Get_NumIndices()
{
	_uint iNumIndices;
	for (auto iter = m_Meshes.begin(); iter != m_Meshes.end(); iter++)
	{
		iNumIndices = ((*iter)->Get_NumIndices());
	}
	return iNumIndices;
}

_mat CModel::Get_PivotMatrix()
{
	return XMLoadFloat4x4(&m_PivotMatrix);
}

vector<class CAnimation*>& CModel::Get_Animations()
{
	return m_Animations;
}

CAnimation* CModel::Get_Animation(_uint iAnimIndex)
{
	return m_Animations[iAnimIndex];
}

vector<class CBone*>& CModel::Get_Bones()
{
	return m_Bones;
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
	strcpy_s(m_szFilePath, MAX_PATH, strFilePath.c_str());
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


		if (eType == ModelType::Anim)
		{
			if (FAILED(Read_TriggerEffects(strFilePath)))
			{
				return E_FAIL;
			}
		}
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
	if (pArg != nullptr)
	{
		m_pOwnerTransform = reinterpret_cast<CTransform*>(pArg);
		Safe_AddRef(m_pOwnerTransform);
	}

	for (size_t i = 0; i < m_TriggerEffects.size(); i++)
	{
		_mat* pMatrix = new _mat{};
		m_EffectMatrices.push_back(pMatrix);
	}

	return S_OK;
}

void CModel::Play_Animation(_float fTimeDelta)
{
	m_Animations[m_AnimDesc.iAnimIndex]->Update_TransformationMatrix(m_Bones, fTimeDelta * m_AnimDesc.fAnimSpeedRatio, m_isAnimChanged, m_AnimDesc.isLoop,
		m_AnimDesc.bSkipInterpolation, m_AnimDesc.fInterpolationTime, m_AnimDesc.fDurationRatio);

	for (auto& pBone : m_Bones)
	{
		pBone->Update_CombinedMatrix(m_Bones);
	}
	
	for (size_t i = 0; i < m_TriggerEffects.size(); i++)
	{
		if (m_AnimDesc.iAnimIndex == m_TriggerEffects[i].iStartAnimIndex &&
			m_Animations[m_AnimDesc.iAnimIndex]->Get_CurrentAnimPos() >= m_TriggerEffects[i].fStartAnimPos)
		{
			//이펙트 생성
			_mat PosOffset{};
			PosOffset.Position(m_TriggerEffects[i].OffsetMatrix.Position());
			_mat ScaleRotationOffset = m_TriggerEffects[i].OffsetMatrix;
			ScaleRotationOffset.Position(_vec4(0.f, 0.f, 0.f, 1.f));
			
			*m_EffectMatrices[i] = ScaleRotationOffset * *m_Bones[m_TriggerEffects[i].iBoneIndex]->Get_CombinedMatrix() * m_PivotMatrix * m_pOwnerTransform->Get_World_Matrix() * PosOffset;
			m_pGameInstance->Create_Effect(m_TriggerEffects[i].strEffectName, m_EffectMatrices[i], m_TriggerEffects[i].IsFollow);
			if (not m_TriggerEffects[i].IsRotateToBone)
			{
				m_TriggerEffects[i].BoneCombinedMatrix = *m_Bones[m_TriggerEffects[i].iBoneIndex]->Get_CombinedMatrix();
			}
		}
		if (m_AnimDesc.iAnimIndex == m_TriggerEffects[i].iEndAnimIndex &&
			m_Animations[m_AnimDesc.iAnimIndex]->Get_CurrentAnimPos() >= m_TriggerEffects[i].fEndAnimPos)
		{
			//이펙트 제거
			m_pGameInstance->Delete_Effect(m_EffectMatrices[i]);
		}
	}

	for (size_t i = 0; i < m_TriggerEffects.size(); i++)
	{
		if (m_TriggerEffects[i].IsFollow)
		{
			//이펙트 위치 갱신
			_mat PosOffset{};
			PosOffset.Position(m_TriggerEffects[i].OffsetMatrix.Position());
			_mat ScaleRotationOffset = m_TriggerEffects[i].OffsetMatrix;
			ScaleRotationOffset.Position(_vec4(0.f, 0.f, 0.f, 1.f));
			if (m_TriggerEffects[i].IsRotateToBone)
			{
				*m_EffectMatrices[i] = ScaleRotationOffset * *m_Bones[m_TriggerEffects[i].iBoneIndex]->Get_CombinedMatrix() * m_PivotMatrix * m_pOwnerTransform->Get_World_Matrix() * PosOffset;
			}
			else if (not m_TriggerEffects[i].IsRotateToBone)
			{
				_vector vScale{}, vRotation{}, vPosition{};
				XMMatrixDecompose(&vScale, &vRotation, &vPosition, m_TriggerEffects[i].BoneCombinedMatrix);
				_mat BoneCombinedMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), XMVectorSet(0.f, 0.f, 0.f, 0.f), vPosition);
				*m_EffectMatrices[i] = ScaleRotationOffset * BoneCombinedMatrix * m_PivotMatrix * m_pOwnerTransform->Get_World_Matrix() * PosOffset;
			}
		}
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

_bool CModel::Intersect_RayModel(_fmatrix WorldMatrix, _vec4* pPickPos)
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
		CAnimation* pAnimation = CAnimation::Create(File, m_Bones, m_PivotMatrix);
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

HRESULT CModel::Read_TriggerEffects(const string& strFilePath)
{
	_char szTriggerFilePath[MAX_PATH]{};
	_char szDirectory[MAX_PATH]{};
	_char szFileName[MAX_PATH]{};
	_char szExt[MAX_PATH] = ".effecttrigger";
	_splitpath_s(strFilePath.c_str(), nullptr, 0, szDirectory, MAX_PATH, szFileName, MAX_PATH, nullptr, 0);
	strcpy_s(szTriggerFilePath, MAX_PATH, szDirectory);
	strcat_s(szTriggerFilePath, MAX_PATH, szFileName);
	strcat_s(szTriggerFilePath, MAX_PATH, szExt);

	ifstream TriggerFile(szTriggerFilePath, ios::binary);
	if (TriggerFile.is_open())
	{
		_uint iNumTriggerEffect = { 0 };
		TriggerFile.read(reinterpret_cast<char*>(&iNumTriggerEffect), sizeof _uint);
		for (_uint i = 0; i < iNumTriggerEffect; i++)
		{
			TRIGGEREFFECT_DESC EffectDesc{};
			TriggerFile.read(reinterpret_cast<_char*>(&EffectDesc.iStartAnimIndex), sizeof(_int));
			TriggerFile.read(reinterpret_cast<_char*>(&EffectDesc.fStartAnimPos), sizeof(_float));
			TriggerFile.read(reinterpret_cast<_char*>(&EffectDesc.iEndAnimIndex), sizeof(_int));
			TriggerFile.read(reinterpret_cast<_char*>(&EffectDesc.fEndAnimPos), sizeof(_float));
			TriggerFile.read(reinterpret_cast<_char*>(&EffectDesc.IsFollow), sizeof(_bool));

			size_t iNameSize{};
			_tchar* pBuffer{};
			TriggerFile.read(reinterpret_cast<_char*>(&iNameSize), sizeof size_t);
			pBuffer = new _tchar[iNameSize / sizeof(_tchar)];
			TriggerFile.read(reinterpret_cast<_char*>(pBuffer), iNameSize);
			EffectDesc.strEffectName = pBuffer;
			Safe_Delete_Array(pBuffer);

			TriggerFile.read(reinterpret_cast<_char*>(&EffectDesc.iBoneIndex), sizeof(_uint));
			TriggerFile.read(reinterpret_cast<_char*>(&EffectDesc.OffsetMatrix), sizeof(_mat));
			TriggerFile.read(reinterpret_cast<_char*>(&EffectDesc.IsRotateToBone), sizeof(_bool));

			m_TriggerEffects.push_back(EffectDesc);
			m_iNumTriggersEffect++;
		}
		TriggerFile.close();
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

	for (auto& pEffectMatrix : m_EffectMatrices)
	{
		Safe_Delete(pEffectMatrix);
	}

	Safe_Release(m_pOwnerTransform);
}
