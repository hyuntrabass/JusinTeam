#pragma once

#include "Base.h"
#include "GameInstance.h"
#include "AnimTool_Define.h"

BEGIN(Engine)
class CGameInstance;
END

BEGIN(AnimTool)

class CImgui_Manager final : public CBase
{
public:
    //ImGuizmo
    enum MANIPULATETYPE { TYPE_STATE, TYPE_RESET, MANIPULATE_TYPE_END };

private:
    CImgui_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual ~CImgui_Manager() = default;

public:
    HRESULT Initialize_Prototype(const GRAPHIC_DESC& GraphicDesc);
    void Tick(_float fTimeDelta);
    void Late_Tick(_float fTimeDelta);
    HRESULT Render();

public:
    HRESULT ImGuiMenu();
    HRESULT ImGuizmoMenu();

private:
    HRESULT SaveFile(const string& strModelName);
    HRESULT LoadFile(const string& strModelName);

private:
    ID3D11Device* m_pDevice = { nullptr };
    ID3D11DeviceContext* m_pContext = { nullptr };
    CGameInstance* m_pGameInstance = { nullptr };
    HWND m_hWnd;
    _uint m_iWinSizeX = { 0 };
    _uint m_iWinSizeY = { 0 };

private:
    class CPlayer* m_pPlayer = { nullptr }; 
    ANIM_DESC m_AnimDesc{};
    POINT m_ptMouse = {};
    vector<const _char*> m_AnimationNames;

    //ImGuizmo
    ImGuizmo::OPERATION m_eStateType = { ImGuizmo::OPERATION::TRANSLATE };
    _int m_iManipulateType = 0;
    MANIPULATETYPE m_eManipulateType = { TYPE_STATE };
    _int m_iPickedObjectID = { -1 };
    _vec3 m_vPreScale = {};
    _vec4 m_vPreRight = {};
    _vec4 m_vPreUp = {};
    _vec4 m_vPreLook = {};
    _vec4 m_vPrePosition = {};
    _vec4 m_vCurrentScale = {};

public:
    static CImgui_Manager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const GRAPHIC_DESC& GraphicDesc);
    virtual void Free() override;
};

END