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

	// Monster
	HRESULT Ready_Void05();
	HRESULT Ready_Rabbit();
	HRESULT Ready_Penguin();

public:
	static CLevel_GamePlay* Create(_dev pDevice, _context pContext);
	virtual void Free() override;
};

END