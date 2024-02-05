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
	void SetChannelStartVolume(_uint iChannel);
	void Update();
	HRESULT FadeoutSound(_uint iChannel, _float fTimeDelta, _float fFadeoutSecond);
	HRESULT FadeinSound(_uint iChannel, _float fTimeDelta, _float fFadeinSecond);
	_bool Get_IsPlayingSound(_uint iChannel);
	_float GetChannelVolume(_uint iChannel);

private:
	_float m_fFadeTimeDelta{};
	map<const wstring, FMOD::Sound*> m_Sound;
	FMOD::Channel** m_pChannelArr{};
	FMOD::System* m_pSystem{ nullptr };
	vector<_bool> m_IsPlayingSounds;
	vector<_float> m_StartVolumes;
	vector<_bool> m_IsFadingoutSounds;
	vector<_bool> m_IsFadinginSounds;
	vector<_float> m_FadeSeconds;
	
private:
	HRESULT LoadSoundFile();
	FMOD::Sound* Find_Sound(const wstring& strSoundTag);
	void FadingoutSound(_uint iChannel);
	void FadinginSound(_uint iChannel);

public:
	static CSound_Manager* Create();
	virtual void Free() override;
};

END