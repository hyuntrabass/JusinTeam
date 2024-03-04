#pragma once

#include "Client_Define.h"
#include "Pet.h"

BEGIN(Client)

class CBlackCat final : public CGameObject
{
public:
	enum PET_CAT_ANIM
	{
		COLLECT,
		EMOTION,
		IDLE,
		RUN,
		TELEPORT_END,
		TELEPORT_START,
		ANIM_END
	};

	enum PET_CAT_STATE
	{
		STATE_IDLE,
		STATE_CHASE,
		STATE_HIT,
		STATE_EMOTION,
		STATE_DIE,
		STATE_CHANGE,
		STATE_END
	};

private:
	CBlackCat(_dev pDevice, _context pContext);
	CBlackCat(const CBlackCat& rhs);
	virtual ~CBlackCat() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg = nullptr) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CShader* m_pShaderCom = { nullptr };
	CRenderer* m_pRendererCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	CCollider* m_pColliderCom = { nullptr };

public:
	void Init_State(_float fTimeDelta);
	void Tick_State(_float fTimeDelta);

private:
	PET_CAT_STATE m_ePreState = STATE_END;
	PET_CAT_STATE m_eCurState = STATE_END;

private:
	_bool m_bChangePhase{};
	_bool m_bChangePass{};
	_bool m_bHit{};

	_uint m_iPassIndex{};
	_uint m_iHitCount{};

	_float m_fIdleTime = {};
	_float m_fHitTime = {};

	ANIM_DESC m_Animation{};
	_mat m_EffectMatrix{};

	vector<wstring> m_vecText;
	CGameObject* m_pDialog{ nullptr };


	class C3DUITex* m_pHpBG{ nullptr };
	C3DUITex* m_pHpBar{ nullptr };
	C3DUITex* m_pHpBorder{ nullptr };
	
	_float m_fBarFloating{ 0.f };
	_float m_fTargetHp{ 15.f };
	_vec2 m_Hp{ _vec2(15.f, 15.f)};

private:
	void Update_Collider();
	HRESULT Add_Collider();
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CBlackCat* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END