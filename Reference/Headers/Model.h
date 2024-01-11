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
	// 애님 인덱스
	_uint iAnimIndex{};
	// 루프인가
	_bool isLoop{};
	// 애님 스피드 비율 (1 = 정상 속도, 2 = 2배, 0.5 = 0.5배)
	_float fAnimSpeedRatio{ 1.f };
	// 선형 보간 스킵 할것인가
	_bool bSkipInterpolation{};
	// 선형 보간하는 시간
	_float fInterpolationTime{ 0.2f };
	// 얼마나 재생할지 비율 (1 = 끝까지 재생, 0.5 = 절반만 재생하고 끝남)
	_float fDurationRatio{ 1.f };
	// 처음부터 재생할지 (true로 두면 처음부터 재생함. Tick에서 true로 되어 있으면 계속 처음만 재생함)
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
	const _uint& Get_NumAnim() const;
	const _bool& IsAnimationFinished(_uint iAnimIndex) const;
	const _uint& Get_CurrentAnimationIndex() const;
	const _float& Get_CurrentAnimPos() const;
	const _mat* Get_BoneMatrix(const _char* pBoneName) const;
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

	_bool Intersect_RayModel(_fmatrix WorldMatrix, _vec4* pPickPos);

private:
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