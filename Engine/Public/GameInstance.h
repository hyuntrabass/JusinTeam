#pragma once
#include "Base.h"
#include "Component_Manager.h"
#include "PipeLine.h"
#include "Input_Device.h"
#include "PhysX_Manager.h"
#include "Collision_Manager.h"
#include "Sound_Manager.h"

BEGIN(Engine)

class ENGINE_DLL CGameInstance final : public CBase
{
	DECLARE_SINGLETON(CGameInstance)
private:
	CGameInstance();
	virtual ~CGameInstance() = default;

public:
	HRESULT Init_Engine(_uint iNumLevels, const GRAPHIC_DESC& GraphicDesc, _Inout_ _dev* ppDevice, _Inout_ _context* ppContext);
	void Tick_Engine(_float fTimeDelta);
	void Clear(_uint iLevelIndex);

public: // Graphic Device
	HRESULT Clear_BackBuffer_View(_vec4 vClearColor);
	HRESULT Clear_DepthStencil_View();
	HRESULT Present();
	ID3D11Resource* Get_BackBufferTexture();

public: // Timer Manager
	HRESULT Add_Timer(const wstring& strTimerTag);
	_float Compute_TimeDelta(const wstring& strTimerTag);

public: // Level Manager
	HRESULT Open_Level(_uint iLevelIndex, class CLevel* pNextLevel);
	HRESULT Render();

public: // Object Manager
	HRESULT Add_Prototype_GameObejct(const wstring& strPrototypeTag, class CGameObject* pPrototype);
	HRESULT Add_Layer(_uint iLevelIndex, const wstring strLayerTag, const wstring& strPrototypeTag, void* pArg = nullptr);
	CGameObject* Clone_Object(const wstring& strPrototypeTag, void* pArg = nullptr);
	class CComponent* Get_Component(_uint iLevelIndex, const wstring& strLayerTag, const wstring& strComponentTag, _uint iIndex = 0);
	class CGameObject* Find_Prototype(const wstring& strPrototypeTag);
	class CLayer* Find_Layer(_uint iLevelIndex, const wstring& strLayerTag);
	_uint Get_LayerSize(_uint iLevelIndex, const wstring& strLayerTag);


public: // Component Manager
	HRESULT Add_Prototype_Component(_uint iLevelIndex, const wstring& strPrototype, class CComponent* pPrototype);
	class CComponent* Clone_Component(_uint iLevelIndex, const wstring& strPrototypeTag, void* pArg = nullptr);

public: // Input Manager
	_bool Key_Pressing(_ubyte iKey);
	_bool Key_Down(_ubyte iKey, InputChannel eInputChannel = InputChannel::Default);
	_bool Key_Up(_ubyte iKey, InputChannel eInputChannel = InputChannel::Default);
	_bool Mouse_Pressing(_long iKey);
	_bool Mouse_Down(_long iKey, InputChannel eInputChannel = InputChannel::Default);
	_bool Mouse_Up(_long iKey, InputChannel eInputChannel = InputChannel::Default);
	_long Get_MouseMove(MouseState eMouseState);
	_bool Gamepad_Pressing(GAMPAD_KEY_STATE eKey);
	_bool Gamepad_Down(GAMPAD_KEY_STATE eKey, InputChannel eInputChannel = InputChannel::Default);
	_bool Gamepad_Up(GAMPAD_KEY_STATE eKey, InputChannel eInputChannel = InputChannel::Default);
	// Trigger pushed amount between 0 and 1
	const _float& Gamepad_LTrigger() const;
	// Trigger pushed amount between 0 and 1
	const _float& Gamepad_RTrigger() const;
	// Stick coord between 0 and 1
	const _float2& Gamepad_LStick() const;
	// Stick coord between 0 and 1
	const _float2& Gamepad_RStick() const;
	void Gamepad_Vibration(_ushort LMotorSpeed, _ushort RMotorSpeed);

public: // Light Manager
	LIGHT_DESC* Get_LightDesc(_uint iLevelIndex, const wstring& strLightTag);
	HRESULT Add_Light(_uint iLevelIndex, const wstring& strLightTag, const LIGHT_DESC& LightDesc);
	HRESULT Delete_Light(_uint iLevelIndex, const wstring& strLightTag, _float fDimmerDuration = 0.5f);
	HRESULT Render_Lights(_uint iLevelIndex, class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);
	HRESULT Bind_Light_ViewProjMatrix(_uint iLevelIndex, const wstring& strLightTag, class CShader* pShader, const _char* pViewVariableName, const _char* pProjVariableName);

public: // PipeLine
	_vec4 Get_CameraPos() const;
	_vec4 Get_CameraLook() const;
	_matrix Get_Transform(TransformType eState) const;
	_matrix Get_Transform_Inversed(TransformType eState) const;

	void Set_Transform(TransformType eState, const _mat& TransformMatrix);

	// ����
	void Set_OldViewMatrix(const _mat& TransformMatrix);
	_mat Get_OldViewMatrix_vec4x4() const;
	_matrix Get_OldViewMatrix() const;

	_mat ChagneViewForReflection(_float fWaterHeight);
	void Update_PipeLine();

public: // Picking

	void TransformRay_ToLocal(_mat WorldMatrix);
	_bool Picking_InWorld(_vec4 vPoint1, _vec4 vPoint2, _vec4 vPoint3, _Inout_ _vec3* pPickPos);
	_bool Picking_InLocal(_vec4 vPoint1, _vec4 vPoint2, _vec4 vPoint3, _Inout_ _vec4* pPickPos);
	_bool Picking_InLocal(_vec4 vPoint1, _vec4 vPoint2, _vec4 vPoint3, _vec4 vNormal, _Inout_ _vec4* pPickPos);
	_float4 PickingDepth(_float x, _float y);
	_int FastPicking(_uint x, _uint y);

	_vec4 Compute_MousePicked_Terrain(_float44 matTerrainWorld, _float3* pVerticesPos, _uint iNumVerticesX, _uint iNumVerticesZ);
	_vec4 Compute_MousePicked_MeshTerrain(_float44 matTerrainWorld, vector<VTXNORTEX> vVertices, vector<_ulong> vIndices);

	HRESULT Ready_Texture2D();
	HRESULT Ready_FastPicking();
	_vec4 Get_World_Pos();
	_vec4 Get_World_Dir();


public: // Font
	HRESULT Add_Font(const wstring& strFontTag, const wstring& strFilePath);
	HRESULT Render_Text(const wstring& strFontTag, const wstring& strText, const _float2& vPosition, _float fScale = 1.f, _vec4 vColor = _vec4(1.f), _float fRotation = 0.f, _bool isFront = false);
	_vec2 Get_TextSize(const wstring& strFontTag, const wstring& strText);

public: // Frustum
	_bool IsIn_Fov_World(_vec4 vPos, _float fRange = 2.f);
	void Transform_ToLocalSpace(_mat vWorldInversed);
	_bool IsIn_Fov_Local(_vec4 vPos, _float fRange = 0.f);

public: // Collision
	HRESULT Register_CollisionObject(class CGameObject* pObject, class CCollider* pHitCollider, _bool IsPlayer = false, class CCollider* AttRangeCollider = nullptr, class CCollider* ParryingCollider = nullptr);
	void Delete_CollisionObject(class CGameObject* pObject, _bool IsPlayer = false);
	void Attack_Monster(class CCollider* pCollider, _uint iDamage, _uint iDamageType = 0);
	_bool CheckCollision_Monster(class CCollider* pCollider);
	_bool CheckCollision_Parrying(class CCollider* pCollider);
	_bool Attack_Player(class CCollider* pCollider, _uint iDamage, _uint iDamageType = 0);
	_bool CheckCollision_Player(class CCollider* pCollider); // �ʿ����
	CCollider* Get_Nearest_MonsterCollider();
	CollideFace Get_CollideFace(CCollider* pAABBCollider, CCollider* pShereCollider);

public: // PhysX
	void Init_PhysX_Character(class CTransform* pTransform, CollisionGroup eGroup, PxCapsuleControllerDesc* pDesc = nullptr);
	void Init_PhysX_MoveableObject(class CTransform* pTransform);
	void Apply_PhysX(class CTransform* pTransform);
	void Update_PhysX(class CTransform* pTransform);
	PxRigidStatic* Cook_StaticMesh(_uint iNumVertices, void* pVertices, _uint iNumIndices, void* pIndices);
	_bool Raycast(_vec3 vOrigin, _vec3 vDir, _float fDist, PxRaycastBuffer& Buffer);
	_bool Raycast(_vec4 vOrigin, _vec4 vDir, _float fDist, PxRaycastBuffer& Buffer, PxQueryFilterData Filter);
	_bool Raycast(_vec4 vOrigin, _vec4 vDir, _float fDist, PxRaycastBuffer& Buffer);
	void PhysXTick(_float fTimeDelta);
#ifdef _DEBUG
	HRESULT Render_PhysX();
#endif // _DEBUG

public: // RenderTarget
	HRESULT Add_RenderTarget(const wstring& strTargetTag, _uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, const _vec4& vColor, const _bool isMultiSample = false);
	HRESULT Add_MRT(const wstring& strMRTTag, const wstring& strTargetTag);
	HRESULT Begin_MRT(const wstring& strMRTTag, ID3D11DepthStencilView* pDepthStencillView = nullptr, const _bool shouldClear = true);
	HRESULT End_MRT();
	HRESULT Bind_ShaderResourceView(class CShader* pShader, const _char* pVariableName, const wstring& strTargetTag);
	ID3D11Texture2D* Get_Texture2D(const wstring& strTargetTag);
	ID3D11ShaderResourceView* Get_SRV(const wstring& strTargetTag);
	HRESULT Copy_Texture(const wstring& strTargetTag, ID3D11Resource* pTexture);

#ifdef _DEBUG
public:
	HRESULT Ready_Debug_RT(const wstring& strTargetTag, _float2 vPos, _float2 vSize);
	HRESULT Render_Debug_RT(const wstring& strMRTTag, class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);
#endif // _DEBUG

public: // Sound Manager
	HRESULT Init_SoundManager();
	_bool Is_SoundManager_Ready();
	_int Play_Sound(const wstring& strSoundTag, _float fVolume = 0.5f, _bool isLoop = false, _float fStartPosRatio = 0.f);
	void PlayBGM(const wstring& strSoundTag, float fVolume = 0.3f);
	void StopSound(_uint iChannel);
	void StopAll();
	//fFadeSoundRatio 0.f ~ 2.f
	HRESULT FadeoutSound(_uint iChannel, _float fTimeDelta, _float fFadeoutSecond = 1.f, _bool IsReusable = true, _float fFadeSoundRatio = 0.f);
	HRESULT FadeinSound(_uint iChannel, _float fTimeDelta, _float fFadeinSecond = 1.f, _float fFadeSoundRatio = 1.f);

	// ���� ä���� ����������� ��ȯ ��.
	_bool Get_IsPlayingSound(_uint iChannel);
	// ���� ä���� ������ ��ȯ ��.
	_float Get_ChannelVolume(_uint iChannel);
	// ���� ä���� ������������ ��ȯ ��.
	_bool Get_IsLoopingSound(_uint iChannel);
	// ���� ä���� ���� �����ġ�� ������ ��ȯ ��.
	_float Get_ChannelCurPosRatio(_uint iChannel);

	// ���� ä���� ������ ������.
	void Set_ChannelVolume(_uint iChannel, _float fVolume);
	// ���� ä���� ������ �ʱ� �������� �ǵ���.
	void Set_ChannelStartVolume(_uint iChannel);

public: // Effect Callback
	using Func_CreateFX = function<void(const wstring&, _mat*, const _bool&)>;
	using Func_DeleteFX = function<void(const void*)>;
	using Func_TickFX = function<void(_float)>;
	using Func_HasCreatedFX = function<_bool(const void*)>;
	using Func_GetInfoFX = function<EffectInfo(const void*)>;
	using Func_ClearFX = function<void(_uint)>;

	void Register_CreateEffect_Callback(Func_CreateFX Function);
	void Register_DeleteEffect_Callback(Func_DeleteFX Function);
	void Register_Tick_LateTick_Callback(Func_TickFX Tick, Func_TickFX Late_Tick);
	void Register_HasCreated_Callback(Func_HasCreatedFX Function);
	void Register_Clear_Callback(Func_ClearFX Function);

	void Create_Effect(const wstring& strEffectTag, _mat* pMatrix, const _bool& isFollow);
	void Delete_Effect(const void* pMatrix);
	_bool Has_Created_Effect(const void* pMatrixKey);

public: // Video Manager
	// ������ ��� �մϴ�. ���ϸ� Ȯ���ڱ��� �ٿ��� ��.
	HRESULT Play_Video(const wstring& strVideoFilePath);
	// ������ ���� �մϴ�. �ظ��ϸ� Ŭ�󿡼� ���� ����.
	void Stop_Video();
	// ������ ��������� ��ȯ �˴ϴ�.
	const _bool Is_Playing_Video();

	// ������ �ߴܽ�ų Ű�� �����մϴ�.
	void Set_StopKey(_ubyte iKey);

public: // Cascade Mananger
	CASCADE_DESC Get_CascadeDesc();

public: // Get_Set
	// ī�޶��� near�� far�� ��ȯ ��. x�� near, y�� far.
	const _float2& Get_CameraNF() const;
	// ���� ������ ��ȯ��. client define�� �̳� ����.
	const _uint& Get_CurrentLevelIndex() const;
	// �ð� ������ ��ȯ ��. 1�̸� ���� �ӵ�, 0.5�� ���� �ӵ��� ���ο�.
	const _float& Get_TimeRatio() const;
	// ���� �Ȱ��� near ���� far�� ��ȯ ��.
	const _float2& Get_FogNF() const;
	// ���� �Ȱ��� ���� ��ȯ ��.
	const _color& Get_FogColor() const;
	// ������ ���۵Ǵ� ���̸� ��ȯ ��. (���� ���̺��� ������ ���� ��ο����°� hell�̶�� �س���.)
	const _float& Get_HellHeight() const;
#ifdef _DEBUG
	// �ֿܼ� ����� ��Ʈ���� ������.
	stringstream& Get_StringStream();
	void Add_String_to_Stream(const string& strText);
#endif

	// ī�޶��� near, far�� ������. �̰͵� ī�޶󿡼��� ȣ�� �Ұ�.
	void Set_CameraNF(const _float2& vCamNF);
	// ���� ������ ������. �� ������ Init()�Լ��� �־��ּ���.
	void Set_CurrentLevelIndex(const _uint& iIndex);
	// ���ο���, �������⸦ �����Ű�� �Լ�. �������� �־��ش�.
	void Set_TimeRatio(const _float fRatio);
	// �Ȱ��� ������ ������ �� ��. near���� �Ȱ��� ���� �����ؼ� far�� �� ���� �Ȱ��� ������.
	void Set_FogNF(const _float2& vFogNF);
	// �Ȱ��� ���� ����.
	void Set_FogColor(const _color& vFogColor);
	// hell ���̸� �����Ѵ�.
	void Set_HellHeight(const _float& fHeight);

	void Set_InputString(const wstring& strInput);
	void Set_CompleteInputString(const wstring& strInput);
	void Set_ComposingInputString(const wstring& strInput);
	void Set_ComposingState(const _bool isComposing);
	void Popback_InputString();

	//void Set_CameraAttackZoom(_float fAttackZoom) { m_fCameraAttackZoom = fAttackZoom; }
	//const _float& Get_CameraAttackZoom() { return m_fCameraAttackZoom; }
	const _bool& IsSkipDebugRendering() const;
	const wstring& Get_InputString();
	const wstring& Get_CompleteInputString() const;

	//����
	_bool Get_TurnOnShadow() const {
		return m_bTurnOnShadow;
	}

public:
	void Level_ShutDown(_uint iCurrentLevel);
	_bool Is_Level_ShutDown(_uint iCurrentLevel);

private:
	class CGraphic_Device* m_pGraphic_Device{ nullptr };

private:
	class CTimer_Manager* m_pTimer_Manager{ nullptr };
	class CLevel_Manager* m_pLevel_Manager{ nullptr };
	class CObject_Manager* m_pObject_Manager{ nullptr };
	class CComponent_Manager* m_pComponent_Manager{ nullptr };
	class CInput_Device* m_pInput_Manager{ nullptr };
	class CLight_Manager* m_pLight_Manager{ nullptr };
	class CFont_Manager* m_pFont_Manager{ nullptr };
	class CFrustum* m_pFrustum{ nullptr };
	class CCollision_Manager* m_pCollision_Manager{ nullptr };
	class CRenderTarget_Manager* m_pRenderTarget_Manager{ nullptr };
	class CSound_Manager* m_pSound_Manager{ nullptr };

	class CPipeLine* m_pPipeLine{ nullptr };
	class CPicking* m_pPicking{ nullptr };
	class CPhysX_Manager* m_pPhysX_Manager{ nullptr };
	class CVideo_Manager* m_pVideo_Manager{ nullptr };

	// ����
	class CCascade_Manager* m_pCascade_Manager{ nullptr };

private:
	_uint m_iLevelIndex{};
	_float m_fTimeRatio{ 1.f };
	_float2 m_vCameraNF{};
	_float2 m_vFogNF{ 2000.f, 2000.f };
	_color m_vFogColor{ 0.9f };
	_float m_fHellHeight{ -1000.f };
	_bool m_bSkipDebugRender{ true };
	wstring m_strInput{};
	wstring m_strCompleteInput{};
	wstring m_strComposingInput{};
	_bool m_isComposing{};
	_bool m_isPopInput{};

	// ����
	_bool m_bTurnOnShadow{ false };

#ifdef _DEBUG
	stringstream m_OutputStream{};
	string m_strPrevStream{};
#endif

private:
	vector<_bool> m_vecLevelInvalid{};

private:
	Func_CreateFX m_Function_Create_FX{};
	Func_DeleteFX m_Function_Delete_FX{};
	Func_TickFX m_Function_Tick_FX{};
	Func_TickFX m_Function_LateTick_FX{};
	Func_HasCreatedFX m_Function_HasCreated{};
	Func_ClearFX m_Function_Clear_FX{};

public:
	static void Release_Engine();

private:
#ifdef _DEBUG
	void Print_StringStream();
#endif
	void Clear_Managers();

public:
	virtual void Free() override;
};

END