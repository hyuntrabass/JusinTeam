#pragma once

#include "Client_Define.h"
#include "GameObject.h"

BEGIN(Client)

class CSun final :
    public CGameObject
{
private:
	CSun(_dev pDevice, _context pContext);
	CSun(const CSun& rhs);
	virtual ~CSun() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Reflection(_float4 vClipPlane);

private:
	CRenderer* m_pRendererCom = nullptr;
	CShader* m_pShaderCom = nullptr;
	CTexture* m_pTexturesCom = nullptr;
	CVIBuffer_Rect* m_pVIBufferCom = nullptr;

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();


public:
	static CSun* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END