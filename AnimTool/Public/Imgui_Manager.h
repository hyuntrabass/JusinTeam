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
    enum TRIGGER { TRIGGER_EFFECT, TRIGGER_SOUND, TRIGGER_END };

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
    vector<string> m_MonsterNames;
    const _char** m_szMonsterNames;
    class CPlayer* m_pPlayer = { nullptr };
    ANIM_DESC m_AnimDesc{};
    vector<const _char*> m_AnimationNames;
    vector<const _char*> m_BoneNames;
    _uint m_iCurrentModelIndex = { 0 };
    _int m_iCurrentBoneIndex{};
    _bool m_IsCreateModel = { false };
    POINT m_ptMouse = {};
    _int m_ePreModelType = { TYPE_MONSTER };
    _int m_eModelType = { TYPE_MONSTER };

    //Ʈ����
    _int m_ePreTriggerType = { TRIGGER_EFFECT };
    _int m_eTriggerType = { TRIGGER_EFFECT };
    _int m_iSelectFile{};
    _int m_iCurTriggerIndex{};
    //����Ʈ
    vector<string> m_EffectFiles;
    const _char** m_szEffectFiles;
    _mat m_OffsetMatrix{};
    //����
    vector<string> m_SoundFiles;
    const _char** m_szSoundFiles;

    _float m_fTimeDelta{};

    //ImGuizmo
    SELECT m_eSelect = { SELECT_EFFECT };
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