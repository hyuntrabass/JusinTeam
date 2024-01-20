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
    enum TYPE { TYPE_MONSTER, TYPE_PLAYER, TYPE_END };

public:
    //ImGuizmo
    enum MANIPULATETYPE { TYPE_STATE, TYPE_RESET, MANIPULATE_TYPE_END };
    enum SELECT { SELECT_PLAYER, SELECT_EFFECT, SELECT_END };

private:
    CImgui_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual ~CImgui_Manager() = default;

public:
    void CreateModel() {
        m_IsCreateModel = true;
    }

public:
    HRESULT Initialize_Prototype(const GRAPHIC_DESC& GraphicDesc);
    void Tick(_float fTimeDelta);
    void Late_Tick(_float fTimeDelta);
    HRESULT Render();

public:
    HRESULT ImGuiMenu();
    HRESULT ImGuizmoMenu();

private:
    HRESULT SaveFile();
    HRESULT LoadFile();

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
    vector<const _char*> m_AnimationNames;
    vector<const _char*> m_BoneNames;
    _uint m_iCurrentModelIndex = { 0 };
    _int m_iCurrentBone{};

    vector<const _char*> m_EffectDescNames;
    _int m_iSelectEffectFile{};
    _int m_iCurrentEffect{};

    _bool m_IsCreateModel = { false };
    POINT m_ptMouse = {};
    _int m_ePreType = { TYPE_MONSTER };
    _int m_eType = { TYPE_MONSTER };
    _float m_fTimeDelta{};

    vector<string> m_FBXDataName;
    const _char** m_szFBXDataName;
    vector<string> m_EffectNames;
    const _char** m_szEffectNames;

    vector<const _char*> m_TriggerTimes;

    //ImGuizmo
    SELECT m_eSelect = { SELECT_PLAYER };
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