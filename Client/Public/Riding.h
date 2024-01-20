#pragma once

#include "Client_Define.h"
#include "GameObject.h"

BEGIN(Client)

enum Riding_Type
{
	Riding_Bird,
	Riding_Tiger,
	Riding_Nihilir,
	Riding_End
};
struct Riding_Desc
{
	Riding_Type Type;
};
enum Bird_Anim
{
	Bird_1005_Fly,
	Bird_1005_Fly_Add_L,
	Bird_1005_Fly_Add_R,
	Bird_1005_Fly_L,
	Bird_1005_Fly_R,
	Bird_1005_Landing,
	Bird_1005_Start,
	Bird_1005_Takeoff,
	Bird_2005_Fly,
	Bird_2005_Fly_Add_L,
	Bird_2005_Fly_Add_R,
	Bird_2005_Fly_L,
	Bird_2005_Fly_R,
	Bird_2005_Landing,
	Bird_2005_Start,
	Bird_2005_Takeoff
};
enum Tiger_Anim
{
	Tiger_1003_Attack01,
	Tiger_1003_Idle,
	Tiger_1003_Jump_End,
	Tiger_1003_Jump_End_Run,
	Tiger_1003_Jump_loop,
	Tiger_1003_Jump_Start,
	Tiger_1003_Run,
	Tiger_1003_Run_Add_L,
	Tiger_1003_Run_Add_R,
	Tiger_1003_Run_L,
	Tiger_1003_Run_R,
	Tiger_1003_Start,
	Tiger_1003_Walk,
	Tiger_2003_Attack01,
	Tiger_2003_Idle,
	Tiger_2003_Jump_End,
	Tiger_2003_Jump_End_Run,
	Tiger_2003_Jump_loop,
	Tiger_2003_Jump_Start,
	Tiger_2003_Run,
	Tiger_2003_Run_Add_L,
	Tiger_2003_Run_Add_R,
	Tiger_2003_Run_L,
	Tiger_2003_Run_R
	Tiger_2003_Start,
	Tiger_2003_Walk
};
enum Nihilir
{
	Nihilir_VC_Nihilir_5002_Attack01,
	Nihilir_VC_Nihilir_5002_Attack01_End,
	Nihilir_VC_Nihilir_5002_Attack01_Loop,
	Nihilir_VC_Nihilir_5002_Attack01_Start,
	Nihilir_VC_Nihilir_5002_Attack02,
	Nihilir_VC_Nihilir_5002_Attack03,
	Nihilir_VC_Nihilir_5002_Idle,
	Nihilir_VC_Nihilir_5002_Jump_End,
	Nihilir_VC_Nihilir_5002_Jump_End_Run,
	Nihilir_VC_Nihilir_5002_Jump_Loop,
	Nihilir_VC_Nihilir_5002_Jump_Start,
	Nihilir_VC_Nihilir_5002_Roar,
	Nihilir_VC_Nihilir_5002_Run,
	Nihilir_VC_Nihilir_5002_Run_add_L,
	Nihilir_VC_Nihilir_5002_Run_add_R,
	Nihilir_VC_Nihilir_5002_Run_L,
	Nihilir_VC_Nihilir_5002_Run_R,
	Nihilir_VC_Nihilir_5002_Walk
};

class CRiding final : public CGameObject
{
public:
	
private:
	CRiding(_dev pDevice, _context pContext);
	CRiding(const CRiding& rhs);
	virtual ~CRiding() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg = nullptr) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Update_Collider();
	void Delete_Riding();
	_bool Get_Ready_Dead() { return m_bReady_Dead; }
private:
	CShader* m_pShaderCom = { nullptr };
	CRenderer* m_pRendererCom = { nullptr };
	vector<CModel*> m_vecModel{ nullptr };

	CCollider* m_pBodyColliderCom = { nullptr };
	CCollider* m_pAttackColliderCom = { nullptr };

private:
	ANIM_DESC m_Animation{};
	_uint m_iNumVariations{};
	Riding_Type m_CurrentIndex{ Riding_End };
	_bool m_bReady_Dead{};
public:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CRiding* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END