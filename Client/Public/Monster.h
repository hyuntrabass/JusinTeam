#pragma once

#include "Client_Define.h"
#include "GameObject.h"

BEGIN(Client)

class CMonster abstract : public CGameObject
{
protected:
	CMonster(_dev pDevice, _context pContext);
	CMonster(const CMonster& rhs);
	virtual ~CMonster() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg = nullptr) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

protected:
	CShader* m_pShaderCom = { nullptr };
	CRenderer* m_pRendererCom = { nullptr };
	CModel* m_pModelCom = { nullptr };

protected:
	wstring m_strModelTag = {};

protected:
	_uint m_iPassIndex = {};

public:
	HRESULT Add_Component();
	HRESULT Bind_ShaderResources();

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END