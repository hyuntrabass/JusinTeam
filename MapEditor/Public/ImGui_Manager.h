#pragma once

#include "Base.h"
#include "MapEditor_Define.h"

BEGIN(Engine)
class CGameInstance;
END

BEGIN(MapEditor)

enum class ItemType
{
	Map,
	Objects,
	Monster,
	NPC,
	Trigger,
	Camera,
	End
};

struct DummyInfo
{
	wstring Prototype{};
	enum class ItemType eType {};
	_uint iStageIndex{};
	_float4 vPos{};
	_float4 vLook{};
	_uint iTriggerNum{};
	class CDummy** ppDummy{ nullptr };
};


class CImGui_Manager final : public CBase
{
	DECLARE_SINGLETON(CImGui_Manager)

private:
	CImGui_Manager();
	virtual ~CImGui_Manager() = default;

public:
	HRESULT Initialize_Prototype(const GRAPHIC_DESC& GraphicDesc);
	void Tick(_float fTimeDelta);
	HRESULT Render();

	_bool ComputePickPos();
	_bool ComputeSelection();

	void SetPos(const _float4& vPos, class CDummy* pDummy);
	void Select(const _vec4& vPos, class CDummy* pDummy);

private:
	HRESULT ImGuiMenu();
	HRESULT ImGuizmoMenu();

	void Create_Dummy(const _int& iListIndex);
	void Select(const _float4& vPos, CDummy* pDummy);
	const char* Search_Files();

private:
	ID3D11Device* m_pDevice{ nullptr };
	ID3D11DeviceContext* m_pContext{ nullptr };
	CGameInstance* m_pGameInstance{ nullptr };
	HWND m_hWnd;
	_uint m_iWinSizeX{ 0 };
	_uint m_iWinSizeY{ 0 };

	POINT m_ptMouse = {};
	_float2 m_vMousePos{ 0.f,0.f };
	_vec4 m_pTerrainPos{ 0.f, 0.f, 0.f, 0.f };
	_float m_fCamDist{};

	_bool m_ComputePickPos{};
	_bool m_ComputeSelection{};

	_float4 m_vPos{ 0.f, 0.f, 0.f, 1.f };
	_float4 m_vLook{ 0.f, 0.f, 1.f, 0.f };

	ItemType m_eItemType{ ItemType::End };

private:
	// 파일의 이름 가져와서 저장
	vector<const char*> Maps;
	vector<const char*> Objects;
	vector<const char*> Monsters;

	list<class CDummy*> m_ObjectsList{};
	list<class CDummy*> m_MonsterList{};
	class CDummy* m_pSelectedDummy{ nullptr };


	_int m_iSelectIdx = {-1 };

public:
	static CImGui_Manager* Create( const GRAPHIC_DESC& GraphicDesc);
	virtual void Free() override;

};
END
