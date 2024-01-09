#pragma once
#include "Base.h"

BEGIN(Engine)

class CChannel final : public CBase
{
private:
	CChannel();
	virtual ~CChannel() = default;

public:
	HRESULT Init(ifstream& ModelFile);
	void Update_TransformationMatrix(const vector<class CBone*>& Bones, _float& fCurrentAnimPos, _bool& isAnimChanged, _float fInterpolationTime);

private:
	_char m_szName[MAX_PATH]{};

	_uint m_iNumkeyFrames{};
	_uint m_iCurrentKeyFrame{};
	vector<KEYFRAME> m_KeyFrames{};
	_uint m_iBoneIndex{};
	_float44 m_PrevTransformation{};

public:
	static CChannel* Create(ifstream& ModelFile);
	CChannel* Clone();
	virtual void Free() override;
};

END