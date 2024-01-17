#pragma once
#include "Component.h"

BEGIN(Engine)


typedef struct tagAnimTransArray {
	using TransformArrayType = array<_mat, 172>;
	array<TransformArrayType, 441> TransformArray;
}ANIMTRANS_ARRAY;

class ENGINE_DLL CRealtimeVTFModel final :
    public CComponent
{
private:
	CRealtimeVTFModel(_dev pDevice, _context pContext);
	CRealtimeVTFModel(const CRealtimeVTFModel& rhs);
	virtual ~CRealtimeVTFModel() = default;

public:
	HRESULT Init_Prototype(const string& strFilePath, const _bool& isCOLMesh, _fmatrix PivotMatrix);
	HRESULT Init(void* pArg) override;

public:
	HRESULT Play_Animation(_float fTimeDelta);

	HRESULT Set_NextAnimation(_uint iAnimIndex, _bool isLoop);

	void Set_NextAnimationIndex(_int iAnimIndex) {
		m_iNextAnimIndex = iAnimIndex;
	}

	const _uint& Get_NumMeshes() const {
		return m_iNumMeshes;
	}

	HRESULT Bind_Material(class CShader* pShader, const _char* pVariableName, _uint iMeshIndex, TextureType eTextureType);
	HRESULT Bind_Animation(class CShader* pShader);

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
	_int m_iCurrentAnimIndex = 0;
	_int m_iNextAnimIndex = -1;
	_bool m_isLoop = false;
	_bool m_isFinished = false;

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
	HRESULT CreateAnimationTransform(_uint iIndex, vector<ANIMTRANS_ARRAY>& AnimTransforms);

public:
	static CRealtimeVTFModel* Create(_dev pDevice, _context pContext, const string& strFilePath, const _bool& isCOLMesh = false, _fmatrix PivotMatrix = XMMatrixIdentity());
    // CComponent을(를) 통해 상속됨
    virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END