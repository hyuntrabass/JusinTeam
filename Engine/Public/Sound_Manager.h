#pragma once
#include "Base.h"

BEGIN(Engine)

class CSound_Manager final : public CBase
{
private:
	CSound_Manager();
	virtual ~CSound_Manager() = default;

public:
	HRESULT Init();

public:
	_int Play_Sound(const wstring& strSoundTag, _float fVolume, _bool isLoop);
	void PlayBGM(const wstring& strSoundTag, _float fVolume);
	void StopSound(_uint iChannel);
	void StopAll();
	void SetChannelVolume(_uint iChannel, _float fVolume);
	void Update();
	_bool Get_IsPlayingSound(_uint iChannel);
	_float GetChannelVolume(_uint iChannel);

private:
	map<const wstring, FMOD::Sound*> m_Sound;
	FMOD::Channel** m_pChannelArr{};
	FMOD::System* m_pSystem{ nullptr };
	vector<_bool> m_IsPlayingSounds;
	
private:
	HRESULT LoadSoundFile();
	FMOD::Sound* Find_Sound(const wstring& strSoundTag);

public:
	static CSound_Manager* Create();
	virtual void Free() override;
};

END