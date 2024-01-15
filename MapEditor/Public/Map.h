#pragma once
#include "MapEditor_Define.h"
#include "BlendObject.h"
#include "ImGui_Manager.h"

BEGIN(MapEditor)

class CMap final : public CBlendObject
{
private:
	CMap(_dev pDevice, _context pContext);
	CMap(const CMap& rhs);
	virtual ~CMap() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	void Select(const _bool& isSelected);
	_bool Get_Selected() { return m_isSelected; }
	_int Get_ID() const { return m_iID; }
	void Set_Dead() { m_isDead = true; }

private:
	CRenderer* m_pRendererCom{ nullptr };
	CShader* m_pShaderCom{ nullptr };
	CModel* m_pModelCom{ nullptr };

private:

	MapInfo m_Info{};
	_bool m_isSelected{};
	_uint m_iShaderPass{};
	_uint m_iOutLineShaderPass{};

	_int m_iID = 0;

	_bool m_isCreate{ false };
private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();


public:
	static CMap* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END