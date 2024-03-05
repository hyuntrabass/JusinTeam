#pragma once
#include "Client_Define.h"
#include "Trigger_Manager.h"
#include "GameObject.h"

BEGIN(Client)

class CMiniDungeon_Teleport final : public CGameObject
{

private:
	CMiniDungeon_Teleport(_dev pDevice, _context pContext);
	CMiniDungeon_Teleport(const CMiniDungeon_Teleport& rhs);
	virtual ~CMiniDungeon_Teleport() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;

	_uint Get_TriggerNum() { return m_iTriggerNumber; }
	_bool Get_Collision() { return m_isCollision; }
	_bool Get_Limited() { return m_isLimited; }
	_mat Get_Matrix() { return m_Matrix; }
private:
	HRESULT Add_Components();


private:
	CRenderer* m_pRendererCom{ nullptr };
	CCollider* m_pCollider{ nullptr };

private:
	_mat m_Matrix{};
	_float m_iColliderSize{};
	_int 	m_iTriggerNumber{};
	_bool	m_isCollision{ false };
	_bool	m_isLimited{ false };


public:
	static CMiniDungeon_Teleport* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END