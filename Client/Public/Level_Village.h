#pragma once
#include "Client_Define.h"
#include "Level.h"


BEGIN(Client)

class CLevel_Village final : public CLevel
{
private:
	CLevel_Village(_dev pDevice, _context pContext);
	virtual ~CLevel_Village() = default;

public:
	virtual HRESULT Init() override;
	virtual void Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_Camera();
	HRESULT Ready_Light();
	HRESULT Ready_Player();
	HRESULT Ready_Map();
	HRESULT Ready_Dungeon();

	HRESULT Ready_Object();
	HRESULT Ready_Environment();

	HRESULT In_To_Dungeon();

	HRESULT Ready_NpcvsMon();

private:
	HRESULT Ready_Village_Monster();

private:
	HRESULT Ready_NPC();
	HRESULT Ready_NPC_Dummy();

	//UI
	HRESULT Ready_UI();

public:
	static CLevel_Village* Create(_dev pDevice, _context pContext);
	virtual void Free() override;
};

END