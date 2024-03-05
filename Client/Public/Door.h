#pragma once

#include "Client_Define.h"
#include "GameObject.h"

BEGIN(Client)

struct DoorInfo {
    _uint iIndex{};
    _mat mMatrix{};
};

class CDoor final :
    public CGameObject
{
private:
    CDoor(_dev pDevice, _context pContext);
    CDoor(const CDoor& rhs);
    virtual ~CDoor() = default;

public:
    virtual HRESULT Init_Prototype() override;
    virtual HRESULT Init(void* pArg = nullptr) override;
    virtual void Tick(_float fTimeDelta) override;
    virtual void Late_Tick(_float fTimeDelta) override;
    virtual HRESULT Render() override;

private:
    CShader* m_pShaderCom = nullptr;
    CRenderer* m_pRendererCom = nullptr;
    CModel* m_pModelCom = nullptr;

private:
    DoorInfo m_Info{};
    ANIM_DESC m_Animation{};

public:
    HRESULT Add_Components();
    HRESULT Bind_ShaderResources();


public:
    static CDoor* Create(_dev pDevice, _context pContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;
};

END