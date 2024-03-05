#pragma once
#include "Client_Define.h"
#include "Level.h"


BEGIN(Client)

class CLevel_Tower final : public CLevel
{
private:
	CLevel_Tower(_dev pDevice, _context pContext);
	virtual ~CLevel_Tower() = default;

public:
	virtual HRESULT Init() override;
	virtual void Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_Light();

private:
	HRESULT Ready_DragonMap_Effect();

private:
	HRESULT Ready_Trigger();

public:
	static CLevel_Tower* Create(_dev pDevice, _context pContext);
	virtual void Free() override;
};

END