#pragma once
#include "Base.h"
#include <cooking/PxCooking.h>

BEGIN(Engine)

class CPhysX final : public CBase
{
private:
	CPhysX(_dev pDevice, _context pContext);
	virtual ~CPhysX() = default;

public:
	HRESULT Init(class CGameInstance* pGameInstance);
	void Tick(_float fTimeDelta);
#ifdef _DEBUG
	HRESULT Render();
#endif // _DEBUG


public:
	void Init_PhysX_Character(class CTransform* pTransform);
	void Init_PhysX_MoveableObject(class CTransform* pTransform);
	void Apply_PhysX(class CTransform* pTransform);
	void Update_PhysX(class CTransform* pTransform);
	void Cook_StaticMesh(_uint iNumVertices, void* pVertices, _uint iNumIndices, void* pIndices);
	void Move(class CTransform* pTransform, _float fTimeDelta);

private:
	_dev m_pDevice{ nullptr };
	_context m_pContext{ nullptr };

	class CGameInstance* m_pGameInstance{ nullptr };

	PxFoundation* m_pFoundation{ nullptr };
	//PxPvd* m_pPvd{ nullptr };
	PxPhysics* m_pPhysics{ nullptr };

	PxScene* m_pScene{ nullptr };
	PxDefaultCpuDispatcher* m_pDispatcher{ nullptr };
	PxMaterial* m_pMaterial{ nullptr };

	map<class CTransform*, PxController*> m_Characters{};
	map<class CTransform*, PxRigidDynamic*> m_DynamicActors{};
	map<class CTransform*, PxRigidStatic*> m_StaticActors{};

#ifdef _DEBUG
	class CVIBuffer_PhysX* m_pVIBufferCom{ nullptr };
	class CShader* m_pDebugShader{ nullptr };
#endif // _DEBUG

private:
	const PxVec3 VectorToPxVec3(_fvector vVector);
	const _vector PxExVec3ToVector(PxExtendedVec3 Src, _float w);

public:
	static CPhysX* Create(_dev pDevice, _context pContext, class CGameInstance* pGameInstance);
	virtual void Free() override;
};

END