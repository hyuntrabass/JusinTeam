#pragma once
#include "Component.h"

BEGIN(Engine)

typedef struct tagAnimTransArray {
	using TransformArrayType = array<_mat, 300>;
	array<TransformArrayType, 1000> TransformArray;
}ANIMTRANS_ARRAY;

// 이 VTF는 Prototype 단계에서 애니메이션을 미리 계산합니다

class ENGINE_DLL CVTFModel final :
    public CComponent
{
private:
	CVTFModel(_dev pDevice, _context pContext);
	CVTFModel(const CVTFModel& rhs);
	virtual ~CVTFModel() = default;

public:
	HRESULT Init_Prototype(const string& strFilePath, const _bool& isCOLMesh, _fmatrix PivotMatrix);
	HRESULT Init(void* pArg) override;

public:
	HRESULT Play_Animation(_float fTimeDelta);

	void Set_Animation(ANIM_DESC Animation_Desc);

	void Set_NextAnimationIndex(_int iAnimIndex) {
		m_iNextAnimIndex = iAnimIndex;
	}

	const _uint& Get_NumMeshes() const {
		return m_iNumMeshes;
	}

	HRESULT Bind_Material(class CShader* pShader, const _char* pVariableName, _uint iMeshIndex, TextureType eTextureType);
	HRESULT Bind_Animation(class CShader* pShader);
	HRESULT Bind_OldAnimation(class CShader* pShader);

	HRESULT Render(_uint iMeshIndex);

private:
	ModelType m_eType = ModelType::End;

	_uint m_iNumMeshes = 0;
	vector<class CMesh*> m_Meshes;

	_uint m_iNumMaterials = 0;
	vector<Model_Material> m_Materials;

	vector<class CBone*> m_Bones;

	_uint m_iNumAnimations = 0;
	vector<class CAnimation*> m_Animations;

	_mat m_PivotMatrix = {};

	PLAYANIM_DESC m_PlayAnimDesc;
	ANIMTIME_DESC m_OldAnimDesc;
	_int m_iCurrentAnimIndex = 0;
	_int m_iNextAnimIndex = -1;
	_bool m_isLoop = false;
	_bool m_isFinished = false;

	_bool m_isAnimChanged{};
	_bool m_isInterpolating{};
	ANIM_DESC m_AnimDesc{};

private:
	// For_Animation
	ID3D11Texture2D* m_pTexture = nullptr;
	ID3D11ShaderResourceView* m_pSRV = nullptr;

private:
	HRESULT Read_Bones(ifstream& File);
	HRESULT Read_Meshes(ifstream& File, const ModelType& eType, _fmatrix PivotMatrix);
	HRESULT Read_Animations(ifstream& File);
	HRESULT Read_Materials(ifstream& File, const string& strFilePath);
	HRESULT CreateVTF(_uint MaxFrame);
	HRESULT CreateAnimationTransform(_uint iIndex, ANIMTRANS_ARRAY* pAnimTransform);

public:
	static CVTFModel* Create(_dev pDevice, _context pContext, const string& strFilePath, const _bool& isCOLMesh = false, _fmatrix PivotMatrix = XMMatrixIdentity());
    // CComponent을(를) 통해 상속됨
    virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END