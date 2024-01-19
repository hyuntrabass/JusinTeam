#pragma once
#include "Client_Define.h"
#include "Base.h"
#include "GameInstance.h"

BEGIN(Client)

enum Effect_Type
{
	ET_PARTICLE,
	ET_RECT,
	ET_MESH,
	ET_END
};

class CEffect_Manager final : public CBase
{
	DECLARE_SINGLETON(CEffect_Manager)
private:
	CEffect_Manager();
	virtual ~CEffect_Manager() = default;

public:
	EffectInfo Get_EffectInformation(const wstring& strEffectTag);
	HRESULT Add_Layer_Effect(EffectInfo* pInfo);
	class CEffect_Dummy* Clone_Effect(EffectInfo* pInfo);

	void Create_Effect(const wstring& strEffectTag, _mat* pMatrix, _bool isFollow);
	void Delete_Effect(void* pMatrix);

public:
	HRESULT Read_EffectFile();

private:
	CGameInstance* m_pGameInstance{ nullptr };
	map<const wstring, EffectInfo> m_EffectInfos{};
	map<const void*, class CEffect_Dummy*> m_Effects{};

public:
	virtual void Free() override;
};

END