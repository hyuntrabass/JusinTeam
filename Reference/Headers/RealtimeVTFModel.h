#pragma once
#include "Component.h"

BEGIN(Engine)


class ENGINE_DLL CRealtimeVTFModel final :
    public CComponent
{
public:
	typedef struct tagPartName {
		_uint ePartType = 0;
		string FileName;
	}PART_DESC;
private:
	CRealtimeVTFModel(_dev pDevice, _context pContext);
	CRealtimeVTFModel(const CRealtimeVTFModel& rhs);
	virtual ~CRealtimeVTFModel() = default;

public:
	HRESULT Init_Prototype(const string& strFilePath, _fmatrix PivotMatrix);
	HRESULT Init(void* pArg) override;

	// 해당 파츠 타입에 이 해당 모델파일에 넣어 정리할겁니다(isRender는 true 넣으면 랜더 할거라고 정합니다)
	HRESULT Place_Parts(PART_DESC& ePartDesc, _bool isRender = false);

public:
	HRESULT Play_Animation(_float fTimeDelta);

	void Set_Animation(ANIM_DESC Animation_Desc);

	_bool Get_isMotionBlur() const {
		return m_isUsingMotionBlur;
	}

	void Set_UsingMotionBlur(_bool UsingBlur) {
		m_isUsingMotionBlur = UsingBlur;
	}

	// 이 베이스 모델에 있는 메쉬의 갯수를 가져옵니다
	const _uint& Get_NumMeshes() const {
		return m_iNumMeshes;
	}

	// 총 파츠 타입이 몇개인지 가져옵니다
	const _uint Get_NumPartType() const {
		return m_Parts.size();
	}

	const _bool& IsAnimationFinished(_uint iAnimIndex) const;
	const _uint& Get_CurrentAnimationIndex() const;
	const _float& Get_CurrentAnimPos() const;
	const _mat* Get_BoneMatrix(const _char* pBoneName) const;

	// 해당 파츠타입에 있는 파츠가 그려야 하는지 알아봅니다
	const _bool Get_PartIsRender(_uint iPartType, _uint iPartID);

	// 해당 파츠 타입에 있는 파츠의 메쉬 갯수를 가져옵니다
	const _uint Get_Num_PartMeshes(_uint iPartType, _uint iPartID) const;

	// 해당 파츠타입에 있는 모델의 갯수를 가져옵니다
	const _uint Get_NumPart(_uint iPartType) const;
	
	// 해당 파츠타입에 있는 파츠만 그리게 할겁니다.
	HRESULT Seting_Render(_uint iPartType, _uint iPartID);

	// 해당 베이스 모델에 있는 머테리얼을 바인드합니다
	HRESULT Bind_Material(class CShader* pShader, const _char* pVariableName, _uint iMeshIndex, TextureType eTextureType);

	// 해당 베이스 모델에 있는 뼈를 바인드 합니다
	HRESULT Bind_Bone(class CShader* pShader);

	// 해당 베이스 모델에 있는 메쉬를 그립니다
	HRESULT Render(_uint iMeshIndex);

	// 해당 파츠 모델의 마테리얼을 가져옵니다
	HRESULT Bind_Part_Material(class CShader* pShader, const _char* pVariableName, TextureType eTextureType, _uint iPartType, _uint iPartID, _uint iPartMeshIndex);
	
	// 해당 파츠 모델의 메쉬를 그립니다
	HRESULT Render_Part(_uint iPartType, _uint iPartID, _uint iPartMeshIndex);

public:
	// 이 베이스 모델에 해당 파츠 모델 파일을 붙입니다.(Loader 에서 해야합니다)
	HRESULT Seting_Parts(const string& strFilePath);

private:
	ModelType m_eType = ModelType::End;

	map<const string, class CPart_Model*> m_PrototypeParts;
	map<_uint, vector<class CPart_Model*>> m_Parts;
	//vector<class CPart_Model*> m_Parts;

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
    // CComponent을(를) 통해 상속됨
    virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END