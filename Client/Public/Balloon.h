#pragma once

#include "Client_Define.h"
#include "GameObject.h"

BEGIN(Client)

enum BrickColor { RED, BLUE, COLOR_END };
class CBalloon final : public CGameObject
{
public:
	typedef struct tagBalloonDesc
	{
		_bool isEmpty{};
		_vec3 vPosition{};
		_vec4 vColor;
	}BALLOON_DESC;

	enum BALLOON_STATE
	{
		STATE_IDLE,
		STATE_HIT,
		STATE_ATTACK,
		STATE_DIE,
		STATE_END
	};

private:
	CBalloon(_dev pDevice, _context pContext);
	CBalloon(const CBalloon& rhs);
	virtual ~CBalloon() = default;

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
	CCollider* m_pBodyColliderCom = { nullptr };
	CTexture* m_pDissolveTextureCom = { nullptr };	
	CTexture* m_pMaskTextureCom = { nullptr };

private:
	BrickColor		m_eCurColor{};

public:
	void Init_State(_float fTimeDelta);
	void Tick_State(_float fTimeDelta);

private:
	BALLOON_STATE m_ePreState = STATE_IDLE;
	BALLOON_STATE m_eCurState = STATE_IDLE;

private:
	_bool m_isReadyToFall{ };
	_bool m_isFall{ true };
	_bool m_isEmpty{};
	_bool m_isColl{};
	_bool m_bParticle{};
	_bool m_bDamaged = { false };
	_bool m_bStart = { false };

	_uint m_iCount{ 1 };

	_float m_fDelayTime = {};
	_float m_fIdleTime = {};
	_float m_fDeadTime = {};
	_float m_bHit = {};
	_float m_fX = {};

	_vec4 m_vColor{};

public:
	const _bool& Is_Coll() const { return m_isColl; }

private:
	void Create_Item();
	void Set_Color();
	void Set_RandomColor();

private:
	HRESULT Add_Collider();
	void Update_BodyCollider();
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CBalloon* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END