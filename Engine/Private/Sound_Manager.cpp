#include "Sound_Manager.h"

CSound_Manager::CSound_Manager()
{
}

HRESULT CSound_Manager::Init(_uint iNumChannels)
{
	FMOD::System_Create(&m_pSystem);

	m_pSystem->init(32, FMOD_INIT_NORMAL, NULL);

	if (FAILED(LoadSoundFile()))
	{
		return E_FAIL;
	}

	m_iNumChannels = iNumChannels;
	m_pChannelArr = new FMOD::Channel*[m_iNumChannels]{};

	return S_OK;
}


void CSound_Manager::Play_Sound(const wstring& strSoundTag, _uint iChannel, _float fVolume, _bool isLoop)
{
	FMOD::Sound* pSound = Find_Sound(strSoundTag);

	if (!pSound)
	{
		return;
	}

	_bool bPlay = FALSE;

	if (m_pChannelArr[iChannel]->isPlaying(&bPlay))
	{
		m_pSystem->playSound(pSound, 0, false, &m_pChannelArr[iChannel]);
	}

	if (isLoop)
	{
		m_pChannelArr[iChannel]->setMode(FMOD_LOOP_NORMAL);
	}
	else
	{
		m_pChannelArr[iChannel]->setMode(FMOD_LOOP_OFF);
	}

	m_pChannelArr[iChannel]->setVolume(fVolume);

	m_pSystem->update();
}

void CSound_Manager::PlayBGM(const wstring& strSoundTag, float fVolume)
{
	FMOD::Sound* pSound = Find_Sound(strSoundTag);
	if (!pSound)
	{
		return;
	}

	m_pSystem->playSound(pSound, 0, false, &m_pChannelArr[0]);

	m_pChannelArr[0]->setMode(FMOD_LOOP_NORMAL);

	m_pChannelArr[0]->setVolume(fVolume);

	m_pSystem->update();
}

void CSound_Manager::StopSound(_uint iChannel)
{
	m_pChannelArr[iChannel]->stop();
}

void CSound_Manager::StopAll()
{
	for (int i = 0; i < m_iNumChannels; ++i)
	{
		m_pChannelArr[i]->stop();
	}
}

void CSound_Manager::SetChannelVolume(_uint iChannel, _float fVolume)
{
	m_pChannelArr[iChannel]->setVolume(fVolume);

	m_pSystem->update();
}

HRESULT CSound_Manager::LoadSoundFile()
{
	string FilePath = "../../Client/Bin/Resources/Sound/";

	for (const auto& entry : filesystem::recursive_directory_iterator(FilePath))
	{
		if (entry.is_regular_file())
		{
			FMOD::Sound* pSound = nullptr;

			FMOD_RESULT eRes = m_pSystem->createSound(entry.path().string().c_str(), FMOD_DEFAULT, 0, &pSound);

			if (eRes == FMOD_OK)
			{
				m_Sound.emplace(entry.path().stem().wstring(), pSound);
			}
		}
	}

	return S_OK;
}

FMOD::Sound* CSound_Manager::Find_Sound(const wstring& strSoundTag)
{
	auto& it = m_Sound.find(strSoundTag);

	if (it == m_Sound.end())
	{
		return nullptr;
	}

	return it->second;
}

CSound_Manager* CSound_Manager::Create(_uint iNumChannels)
{
	CSound_Manager* pInstance = new CSound_Manager();

	if (FAILED(pInstance->Init(iNumChannels)))
	{
		MSG_BOX("Failed to Create : CSound_Manager");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSound_Manager::Free()
{
	Safe_Delete_Array(m_pChannelArr);
	
	for (auto& Mypair : m_Sound)
	{
		Mypair.second->release();
	}
	m_Sound.clear();

	m_pSystem->release();
	m_pSystem->close();
}
