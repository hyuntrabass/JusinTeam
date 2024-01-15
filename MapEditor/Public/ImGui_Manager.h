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
	Terrain,
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

struct MapInfo
{
	wstring Prototype{};
	enum class ItemType eType {};
	_uint iStageIndex{};
	_uint iTriggerNum{};
	_float4 vPos{};
	class CMap** ppMap{ nullptr };
};

struct TerrainInfo {
	_uint m_iNumVerticesX{};
	_uint m_iNumVerticesZ{};
	class CTerrain** ppTerrain{ nullptr };
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

	_vec4 Get_MouseWorld() { return m_vMouseWorld; }
private:
	HRESULT ImGuiMenu();
	HRESULT ImGuiPos();
	HRESULT ImGuizmoMenu();

	void Create_Dummy(const _int& iListIndex);
	void Create_Map(const _int& iListIndex);
	HRESULT Create_Terrain();
	
	void Delete_Dummy();
	void Delete_Map();
	HRESULT Modify_Terrain();

	const char* Search_Files();
	void Reset();
	void Mouse_Pos();
	void FastPicking();
	void TerrainPicking();

	HRESULT Save_Monster();
	HRESULT Load_Monster();

private:

	CGameInstance* m_pGameInstance{ nullptr };
	HWND m_hWnd;
	_uint m_iWinSizeX{ 0 };
	_uint m_iWinSizeY{ 0 };

	POINT m_ptMouse = {};
	_float2 m_vMousePos{ 0.f,0.f };
	_vec4 m_PickingPos{ 0.f, 0.f, 0.f, 0.f };
	_vec4 m_TerrainPos{ 0.f, 0.f, 0.f, 0.f };
	_float m_fCamDist{};
	_int TerrainCount[1]{};

	_float4 m_vPos{ 0.f, 0.f, 0.f, 1.f };
	_float4 m_vLook{ 0.f, 0.f, 1.f, 0.f };
	ItemType m_eItemType{ ItemType::End };
	_vec4 m_vMouseWorld{ 0.f,0.f,0.f,0.f };

	_int DummyIndex{0};
	_int MapIndex{0};
	_bool m_isTerrain{false};

private:
	// 파일의 이름 가져와서 저장
	vector<const char*> Maps;
	vector<const char*> Objects;
	vector<const char*> Monsters;
	vector<const char*> NPCs;
	vector<const char*> Triggers;

	vector<class CDummy*> m_ObjectsList{};
	vector<class CDummy*> m_MonsterList{};
	vector<class CDummy*> m_NPCList{};
	vector<class CMap*> m_MapsList{};

	map<int, class CDummy*>m_DummyList{};
	map<int, class CMap*>m_Map{};

	class CDummy* m_pSelectedDummy{ nullptr };
	class CMap* m_pSelectMap{ nullptr };
	class CTerrain* m_pTerrain{ nullptr };

	_mat	m_ObjectMatrix{};
	_mat	m_MapMatrix{};
	_mat	m_ViewMatrix = {};
	_mat	m_ProjMatrix = {};

	_int m_iSelectIdx = {-1 };

public:
	static CImGui_Manager* Create( const GRAPHIC_DESC& GraphicDesc);
	virtual void Free() override;

};
END
