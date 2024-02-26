#pragma once

#include "Client_Define.h"
#include "GameObject.h"

BEGIN(Client)

class COcean final :
    public CGameObject
{
private:
    COcean(_dev pDevice, _context pContext);
    COcean(const COcean& rhs);
    virtual ~COcean() = default;

public:
    virtual HRESULT Init_Prototype() override;
    virtual HRESULT Init(void* pArg)override;
    virtual void Tick(_float fTimeDelta)override;
    virtual void Late_Tick(_float fTimeDelta)override;
    virtual HRESULT Render()override;

private:
    CRenderer* m_pRendererCom = nullptr;
    CTexture* m_pTextureCom = nullptr;
    CShader* m_pShaderCom = nullptr;
    CVIBuffer_Terrain* m_pVIBufferCom = nullptr;

private:
    HRESULT Add_Component();
    HRESULT Bind_ShaderResources();

public:
    static COcean* Create(_dev pDevice, _context pContext);
    // CGameObject을(를) 통해 상속됨
    virtual CGameObject* Clone(void*) override;
    virtual void Free() override;
};

END