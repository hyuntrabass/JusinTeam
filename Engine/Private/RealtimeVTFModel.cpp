#include "RealtimeVTFModel.h"
#include "Bone.h"
#include "Mesh.h"
#include "Texture.h"
#include "Animation.h"
#include "Shader.h"

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
}

HRESULT CRealtimeVTFModel::Init_Prototype(const string& strFilePath, const _bool& isCOLMesh, _fmatrix PivotMatrix)
{
	XMStoreFloat4x4(&m_PivotMatrix, PivotMatrix);

	m_eType = ModelType::End;

	_char szDirectory[MAX_PATH]{};
	_char szFileName[MAX_PATH]{};
	_char szTriggerExt[MAX_PATH] = ".animtrigger";
	_char szExt[MAX_PATH]{};

	if (false == isCOLMesh) {
		_splitpath_s(strFilePath.c_str(), nullptr, 0, szDirectory, MAX_PATH, szFileName, MAX_PATH, szExt, MAX_PATH);
		if (!strcmp(szExt, ".hyuntraanimmesh"))
		{
			m_eType = ModelType::Anim;
		}
		else
		{
			m_eType = ModelType::Static;
		}
	}
	else {
		m_eType = ModelType::Collision;
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
	m_isLoop = true;

	return S_OK;
}

HRESULT CRealtimeVTFModel::Play_Animation(_float fTimeDelta)
{

	return S_OK;
}

HRESULT CRealtimeVTFModel::Set_NextAnimation(_uint iAnimIndex, _bool isLoop)
{
	m_iNextAnimIndex = iAnimIndex;
	m_isLoop = isLoop;

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
	if (FAILED(pShader->Bind_ShaderResourceView("g_BoneTexture", m_pSRV)))
		return E_FAIL;

	return S_OK;
}

HRESULT CRealtimeVTFModel::Render(_uint iMeshIndex)
{
	if (FAILED(m_Meshes[iMeshIndex]->Render()))
		return E_FAIL;
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

HRESULT CRealtimeVTFModel::CreateVTF(_uint MaxFrame)
{
	vector<ANIMTRANS_ARRAY> AnimTransforms;
	AnimTransforms.resize(m_iNumAnimations);


	D3D11_TEXTURE2D_DESC Desc = {};
	Desc.Width = m_Bones.size() * 4;
	Desc.Height = 1;
	Desc.ArraySize = 1;
	Desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	Desc.Usage = D3D11_USAGE_IMMUTABLE;
	Desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	Desc.MipLevels = 1;
	Desc.SampleDesc.Count = 1;

	const _uint BoneMatrixSize = m_Bones.size() * sizeof(_mat);
	const _uint AnimationSize = BoneMatrixSize * MaxFrame;
	void* AllAnimationPtr = malloc(AnimationSize * m_iNumAnimations);

	for (size_t i = 0; i < m_iNumAnimations; i++)
	{
		_uint iAnimSize = i * AnimationSize;

		BYTE* AnimationPtr = reinterpret_cast<BYTE*>(AllAnimationPtr) + iAnimSize;

		for (size_t j = 0; j < MaxFrame; j++)
		{
			void* Ptr = AnimationPtr + j * BoneMatrixSize;
			memcpy(Ptr, AnimTransforms[i].TransformArray[j].data(), BoneMatrixSize);
		}
	}

	vector<D3D11_SUBRESOURCE_DATA> SubResourceData(m_iNumAnimations);

	for (size_t i = 0; i < m_iNumAnimations; i++)
	{
		void* Ptr = reinterpret_cast<BYTE*>(AllAnimationPtr) + i * BoneMatrixSize;
		SubResourceData[i].pSysMem = Ptr;
		SubResourceData[i].SysMemPitch = BoneMatrixSize;
		SubResourceData[i].SysMemSlicePitch = AnimationSize;
	}

	if (FAILED(m_pDevice->CreateTexture2D(&Desc, SubResourceData.data(), &m_pTexture)))
		return E_FAIL;

	free(AllAnimationPtr);

	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};
	SRVDesc.Format = Desc.Format;
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
	SRVDesc.Texture2DArray.MipLevels = 1;
	SRVDesc.Texture2DArray.ArraySize = m_iNumAnimations;

	if (FAILED(m_pDevice->CreateShaderResourceView(m_pTexture, &SRVDesc, &m_pSRV)))
		return E_FAIL;

	return S_OK;
}

HRESULT CRealtimeVTFModel::CreateAnimationTransform(_uint iIndex, vector<ANIMTRANS_ARRAY>& AnimTransforms)
{
	CAnimation* pAnimation = m_Animations[iIndex];

	for (size_t i = 0; i < pAnimation->Get_MaxFrame(); i++)
	{
		if (FAILED(pAnimation->Prepare_Animation(m_Bones, i)))
			return E_FAIL;


		for (size_t j = 0; j < m_Bones.size(); j++)
		{
			m_Bones[j]->Update_CombinedMatrix(m_Bones);

			AnimTransforms[iIndex].TransformArray[i][j] = m_Bones[j]->Get_OffsetMatrix() * *(m_Bones[j]->Get_CombinedMatrix()) * m_PivotMatrix;
		}
	}

	return S_OK;
}

CRealtimeVTFModel* CRealtimeVTFModel::Create(_dev pDevice, _context pContext, const string& strFilePath, const _bool& isCOLMesh, _fmatrix PivotMatrix)
{
	CRealtimeVTFModel* pInstance = new CRealtimeVTFModel(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype(strFilePath, isCOLMesh, PivotMatrix)))
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

	Safe_Release(m_pTexture);
	Safe_Release(m_pSRV);
}
