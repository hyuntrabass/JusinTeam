#pragma once
#include "Client_Define.h"
#include "GameObject.h"

BEGIN(Client)


enum WEAPON_TYPE
{
	WP_BOW,
	WP_SWORD,
	WP_END
};


struct BODYPART_DESC
{
	PART_TYPE eType{};
	_uint iNumVariations{};


	ANIM_DESC* Animation{};
	CTransform* pParentTransform{ nullptr };
};

class CPlayer final : public CGameObject
{
private:
	CPlayer(_dev pDevice, _context pContext);
	CPlayer(const CPlayer& rhs);
	virtual ~CPlayer() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	HRESULT Add_Parts();

public:
	void Change_Parts(PART_TYPE PartsType,_int ChangeIndex);
	void Reset_PartsAnim();
	void Set_Key(_float fTimeDelta);
	
public:
	void Attack();

private:
	vector<CGameObject*> m_vecParts{};
	CGameObject* m_pWeapon{};

private:
	ANIM_DESC m_Animation{};
	_float4 m_vPos{};
	_float m_fGravity{};
	WEAPON_TYPE m_Current_Weapon{WP_END};

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CPlayer* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END