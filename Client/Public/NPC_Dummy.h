#pragma once

#include "Client_Define.h"
#include "NPC.h"

BEGIN(Client)

// ITEM MERCHANT ANIM
// IDLE, TALK
// 
// SKILL MERCHANT ANIM
// IDLE, TALK, TALK02, WALK
//
// GROAR ANIM
// DIE, IDLE, TALK

enum NPC_TYPE
{
	ITEM_MERCHANT,
	SKILL_MERCHANT,
	GROAR, // NPC였다가 보스
	TYPE_END
};

class CNPC_Dummy final : public CNPC
{
public:
	enum NPC_STATE
	{
		STATE_IDLE,
		STATE_TALK,
		STATE_END
	};

private:
	CNPC_Dummy(_dev pDevice, _context pContext);
	CNPC_Dummy(const CNPC_Dummy& rhs);
	virtual ~CNPC_Dummy() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg = nullptr) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	NPC_STATE m_ePreState = STATE_END;
	NPC_STATE m_eCurState = STATE_END;

private:
	NPC_TYPE m_eNPCType = TYPE_END;

public:
	static CNPC_Dummy* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END