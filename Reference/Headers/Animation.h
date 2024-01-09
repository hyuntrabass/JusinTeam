#pragma once
#include "Base.h"

BEGIN(Engine)

class CAnimation final : public CBase
{
private:
	CAnimation();
	CAnimation(const CAnimation& rhs);
	virtual ~CAnimation() = default;
	
public:
	const _bool& IsFinished() const;
	const _float Get_CurrentAnimPos() const;

	void ResetFinished();

public:
	HRESULT Init(ifstream& ModelFile);
	void Update_TransformationMatrix(const vector<class CBone*>& Bones, _float fTimeDelta, _bool& isAnimChanged, const _bool& isLoop, const _bool& bSkipInterpolation, _float fInterpolationTime, _float fDurationRatio);

private:
	_char m_szName[MAX_PATH]{};
	
	_float m_fDuration{};
	_float m_fTickPerSec{};
	_float m_fCurrentAnimPos{};
	//vector<_uint> m_CurrentKeyFrames{};
	//vector<_float4x4> m_PrevTransformations{};

	_uint m_iNumChannels{};
	vector<class CChannel*> m_Channels{};

	_bool m_isFinished{};
	_bool m_isInterpolating{};

public:
	static CAnimation* Create(ifstream& ModelFile);
	CAnimation* Clone();
	virtual void Free() override;
};

END