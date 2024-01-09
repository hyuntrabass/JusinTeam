#pragma once
#include "Base.h"

BEGIN(Engine)

enum class TransformType
{
	View,
	Proj,
	End
};

class CPipeLine final : public CBase
{
private:
	CPipeLine();
	virtual ~CPipeLine() = default;

public:
	_float4 Get_CameraPos() const;
	_float4 Get_CameraLook() const;
	_float44 Get_Transform_Float4x4(TransformType eState) const;
	_float44 Get_Transform_Inversed_Float4x4(TransformType eState) const;
	_matrix Get_Transform(TransformType eState) const;
	_matrix Get_Transform_Inversed(TransformType eState) const;

	void Set_Transform(TransformType eState, const _float44& TransformMatrix);
	void Set_Transform(TransformType eState, _fmatrix TransformMatrix);

public:
	HRESULT Init();
	void Tick();

private:
	_float4 m_vCameraPos{};
	_float4 m_vCameraLook{};
	_float44 m_TransformMatrix[ToIndex(TransformType::End)]{};
	_float44 m_TransformMatrix_Inversed[ToIndex(TransformType::End)]{};

public:
	static CPipeLine* Create();
	virtual void Free() override;
};

END