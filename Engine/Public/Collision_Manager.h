#pragma once
#include "Base.h"

BEGIN(Engine)

class CCollision_Manager final : public CBase
{
private:
	CCollision_Manager() = default;
	virtual ~CCollision_Manager() = default;

public:
	HRESULT Register_CollisionObject(class CGameObject* pObject, class CCollider* pHitCollider, _bool IsPlayer = false , class CCollider* AttRangeCollider = nullptr, class CCollider* ParryingCollider = nullptr);
	HRESULT Register_CollisionCulling(CGameObject* pObject, CCollider* pCollider);
	void Delete_CollisionObject(class CGameObject* pObject, _bool IsPlayer);
	void Attack_Monster(class CCollider* pCollider, _uint iDamage, _uint iDamageType);
	_bool CheckCollision_Monster(class CCollider* pCollider);
	_bool CheckCollision_Parrying(class CCollider* pCollider);
	_bool Attack_Player(class CCollider* pCollider, _uint iDamage, _uint iDamageType);
	_bool CheckCollision_Player(class CCollider* pCollider); // �ʿ����
	_bool CheckCollision_Culling(class CCollider* pCollider);
	class CCollider* Get_Nearest_MonsterCollider();
private:
	map<class CGameObject*, class CCollider*> m_Monsters{};
	map<class CGameObject*, class CCollider*> m_Objects{};
	class CGameObject* m_pPlayer{ nullptr };
	class CCollider* m_pPlayerHitCollider{ nullptr };
	class CCollider* m_pPlayerParryingCollider{ nullptr };
	class CCollider* m_pPlayerAttRangeCollider{ nullptr };
	class CCollider* m_pObjectCollider{ nullptr };

public:
	static CCollision_Manager* Create();
	virtual void Free() override;
};

END