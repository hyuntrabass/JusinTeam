#pragma once
#include "Base.h"

BEGIN(Engine)

class CSound_Manager final : public CBase
{
public:
	typedef struct tagSoundDesc {

		_bool IsPlayingSound{};
		_float fStartVolume = 0.5f;
		_bool IsFadingout{};
		_bool IsFadingin{};
		_float fFadeSecond = 1.f;
		_bool IsReusable{};
		_float fFadeSoundRatio{};
		_float fDurationRatio = 1.f;

	}SOUND_DESC;

private:
	CSound_Manager();
	virtual ~CSound_Manager() = default;

public:
	HRESULT Init();

public:
	_int Play_Sound(const wstring& strSoundTag, _float fVolume, _bool isLoop, _float fStartPosRatio, _float fDurationRatio);
	void PlayBGM(const wstring& strSoundTag, _float fVolume);
	void StopSound(_int iChannel);
	void StopAll();
	void SetChannelVolume(_int iChannel, _float fVolume);
	void SetChannelStartVolume(_int iChannel);
	void SetSystemVolume(_float fSystemVolume);
	void SetBackGroundVolume(_float fBackGroundVolume);
	void SetEnvironmentVolume(_float fEnvironmentVolume);
	void SetEffectVolume(_float fEffectVolume);
	void Update();
	HRESULT FadeoutSound(_int iChannel, _float fTimeDelta, _float fFadeoutSecond, _bool IsReusable, _float fFadeSoundRatio);
	HRESULT FadeinSound(_int iChannel, _float fTimeDelta, _float fFadeinSecond, _float fFadeSoundRatio);
	_bool Get_IsPlayingSound(_int iChannel);
	_float GetChannelVolume(_int iChannel);
	_bool Get_IsLoopingSound(_int iChannel);
	_float Get_CurPosRatio(_int iChannel);

private:
	_float m_fSystemVolume{ 1.f };
	_float m_fBackGroundVolume{ 1.f };
	_float m_fEnvironmentVolume{ 1.f };
	_float m_fEffectVolume{ 1.f };
	_float m_fFadeTimeDelta{};
	map<const wstring, FMOD::Sound*> m_Sound;
	FMOD::Channel** m_pChannelArr{};
	FMOD::System* m_pSystem{ nullptr };
	vector<SOUND_DESC> m_SoundDescs;
	
private:
	HRESULT LoadSoundFile();
	FMOD::Sound* Find_Sound(const wstring& strSoundTag);
	void FadingoutSound(_int iChannel);
	void FadinginSound(_int iChannel);
	_float Standard_FadeSound(_int iChannel);

public:
	static CSound_Manager* Create();
	virtual void Free() override;
};

END