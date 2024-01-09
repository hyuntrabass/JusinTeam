#pragma once
#include "Component.h"

BEGIN(Engine)

enum class State
{
	Right,
	Up,
	Look,
	Pos,
	End
};

class ENGINE_DLL CTransform final : public CComponent
{
public:
	using TRANSFORM_DESC = struct tagTransformDesc
	{
		_float fSpeedPerSec{};
		_float fRotationPerSec{};
	};

private:
	CTransform(_dev pDevice, _context pContext);
	CTransform(const CTransform& rhs);
	virtual ~CTransform() = default;

public:
	_matrix Get_World_Matrix() const;
	const _float44& Get_World_float4x4() const;
	_float44 Get_World_Inverse_float4x4() const;
	_vector Get_State(State eState) const;
	_float3 Get_Scale() const;
	const _float& Get_Speed() const;
	const _bool& Is_Jumping() const;
	PxController* Get_Controller() const;
	_vector Get_CenterPos() const;
	const _bool Is_OnGround() const;

	void Set_Position(_float3 vPosition);
	void Set_FootPosition(_float3 vPosition);
	void Set_FootPosition(PxVec3 vPosition);
	void Set_UpDirection(_fvector vUp);
	void Set_State(State eState, _fvector vState);
	void Set_Scale(_float3 fScale);
	void Set_Speed(_float fSpeed);
	void Set_RotationPerSec(_float fAngle);
	void Set_Controller(PxController* pController);

public:
	HRESULT Init_Prototype() override;
	HRESULT Init(void* pArg) override;

public:
	void Gravity(_float fTimeDelta, _fvector vUpDir = XMVectorSet(0.f, 1.f, 0.f, 0.f));
	void Reset_Gravity();
	void WallTest();

	void Go_Straight(_float fTimeDelta);
	const _bool Go_To(_fvector vTargetPos, _float fTimeDelta, _float fOffset = 1.f);
	void Go_To_Dir(_fvector vDir, _float fTimeDelta);
	void Go_Backward(_float fTimeDelta);
	void Go_Left(_float fTimeDelta);
	void Go_Right(_float fTimeDelta);
	void Go_Up(_float fTimeDelta);
	void Go_Down(_float fTimeDelta);
	void Jump(_float fJumpForce = 10.f);

	void LookAt(_fvector vTargetPos);
	void LookAway(_fvector vTargetPos);
	void LookAt_Dir(_fvector vDir);
	void Move_to(_fvector vTargetPos, _float fTimeDelta, _float fMargin = 0.1f);
	void Turn(_fvector vAxis, _float fTimeDelta);
	void Rotation(_fvector vAxis, _float fAngle);
	void Set_Rotation(_vector vQuaternion);

public:
	HRESULT Bind_WorldMatrix(class CShader* pShader, const _char* pVariableName);

private:
	_float44 m_WorldMatrix{};

	PxScene* m_pScene{ nullptr };
	PxController* m_pController{ nullptr };
	PxControllerCollisionFlags m_CollisionFlags{};

	_float m_fGravity{};

	_bool m_isJumping{};
	_float m_fJumpForce{};

	_bool m_isOnWall{};

	_float m_fSpeedPerSec{10.f};
	_float m_fRotationPerSec{XMConvertToRadians(90.f)};

public:
	static CTransform* Create(_dev pDevice, _context pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END