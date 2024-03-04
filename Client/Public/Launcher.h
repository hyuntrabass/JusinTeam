#pragma once

#include "Client_Define.h"
#include "GameObject.h"

BEGIN(Client)

class CLauncher final : public CGameObject
{
public:
	enum LAUNCHER_TYPE
	{
		TYPE_RANDOM_POS,
		TYPE_CANNON,
		TYPE_BLUEGEM,
		TYPE_BARRICADE,
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
	virtual void Set_Damage(_int iDamage, _uint iDamageType = 0) override;

private:
	CShader* m_pShaderCom = { nullptr };
	CRenderer* m_pRendererCom = { nullptr };
	CCollider* m_pColliderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	CModel* m_pDestroyModelCom = { nullptr };

	CTexture* m_pDissolveTextureCom = { nullptr };

private:
	class CEffect_Dummy* m_pFrameEffect = { nullptr };
	class CEffect_Dummy* m_pBaseEffect = { nullptr };

	class CEffect_Dummy* m_pLauncher = { nullptr };
	class CEffect_Dummy* m_pLauncherParticle = { nullptr };


private:
	LAUNCHER_TYPE m_eType = { TYPE_END };
	wstring m_strModelTag = {};

private:
	ANIM_DESC m_Animation = {};

private:
	_uint m_iPassIndex = {};

private:
	_float m_fTime = {};
	_float m_fProjectileCreateTime = {};

	_uint m_iProjectileCount = {};

private:
	_float m_fDissolveRatio = { 1.f };

private:
	_bool m_bCreateProjectile = { false };

private:
	_bool m_bDestroy = { false };

private:
	static _uint m_iLauncherID;
	static _uint m_iDestroyCount;

public:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CLauncher* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END