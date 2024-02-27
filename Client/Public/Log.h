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

private:
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	CRenderer* m_pRendererCom = { nullptr };
	CCollider* m_pBodyColliderCom = { nullptr };
	CTexture* m_pDissolveTextureCom = { nullptr };

private:
	STATE m_eState{ State_End };
	STATE m_ePreState{ State_End };

private:
	void Init_State(_float fTimeDelta);
	void Tick_State(_float fTimeDelta);

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CLog* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END