#pragma once

#include "Client_Define.h"
#include "GameObject.h"

BEGIN(Client)

typedef struct MonsterInfo
{
	wstring strMonsterPrototype{};
	_mat	MonsterWorldMat{};
}Info;

class CMonster abstract : public CGameObject
{
protected:
	CMonster(_dev pDevice, _context pContext);
	CMonster(const CMonster& rhs);
	virtual ~CMonster() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg = nullptr) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual _vec4 Compute_PlayerPos();
	virtual _float Compute_PlayerDistance();

protected:
	MonsterInfo m_pInfo{};

protected:
	CShader* m_pShaderCom = { nullptr };
	CRenderer* m_pRendererCom = { nullptr };
	CModel* m_pModelCom = { nullptr };

	CCollider* m_pBodyColliderCom = { nullptr };
	CCollider* m_pAttackColliderCom = { nullptr };

	CTexture* m_pDissolveTextureCom = { nullptr };

protected:
	wstring m_strModelTag = {};
	ANIM_DESC m_Animation{};

protected:
	_uint m_iPassIndex = { AnimPass_Default };

protected:
	_float m_fBackPower = { 0.5f };

	_float m_fDissolveTime = {};
	_float m_fDissolveRatio = {};

protected:
	_uint m_iSmallDamage = { 50 };
	_uint m_iDefaultDamage1 = { 100 };
	_uint m_iDefaultDamage2 = { 200 };
	_uint m_iDefaultDamage3 = { 300 };


private: // Collier 사이즈 조절용
	_float m_fSizeX = {}, m_fSizeY = {}, m_fSizeZ = {};
	_float m_fScale = { 0.2f };

protected:
	virtual HRESULT Add_Collider() = 0;
	virtual void Update_Collider() = 0;

	void Update_MonsterCollider();

	// Collier 사이즈 조절용
	void Change_Extents(_vec3 vStartSize);

public:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END