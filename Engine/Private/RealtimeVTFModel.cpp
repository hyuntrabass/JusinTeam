#include "RealtimeVTFModel.h"
#include "Bone.h"
#include "Mesh.h"
#include "Texture.h"
#include "Animation.h"
#include "Shader.h"
#include "Part_Model.h"

CRealtimeVTFModel::CRealtimeVTFModel(_dev pDevice, _context pContext)
	: CComponent(pDevice, pContext)
{
}

CRealtimeVTFModel::CRealtimeVTFModel(const CRealtimeVTFModel& rhs)
	: CComponent(rhs)
	, m_eType(rhs.m_eType)
	, m_iNumMeshes(rhs.m_iNumMeshes)
	, m_iNumMaterials(rhs.m_iNumMaterials)
	, m_Materials(rhs.m_Materials)
	, m_PivotMatrix(rhs.m_PivotMatrix)
	, m_iNumAnimations(rhs.m_iNumAnimations)
{
	for (auto& pPrototypeBone : rhs.m_Bones) {
		CBone* pBone = pPrototypeBone->Clone();

		m_Bones.push_back(pBone);
	}

	for (auto& pPrototypeAnimation : rhs.m_Animations) {
		CAnimation* pAnimation = pPrototypeAnimation->Clone();

		m_Animations.push_back(pAnimation);
	}

	for (auto& pPrototypeMesh : rhs.m_Meshes) {
		CMesh* pMesh = reinterpret_cast<CMesh*>(pPrototypeMesh->Clone());

		m_Meshes.push_back(pMesh);
	}

	for (auto& Material : m_Materials)
		for (auto& pTexture : Material.pMaterials)
			Safe_AddRef(pTexture);

	//for (auto& PrototypePartVector : rhs.m_Parts) {
	//	vector<CPart_Model*> Parts;
	//	for (auto& pPrototypePart : PrototypePartVector.second) {
	//		CPart_Model* pPart = reinterpret_cast<CPart_Model*>(pPrototypePart->Clone());

	//		Parts.push_back(pPart);
	//	}
	//	m_Parts.emplace(PrototypePartVector.first, Parts);
	//}

	for (auto& PrototypePartPair : rhs.m_PrototypeParts) {
		CPart_Model* pPart = reinterpret_cast<CPart_Model*>(PrototypePartPair.second->Clone());
		m_PrototypeParts.emplace(PrototypePartPair.first, pPart);
	}
}

HRESULT CRealtimeVTFModel::Init_Prototype(const string& strFilePath, _fmatrix PivotMatrix)
{
	XMStoreFloat4x4(&m_PivotMatrix, PivotMatrix);

	m_eType = ModelType::End;

	_char szDirectory[MAX_PATH]{};
	_char szFileName[MAX_PATH]{};
	_char szTriggerExt[MAX_PATH] = ".animtrigger";
	_char szExt[MAX_PATH]{};

	_splitpath_s(strFilePath.c_str(), nullptr, 0, szDirectory, MAX_PATH, szFileName, MAX_PATH, szExt, MAX_PATH);
	if (!strcmp(szExt, ".hyuntraanimmesh"))
	{
		m_eType = ModelType::Anim;
	}
	else
	{
		m_eType = ModelType::Static;
	}

	ifstream ModelFile(strFilePath.c_str(), ios::binary);
	if (ModelFile.is_open())
	{
		if (m_eType == ModelType::Anim)
		{
			if (FAILED(Read_Bones(ModelFile)))
			{
				return E_FAIL;
			}
		}

		if (FAILED(Read_Meshes(ModelFile, m_eType, PivotMatrix)))
		{
			return E_FAIL;
		}

		if (m_eType == ModelType::Anim) {
			for (auto& pMesh : m_Meshes)
				pMesh->Set_Bone_Offset(m_Bones);
		}


		if (FAILED(Read_Materials(ModelFile, strFilePath)))
		{
			return E_FAIL;
		}

		if (m_eType == ModelType::Anim)
		{
			if (FAILED(Read_Animations(ModelFile)))
			{
				return E_FAIL;
			}
		}

		ModelFile.close();

		if (m_eType == ModelType::Anim)
		{
			_char szTriggerFilePath[MAX_PATH]{};
			strcpy_s(szTriggerFilePath, MAX_PATH, szDirectory);
			strcat_s(szTriggerFilePath, MAX_PATH, szFileName);
			strcat_s(szTriggerFilePath, MAX_PATH, szTriggerExt);

			ifstream TriggerFile(szTriggerFilePath, ios::binary);
			if (TriggerFile.is_open())
			{
				_uint iAnimIndex = { 0 };
				TriggerFile.read(reinterpret_cast<char*>(&iAnimIndex), sizeof _uint);

				_uint iNumTrigger = { 0 };
				TriggerFile.read(reinterpret_cast<char*>(&iNumTrigger), sizeof _uint);
				for (_uint i = 0; i < iNumTrigger; i++)
				{
					_float fTrigger = { 0.f };
					TriggerFile.read(reinterpret_cast<char*>(&fTrigger), sizeof _float);
					m_Animations[iAnimIndex]->Add_Trigger(fTrigger);
				}

				TriggerFile.close();
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

HRESULT CRealtimeVTFModel::Init(void* pArg)
{
	if (FAILED(CreateVTF()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRealtimeVTFModel::Place_Parts(PART_DESC& ePartDesc, _bool isRender)
{

	auto Protoiter = m_PrototypeParts.find(ePartDesc.FileName);

	if (m_PrototypeParts.end() == Protoiter) {
		MSG_BOX("그런 파일 이름 없어용");
		return E_FAIL;
	}

	auto iter = m_Parts.find(ePartDesc.ePartType);

	if (m_Parts.end() == iter) {
		vector<CPart_Model*> PartVec;
		PartVec.push_back(Protoiter->second);

		m_Parts.emplace(ePartDesc.ePartType, PartVec);
	}
	else
		iter->second.push_back(Protoiter->second);

	Protoiter->second->Set_isRender(isRender);

	return S_OK;
}

HRESULT CRealtimeVTFModel::Play_Animation(_float fTimeDelta)
{
	if (true == m_isUsingMotionBlur)
		m_pContext->CopyResource(m_pOldBoneTexture, m_pBoneTexture);

	m_Animations[m_AnimDesc.iAnimIndex]->Update_TransformationMatrix(m_Bones, fTimeDelta * m_AnimDesc.fAnimSpeedRatio, m_isAnimChanged, m_AnimDesc.isLoop,
		m_AnimDesc.bSkipInterpolation, m_AnimDesc.fInterpolationTime, m_AnimDesc.fDurationRatio);


	vector<_mat> CombinedBones;

	for (auto& pBone : m_Bones) {
		pBone->Update_CombinedMatrix(m_Bones);
		_mat BoneMatrix = pBone->Get_OffsetMatrix() * *(pBone->Get_CombinedMatrix()) * m_PivotMatrix;
		CombinedBones.push_back(BoneMatrix);
	}

	if (FAILED(UpdateBoneTexture(CombinedBones)))
		return E_FAIL;

	CombinedBones.clear();

	return S_OK;
}

void CRealtimeVTFModel::Set_Animation(ANIM_DESC Animation_Desc)
{
	if (m_AnimDesc.iAnimIndex != Animation_Desc.iAnimIndex or
		Animation_Desc.bRestartAnimation) {

		m_AnimDesc = Animation_Desc;

		m_isAnimChanged = true;

		for (auto& pAnim : m_Animations)
			pAnim->ResetFinished();

		if (m_AnimDesc.iAnimIndex >= m_iNumAnimations)
			m_AnimDesc.iAnimIndex = m_iNumAnimations - 1;
		else if (0 > m_AnimDesc.iAnimIndex)
			m_AnimDesc.iAnimIndex = 0;
	}

}

const _bool& CRealtimeVTFModel::IsAnimationFinished(_uint iAnimIndex) const
{
	return m_Animations[iAnimIndex]->IsFinished();
}

const _uint& CRealtimeVTFModel::Get_CurrentAnimationIndex() const
{
	return m_AnimDesc.iAnimIndex;
}

const _float& CRealtimeVTFModel::Get_CurrentAnimPos() const
{
	return m_Animations[m_AnimDesc.iAnimIndex]->Get_CurrentAnimPos();
}

const _mat* CRealtimeVTFModel::Get_BoneMatrix(const _char* pBoneName) const
{
	auto iter = find_if(m_Bones.begin(), m_Bones.end(), [&pBoneName](CBone* pBone) {
		if (!strcmp(pBone->Get_BoneName(), pBoneName))
			return true;
		return false;
	});

	if (m_Bones.end() == iter) {
		MSG_BOX("Can't Find Bone");
		return nullptr;
	}

	return (*iter)->Get_CombinedMatrix();
}

const _bool CRealtimeVTFModel::Get_PartIsRender(_uint iPartType, _uint iPartID)
{
	auto iter = m_Parts.find(iPartType);

	if (m_Parts.end() == iter) {
		MSG_BOX("그런 파츠 타입 없어용");
		return false;
	}

	if (0 > iPartID or iter->second.size() <= iPartID) {
		MSG_BOX("해당 파츠는 없어용");
		return false;
	}

	return iter->second[iPartID]->Get_isRender();
}

const _uint CRealtimeVTFModel::Get_Num_PartMeshes(_uint iPartType, _uint iPartID) const
{
	auto iter = m_Parts.find(iPartType);

	if (m_Parts.end() == iter) {
		MSG_BOX("그 파츠 타입은 없구연");
		return 0;
	}

	return iter->second[iPartID]->Get_NumMeshes();
}

const _uint CRealtimeVTFModel::Get_NumPart(_uint iPartType) const
{
	auto iter = m_Parts.find(iPartType);

	if (m_Parts.end() == iter) {
		MSG_BOX("그 파츠 타입은 없구연");
		return 0;
	}

	return iter->second.size();
}

HRESULT CRealtimeVTFModel::Seting_Render(_uint iPartType, _uint iPartID)
{
	auto iter = m_Parts.find(iPartType);

	if (m_Parts.end() == iter) {
		MSG_BOX("그 파츠 타입은 없구연");
		return E_FAIL;
	}

	if (0 > iPartID or iter->second.size() <= iPartID) {
		MSG_BOX("그 파츠는 없구연");
		return E_FAIL;
	}

	for (auto& pPart : iter->second) {
		pPart->Set_isRender(false);
	}

	iter->second[iPartID]->Set_isRender(true);

	return S_OK;
}

HRESULT CRealtimeVTFModel::Bind_Material(CShader* pShader, const _char* pVariableName, _uint iMeshIndex, TextureType eTextureType)
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

HRESULT CRealtimeVTFModel::Bind_Bone(CShader* pShader)
{
	if (FAILED(pShader->Bind_ShaderResourceView("g_BoneTexture", m_pBoneSRV)))
		return E_FAIL;

	if (true == m_isUsingMotionBlur) {
		if (FAILED(pShader->Bind_ShaderResourceView("g_OldBoneTexture", m_pOldBoneSRV)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CRealtimeVTFModel::Render(_uint iMeshIndex)
{
	if (iMeshIndex >= m_iNumMeshes)
		return E_FAIL;

	if (FAILED(m_Meshes[iMeshIndex]->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRealtimeVTFModel::Bind_Part_Material(CShader* pShader, const _char* pVariableName, TextureType eTextureType, _uint iPartType, _uint iPartID, _uint iPartMeshIndex)
{
	auto iter = m_Parts.find(iPartType);

	if (m_Parts.end() == iter) {
		MSG_BOX("그 파츠 타입은 없구연");
		return E_FAIL;
	}

	return iter->second[iPartID]->Bind_Material(pShader, pVariableName, iPartMeshIndex, eTextureType);
}

HRESULT CRealtimeVTFModel::Render_Part(_uint iPartType, _uint iPartID, _uint iPartMeshIndex)
{
	auto iter = m_Parts.find(iPartType);

	if (m_Parts.end() == iter) {
		MSG_BOX("그 파츠 타입은 없구연");
		return E_FAIL;
	}

	return iter->second[iPartID]->Render(iPartMeshIndex);
}

HRESULT CRealtimeVTFModel::Seting_Parts(const string& strFilePath)
{
	CPart_Model* pPart = CPart_Model::Create(m_pDevice, m_pContext, strFilePath);

	if (nullptr == pPart)
		return E_FAIL;

	if (FAILED(pPart->Get_Bone_Offset(m_Bones)))
		return E_FAIL;

	_char szFileName[MAX_PATH]{};

	_splitpath_s(strFilePath.c_str(), nullptr, 0, nullptr, 0, szFileName, MAX_PATH, nullptr, 0);

	m_PrototypeParts.emplace(szFileName, pPart);

	return S_OK;
}

HRESULT CRealtimeVTFModel::Read_Bones(ifstream& File)
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

HRESULT CRealtimeVTFModel::Read_Meshes(ifstream& File, const ModelType& eType, _fmatrix PivotMatrix)
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

HRESULT CRealtimeVTFModel::Read_Animations(ifstream& File)
{
	File.read(reinterpret_cast<_char*>(&m_iNumAnimations), sizeof _uint);

	for (size_t i = 0; i < m_iNumAnimations; i++)
	{
		CAnimation* pAnimation = CAnimation::Create(File, m_Bones);
		if (!pAnimation)
		{
			MSG_BOX("Failed to Read Animations!");
			return E_FAIL;
		}
		m_Animations.push_back(pAnimation);
	}

	return S_OK;
}

HRESULT CRealtimeVTFModel::Read_Materials(ifstream& File, const string& strFilePath)
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

HRESULT CRealtimeVTFModel::CreateVTF()
{
	D3D11_TEXTURE2D_DESC Desc = {};
	Desc.Width = m_Bones.size() * 4;
	Desc.Height = 1;
	Desc.MipLevels = 1;
	Desc.ArraySize = 1;
	Desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	Desc.Usage = D3D11_USAGE_DYNAMIC;
	Desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	Desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	Desc.SampleDesc.Count = 1;

	if (FAILED(m_pDevice->CreateTexture2D(&Desc, nullptr, &m_pBoneTexture)))
		return E_FAIL;

	Desc.Usage = D3D11_USAGE_DEFAULT;
	Desc.CPUAccessFlags = 0;

	if (FAILED(m_pDevice->CreateTexture2D(&Desc, nullptr, &m_pOldBoneTexture)))
		return E_FAIL;
	
	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};
	SRVDesc.Format = Desc.Format;
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
	SRVDesc.Texture2DArray.MipLevels = 1;
	SRVDesc.Texture2DArray.ArraySize = 1;

	if (FAILED(m_pDevice->CreateShaderResourceView(m_pBoneTexture, &SRVDesc, &m_pBoneSRV)))
		return E_FAIL;

	if (FAILED(m_pDevice->CreateShaderResourceView(m_pOldBoneTexture, &SRVDesc, &m_pOldBoneSRV)))
		return E_FAIL;

	return S_OK;
}

HRESULT CRealtimeVTFModel::UpdateBoneTexture(vector<_mat>& CombinedBones)
{
	D3D11_MAPPED_SUBRESOURCE TexData = {};
	if (FAILED(m_pContext->Map(m_pBoneTexture, 0, D3D11_MAP_WRITE_DISCARD, 0, &TexData)))
		return E_FAIL;

	memcpy(TexData.pData, CombinedBones.data(), m_Bones.size() * sizeof(_mat));

	m_pContext->Unmap(m_pBoneTexture, 0);

	return S_OK;
}

CRealtimeVTFModel* CRealtimeVTFModel::Create(_dev pDevice, _context pContext, const string& strFilePath, _fmatrix PivotMatrix)
{
	CRealtimeVTFModel* pInstance = new CRealtimeVTFModel(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype(strFilePath, PivotMatrix)))
	{
		MSG_BOX("Failed to Create : CRealtimeVTFModel");
	}

	return pInstance;
}

CComponent* CRealtimeVTFModel::Clone(void* pArg)
{
	CRealtimeVTFModel* pInstance = new CRealtimeVTFModel(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CRealtimeVTFModel");
	}

	return pInstance;
}

void CRealtimeVTFModel::Free()
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

	for (auto& pPart : m_PrototypeParts)
		Safe_Release(pPart.second);

	m_PrototypeParts.clear();

	m_Parts.clear();

	Safe_Release(m_pOldBoneTexture);
	Safe_Release(m_pOldBoneSRV);

	Safe_Release(m_pBoneTexture);
	Safe_Release(m_pBoneSRV);
}
