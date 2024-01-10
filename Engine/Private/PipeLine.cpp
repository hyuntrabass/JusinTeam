#include "PipeLine.h"

CPipeLine::CPipeLine()
{
}

_vec4 CPipeLine::Get_CameraPos() const
{
	return m_vCameraPos;
}

_vec4 CPipeLine::Get_CameraLook() const
{
	return m_vCameraLook;
}

_mat CPipeLine::Get_Transform_vec4x4(TransformType eState) const
{
	return m_TransformMatrix[ToIndex(eState)];
}

_mat CPipeLine::Get_Transform_Inversed_vec4x4(TransformType eState) const
{
	return m_TransformMatrix_Inversed[ToIndex(eState)];
}

_mat CPipeLine::Get_Transform(TransformType eState) const
{
	return XMLoadFloat4x4(&m_TransformMatrix[ToIndex(eState)]);
}

_mat CPipeLine::Get_Transform_Inversed(TransformType eState) const
{
	return XMLoadFloat4x4(&m_TransformMatrix_Inversed[ToIndex(eState)]);
}

void CPipeLine::Set_Transform(TransformType eState, const _mat& TransformMatrix)
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
		m_TransformMatrix_Inversed[i] = m_TransformMatrix[i].Invert();
	}
	m_vCameraPos = _vec4(m_TransformMatrix_Inversed[ToIndex(TransformType::View)].Translation(), 1.f);
	m_vCameraLook = _vec4(m_TransformMatrix_Inversed[ToIndex(TransformType::View)].Backward());
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
