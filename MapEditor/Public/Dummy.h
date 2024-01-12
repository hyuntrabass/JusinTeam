#pragma once
#include "MapEditor_Define.h"
#include "GameObject.h"
#include "ImGui_Manager.h"

BEGIN(MapEditor)

//class CDummy final : public CBlendObject
class CDummy final : public CGameObject
{
private:
	CDummy(_dev pDevice, _context pContext);
	CDummy(const CDummy& rhs);
	virtual ~CDummy() = default;

public:
	void Select(const _bool& isSelected);
	void Modify(_fvector vPos, _fvector vLook);
	
	void Get_State(_float4& vPos, _float4& vLook);

	void Set_Create() { m_isCreate = true; }
	void Set_Dead() { m_isDead = true; }
	_bool Get_Create() { return m_isCreate; }
	
	_int Get_ID() const {return m_iID;}

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CRenderer* m_pRendererCom{ nullptr };
	CShader* m_pShaderCom{ nullptr };
	CModel* m_pModelCom{ nullptr };
	//CImGui_Manager* m_pImGui_Manager{ nullptr };

private:

	DummyInfo m_Info{};
	_bool m_isSelected{};
	_uint m_iShaderPass{};
	_uint m_iOutLineShaderPass{};
	_bool m_isAnim{};
	ANIM_DESC m_Animation{};

	_int m_iID = 0;

	_bool m_isCreate{false};
private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CDummy* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END