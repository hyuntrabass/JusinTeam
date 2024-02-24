#pragma once

#include "Client_Define.h"
#include "GameObject.h"

BEGIN(Client)

class CLauncher final : public CGameObject
{
public:
	enum TYPE
	{
		TYPE_END
	};

private:
	CLauncher(_dev pDevice, _context pContext);
	CLauncher(const CLauncher& rhs);
	virtual ~CLauncher() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	static CLauncher* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END