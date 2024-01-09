#pragma once
#include "Base.h"
#include "GameInstance.h"

BEGIN(Engine)

class ENGINE_DLL CGameObject abstract : public CBase
{
protected:
	CGameObject(_dev pDevice, _context pContext);
	CGameObject(const CGameObject& rhs);
	virtual ~CGameObject() = default;

public:
	const _bool& isDead() const { return m_isDead; }
	const _int& Get_HP() const { return m_iHP; }

	void Kill() { m_isDead = true; }
	virtual void Set_Damage(_int iDamage, _uint iDamageType = 0) { m_iHP -= iDamage; }

public:
	virtual HRESULT Init_Prototype();
	virtual HRESULT Init(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual HRESULT Render_Shadow();

public:
	class CComponent* Find_Component(const wstring& strComTag);

protected:
	_dev m_pDevice{ nullptr };
	_context m_pContext{ nullptr };
	CGameInstance* m_pGameInstance{ nullptr };
	map<const wstring, class CComponent*> m_Components{};
	class CTransform* m_pTransformCom{ nullptr };
	_bool m_isDead{ false };
	_int m_iHP{};

protected:
	HRESULT Add_Component(_uint iLevelIndex, const wstring& strPrototypeTag, const wstring& strComTag, _Inout_ CComponent** ppComponent, void* pArg = nullptr);

public:
	virtual CGameObject* Clone(void*) = 0;
	virtual void Free() override;
};

END