#include "Effect_Manager.h"
#include "Effect_Dummy.h"

IMPLEMENT_SINGLETON(CEffect_Manager)

CEffect_Manager::CEffect_Manager()
	: m_pGameInstance(CGameInstance::Get_Instance())
{
	Safe_AddRef(m_pGameInstance);
}

EffectInfo CEffect_Manager::Get_EffectInformation(const wstring& strEffectTag)
{
	auto iter = m_Effects.find(strEffectTag);
	if (iter == m_Effects.end())
	{
		MSG_BOX("Failed to Find Effect Information");
		return EffectInfo();
	}

	return iter->second;
}

HRESULT CEffect_Manager::Add_Layer_Effect(EffectInfo* pInfo)
{
	return m_pGameInstance->Add_Layer(m_pGameInstance->Get_CurrentLevelIndex(), L"Layer_Effect", L"Prototype_GameObject_EffectDummy", pInfo);
}

CEffect_Dummy* CEffect_Manager::Clone_Effect(EffectInfo* pInfo)
{
	return dynamic_cast<CEffect_Dummy*>(m_pGameInstance->Clone_Object(L"Prototype_GameObject_EffectDummy", pInfo));
}

HRESULT CEffect_Manager::Read_EffectFile()
{
	string strInputFilePath = "../../Client/Bin/EffectData/";
	for (const auto& entry : std::filesystem::recursive_directory_iterator(strInputFilePath))
	{
		EffectInfo Info{};
		if (entry.is_regular_file())
		{
			ifstream File(entry.path().c_str(), ios::binary);

			if (File.is_open())
			{
				File.read(reinterpret_cast<_char*>(&Info), sizeof Info);

				m_Effects.emplace(entry.path().stem().string(), Info);

				File.close();
			}
			else
			{
				return E_FAIL;
			}
		}
	}

	return S_OK;
}

void CEffect_Manager::Free()
{
	Safe_Release(m_pGameInstance);
}
