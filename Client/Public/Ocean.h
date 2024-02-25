#pragma once

#include "Client_Define.h"
#include "GameObject.h"

BEGIN(Client)

class COcean final :
    public CGameObject
{
private:
    COcean(_dev pDevice, _context pContext);
    COcean(const COcean& rhs);
    virtual ~COcean() = default;

public:
    static COcean* Create(_dev pDevice, _context pContext);
    // CGameObject을(를) 통해 상속됨
    CGameObject* Clone(void*) override;
};

END