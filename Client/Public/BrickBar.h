#pragma once

#include "Client_Define.h"
#include "GameObject.h"
#include "Balloon.h"

enum BAR_DIR{ BAR_LEFT, BAR_RIGHT, BAR_STOP, BAR_END };
BEGIN(Client)

class CBrickBar final : public CGameObject
{
private:
	CBrickBar(_dev pDevice, _context pContext);
	CBrickBar(const CBrickBar& rhs);
	virtual ~CBrickBar() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg = nullptr) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	HRESULT Add_Collider();
	void Update_Collider();

private:
	CShader* m_pShaderCom = { nullptr };
	CRenderer* m_pRendererCom = { nullptr };
	CCollider* m_pColliderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	CTexture* m_pMaskTextureCom = { nullptr };

private:	
	BrickColor				m_eCurBrickColor{};
	BAR_DIR					m_eCurDir{};
	_bool					m_isColl{};
	_bool					m_isChanged{};
	_bool					m_isSpeedUp{};

	_uint					m_iCollNum{};
	_uint					m_iBallColor{};

	_float					m_fTime{};
	_float					m_fSpeed{};
	_vec3					m_vDir{};

	_vec4					m_vColor{};
	_mat					m_EffectMatrix{};
	class CEffect_Dummy*	m_pEffect_Ball{ nullptr };
	class CBrickCat*		m_pCat{ nullptr };

private:
	void Set_BarColor();

public:
	BrickColor Get_CurrentColor() { return m_eCurBrickColor; }
	void Set_SpeedUp() { m_fSpeed = 20; m_isSpeedUp = true; }
	void Set_SpeedDefault() { m_fSpeed = 10; m_isSpeedUp = false; }

public:
	CTransform* Get_Transform() { return m_pTransformCom; }
	CCollider* Get_BrickBarCollider() { return m_pColliderCom; }
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CBrickBar* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END