#pragma once
#include "Client_Define.h"
#include "BlendObject.h"

BEGIN(Client)

class CSickle : public CBlendObject
{
private:
	CSickle(_dev pDevice, _context pContext);
	CSickle(const CSickle& rhs);
	virtual ~CSickle() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;

private:
	CCollider* m_pColliderCom{ nullptr };
	static const _uint m_iNumEffects{ 4 };
	class CEffect_Dummy* m_pEffects[m_iNumEffects]{};

#ifdef _DEBUG
	CRenderer* m_pRendererCom{ nullptr };
#endif // _DEBUG

private:
	_mat m_EffectMatrices{};
	_float m_fLifeTimer{};
	_float m_fLifeTimeLimit{};
	_bool m_hasAttacked{};

public:
	HRESULT Add_Components();

public:
	static CSickle* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END