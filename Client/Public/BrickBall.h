#pragma once

#include "Client_Define.h"
#include "GameObject.h"

BEGIN(Client)

class CBrickBall final : public CGameObject
{
private:
	CBrickBall(_dev pDevice, _context pContext);
	CBrickBall(const CBrickBall& rhs);
	virtual ~CBrickBall() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg = nullptr) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	HRESULT Add_Collider();
	void Update_Collider();

public:
	void Parry_Success();
	void RayCast();

private:
	CShader* m_pShaderCom = { nullptr };
	CRenderer* m_pRendererCom = { nullptr };
	CCollider* m_pColliderCom = { nullptr };

private:
	_bool m_bShoot = { false };
	_float m_fLifeTime = {};

private: // SIX_MISSILE
	static _uint m_iMissileID;
	_uint m_iMissileIndex = {};
	_float m_fDepartTime = {};
	_float m_fEffectTimer{};
	_bool m_bParryingOk{};

private:
	_mat m_EffectMatrix{};
	class CEffect_Dummy* m_pEffect_Ball{};

public:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CBrickBall* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END