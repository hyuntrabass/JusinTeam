#pragma once

#include "Client_Define.h"
#include "GameObject.h"

BEGIN(Client)

class CTestVTFModel final : public CGameObject
{
public:
	enum SELECTMODEL_ANIM { S_IDLE, S_MOTION, S_CANCEL, S_PICK_IDLE, S_END };

private:
	CTestVTFModel(_dev pDevice, _context pContext);
	CTestVTFModel(const CTestVTFModel& rhs);
	virtual ~CTestVTFModel() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg = nullptr) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

private:
	CShader* m_pShaderCom = { nullptr };
	CRenderer* m_pRendererCom = { nullptr };
	CVTFModel* m_pModelCom = { nullptr };

private:
	SELECTMODEL_ANIM m_eCurAnimState{ S_IDLE };
	wstring m_strModelTag = {};
	ANIM_DESC m_Animation{};

public:
	static CTestVTFModel* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END