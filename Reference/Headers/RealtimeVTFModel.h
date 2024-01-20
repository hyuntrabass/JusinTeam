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

	// �ش� ���� Ÿ�Կ� �� �ش� �����Ͽ� �־� �����Ұ̴ϴ�(isRender�� true ������ ���� �ҰŶ�� ���մϴ�)
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

	// �� ���̽� �𵨿� �ִ� �޽��� ������ �����ɴϴ�
	const _uint& Get_NumMeshes() const {
		return m_iNumMeshes;
	}

	// �� ���� Ÿ���� ����� �����ɴϴ�
	const _uint Get_NumPartType() const {
		return m_Parts.size();
	}

	const _bool& IsAnimationFinished(_uint iAnimIndex) const;
	const _uint& Get_CurrentAnimationIndex() const;
	const _float& Get_CurrentAnimPos() const;
	const _mat* Get_BoneMatrix(const _char* pBoneName) const;

	// �ش� ����Ÿ�Կ� �ִ� ������ �׷��� �ϴ��� �˾ƺ��ϴ�
	const _bool Get_PartIsRender(_uint iPartType, _uint iPartID);

	// �ش� ���� Ÿ�Կ� �ִ� ������ �޽� ������ �����ɴϴ�
	const _uint Get_Num_PartMeshes(_uint iPartType, _uint iPartID) const;

	// �ش� ����Ÿ�Կ� �ִ� ���� ������ �����ɴϴ�
	const _uint Get_NumPart(_uint iPartType) const;
	
	// �ش� ����Ÿ�Կ� �ִ� ������ �׸��� �Ұ̴ϴ�.
	HRESULT Seting_Render(_uint iPartType, _uint iPartID);

	// �ش� ���̽� �𵨿� �ִ� ���׸����� ���ε��մϴ�
	HRESULT Bind_Material(class CShader* pShader, const _char* pVariableName, _uint iMeshIndex, TextureType eTextureType);

	// �ش� ���̽� �𵨿� �ִ� ���� ���ε� �մϴ�
	HRESULT Bind_Bone(class CShader* pShader);

	// �ش� ���̽� �𵨿� �ִ� �޽��� �׸��ϴ�
	HRESULT Render(_uint iMeshIndex);

	// �ش� ���� ���� ���׸����� �����ɴϴ�
	HRESULT Bind_Part_Material(class CShader* pShader, const _char* pVariableName, TextureType eTextureType, _uint iPartType, _uint iPartID, _uint iPartMeshIndex);
	
	// �ش� ���� ���� �޽��� �׸��ϴ�
	HRESULT Render_Part(_uint iPartType, _uint iPartID, _uint iPartMeshIndex);

public:
	// �� ���̽� �𵨿� �ش� ���� �� ������ ���Դϴ�.(Loader ���� �ؾ��մϴ�)
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
    // CComponent��(��) ���� ��ӵ�
    virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END