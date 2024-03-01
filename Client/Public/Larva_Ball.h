#pragma once

#include "GameObject.h"
#include "Client_Define.h"

BEGIN(Client)

class CLarva_Ball final : public CGameObject
{
public:
	typedef struct tagLarvaBallDesc
	{
		_vec3 vPosition;
		_vec4 vDirection;

	}LARVABALL_DESC;
private:
	CLarva_Ball(_dev pDevice, _context pContext);
	CLarva_Ball(const CLarva_Ball& rhs);
	virtual ~CLarva_Ball() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg = nullptr) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;

private:
	CCollider* m_pColliderCom = { nullptr };
#ifdef _DEBUG
	CRenderer* m_pRendererCom = { nullptr };
#endif // _DEBUG

private:
	_float m_fLifeTime{};
	_mat m_EffectMatrix{};

private:
	HRESULT Add_Components();

public:
	static CLarva_Ball* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END