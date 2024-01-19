#pragma once
#include "Component.h"

BEGIN(Engine)


class ENGINE_DLL CRealtimeVTFModel final :
    public CComponent
{
private:
	CRealtimeVTFModel(_dev pDevice, _context pContext);
	CRealtimeVTFModel(const CRealtimeVTFModel& rhs);
	virtual ~CRealtimeVTFModel() = default;

public:
	HRESULT Init_Prototype(const string& strFilePath, _fmatrix PivotMatrix);
	HRESULT Init(void* pArg) override;

public:
	HRESULT Play_Animation(_float fTimeDelta);

	void Set_Animation(ANIM_DESC Animation_Desc);

	_bool Get_isMotionBlur() const {
		return m_isUsingMotionBlur;
	}

	HRESULT Set_UsingMotionBlur(_bool UsingBlur);

	const _uint& Get_NumMeshes() const {
		return m_iNumMeshes;
	}

	const _uint Get_NumPart() const {
		return m_Parts.size();
	}

	const _uint Get_Num_PartMeshes(_uint iPartIndex) const;

	HRESULT Bind_Material(class CShader* pShader, const _char* pVariableName, _uint iMeshIndex, TextureType eTextureType);
	HRESULT Bind_Bone(class CShader* pShader);

	HRESULT Render(_uint iMeshIndex);

	HRESULT Bind_Part_Material(class CShader* pShader, const _char* pVariableName, TextureType eTextureType, _uint iPartIndex, _uint iPartMeshIndex);
	HRESULT Render_Part(_uint iPartIndex, _uint iPartMeshIndex);

public:
	HRESULT Seting_Parts(const string& strFilePath);

private:
	ModelType m_eType = ModelType::End;


	vector<class CPart_Model*> m_Parts;

	_uint m_iNumMeshes = 0;
	vector<class CMesh*> m_Meshes;

	_uint m_iNumMaterials = 0;
	vector<Model_Material> m_Materials;

	vector<class CBone*> m_Bones;

	_uint m_iNumAnimations = 0;
	vector<class CAnimation*> m_Animations;

	_mat m_PivotMatrix = {};

	ANIM_DESC m_AnimDesc{};
	_bool m_isAnimChanged = false;

	_bool m_isUsingMotionBlur = false;

private:
	// For_Animation
	ID3D11Texture2D* m_pOldBoneTexture = nullptr;
	ID3D11ShaderResourceView* m_pOldBoneSRV = nullptr;
	
	ID3D11Texture2D* m_pBoneTexture = nullptr;
	ID3D11ShaderResourceView* m_pBoneSRV = nullptr;

private:
	HRESULT Read_Bones(ifstream& File);
	HRESULT Read_Meshes(ifstream& File, const ModelType& eType, _fmatrix PivotMatrix);
	HRESULT Read_Animations(ifstream& File);
	HRESULT Read_Materials(ifstream& File, const string& strFilePath);
	HRESULT CreateVTF();
	HRESULT UpdateBoneTexture(vector<_mat>& CombinedBones);

public:
	static CRealtimeVTFModel* Create(_dev pDevice, _context pContext, const string& strFilePath, _fmatrix PivotMatrix = XMMatrixIdentity());
    // CComponent��(��) ���� ��ӵ�
    virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END