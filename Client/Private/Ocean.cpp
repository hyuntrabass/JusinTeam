#include "Ocean.h"

COcean::COcean(_dev pDevice, _context pContext)
	: CGameObject(pDevice, pContext)
{
}

COcean::COcean(const COcean& rhs)
	: CGameObject(rhs)
{
}

CGameObject* COcean::Clone(void*)
{
	return nullptr;
}
