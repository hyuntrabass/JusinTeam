#include "Collision_Manager.h"
#include "GameObject.h"
#include "Collider.h"

HRESULT CCollision_Manager::Register_CollisionObject(CGameObject* pObject, class CCollider* pHitCollider, _bool IsPlayer, class CCollider* AttRangeCollider, class CCollider* ParryingColliderr)
{
	if (not pObject or not pHitCollider)
	{
		return E_FAIL;
	}

	if (IsPlayer)
	{
		if (m_pPlayer)
		{
			MSG_BOX("Player Already Exist! : Collision_Manager");
			return E_FAIL;
		}
		m_pPlayer = pObject;
		m_pPlayerHitCollider = pHitCollider;
		m_pPlayerAttRangeCollider = AttRangeCollider;
		m_pPlayerParryingCollider = ParryingColliderr;
		Safe_AddRef(m_pPlayer);
		Safe_AddRef(m_pPlayerHitCollider);
		Safe_AddRef(m_pPlayerAttRangeCollider);
		Safe_AddRef(m_pPlayerParryingCollider);
	}
	else
	{
		auto iter = m_Monsters.find(pObject);
		if (iter != m_Monsters.end())
		{
			MSG_BOX("Monster Already Exist! : Collision_Manager");
			return E_FAIL;
		}

		m_Monsters.emplace(pObject, pHitCollider);
		Safe_AddRef(pObject);
		Safe_AddRef(pHitCollider);
	}

	return S_OK;
}

void CCollision_Manager::Delete_CollisionObject(CGameObject* pObject, _bool IsPlayer)
{
	if (IsPlayer)
	{
		Safe_Release(m_pPlayer);
		Safe_Release(m_pPlayerHitCollider);

		m_pPlayer = nullptr;
		m_pPlayerHitCollider = nullptr;
	}
	else
	{
		auto iter = m_Monsters.find(pObject);
		if (iter != m_Monsters.end())
		{
			Safe_Release(const_cast<CGameObject*>(iter->first));
			Safe_Release(iter->second);
			m_Monsters.erase(iter);
		}
	}
}

void CCollision_Manager::Attack_Monster(CCollider* pCollider, _uint iDamage, _uint iDamageType)
{
	for (auto& Monster : m_Monsters)
	{
		if (Monster.second->Intersect(pCollider))
		{
			Monster.first->Set_Damage(iDamage, iDamageType);
		}
	}
}

_bool CCollision_Manager::CheckCollision_Monster(CCollider* pCollider)
{
	for (auto& Monster : m_Monsters)
	{
		if (Monster.second->Intersect(pCollider))
		{
			return true;
		}
	}
	
	return false;
}

_bool CCollision_Manager::CheckCollision_Parrying(CCollider* pCollider)
{
	if (pCollider->Intersect(m_pPlayerParryingCollider))
	{
		m_pPlayer->Set_Damage(0, 4);
		return (_bool)m_pPlayer->Get_HP();
	}

	return false;
}

_bool CCollision_Manager::Attack_Player(CCollider* pCollider, _uint iDamage, _uint iDamageType)
{
	if (not pCollider or pCollider->Intersect(m_pPlayerHitCollider))
	{
		m_pPlayer->Set_Damage(iDamage, iDamageType);
		return true;
	}

	return false;
}

_bool CCollision_Manager::CheckCollision_Player(CCollider* pCollider)
{
	if (pCollider->Intersect(m_pPlayerHitCollider))
	{
		return true;
	}

	return false;
}

class CCollider* CCollision_Manager::Get_Nearest_MonsterCollider()
{

	CCollider* nearestMonsterCollider = nullptr;
	_float minDistance = 300.f;

	for (auto& Monster : m_Monsters)
	{
		if (Monster.second->Intersect(m_pPlayerAttRangeCollider))
		{
			_vec4 monsterPos = _vec4(Monster.second->Get_ColliderPos(),1.f);
			_float distance = _vec4::Distance(_vec4(m_pPlayerHitCollider->Get_ColliderPos(), 1.f), monsterPos);
			if (distance < minDistance)
			{
				minDistance = distance;
				nearestMonsterCollider = Monster.second;
			}
		}
	}
	return nearestMonsterCollider;
}

CCollision_Manager* CCollision_Manager::Create()
{
	CCollision_Manager* pInstance = new CCollision_Manager();

	return pInstance;
}

void CCollision_Manager::Free()
{
	Safe_Release(m_pPlayer);
	Safe_Release(m_pPlayerHitCollider);
	Safe_Release(m_pPlayerParryingCollider);
	Safe_Release(m_pPlayerAttRangeCollider);

	for (auto& Monster : m_Monsters)
	{
		Safe_Release(Monster.second);
		Safe_Release(const_cast<CGameObject*>(Monster.first));
	}
	m_Monsters.clear();
}
