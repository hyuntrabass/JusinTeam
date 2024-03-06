#pragma once
#include "Client_Define.h"
#include "BlendObject.h"

BEGIN(Client)

class CSickleTrap : public CBlendObject
{
private:
	CSickleTrap(_dev pDevice, _context pContext);
	CSickleTrap(const CSickleTrap& rhs);
	virtual ~CSickleTrap() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;

public:
	CCollider* m_pColliderCom{ nullptr };
	static const _uint m_iNumEffects{ 1 };
	class CEffect_Dummy* m_pEffects[m_iNumEffects]{};

#ifdef _DEBUG
	CRenderer* m_pRendererCom{ nullptr };
#endif // _DEBUG

private:
	_int m_iSoundChannel = -1;
	_mat m_EffectMatrices{};
	_float m_fLifeTimer{};
	_float m_fLifeTimeLimit{};
	_float m_fAttDelay{};
public:
	HRESULT Add_Components();

public:
	static CSickleTrap* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END