#include "Channel.h"
#include "Bone.h"

CChannel::CChannel()
{
}

HRESULT CChannel::Init(ifstream& ModelFile)
{
	_uint iNameSize{};
	ModelFile.read(reinterpret_cast<_char*>(&iNameSize), sizeof _uint);
	if (iNameSize >= sizeof(m_szName))
	{
		MSG_BOX("Name Is Too Long!");
	}
	ModelFile.read(m_szName, iNameSize);

	ModelFile.read(reinterpret_cast<_char*>(&m_iBoneIndex), sizeof _uint);

	ModelFile.read(reinterpret_cast<_char*>(&m_iNumkeyFrames), sizeof _uint);

	KEYFRAME KeyFrame{};

	for (size_t i = 0; i < m_iNumkeyFrames; i++)
	{
		ModelFile.read(reinterpret_cast<_char*>(&KeyFrame.vScaling), sizeof _float4);
		ModelFile.read(reinterpret_cast<_char*>(&KeyFrame.vRotation), sizeof _float4);
		ModelFile.read(reinterpret_cast<_char*>(&KeyFrame.vPosition), sizeof _float4);
		ModelFile.read(reinterpret_cast<_char*>(&KeyFrame.fTime), sizeof _float);

		m_KeyFrames.push_back(KeyFrame);
	}

	return S_OK;
}

void CChannel::Update_TransformationMatrix(const vector<class CBone*>& Bones, _float& fCurrentAnimPos, _bool& isAnimChanged, _float fInterpolationTime)
{
	if (fCurrentAnimPos == 0.f && !isAnimChanged)
	{
		m_iCurrentKeyFrame = 0;
	}

	KEYFRAME LastKeyFrame = m_KeyFrames.back();

	_vector vScaling{};
	_vector vRotation{};
	_vector vPosition{};

	if (isAnimChanged)
	{
		m_iCurrentKeyFrame = 0;
		if (m_PrevTransformation.m[3][3] == 0.f)
		{
			m_PrevTransformation = Bones[m_iBoneIndex]->Get_Transformation();
		}
		_matrix PrevTransformation = XMLoadFloat4x4(&m_PrevTransformation);
		_vector vSrcScaling{}, vDstScaling{};
		_vector vSrcRotation{}, vDstRotation{};
		_vector vSrcPotition{}, vDstPosition{};
		_float fRatio = fCurrentAnimPos / fInterpolationTime;

		vSrcScaling.m128_f32[0] = XMVector4Length(PrevTransformation.r[0]).m128_f32[0];
		vSrcScaling.m128_f32[1] = XMVector4Length(PrevTransformation.r[1]).m128_f32[0];
		vSrcScaling.m128_f32[2] = XMVector4Length(PrevTransformation.r[2]).m128_f32[0];
		vSrcScaling.m128_f32[3] = 0.f;
		vDstScaling = XMLoadFloat4(&m_KeyFrames[0].vScaling);
		vScaling = XMVectorLerp(vSrcScaling, vDstScaling, fRatio);

		vSrcRotation = XMQuaternionRotationMatrix(PrevTransformation);
		vDstRotation = XMLoadFloat4(&m_KeyFrames[0].vRotation);
		vRotation = XMQuaternionSlerp(vSrcRotation, vDstRotation, fRatio);

		vSrcPotition = PrevTransformation.r[3];
		vDstPosition = XMLoadFloat4(&m_KeyFrames[0].vPosition);
		vPosition = XMVectorLerp(vSrcPotition, vDstPosition, fRatio);
		
		if (fCurrentAnimPos >= fInterpolationTime)
		{
			isAnimChanged = false;
			fCurrentAnimPos = 0.f;
		}
	}
	else if (fCurrentAnimPos >= LastKeyFrame.fTime)
	{
		vScaling = XMLoadFloat4(&LastKeyFrame.vScaling);
		vRotation = XMLoadFloat4(&LastKeyFrame.vRotation);
		vPosition = XMLoadFloat4(&LastKeyFrame.vPosition);
	}
	else
	{
		m_PrevTransformation = {};
		while (fCurrentAnimPos >= m_KeyFrames[m_iCurrentKeyFrame + 1].fTime)
		{
			m_iCurrentKeyFrame++;
		}

		_float fRatio = (fCurrentAnimPos - m_KeyFrames[m_iCurrentKeyFrame].fTime) / (m_KeyFrames[m_iCurrentKeyFrame + 1].fTime - m_KeyFrames[m_iCurrentKeyFrame].fTime);

		_vector vSrcScaling{}, vDstScaling{};
		_vector vSrcRotation{}, vDstRotation{};
		_vector vSrcPotition{}, vDstPosition{};

		vSrcScaling = XMLoadFloat4(&m_KeyFrames[m_iCurrentKeyFrame].vScaling);
		vDstScaling = XMLoadFloat4(&m_KeyFrames[m_iCurrentKeyFrame + 1].vScaling);
		vScaling = XMVectorLerp(vSrcScaling, vDstScaling, fRatio);

		vSrcRotation = XMLoadFloat4(&m_KeyFrames[m_iCurrentKeyFrame].vRotation);
		vDstRotation = XMLoadFloat4(&m_KeyFrames[m_iCurrentKeyFrame + 1].vRotation);
		vRotation = XMQuaternionSlerp(vSrcRotation, vDstRotation, fRatio);

		vSrcPotition = XMLoadFloat4(&m_KeyFrames[m_iCurrentKeyFrame].vPosition);
		vDstPosition = XMLoadFloat4(&m_KeyFrames[m_iCurrentKeyFrame + 1].vPosition);
		vPosition = XMVectorLerp(vSrcPotition, vDstPosition, fRatio);
	}
	
	_matrix Transformation = XMMatrixAffineTransformation(vScaling, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vPosition);

	Bones[m_iBoneIndex]->Set_Transformation(Transformation);
}

CChannel* CChannel::Create(ifstream& ModelFile)
{
	CChannel* pInstance = new CChannel();

	if (FAILED(pInstance->Init(ModelFile)))
	{
		MSG_BOX("Failed to Create : CChannel");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CChannel* CChannel::Clone()
{
	return new CChannel(*this);
}

void CChannel::Free()
{
}
