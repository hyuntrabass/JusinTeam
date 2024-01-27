#pragma once

#include "Client_Define.h"
#include "GameObject.h"

BEGIN(Client)

class CArrow final : public CGameObject
{

private:
	CArrow(_dev pDevice, _context pContext);
	CArrow(const CArrow& rhs);
	virtual ~CArrow() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg = nullptr) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CCollider* m_pCollider{ nullptr };
	CShader* m_pShaderCom{ nullptr };
	CRenderer* m_pRendererCom{ nullptr };
	CModel* m_pModelCom{ nullptr };
private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CArrow* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END