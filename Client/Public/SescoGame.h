#pragma once

#include "Base.h"
#include "Client_Define.h"

BEGIN(Client)

class CSescoGame final : public CBase
{
private:
	CSescoGame(_dev pDevice, _context pContext);
	virtual ~CSescoGame() = default;

public:
	virtual HRESULT Init();
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);

private:
	_dev m_pDevice{};
	_context m_pContext{};

public:
	static CSescoGame* Create(_dev pDevice, _context pContext);
	virtual void Free() override;
};

END