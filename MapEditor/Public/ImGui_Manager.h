#pragma once

#include "Base.h"
#include "MapEditor_Define.h"

BEGIN(Engine)
class CGameInstance;
class CVIBuffer_Terrain;
END

BEGIN(MapEditor)

enum class ItemType
{
	Map,
	Terrain,
	Objects,
	Environment,
	Monster,
	NPC,
	Trigger,
	Interaction,
	Camera,
	End
};

struct DummyInfo
{
	wstring Prototype{};
	enum class ItemType eType {};
	_uint iStageIndex{};
	_int iTriggerNum{};
	_float fTriggerSize{};
	_float4 vPos{};
	_float4 vLook{};
	class CDummy** ppDummy{ nullptr };
};

struct MapInfo
{
	wstring Prototype{};
	enum class ItemType eType {};
	_uint iStageIndex{};
	_vec4 vPos{};
	class CMap** ppMap{ nullptr };
};


struct TriggerInfo
{
	_uint iTriggerNum{};
	_float fTriggerSize{};
	_vec4 vPos{};
	class CTrigger** ppTrigger{ nullptr };
};

struct TERRAIN_INFO_MAPTOOL : public TERRAIN_INFO
{
	std::vector<VTXSTATICMESH> Vertices{};
	std::vector<unsigned long> Indices{};
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
	_int FindByName(char* SearchStr, vector<const char*> List);
	HRESULT ImGuiMenu();
	HRESULT ImGuiPos();
	HRESULT ImGuizmoMenu();

	void Create_Dummy(const _int& iListIndex);
	void Create_Map(const _int& iListIndex);
	HRESULT Create_Terrain();
	HRESULT Modify_Terrain();
	void Delete_Dummy();
	void Delete_Map();
	void Delete_Terrain();

	void Reset();

	void Search_Map();
	void Search_Object();
	void Search_Monster();
	void Search_NPC();
	void Search_Envir();

	void Mouse_Pos();
	void FastPicking();
	void Picking_On_Terrain();
	void MeshToMask();

	HRESULT Save_Map_Vertex_Info();
	HRESULT Load_Map_Vertex_Info_For_Terrain();

	// 데이터 파싱
	// 맵
	HRESULT Save_Map();
	HRESULT Load_Map();

	// 오브젝트
	HRESULT Save_Object();
	HRESULT Load_Object();

	// 몬스터
	HRESULT Save_Monster();
	HRESULT Load_Monster();

	// NPC
	HRESULT Save_NPC();
	HRESULT Load_NPC();

	// 환경
	HRESULT Save_Envir();
	HRESULT Load_Envir();

	// 트리거
	HRESULT Save_Trigger();
	HRESULT Load_Trigger();

	// 현재 위치 저장
	HRESULT Save_Pos();

private:
	CGameInstance* m_pGameInstance{ nullptr };
	HWND m_hWnd;
	_uint m_iWinSizeX{ 0 };
	_uint m_iWinSizeY{ 0 };
	float m_iCameraSpeed{ 0 };

	POINT m_ptMouse = {};
	_float2 m_vMousePos{ 0.f,0.f };
	_vec4 m_PickingPos{ 0.f, 0.f, 0.f, 1.f };
	_vec4 m_TerrainPos{ 0.f, 0.f, 0.f, 1.f };
	_float m_fCamDist{};
	_int TerrainCount[2]{ 0,0 };
	_float TerrainHight{ 0.f };
	_float BrushSize{ 1.f };

	_float4 m_vPos{ 0.f, 0.f, 0.f, 1.f };
	_float4 m_vLook{ 0.f, 0.f, 1.f, 0.f };
	ItemType m_eItemType{ ItemType::End };
	_vec4 m_vMouseWorld{ 0.f,0.f,0.f,0.f };

	_int DummyIndex{ 0 };
	_int MapIndex{ 0 };
	_bool m_isMode{ false };

private:
	// 파일의 이름 가져와서 저장
	unordered_map<wstring, vector<const char*>> Maps;
	unordered_map<wstring, vector<const char*>> Objects;
	unordered_map<wstring, vector<const char*>> Monsters;
	vector<const char*> NPCs;
	unordered_map<wstring, vector<const char*>> Envirs;

	vector<class CMap*> m_MapsList;
	vector<class CDummy*> m_ObjectsList;
	vector<class CDummy*> m_MonsterList;
	vector<class CDummy*> m_NPCList;
	vector<class CDummy*> m_EnvirList;
	vector<class CDummy*> m_TriggerList;

	map<int, class CDummy*>m_DummyList;
	map<int, class CMap*>m_Map{};

	class CDummy* m_pSelectedDummy{ nullptr };
	class CMap* m_pSelectMap{ nullptr };
	class CTerrain* m_pTerrain{ nullptr };
	char Serch_Name[MAX_PATH]{};

	_mat	m_ObjectMatrix{};
	_mat	m_MapMatrix{};
	_mat	m_ViewMatrix = {};
	_mat	m_ProjMatrix = {};

	_int m_iSelectIdx = { -1 };
	_bool m_iImGuizmoCheck = { false };
	CTexture* m_pTextures{ nullptr };
	wstring m_eType{};
	_float m_fTriggerSize{1.f};
	_int m_iTriggerNumber{0};


public:
	static CImGui_Manager* Create(const GRAPHIC_DESC& GraphicDesc);
	virtual void Free() override;

};
END
