#include "Animation.h"
#include "Channel.h"

CAnimation::CAnimation()
{
}

CAnimation::CAnimation(const CAnimation& rhs)
	: m_fDuration(rhs.m_fDuration)
	, m_fTickPerSec(rhs.m_fTickPerSec)
	, m_iNumChannels(rhs.m_iNumChannels)
	, m_iNumTriggers(rhs.m_iNumTriggers)
	, m_Triggers(rhs.m_Triggers)
	//, m_Channels(rhs.m_Channels)
	//, m_CurrentKeyFrames(rhs.m_CurrentKeyFrames)
	//, m_PrevTransformations(rhs.m_PrevTransformations)
{
	strcpy_s(m_szName, rhs.m_szName);
	//for (auto& pChannel : m_Channels)
	//{
	//	Safe_AddRef(pChannel);
	//}
	for (auto& pPrototypeChannel : rhs.m_Channels)
	{
		CChannel* pChannel = pPrototypeChannel->Clone();

		m_Channels.push_back(pChannel);
	}
}

const _char* CAnimation::Get_Name() const
{
	return m_szName;
}

const _bool& CAnimation::IsFinished() const
{
	return m_isFinished;
}

const _float CAnimation::Get_CurrentAnimPos() const
{
	return m_fCurrentAnimPos;
}

const _float CAnimation::Get_Duration() const
{
	return m_fDuration;
}

const _uint CAnimation::Get_NumTrigger() const
{
	return m_iNumTriggers;
}

vector<_float>& CAnimation::Get_Triggers()
{
	return m_Triggers;
}

void CAnimation::Add_Trigger(_float fAnimPos)
{
	auto	iter = find_if(m_Triggers.begin(), m_Triggers.end(), [&](_float fTrigger)->_bool {
		if (fTrigger == fAnimPos)
			return true; 

		return false;
		});

	if (iter == m_Triggers.end())
	{
		m_Triggers.push_back(fAnimPos);
		m_iNumTriggers++;
	}
}

void CAnimation::Reset_Trigger()
{
	m_iNumTriggers = 0;
	m_Triggers.clear();
}

void CAnimation::ResetFinished()
{
	m_isFinished = false;
	m_fCurrentAnimPos = {};
}

void CAnimation::Set_CurrentAnimPos(_float fCurrentAnimPos)
{
	m_fCurrentAnimPos = fCurrentAnimPos;
}

HRESULT CAnimation::Init(ifstream& ModelFile)
{
	_uint iNameSize{};
	ModelFile.read(reinterpret_cast<_char*>(&iNameSize), sizeof _uint);
	if (iNameSize >= sizeof(m_szName))
	{
		MSG_BOX("Name Is Too Long!");
	}
 	ModelFile.read(m_szName, iNameSize);

	ModelFile.read(reinterpret_cast<_char*>(&m_fDuration), sizeof _float);
	ModelFile.read(reinterpret_cast<_char*>(&m_fTickPerSec), sizeof _float);

	ModelFile.read(reinterpret_cast<_char*>(&m_iNumChannels), sizeof _uint);

	//m_CurrentKeyFrames.resize(m_iNumChannels);
	//m_PrevTransformations.resize(m_iNumChannels);

	for (size_t i = 0; i < m_iNumChannels; i++)
	{
		CChannel* pChannel = CChannel::Create(ModelFile);
		if (!pChannel)
		{
			return E_FAIL;
		}
		m_Channels.push_back(pChannel);
	}

	return S_OK;
}

void CAnimation::Update_TransformationMatrix(const vector<class CBone*>& Bones, _float fTimeDelta, _bool& isAnimChanged, const _bool& isLoop, const _bool& bSkipInterpolation, _float fInterpolationTime, _float fDurationRatio)
{
	if (isAnimChanged)
	{
		if (bSkipInterpolation)
		{
			m_fCurrentAnimPos = 0.f;
			isAnimChanged = false;
		}
		else if (!m_isInterpolating)
		{
			m_fCurrentAnimPos = 0.f;
			m_isInterpolating = true;
		}
		else
		{
			m_fCurrentAnimPos += fTimeDelta;
		}
	}
	else
	{
		if (m_isInterpolating)
		{
			m_isInterpolating = false;
		}

		if (not m_isFinished)
		{
			m_fCurrentAnimPos += m_fTickPerSec * fTimeDelta;
		}

		if (m_fCurrentAnimPos >= m_fDuration * fDurationRatio)
		{
			if (isLoop)
			{
				m_fCurrentAnimPos = 0.f;
			}
			else
			{
				m_isFinished = true;
			}
		}
		else
		{
			m_isFinished = false;
		}
	}

	if (not m_isFinished || not isAnimChanged)
	{
		for (size_t i = 0; i < m_iNumChannels; i++)
		{
			m_Channels[i]->Update_TransformationMatrix(Bones, m_fCurrentAnimPos, isAnimChanged, fInterpolationTime);
		}
	}
	else if (isAnimChanged)
	{
		Update_Lerp_TransformationMatrix();
	}
}

void CAnimation::Update_Lerp_TransformationMatrix(const vector<class CBone*>& Bones, _bool& isAnimChanged, _float fInterpolationTime)
{
	m_iCurrentKeyFrame = 0;
	if (m_PrevTransformation.m[3][3] == 0.f)
	{
		m_PrevTransformation = Bones[m_iBoneIndex]->Get_Transformation();
	}
	_mat PrevTransformation = XMLoadFloat4x4(&m_PrevTransformation);
	_vec4 vSrcScaling{}, vDstScaling{};
	_vec4 vSrcRotation{}, vDstRotation{};
	_vec4 vSrcPotition{}, vDstPosition{};
	_float fRatio = fCurrentAnimPos / fInterpolationTime;

	vSrcScaling.x = PrevTransformation.Right().Length();
	vSrcScaling.y = PrevTransformation.Up().Length();
	vSrcScaling.z = PrevTransformation.Look().Length();
	vSrcScaling.w = 0.f;
	vDstScaling = XMLoadFloat4(&m_KeyFrames[0].vScaling);
	vScaling = XMVectorLerp(vSrcScaling, vDstScaling, fRatio);

	vSrcRotation = XMQuaternionRotationMatrix(PrevTransformation);
	vDstRotation = XMLoadFloat4(&m_KeyFrames[0].vRotation);
	vRotation = XMQuaternionSlerp(vSrcRotation, vDstRotation, fRatio);

	vSrcPotition = _vec4(&PrevTransformation._41);
	vDstPosition = m_KeyFrames[0].vPosition;
	vPosition = XMVectorLerp(vSrcPotition, vDstPosition, fRatio);

	if (fCurrentAnimPos >= fInterpolationTime)
	{
		isAnimChanged = false;
		fCurrentAnimPos = 0.f;
	}
}

CAnimation* CAnimation::Create(ifstream& ModelFile)
{
	CAnimation* pInstance = new CAnimation();

	if (FAILED(pInstance->Init(ModelFile)))
	{
		MSG_BOX("Failed to Create : CAnimation");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CAnimation* CAnimation::Clone()
{
	return new CAnimation(*this);
}

void CAnimation::Free()
{
	for (auto& pChannel : m_Channels)
	{
		Safe_Release(pChannel);
	}
	m_Channels.clear();

	m_Triggers.clear();
}
