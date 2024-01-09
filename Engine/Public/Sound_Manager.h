#pragma once
#include "Base.h"

BEGIN(Engine)

class CSound_Manager final : public CBase
{
private:
	CSound_Manager();
	virtual ~CSound_Manager() = default;

public:
	HRESULT Init(_uint iNumChannels);

public:
	void Play_Sound(const wstring& strSoundTag, _uint eChannel, _float fVolume, _bool isLoop);
	void PlayBGM(const wstring& strSoundTag, float fVolume);
	void StopSound(_uint eChannel);
	void StopAll();

	void SetChannelVolume(_uint eChannel, _float fVolume);

private:
	map<const wstring, FMOD::Sound*> m_Sound;
	_uint m_iNumChannels{};
	FMOD::Channel** m_pChannelArr{};
	FMOD::System* m_pSystem{ nullptr };
	
private:
	HRESULT LoadSoundFile();
	FMOD::Sound* Find_Sound(const wstring& strSoundTag);

public:
	static CSound_Manager* Create(_uint iNumChannels);
	virtual void Free() override;
};

END