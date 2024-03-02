#pragma once

#include "GameObject.h"
#include "Client_Define.h"

BEGIN(Client)

class CLog final : public CGameObject
{
public:
	typedef struct tagLogDesc
	{
		_mat WorldMatrix{};

	}LOG_DESC;

public:
	enum STATE
	{
		State_Idle,
		State_Fall,
		State_Die,
		State_End
	};

private:
	CLog(_dev pDevice, _context pContext);
	CLog(const CLog& rhs);
	virtual ~CLog() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg = nullptr) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual void Set_Damage(_int iDamage, _uint MonAttType = 0) override;

private:
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	CRenderer* m_pRendererCom = { nullptr };
	CCollider* m_pColliderCom = { nullptr };
	CTexture* m_pDissolveTextureCom = { nullptr };

private:
	_randNum m_RandomNumber;
	STATE m_eState{ State_End };
	STATE m_ePreState{ State_End };

	_uint m_iPassIndex{};
	_float m_fDissolveRatio{};
	_bool m_IsFall{};
	_float m_fFallTime{};
	_float m_fJumpForce{ 20.f };

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CLog* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END