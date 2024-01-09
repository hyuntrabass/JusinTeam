#include "PipeLine.h"

CPipeLine::CPipeLine()
{
}

_float4 CPipeLine::Get_CameraPos() const
{
	return m_vCameraPos;
}

_float4 CPipeLine::Get_CameraLook() const
{
	return m_vCameraLook;
}

_float44 CPipeLine::Get_Transform_Float4x4(TransformType eState) const
{
	return m_TransformMatrix[ToIndex(eState)];
}

_float44 CPipeLine::Get_Transform_Inversed_Float4x4(TransformType eState) const
{
	return m_TransformMatrix_Inversed[ToIndex(eState)];
}

_matrix CPipeLine::Get_Transform(TransformType eState) const
{
	return XMLoadFloat4x4(&m_TransformMatrix[ToIndex(eState)]);
}

_matrix CPipeLine::Get_Transform_Inversed(TransformType eState) const
{
	return XMLoadFloat4x4(&m_TransformMatrix_Inversed[ToIndex(eState)]);
}

void CPipeLine::Set_Transform(TransformType eState, const _float44& TransformMatrix)
{
	m_TransformMatrix[ToIndex(eState)] = TransformMatrix;
}

void CPipeLine::Set_Transform(TransformType eState, _fmatrix TransformMatrix)
{
	XMStoreFloat4x4(&m_TransformMatrix[ToIndex(eState)],TransformMatrix);
}

HRESULT CPipeLine::Init()
{
	for (size_t i = 0; i < ToIndex(TransformType::End); i++)
	{
		XMStoreFloat4x4(&m_TransformMatrix[i], XMMatrixIdentity());
		XMStoreFloat4x4(&m_TransformMatrix_Inversed[i], XMMatrixIdentity());
	}

	return S_OK;
}

void CPipeLine::Tick()
{
	for (size_t i = 0; i < ToIndex(TransformType::End); i++)
	{
		XMStoreFloat4x4(&m_TransformMatrix_Inversed[i], XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_TransformMatrix[i])));
	}
	m_vCameraPos = _float4(&m_TransformMatrix_Inversed[ToIndex(TransformType::View)]._41);
	m_vCameraLook = _float4(&m_TransformMatrix_Inversed[ToIndex(TransformType::View)]._31);
}

CPipeLine* CPipeLine::Create()
{
	CPipeLine* pInstance = new CPipeLine();
	
	if (FAILED(pInstance->Init()))
	{
		MSG_BOX("Failed to Create : CPipeLine");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPipeLine::Free()
{
}
