#pragma once
#include "Client_Define.h"
#include "PartObject.h"


BEGIN(Client)

class CWeapon final : public CPartObject
{
private:
	CWeapon(_dev pDevice, _context pContext);
	CWeapon(const CWeapon& rhs);
	virtual ~CWeapon() = default;

public:
	HRESULT Init_Prototype() override;
	HRESULT Init(void* pArg) override;
	void Tick(_float fTimeDelta) override;
	void Late_Tick(_float fTimeDelta) override;
	HRESULT Render() override;
	virtual HRESULT Render_Shadow() override;

public:
	_bool IsAnimationFinished(_uint iAnimIndex);
	_uint Get_CurrentAnimationIndex();
	_float Get_CurrentAnimPos();
	const _mat* Get_BoneMatrix(const _char* pBoneName);
	void Reset_Model();
	void Set_ModelIndex(WEAPON_INDEX ModelIndex) { m_iSelectedModelIndex = ModelIndex;}

	void Set_Hide(_bool Hide) { m_bHide = Hide; }
private:
	CRenderer* m_pRendererCom{ nullptr };
	CShader* m_pShaderCom{ nullptr };
	vector<CModel*> m_Models{ nullptr };

private:
	WEAPON_TYPE m_eType{};
	_uint m_iNumVariations{};
	_uint m_iSelectedModelIndex{};
	ANIM_DESC* m_Animation{};
	_bool m_bHide{};
private:

	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CWeapon* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END