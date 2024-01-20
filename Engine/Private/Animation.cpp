#include "Animation.h"
#include "Channel.h"
#include "Bone.h"
#include "GameInstance.h"

CAnimation::CAnimation()
{
}

CAnimation::CAnimation(const CAnimation& rhs)
	:m_pGameInstance(CGameInstance::Get_Instance())
	, m_fDuration(rhs.m_fDuration)
	, m_fTickPerSec(rhs.m_fTickPerSec)
	, m_iNumChannels(rhs.m_iNumChannels)
	, m_iNumTriggers(rhs.m_iNumTriggers)
	, m_Triggers(rhs.m_Triggers)
	, m_iNumEffectTriggers(rhs.m_iNumEffectTriggers)
	, m_TriggerEffects(rhs.m_TriggerEffects)
	, m_iMaxFrame(rhs.m_iMaxFrame)
	//, m_Channels(rhs.m_Channels)
	//, m_CurrentKeyFrames(rhs.m_CurrentKeyFrames)
	//, m_PrevTransformations(rhs.m_PrevTransformations)
	, m_pPrevTransformation(rhs.m_pPrevTransformation)
	, m_hasCloned(true)
	, m_PivotMatrix(rhs.m_PivotMatrix)
{
	Safe_AddRef(m_pGameInstance);

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

const _uint CAnimation::Get_NumEffectTrigger() const
{
	return m_iNumEffectTriggers;
}

vector<_float>& CAnimation::Get_Triggers()
{
	return m_Triggers;
}

void CAnimation::Add_TriggerEffect(TRIGGEREFFECT_DESC TriggerEffectDesc)
{
	_mat* pMatrix = new _mat{};
	m_EffectMatrices.push_back(pMatrix);
	m_TriggerEffects.push_back(TriggerEffectDesc);
	m_iNumEffectTriggers++;
}

void CAnimation::Reset_TriggerEffects()
{
	m_iNumEffectTriggers = 0;
	m_TriggerEffects.clear();
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

TRIGGEREFFECT_DESC* CAnimation::Get_TriggerEffect(_uint iTriggerEffectIndex)
{
	return &m_TriggerEffects[iTriggerEffectIndex];
}

vector<TRIGGEREFFECT_DESC>& CAnimation::Get_TriggerEffects()
{
	return m_TriggerEffects;
}

void CAnimation::Reset_Trigger()
{
	m_iNumTriggers = 0;
	m_Triggers.clear();
}

void CAnimation::ResetFinished()
{
	for (size_t i = 0; i < m_EffectMatrices.size(); i++)
	{
		m_pGameInstance->Delete_Effect(m_EffectMatrices[i]);
	}
	m_isFinished = false;
	m_fCurrentAnimPos = {};
	for (auto& pChannel : m_Channels)
	{
		pChannel->Reset_CurrentKeyFrame();
	}
}

void CAnimation::Set_CurrentAnimPos(_float fCurrentAnimPos)
{
	m_fCurrentAnimPos = fCurrentAnimPos;
}

HRESULT CAnimation::Init_Prototype(ifstream& ModelFile, const vector<class CBone*>& Bones, const _mat& PivotMatrix)
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

		m_iMaxFrame = max(m_iMaxFrame, pChannel->Get_KeyFrames().size());
	}

	m_pPrevTransformation = new _mat[Bones.size()];

	m_PivotMatrix = PivotMatrix;

	return S_OK;
}

HRESULT CAnimation::Init(void* pArg)
{
	if (pArg != nullptr)
	{
		m_pOwnerTransform = reinterpret_cast<CTransform*>(pArg);
		Safe_AddRef(m_pOwnerTransform);
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
		for (size_t i = 0; i < m_TriggerEffects.size(); i++)
		{
			if (m_fCurrentAnimPos >= m_TriggerEffects[i].fStartAnimPos)
			{
				//이펙트 생성
				m_pGameInstance->Create_Effect(m_TriggerEffects[i].strEffectName, m_EffectMatrices[i]);
				*m_EffectMatrices[i] = m_TriggerEffects[i].OffsetMatrix * *Bones[m_TriggerEffects[i].iBoneIndex]->Get_CombinedMatrix() * m_PivotMatrix * m_pOwnerTransform->Get_World_Matrix();
			}
			if (m_fCurrentAnimPos <= m_TriggerEffects[i].fEndAnimPos)
			{
				//이펙트 제거
				m_pGameInstance->Delete_Effect(m_EffectMatrices[i]);
			}
		}
	}

	if (isAnimChanged)
	{
		Update_Lerp_TransformationMatrix(Bones, isAnimChanged, fInterpolationTime);
	}
	else if (not m_isFinished)
	{
		for (size_t i = 0; i < m_iNumChannels; i++)
		{
			m_Channels[i]->Update_TransformationMatrix(Bones, m_fCurrentAnimPos, isAnimChanged, fInterpolationTime);
		}

		for (size_t i = 0; i < m_TriggerEffects.size(); i++)
		{
			if (m_TriggerEffects[i].IsFollow)
			{
				//이펙트 위치 갱신
				*m_EffectMatrices[i] = m_TriggerEffects[i].OffsetMatrix * *Bones[m_TriggerEffects[i].iBoneIndex]->Get_CombinedMatrix() * m_PivotMatrix * m_pOwnerTransform->Get_World_Matrix();
			}
		}
	}
}

void CAnimation::Update_Lerp_TransformationMatrix(const vector<class CBone*>& Bones, _bool& isAnimChanged, _float fInterpolationTime)
{
	_vec4 vScaling{};
	_vec4 vRotation{};
	_vec4 vPosition{};

	_uint iNumPrevTransformation{};

	if (m_fCurrentAnimPos == 0.f)
	{
		for (auto& pBone : Bones)
		{
			m_pPrevTransformation[iNumPrevTransformation] = pBone->Get_Transformation();
			++iNumPrevTransformation;/*
			if (m_pPrevTransformation[iNumPrevTransformation].m[3][3] == 0.f)
			{
			}*/
		}

		iNumPrevTransformation = 0;
	}

	for (auto& pBone : Bones)
	{
		_mat PrevTransformation;
		PrevTransformation = m_pPrevTransformation[iNumPrevTransformation];
		const _char* szBoneName = pBone->Get_BoneName();

		_vec4 vSrcScaling{}, vDstScaling{};
		_vector vSrcRotation{};
		_vec4 vDstRotation{};
		_vec4 vSrcPotition{}, vDstPosition{};
		_float fRatio = m_fCurrentAnimPos / fInterpolationTime;

		XMMatrixDecompose(&XMLoadFloat4(&vSrcScaling), &vSrcRotation, &XMLoadFloat4(&vSrcPotition), PrevTransformation);
		vSrcScaling.x = PrevTransformation.Right().Length();
		vSrcScaling.y = PrevTransformation.Up().Length();
		vSrcScaling.z = PrevTransformation.Look().Length();
		vSrcScaling.w = 0.f;
		//vSrcRotation = XMQuaternionRotationMatrix(PrevTransformation);
		vSrcPotition = _vec4(&PrevTransformation._41);

		KEYFRAME DestKeyFrame = {};

		auto iter = find_if(m_Channels.begin(), m_Channels.end(), [&](CChannel* pChannel)->_bool {
			if (0 == strcmp(pChannel->Get_ChannelName(), szBoneName))
			{
				DestKeyFrame = pChannel->Get_FirstKeyFrame();
				return true;
			}
			DestKeyFrame.vScaling = vSrcScaling;
			DestKeyFrame.vRotation = vSrcRotation;
			DestKeyFrame.vPosition = vSrcPotition;
			return false;
			});

		vDstScaling = XMLoadFloat4(&DestKeyFrame.vScaling);
		vDstRotation = XMLoadFloat4(&DestKeyFrame.vRotation);
		vDstPosition = XMLoadFloat4(&DestKeyFrame.vPosition);

		vScaling = XMVectorLerp(vSrcScaling, vDstScaling, fRatio);
		vRotation = XMQuaternionSlerp(vSrcRotation, vDstRotation, fRatio);
		vPosition = XMVectorLerp(vSrcPotition, vDstPosition, fRatio);

		_matrix TransformationMatrix = XMMatrixAffineTransformation(vScaling, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vPosition);

		pBone->Set_Transformation(TransformationMatrix);

		++iNumPrevTransformation;
	}

	if (m_fCurrentAnimPos >= fInterpolationTime)
	{
		isAnimChanged = false;
		m_fCurrentAnimPos = 0.f;
	}
}

HRESULT CAnimation::Prepare_Animation(const vector<class CBone*>& Bones, _uint iFrame)
{
	for (auto& pChannel : m_Channels)
		pChannel->Prepare_Transformation(Bones, iFrame);
	return S_OK;
}

CAnimation* CAnimation::Create(ifstream& ModelFile, const vector<class CBone*>& Bones, const _mat& PivotMatrix)
{
	CAnimation* pInstance = new CAnimation();

	if (FAILED(pInstance->Init_Prototype(ModelFile, Bones, PivotMatrix)))
	{
		MSG_BOX("Failed to Create : CAnimation");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CAnimation* CAnimation::Clone(void* pArg)
{
	CAnimation* pInstance = new CAnimation(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CAnimation");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CAnimation::Free()
{
	Safe_Release(m_pGameInstance);

	for (auto& pChannel : m_Channels)
	{
		Safe_Release(pChannel);
	}
	m_Channels.clear();

	m_Triggers.clear();

	if (not m_hasCloned)
	{
		Safe_Delete_Array(m_pPrevTransformation);
	}

	Safe_Release(m_pOwnerTransform);
	
	for (auto& pEffectMatrix : m_EffectMatrices)
	{
		Safe_Delete(pEffectMatrix);
	}
}
