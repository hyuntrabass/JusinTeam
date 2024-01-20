#pragma once
#include "Component.h"

BEGIN(Engine)


class ENGINE_DLL CModel final : public CComponent
{
private:
	CModel(_dev pDevice, _context pContext);
	CModel(const CModel& rhs);
	virtual ~CModel() = default;

public:
	const _char* Get_FilePath() const;
	const _uint& Get_NumMeshes() const;
	const _uint& Get_NumAnim() const;
	const _uint Get_NumBones() const;
	const _bool& IsAnimationFinished(_uint iAnimIndex) const;
	const _uint& Get_CurrentAnimationIndex() const;
	const _float& Get_CurrentAnimPos() const;
	const _mat* Get_BoneMatrix(const _char* pBoneName) const;
	_mat Get_PivotMatrix();
	vector<class CAnimation*>& Get_Animations();
	class CAnimation* Get_Animation(_uint iAnimIndex);
	vector<class CBone*>& Get_Bones();

	void Set_Animation(ANIM_DESC Animation_Desc);
	vector<VTXSTATICMESH> Get_StaticMeshVertices();
	vector<_ulong> Get_StaticMeshIndices();

	_uint Get_NumIndices();

public:
	HRESULT Init_Prototype(const string& strFilePath, const _bool& isCOLMesh, _fmatrix PivotMatrix);
	HRESULT Init(void* pArg) override;

public:
	void Play_Animation(_float fTimeDelta);
	HRESULT Bind_BoneMatrices(_uint iMeshIndex, class CShader* pShader, const _char* pVariableName);
	HRESULT Bind_Material(class CShader* pShader, const _char* pVariableName, _uint iMeshIndex, TextureType eTextureType);
	void Apply_TransformToActor(_fmatrix WorldMatrix);

	HRESULT Render(_uint iMeshIndex);

	_bool Intersect_RayModel(_fmatrix WorldMatrix, _vec4* pPickPos);

private:
	_char m_szFilePath[MAX_PATH] = "";
	_float3* m_Vertices{};

	_uint m_iNumMeshes{};
	vector<class CMesh*> m_Meshes{};

	_uint m_iNumMaterials{};
	vector<Model_Material> m_Materials{};

	vector<class CBone*> m_Bones{};

	_uint m_iNumAnimations{};
	vector<class CAnimation*> m_Animations{};

	_mat m_PivotMatrix{};

	_uint m_iNumFaceMeshes{};
	_uint m_iNumHeadMeshes{};
	_uint m_iNumLowerMeshes{};
	_uint m_iNumUpperMeshes{};

	_uint m_iPrevAnimIndex{};
	_bool m_isAnimChanged{};

	ANIM_DESC m_AnimDesc{};

	ID3D11Texture2D* m_pTexture{ nullptr };
	ID3D11ShaderResourceView* m_pSRV{ nullptr };

private:
	HRESULT Read_Bones(ifstream& File);
	HRESULT Read_Meshes(ifstream& File, const ModelType& eType, _fmatrix PivotMatrix);
	HRESULT Read_Animations(ifstream& File);
	HRESULT Read_Materials(ifstream& File, const string& strFilePath);

public:
	static CModel* Create(_dev pDevice, _context pContext, const string& strFilePath, const _bool& isCOLMesh = false, _fmatrix PivotMatrix = XMMatrixIdentity());
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END