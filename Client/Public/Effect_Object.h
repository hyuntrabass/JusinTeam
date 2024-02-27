#pragma once
#include "Client_Define.h"
#include "BlendObject.h"

BEGIN(Client)

class CEffect_Object final : public CBlendObject
{
private:
	CEffect_Object(_dev pDevice, _context pContext);
	CEffect_Object(const CEffect_Object& rhs);
	virtual ~CEffect_Object() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	HRESULT Add_Components();

private:
	EffectObjectInfo m_Info{};

	CRenderer* m_pRendererCom{ nullptr };
	CCollider* m_pColliderCom{ nullptr };

public:
	static CEffect_Object* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END