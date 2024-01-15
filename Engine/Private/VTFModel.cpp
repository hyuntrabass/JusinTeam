#include "VTFModel.h"
#include "Bone.h"
#include "Mesh.h"
#include "Texture.h"
#include "Animation.h"

CVTFModel::CVTFModel(_dev pDevice, _context pContext)
	: CComponent(pDevice, pContext)
{
}

CVTFModel::CVTFModel(const CVTFModel& rhs)
	: CComponent(rhs)
	, m_eType(rhs.m_eType)
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

HRESULT CVTFModel::Init_Prototype(const string& strFilePath, const _bool& isCOLMesh, _fmatrix PivotMatrix)
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

HRESULT CVTFModel::Init(void* pArg)
{
	if (ModelType::Anim == m_eType) {
		_uint iMax = 0;
		for (auto& pAnimation : m_Animations)
			iMax = max(iMax, pAnimation->Get_MaxFrame());

		if (FAILED(CreateVTF(iMax)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CVTFModel::Read_Bones(ifstream& File)
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

HRESULT CVTFModel::Read_Meshes(ifstream& File, const ModelType& eType, _fmatrix PivotMatrix)
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

HRESULT CVTFModel::Read_Animations(ifstream& File)
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

HRESULT CVTFModel::Read_Materials(ifstream& File, const string& strFilePath)
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

HRESULT CVTFModel::CreateVTF(_uint MaxFrame)
{
	vector<ANIMTRANS_ARRAY> AnimTransforms;
	AnimTransforms.resize(m_iNumAnimations);

	for (size_t i = 0; i < m_iNumAnimations; i++)
	{
		if (FAILED(CreateAnimationTransform(i, AnimTransforms)))
			return E_FAIL;
	}

	D3D11_TEXTURE2D_DESC Desc = {};
	Desc.Width = m_Bones.size() * 4;
	Desc.Height = MaxFrame;
	Desc.ArraySize = m_iNumAnimations;
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
		_uint taehee = i * AnimationSize;

		BYTE* AnimationPtr = reinterpret_cast<BYTE*>(AllAnimationPtr) + taehee;

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

HRESULT CVTFModel::CreateAnimationTransform(_uint iIndex, vector<ANIMTRANS_ARRAY>& AnimTransforms)
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

CVTFModel* CVTFModel::Create(_dev pDevice, _context pContext, const string& strFilePath, const _bool& isCOLMesh, _fmatrix PivotMatrix)
{
	CVTFModel* pInstance = new CVTFModel(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype(strFilePath, isCOLMesh, PivotMatrix)))
	{
		MSG_BOX("Failed to Create : CVTFModel");
	}

	return pInstance;
}

CComponent* CVTFModel::Clone(void* pArg)
{
	CVTFModel* pInstance = new CVTFModel(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CVTFModel");
	}

	return pInstance;
}

void CVTFModel::Free()
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
