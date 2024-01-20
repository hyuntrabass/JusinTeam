#pragma once
#include "Client_Define.h"
#include "Level.h"

BEGIN(Client)

class CLevel_GamePlay final : public CLevel
{
private:
	CLevel_GamePlay(_dev pDevice, _context pContext);
	virtual ~CLevel_GamePlay() = default;

public:
	virtual HRESULT Init() override;
	virtual void Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_Camera();
	HRESULT Ready_Light();
	HRESULT Ready_Player();
	HRESULT Ready_Map();

	HRESULT Ready_Object();

	// Monster
	HRESULT Ready_ModelTest();

	// Monster_Tutorial
	HRESULT Ready_NPCvsMon();
	HRESULT Ready_Void01();
	HRESULT Ready_Void05();

	// Monster_GamePlay
	HRESULT Ready_Rabbit();
	HRESULT Ready_Goat();
	HRESULT Ready_Nastron03();
	HRESULT Ready_Thief04();
	HRESULT Ready_TrilobiteA();

	// NPC
	HRESULT Ready_Cat();
	HRESULT Ready_Dog();
	HRESULT Ready_NPC_Dummy();

	// Boss
	HRESULT Ready_Groar_Boss();

	HRESULT Ready_Monster();


	//UI
	HRESULT Ready_UI();

public:
	static CLevel_GamePlay* Create(_dev pDevice, _context pContext);
	virtual void Free() override;
};

END