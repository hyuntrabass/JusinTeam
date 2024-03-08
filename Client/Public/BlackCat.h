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
		STATE_START,
		STATE_IDLE,
		STATE_ANGRY,
		STATE_CHASE,
		STATE_HIT,
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
	CTexture* m_pDissolveTextureCom = { nullptr };
	CCollider* m_pColliderCom = { nullptr };
	CCollider* m_pWideColliderCom = { nullptr };

public:
	void Init_State(_float fTimeDelta);
	void Tick_State(_float fTimeDelta);
	HRESULT Add_Parts();

private:
	PET_CAT_STATE m_ePreState = STATE_END;
	PET_CAT_STATE m_eCurState = STATE_END;

private:
	_bool m_bGameStart{};
	_bool m_bGameOver{};
	_bool m_bChangePhase{};
	_bool m_bPhaseStart{};;
	_bool m_bChangePass{};
	_bool m_bCreateBlock{};
	_bool m_bHit{};
	_bool m_isDeadMotion{};

	_uint m_iPassIndex{};
	_uint m_iHitCount{};

	_float m_fCreateBlockTime = {};
	_float m_fIdleTime = {};
	_float m_fHitTime = {};
	_float m_fDissolveRatio = {};
	_float m_fDeadTime = {};

	ANIM_DESC m_Animation{};
	_mat m_EffectMatrix{};
	_mat m_EffectMatrixLight{};

	list<wstring>	m_DialogList;
	vector<wstring> m_vecText;

	CGameObject* m_pDialog{ nullptr };


	class C3DUITex* m_pHpBG{ nullptr };
	C3DUITex* m_pHpBar{ nullptr };
	C3DUITex* m_pHpBorder{ nullptr };
	
	_float m_fBarFloating{ 0.f };
	_float m_fTargetHp{ 15.f };
	_vec2 m_Hp{ _vec2(15.f, 15.f)};

	class CTextButton* m_pLine{ nullptr };
	class CDialogText* m_pDialogText{ nullptr };
	class CTextButtonColor* m_pBackGround{ nullptr };

public:
	const _bool& Is_PhaseChange() const { return m_bChangePhase; }
	const _bool& Is_GameStart() const { return m_bGameStart; }
	const _bool& Is_GameOver() const { return m_bGameOver; }
	_bool Create_Bricks();
	CCollider* Get_Collider() { return m_pColliderCom; }

private:
	void Set_Text();

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