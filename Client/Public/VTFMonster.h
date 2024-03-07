#pragma once

#include "Client_Define.h"
#include "GameObject.h"

BEGIN(Client)

class CVTFMonster abstract : public CGameObject
{
public:
	typedef struct tagVTFMonsterDesc
	{
		wstring strModelTag;
		_vec3 vPosition{};
		CTransform* pPlayerTransform;

	}VTFMONSTER_DESC;

protected:
	CVTFMonster(_dev pDevice, _context pContext);
	CVTFMonster(const CVTFMonster& rhs);
	virtual ~CVTFMonster() = default;

public:
	virtual void Set_Damage(_int iDamage, _uint AttackType = 0) override;
	virtual void Play_Sound(_bool IsPlaySound);

	const wstring& Get_ModelTag() const {
		return m_strModelTag;
	}

	_bool Get_IsPlaySound() {
		return m_IsPlaySound;
	}

	_int Get_SoundChannel() {
		return m_iSoundChannel;
	}

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg = nullptr) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Instance() override;

protected:
	CShader* m_pShaderCom = { nullptr };
	CVTFModel* m_pModelCom = { nullptr };
	CRenderer* m_pRendererCom = { nullptr };
	CCollider* m_pBodyColliderCom = { nullptr };
	CTexture* m_pDissolveTextureCom = { nullptr };

	CTransform* m_pPlayerTransform = { nullptr };

protected:
	ANIM_DESC m_Animation{};
	_randNum m_RandomNumber;
	_bool m_IsPlaySound{};
	wstring m_strSoundTag{};
	_int m_iSoundChannel = -1;
	_float m_fSoundStartPosRatio = 0.f;;
	_float m_fSoundVolume = 0.5f;

protected:
	wstring m_strModelTag{};
	_bool m_HasHitted{};
	_float m_fHitTime{};
	_bool m_HasAttacked{};
	_float m_fAttackDelay{};
	_float m_fDissolveRatio{};

protected:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END