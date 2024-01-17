#pragma once
#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CAnimation final : public CBase
{
private:
	CAnimation();
	CAnimation(const CAnimation& rhs);
	virtual ~CAnimation() = default;
	
public:
	const _char* Get_Name() const;
	const _bool& IsFinished() const;
	const _float Get_CurrentAnimPos() const;
	const _float Get_Duration() const;
	const _uint Get_NumTrigger() const;
	vector<_float>& Get_Triggers();
	void Add_Trigger(_float fAnimPos);
	void Reset_Trigger();

	void ResetFinished();
	void Set_CurrentAnimPos(_float fCurrentAnimPos);

	// 원명
	const _uint Get_MaxFrame() const {
		return m_iMaxFrame;
	}

public:
	HRESULT Init(ifstream& ModelFile, const vector<class CBone*>& Bones);
	void Update_TransformationMatrix(const vector<class CBone*>& Bones, _float fTimeDelta, _bool& isAnimChanged, const _bool& isLoop, const _bool& bSkipInterpolation, _float fInterpolationTime, _float fDurationRatio, _uint* iCurrentTrigger);

	HRESULT Prepare_Animation(const vector<class CBone*>& Bones, _uint iFrame);



private:
	_char m_szName[MAX_PATH]{};
	
	_float m_fDuration{};
	_float m_fTickPerSec{};
	_float m_fCurrentAnimPos{};
	_mat* m_pPrevTransformation{};
	//vector<_uint> m_CurrentKeyFrames{};
	//vector<_float4x4> m_PrevTransformations{};

	_uint m_iNumChannels{};
	vector<class CChannel*> m_Channels{};

	_bool m_isFinished{};
	_bool m_isInterpolating{};

	_uint m_iNumTriggers{};
	vector<_float> m_Triggers; // 이펙트 트리거

	vector<_uint> m_NumEffects;
	vector<vector<TRIGGEREFFECT_DESC>> m_TriggerEffects;

	_uint m_iNumSoundTrigger{};
	vector<_float> m_SoundTriggers; // 사운드 트리거
	vector<TRIGGERSOUND_DESC> m_TriggerSounds;

	_uint m_iMaxFrame = 0;

	_bool m_hasCloned{};

private:
	void Update_Lerp_TransformationMatrix(const vector<class CBone*>& Bones, _bool& isAnimChanged, _float fInterpolationTime);

public:
	static CAnimation* Create(ifstream& ModelFile, const vector<class CBone*>& Bones);
	CAnimation* Clone();
	virtual void Free() override;
};

END