#pragma once

#include "Client_Define.h"
#include "NPC.h"

BEGIN(Client)

class CRoskva final : public CNPC
{
public:
	enum ROSKVA_ANIM
	{
		COAST02_SC02_NPC_ROSKVA,
		IDLE01,
		LOADINGSCENE_POSE_ROSKVA,
		TALK01_OLD,
		TALK02,
		TALK03,
		WALK,
		ANIM_END
	};

private:
	CRoskva(_dev pDevice, _context pContext);
	CRoskva(const CRoskva& rhs);
	virtual ~CRoskva() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg = nullptr) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	_bool m_bTalking = { false };

public:
	static CRoskva* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END