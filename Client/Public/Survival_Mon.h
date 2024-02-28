#pragma once

#include "Client_Define.h"
#include "GameObject.h"

BEGIN(Client)

class CSurvival_Mon final : public CGameObject
{
public:
	enum SURVIVAL_MON_TYPE
	{
		TYPE_IMP,
		TYPE_END
	};

public:
	enum IMP_ANIM
	{
		ATTACK01, //
		ATTACK01_H,
		ATTACK02, //
		ATTACK02_H,
		ATTACK03,
		ATTACK03_H,
		ATTACK04, //
		ATTACK05,
		DIE,
		HIT_ADD,
		HIT_L,
		HIT_R,
		IDLE,
		KNOCKDOWN,
		ROAR,
		RUN,
		STUN,
		WALK,
		ANIM_END
	};


private:
	CSurvival_Mon(_dev pDevice, _context pContext);
	CSurvival_Mon(const CSurvival_Mon& rhs);
	virtual ~CSurvival_Mon() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CShader* m_pShaderCom = { nullptr };
	CRenderer* m_pRendererCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	//CCollider* m_pColliderCom = { nullptr };

	CTexture* m_pDissolveTextureCom = { nullptr };

private:
	SURVIVAL_MON_TYPE m_eType = { TYPE_END };
	wstring m_strModelTag = {};

private:
	ANIM_DESC m_Animation = {};

private:
	_uint m_iPassIndex = {};

private:
	_float m_fDissolveRatio = { 1.f };

private:
	_bool m_bSpawned = { false };
	_bool m_bExplode = { false };

public:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CSurvival_Mon* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END