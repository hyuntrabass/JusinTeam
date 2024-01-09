#pragma once
#include "Component.h"

BEGIN(Engine)

enum class ModelType
{
	Static,
	Anim,
	Collision,
	End
};

struct ANIM_DESC
{
	_uint iAnimIndex{};
	_bool isLoop{};
	_float fAnimSpeedRatio{ 1.f };
	_bool bSkipInterpolation{};
	_float fInterpolationTime{ 0.2f };
	_float fDurationRatio{ 1.f };
	_bool bRestartAnimation{ false };
};

class ENGINE_DLL CModel final : public CComponent
{
private:
	CModel(_dev pDevice, _context pContext);
	CModel(const CModel& rhs);
	virtual ~CModel() = default;

public:
	const _uint& Get_NumMeshes() const;
	const _bool& IsAnimationFinished(_uint iAnimIndex) const;
	const _uint& Get_CurrentAnimationIndex() const;
	const _float& Get_CurrentAnimPos() const;
	const _float44* Get_BoneMatrix(const _char* pBoneName) const;
	_matrix Get_PivotMatrix();

	void Set_Animation(ANIM_DESC Animation_Desc);

public:
	HRESULT Init_Prototype(const string& strFilePath, const _bool& isCOLMesh, _fmatrix PivotMatrix);
	HRESULT Init(void* pArg) override;

public:
	void Play_Animation(_float fTimeDelta);
	HRESULT Bind_BoneMatrices(_uint iMeshIndex, class CShader* pShader, const _char* pVariableName);
	HRESULT Bind_Material(class CShader* pShader, const _char* pVariableName, _uint iMeshIndex, TextureType eTextureType);
	void Apply_TransformToActor(_fmatrix WorldMatrix);

	HRESULT Render(_uint iMeshIndex);

	_bool Intersect_RayModel(_fmatrix WorldMatrix, _float4* pPickPos);

private:
	_uint m_iNumMeshes{};
	vector<class CMesh*> m_Meshes{};

	_uint m_iNumMaterials{};
	vector<Model_Material> m_Materials{};

	vector<class CBone*> m_Bones{};

	_uint m_iNumAnimations{};
	vector<class CAnimation*> m_Animations{};

	_float44 m_PivotMatrix{};

	_uint m_iNumFaceMeshes{};
	_uint m_iNumHeadMeshes{};
	_uint m_iNumLowerMeshes{};
	_uint m_iNumUpperMeshes{};

	_uint m_iPrevAnimIndex{};
	_bool m_isAnimChanged{};

	ANIM_DESC m_AnimDesc{};

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