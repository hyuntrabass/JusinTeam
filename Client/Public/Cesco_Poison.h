#pragma once

#include "GameObject.h"
#include "Client_Define.h"

BEGIN(Client)

class CCesco_Poison final : public CGameObject
{

private:
	CCesco_Poison(_dev pDevice, _context pContext);
	CCesco_Poison(const CCesco_Poison& rhs);
	virtual ~CCesco_Poison() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg = nullptr) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;

private:
	class CEffect_Dummy* m_pEffect{ nullptr };
	CRenderer* m_pRendererCom = { nullptr };
	CCollider* m_pBodyColliderCom = { nullptr };
	CTexture* m_pDissolveTextureCom = { nullptr };

private:
	_float m_fLifeTime{};
	_mat m_EffectMatrices{};
private:
	HRESULT Add_Components();


public:
	static CCesco_Poison* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END