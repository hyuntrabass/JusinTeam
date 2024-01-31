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

	HRESULT Ready_Object();

	HRESULT Ready_Environment();

	// Monster
	HRESULT Ready_Void05();
	HRESULT Ready_Rabbit();
	HRESULT Ready_Goat();
	HRESULT Ready_Nastron03();
	HRESULT Ready_NPCvsMon();
	HRESULT Ready_Thief04();
	HRESULT Ready_TrilobiteA();

	// NPC
	HRESULT Ready_Cat();
	HRESULT Ready_Dog();
	HRESULT Ready_NPC_Dummy();

	// Boss
	HRESULT Ready_Groar_Boss();

	HRESULT Ready_NpcvsMon();


	//UI
	HRESULT Ready_UI();

public:
	static CLevel_Village* Create(_dev pDevice, _context pContext);
	virtual void Free() override;
};

END