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
    // CGameObject��(��) ���� ��ӵ�
    CGameObject* Clone(void*) override;
};

END