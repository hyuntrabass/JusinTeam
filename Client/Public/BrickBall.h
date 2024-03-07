#pragma once

#include "Client_Define.h"
#include "GameObject.h"
#include "Balloon.h"
#include "BrickItem.h"
BEGIN(Client)

class CBrickBall final : public CGameObject
{
public:
	typedef struct tagBallDesc
	{
		BrickColor eBrickColor{};
		_vec3 vPos{};
	}BALL_DESC;
	
private:
	CBrickBall(_dev pDevice, _context pContext);
	CBrickBall(const CBrickBall& rhs);
	virtual ~CBrickBall() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg = nullptr) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	HRESULT Add_Collider();
	void Update_Collider();

public:
	void RayCast();


private:
	CModel* m_pModelCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CRenderer* m_pRendererCom = { nullptr };
	CCollider* m_pColliderCom = { nullptr };
	CTexture* m_pMaskTextureCom = { nullptr };

private:
	CBrickItem::TYPE		m_eCurItem{ CBrickItem:: TYPE_END};
	BrickColor				m_eCurBrickColor{};
	_bool					m_isCombo{};
	_bool					m_isBarColl{};
	_bool					m_isColl{};
	_bool					m_isBalloonColl{};
	_uint					m_iCollNum{};
	_uint					m_iBallColor{};

	_float					m_fX{};
	_float					m_fSpeed{};
	_vec3					m_vDir{};
	_vec4					m_vColor{};

	_mat					m_EffectMatrix{};
	class CEffect_Dummy*	m_pEffect_Ball{ nullptr };
	CEffect_Dummy*			m_pEffect_Ball_Parti{ nullptr };
	CCollider*				m_pCurCollider{ nullptr };
	class CCommonTrail*		m_pTrail{ nullptr };
	//CCommonSurfaceTrail* m_pDistortionTrail { nullptr };

public:
	const _bool& Is_Combo() const { return m_isCombo; }
	const _bool& Is_Dead() const { return m_isDead; }
	CBrickItem::TYPE Get_CurItem();

private:
	HRESULT Init_Effect();
	void Check_Collision(_float fTimeDelta);
	void Check_ItemCollision(_float fTimeDelta);
	void Set_BallColor();

public:
	_bool Is_BarColl() { return m_isBarColl; }
	void Set_CurrentBallColor(BrickColor eColor);

public:
	CCollider* Get_BrickBallCollider() { return m_pColliderCom; }
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CBrickBall* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END