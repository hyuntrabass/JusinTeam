#pragma once

#include "Client_Define.h"
#include "GameObject.h"

BEGIN(Client)

class CMissile final : public CGameObject
{
public:
	enum MISSILE_TYPE
	{
		LEFT_THROW,
		RIGHT_THROW,
		SIX_MISSILE,
		TYPE_END
	};

private:
	CMissile(_dev pDevice, _context pContext);
	CMissile(const CMissile& rhs);
	virtual ~CMissile() = default;

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

	CModel* m_pModelCom = { nullptr }; // Sphere(Test)

private: // Groar_Boss Component
	CModel* m_pGroarModel = { nullptr };
	CTransform* m_pGroarTransform = { nullptr };

private:
	MISSILE_TYPE m_eType = TYPE_END;

private:
	_float m_fLifeTime = {};

private: // SIX_MISSILE
	static _uint m_iMissileID;
	_uint m_iMissileIndex = {};
	_float m_fDepartTime = {};

public:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CMissile* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END