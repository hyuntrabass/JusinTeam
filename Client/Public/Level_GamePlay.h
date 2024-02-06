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
	_float m_fWaveTimer{};
	_float m_fWaveGravity{};
	_mat m_WaveMatrix[4]{};
	_bool m_isWave{};

	_mat m_RainMatrix{};

private:
	HRESULT Ready_Camera();
	HRESULT Ready_Light();
	HRESULT Ready_Player();
	HRESULT Ready_Map();
	HRESULT Ready_Environment();
	HRESULT Ready_Object();
	HRESULT Ready_Tutorial_Monster(); // ���� �Ľ�

	//UI
	HRESULT Ready_UI();

	HRESULT Ready_TestTrigger();

private:
	// Monster_Test
	HRESULT Ready_ModelTest();
	HRESULT Ready_Monster_Test();

	// NPC_Test
	HRESULT Ready_NPC_Test();
	HRESULT Ready_NPC_Dummy_Test();

	// Boss_Test
	HRESULT Ready_Groar_Boss();

	// Pet_Test
	HRESULT Ready_Pet();


public:
	static CLevel_GamePlay* Create(_dev pDevice, _context pContext);
	virtual void Free() override;
};

END