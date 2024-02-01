#pragma once
#include "Client_Define.h"
#include "GameObject.h"

BEGIN(Client)

class CWorldMap final : public CGameObject
{
private:
	CWorldMap(_dev pDevice, _context pContext);
	CWorldMap(const CWorldMap& rhs);
	virtual ~CWorldMap() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;



private:
	CRenderer* m_pRendererCom{ nullptr };
	CShader* m_pShaderCom{ nullptr };
	CModel* m_pModelCom[3]{ nullptr };
	CTexture* m_pMaskTexture{ nullptr };
private:

	_bool m_isSelected{};
	_uint m_iShaderPass{};
	_uint m_iOutLineShaderPass{};
	_vec2 m_fWater_Nomal{};
	_vec2 m_fGround_Mask{};
	_vec4 m_vMapPos{};
private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();


public:
	static CWorldMap* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END