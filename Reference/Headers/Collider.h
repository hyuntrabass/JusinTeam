#pragma once
#include "Component.h"

BEGIN(Engine)

enum class ColliderType
{
	AABB,
	OBB,
	Sphere,
	Frustum,
	End
};

struct Collider_Desc
{
	ColliderType eType{};

	_float3 vCenter{};
	_float fRadius{};
	_float3 vExtents{};
	_float3 vRadians{};
	_float44 matFrustum{};
};

class ENGINE_DLL CCollider final : public CComponent
{
private:
	CCollider(_dev pDevice, _context pContext);
	CCollider(const CCollider& rhs);
	virtual ~CCollider() = default;

public:
	virtual HRESULT Init_Prototype();
	virtual HRESULT Init(void* pArg);
	void Update(_fmatrix WorldMatrix);

public:
	_bool Intersect(const class CCollider* pTargetCollider);

#ifdef _DEBUGTEST
public:
	virtual HRESULT Render() override;
#endif // _DEBUG

private:
	ColliderType m_eType{ ColliderType::End };
	void* m_pBounder{ nullptr };
	void* m_pBounder_Origin{ nullptr };
	_bool m_isCollided{};

#ifdef _DEBUGTEST
private:
	PrimitiveBatch<VertexPositionColor>* m_pBatch{ nullptr };
	BasicEffect* m_pEffect{ nullptr };
	ID3D11InputLayout* m_pInputLayout{ nullptr };
#endif // _DEBUG


public:
	static CCollider* Create(_dev pDevice, _context pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END