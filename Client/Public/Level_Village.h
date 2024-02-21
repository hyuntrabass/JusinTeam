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
	//HRESULT Ready_Player();

	HRESULT Ready_Torch();
	HRESULT Ready_Field_Environment();
	HRESULT Ready_Environment();
	HRESULT Ready_Interaction();

	//HRESULT In_To_Dungeon();

	HRESULT Ready_NpcvsMon();

private: // Monster, Boss
	HRESULT Ready_Village_Monster();
	HRESULT Ready_Dungeon_Monster();

	HRESULT Ready_Groar_Boss();

private: // NPC
	HRESULT Ready_NPC();
	HRESULT Ready_NPC_Dummy();

private: //UI
	HRESULT Ready_UI();

private: // Test
	HRESULT Ready_Test(); 

private:
	HRESULT Ready_Trigger();

private:	// Minigame
	HRESULT Ready_Minigame();

public:
	static CLevel_Village* Create(_dev pDevice, _context pContext);
	virtual void Free() override;
};

END