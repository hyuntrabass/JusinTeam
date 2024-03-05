#pragma once

#include "Client_Define.h"
#include "GameObject.h"

BEGIN(Client)

class CSurvivalEffect final : public CGameObject
{
private:
	CSurvivalEffect(_dev pDevice, _context pContext);
	CSurvivalEffect(const CSurvivalEffect& rhs);
	virtual ~CSurvivalEffect() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	//CShader* m_pShaderCom = { nullptr };
	//CRenderer* m_pRendererCom = { nullptr };

private:
	class CEffect_Dummy* m_pEffect[2] = {};

	_mat m_UpdateMatrix = {};

//public:
//	HRESULT Add_Components();
//	HRESULT Bind_ShaderResources();

public:
	static CSurvivalEffect* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};

END