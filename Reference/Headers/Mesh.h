#pragma once
#include "VIBuffer.h"
#include "Model.h"

BEGIN(Engine)

class CMesh final : public CVIBuffer
{
private:
	CMesh(_dev pDevice, _context pContext);
	CMesh(const CMesh& rhs);
	virtual ~CMesh() = default;

public:
	const _uint& Get_MatIndex() const { return m_iMatIndex; }

public:
	HRESULT Init_Prototype(ModelType eType, ifstream& ModelFile, _mat OffsetMatrix);
	HRESULT Init(void* pArg) override;

public:
	HRESULT Bind_BoneMatrices(class CShader* pShader, const vector<class CBone*>& Bones, const _char* pVariableName, _mat PivotMatrix);
	_bool Intersect_RayMesh(_mat WorldMatrix, _vec4* pPickPos);
	void Apply_TransformToActor(_mat WorldMatrix);

	void Set_Bone_Offset(const vector<class CBone*>& Bones);
	vector<_float3>  Get_VerticesPos(){ return m_VerticesPos; }
	vector<_float3>  Get_VerticesNor(){ return m_VerticesNor; }

private:
	_float3* m_pVerticesPos{ nullptr };
	_float3* m_pVerticesNor{ nullptr };
	_ulong* m_pIndices{ nullptr };

	_char m_szName[MAX_PATH]{};
	_uint m_iMatIndex{};
	_uint m_iNumBones{};
	vector<_uint> m_BoneIndices{};
	vector<_mat> m_OffsetMatrices{};

	vector<_float3> m_VerticesPos{};
	vector<_float3> m_VerticesNor{};

	_mat* m_BoneMatrices{};

	PxRigidStatic* m_pActor{ nullptr };
	ModelType m_eType{};


private:
	HRESULT Ready_StaticMesh(ifstream& ModelFile, _mat OffsetMatrix);
	HRESULT Ready_AnimMesh(ifstream& ModelFile);

public:
	static CMesh* Create(_dev pDevice, _context pContext, ModelType eType, ifstream& ModelFile, _mat OffsetMatrix);
	virtual CComponent* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END