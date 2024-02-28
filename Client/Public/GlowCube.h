#pragma once

#include "Client_Define.h"
#include "GameObject.h"

BEGIN(Client)

class CGlowCube final : public CGameObject
{
public:
	typedef struct tagGlowCubeDesc
	{
		_vec4 vColor{};
		_vec4 vPos{};
		CTransform* pParentTransform{};
	}GLOWCUBE_DESC;
private:
	CGlowCube(_dev pDevice, _context pContext);
	CGlowCube(const CGlowCube& rhs);
	virtual ~CGlowCube() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg = nullptr) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CShader* m_pShaderCom = { nullptr };
	CRenderer* m_pRendererCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	CTexture* m_pMaskTextureCom = { nullptr };

private:
	_float					m_fX{};
	_vec3					m_vPos{};
	_vec4					m_vColor{};
	CTransform*				m_pParentTransform{ nullptr };

public:
	void Set_Color(_vec4 vColor) { m_vColor = vColor; }
	void Set_Position(_vec3 vPos) { m_vPos = vPos; }

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CGlowCube* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END