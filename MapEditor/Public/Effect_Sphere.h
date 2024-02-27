#pragma once
#include "MapEditor_Define.h"
#include "GameObject.h"
#include "ImGui_Manager.h"

BEGIN(MapEditor)

class CEffect_Sphere final : public CGameObject
{
private:
	CEffect_Sphere(_dev pDevice, _context pContext);
	CEffect_Sphere(const CEffect_Sphere& rhs);
	virtual ~CEffect_Sphere() = default;

public:
	void Select(const _bool& isSelected);

	void Get_State(_float4& vPos, _float4& vLook);
	_bool Get_Selected() { return m_isSelected; }
	EffectDummyInfo Get_Info() { return m_Info; }

	void Set_Create() { m_isCreate = true; }
	_bool Get_Create() { return m_isCreate; }
	_int Get_ID() const { return m_iID; }

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	_int Get_ID() { return m_iID; }

private:
	CRenderer* m_pRendererCom{ nullptr };
	CShader* m_pShaderCom{ nullptr };
	CModel* m_pModelCom{ nullptr };

private:
	EffectDummyInfo m_Info{};
	_bool m_isSelected{};
	_uint m_iShaderPass{};
	_bool m_isCreate{ false };
	_uint m_iOutLineShaderPass{};
	_mat* mMatrix{ nullptr };
private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CEffect_Sphere* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END