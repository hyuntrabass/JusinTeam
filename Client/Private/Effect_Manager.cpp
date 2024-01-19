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
	auto iter = m_EffectInfos.find(strEffectTag);
	if (iter == m_EffectInfos.end())
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

void CEffect_Manager::Create_Effect(const wstring& strEffectTag, _mat* pMatrix, _bool isFollow)
{
	EffectInfo Info = Get_EffectInformation(strEffectTag);
	
	CEffect_Dummy* pEffect = Clone_Effect(&Info);

	m_Effects.emplace(pMatrix, pEffect);
}

void CEffect_Manager::Delete_Effect(void* pMatrix)
{
	auto iter = m_Effects.find(pMatrix);
	if (iter == m_Effects.end())
	{
		MSG_BOX("이펙트를 없음.");
		return;
	}

	Safe_Release(iter->second);
	m_Effects.erase(iter);
}

HRESULT CEffect_Manager::Read_EffectFile()
{
	string strInputFilePath = "../Bin/EffectData/";
	for (const auto& entry : std::filesystem::recursive_directory_iterator(strInputFilePath))
	{
		EffectInfo Info{};
		if (entry.is_regular_file())
		{
			wstring extension = entry.path().extension().wstring();
			if (entry.path().extension().wstring() != L".effect")
			{
				continue;
			}

			ifstream File(entry.path().c_str(), ios::binary);

			if (File.is_open())
			{
				File.read(reinterpret_cast<_char*>(&Info.iType), sizeof Info.iType);
				File.read(reinterpret_cast<_char*>(&Info.isSprite), sizeof Info.isSprite);
				File.read(reinterpret_cast<_char*>(&Info.vNumSprites), sizeof Info.vNumSprites);
				File.read(reinterpret_cast<_char*>(&Info.fSpriteDuration), sizeof Info.fSpriteDuration);
				File.read(reinterpret_cast<_char*>(&Info.PartiDesc), sizeof Info.PartiDesc);
				File.read(reinterpret_cast<_char*>(&Info.iNumInstances), sizeof Info.iNumInstances);
				File.read(reinterpret_cast<_char*>(&Info.fLifeTime), sizeof Info.fLifeTime);
				File.read(reinterpret_cast<_char*>(&Info.vColor), sizeof Info.vColor);
				File.read(reinterpret_cast<_char*>(&Info.iPassIndex), sizeof Info.iPassIndex);
				File.read(reinterpret_cast<_char*>(&Info.vSize), sizeof Info.vSize);
				File.read(reinterpret_cast<_char*>(&Info.vPosOffset), sizeof Info.vPosOffset);
				File.read(reinterpret_cast<_char*>(&Info.vSizeDelta), sizeof Info.vSizeDelta);
				File.read(reinterpret_cast<_char*>(&Info.bApplyGravity), sizeof Info.bApplyGravity);
				File.read(reinterpret_cast<_char*>(&Info.vGravityDir), sizeof Info.vGravityDir);
				File.read(reinterpret_cast<_char*>(&Info.fDissolveDuration), sizeof Info.fDissolveDuration);
				File.read(reinterpret_cast<_char*>(&Info.bSkipBloom), sizeof Info.bSkipBloom);
				File.read(reinterpret_cast<_char*>(&Info.fUnDissolveDuration), sizeof Info.fUnDissolveDuration);
				File.read(reinterpret_cast<_char*>(&Info.vUVDelta), sizeof Info.vUVDelta);

				size_t iNameSize{};

				{
					_tchar* pBuffer{};

					File.read(reinterpret_cast<_char*>(&iNameSize), sizeof size_t);
					pBuffer = new _tchar[iNameSize / sizeof(_tchar)];
					File.read(reinterpret_cast<_char*>(pBuffer), iNameSize);
					Info.strDiffuseTexture = pBuffer;
					Safe_Delete_Array(pBuffer);
					iNameSize = {};

					File.read(reinterpret_cast<_char*>(&iNameSize), sizeof size_t);
					pBuffer = new _tchar[iNameSize / sizeof(_tchar)];
					File.read(reinterpret_cast<_char*>(pBuffer), iNameSize);
					Info.strMaskTexture = pBuffer;
					Safe_Delete_Array(pBuffer);
					iNameSize = {};

					File.read(reinterpret_cast<_char*>(&iNameSize), sizeof size_t);
					pBuffer = new _tchar[iNameSize / sizeof(_tchar)];
					File.read(reinterpret_cast<_char*>(pBuffer), iNameSize);
					Info.strDissolveTexture = pBuffer;
					Safe_Delete_Array(pBuffer);
					iNameSize = {};

					File.read(reinterpret_cast<_char*>(&iNameSize), sizeof size_t);
					pBuffer = new _tchar[iNameSize / sizeof(_tchar)];
					File.read(reinterpret_cast<_char*>(pBuffer), iNameSize);
					Info.strUnDissolveTexture = pBuffer;
					Safe_Delete_Array(pBuffer);
					iNameSize = {};
				}

				{
					_char* pBuffer{};
					File.read(reinterpret_cast<_char*>(&iNameSize), sizeof size_t);
					pBuffer = new _char[iNameSize];
					File.read(pBuffer, iNameSize);
					Info.strModel = pBuffer;
					Safe_Delete_Array(pBuffer);
					iNameSize = {};
				}

				m_EffectInfos.emplace(entry.path().stem().wstring(), Info);

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
