#pragma once

#include "Client_Define.h"
#include "OrthographicObject.h"

BEGIN(Client)

class CWarning_Mark :
    public COrthographicObject
{
private:
    CWarning_Mark(_dev pDevice, _context pContext);
    CWarning_Mark(const CWarning_Mark& rhs);
    virtual ~CWarning_Mark() = default;

public:
    virtual HRESULT Init_Prototype() override;
    virtual HRESULT Init(void* pArg) override;
    virtual void Tick(_float fTimeDelta) override;
    virtual void Late_Tick(_float fTimeDelta) override;
    virtual HRESULT Render() override;

    const void Set_WarningEnd() { m_isWarningEnd = true; }

private:
    CRenderer* m_pRendererCom{ nullptr };
    CShader* m_pShaderCom{ nullptr };
    CVIBuffer_Rect* m_pVIBufferCom{ nullptr };
    CTexture* m_pTextureCom{ nullptr };

private:
    _float m_fAlpha = 1.f;

    _bool m_isWarningEnd = false;

private:
    class CTextButtonColor* m_pAlphaWarning = nullptr;


private:
    HRESULT Add_Part();
    HRESULT Add_Components();
    HRESULT Bind_ShaderResources();

public:
    static CWarning_Mark* Create(_dev pDevice, _context pContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;
};

END