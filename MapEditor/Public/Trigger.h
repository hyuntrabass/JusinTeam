#pragma once
#include "MapEditor_Define.h"
#include "GameObject.h"
#include "ImGui_Manager.h"

BEGIN(MapEditor)

class CTrigger final : public CGameObject
{
private:
	CTrigger(_dev pDevice, _context pContext);
	CTrigger(const CTrigger& rhs);
	virtual ~CTrigger() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	void Select(const _bool& isSelected);

	_uint Get_TriggerNum() { return m_iTriggerNumber; };

private:
	CRenderer* m_pRendererCom{ nullptr };
	CShader* m_pShaderCom{ nullptr };
	CModel* m_pModelCom{ nullptr };
	CCollider* m_pCollider{ nullptr };

private:
	_vec4 m_vPos{};
	_float m_iColliderSize{};
	_int 	m_iTriggerNumber{};
	_bool m_isSelected{};
	_int m_iID = 0;

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();


public:
	static CTrigger* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END