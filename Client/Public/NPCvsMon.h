#pragma once

#include "Client_Define.h"
#include "GameObject.h"

BEGIN(Client)

class CNPCvsMon final : public CGameObject
{
private:
	CNPCvsMon(_dev pDevice, _context pContext);
	CNPCvsMon(const CNPCvsMon& rhs);
	virtual ~CNPCvsMon() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;	
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CRenderer* m_pRendererCom{ nullptr };
	CShader* m_pShaderCom{ nullptr };
	CModel* m_pModelCom{ nullptr };

private:
	ANIM_DESC m_Animation{};

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CNPCvsMon* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};

END