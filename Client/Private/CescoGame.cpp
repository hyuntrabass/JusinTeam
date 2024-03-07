#include "CescoGame.h"
#include "VTFMonster.h"
#include "Log.h"
#include "Hook.h"
#include "Hive.h"
#include "Buff_Card.h"
#include "TextButtonColor.h"
#include "UI_Manager.h"
#include "Camera_Manager.h"

CCescoGame::CCescoGame(_dev pDevice, _context pContext)
	:CGameObject(pDevice, pContext)
{
}

CCescoGame::CCescoGame(const CCescoGame& rhs)
	:CGameObject(rhs)
	, m_pPlayerTransform(rhs.m_pPlayerTransform)
{
	Safe_AddRef(m_pPlayerTransform);
}

HRESULT CCescoGame::Init_Prototype()
{
	m_pPlayerTransform = GET_TRANSFORM("Layer_Player", LEVEL_STATIC);
	Safe_AddRef(m_pPlayerTransform);

	return S_OK;
}

HRESULT CCescoGame::Init(void* pArg)
{
	//FRONT
	m_SpawnPositions.push_back(_vec3(-3000.f, 1.f, 30.f));
	//BACK
	m_SpawnPositions.push_back(_vec3(-3000.f, 1.f, -30.f));
	//RIGHT
	m_SpawnPositions.push_back(_vec3(-2970.f, 1.f, 0.f));
	//LEFT
	m_SpawnPositions.push_back(_vec3(-3030.f, 1.f, 0.f));

	//DOWN
	//LF
	m_HiveSpawnPositions.push_back(_vec3(-3026.f, 13.5f, 26.f));
	//RF
	m_HiveSpawnPositions.push_back(_vec3(-2974.f, 13.5f, 26.f));
	//LB
	m_HiveSpawnPositions.push_back(_vec3(-3026.f, 13.5f, -26.f));
	//RB
	m_HiveSpawnPositions.push_back(_vec3(-2974.f, 13.5f, -26.f));
	//UP
	//LF
	m_HiveSpawnPositions.push_back(_vec3(-3025.f, 21.5f, 25.f));
	//RF
	m_HiveSpawnPositions.push_back(_vec3(-2975.f, 21.5f, 25.f));
	//LB
	m_HiveSpawnPositions.push_back(_vec3(-3025.f, 21.5f, -25.f));
	//RB
	m_HiveSpawnPositions.push_back(_vec3(-2975.f, 21.5f, -25.f));

	random_device rand;
	m_RandomNumber = _randNum(rand());

	CTextButtonColor::TEXTBUTTON_DESC ColButtonDesc = {};
	ColButtonDesc.eLevelID = LEVEL_STATIC;
	ColButtonDesc.fDepth = (_float)D_ALERT / (_float)D_END;
	ColButtonDesc.fFontSize = 0.8f;
	ColButtonDesc.strText = TEXT("");
	ColButtonDesc.strTexture = TEXT("Prototype_Component_Texture_UI_Tower_TimeLimit");
	ColButtonDesc.vSize = _vec2(200.f, 200.f);
	ColButtonDesc.vPosition = _vec2((_float)g_ptCenter.x, 60.f);
	ColButtonDesc.fAlpha = 0.5f;


	m_pTimeBar = (CTextButtonColor*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButtonColor"), &ColButtonDesc);
	if (not m_pTimeBar)
	{
		return E_FAIL;
	}
	m_pTimeBar->Set_Pass(VTPass_UI_Alpha);
	CUI_Manager::Get_Instance()->Set_isBoss(true);
	CCamera_Manager::Get_Instance()->Set_RidingZoom(true);

	return S_OK;
}

void CCescoGame::Tick(_float fTimeDelta)
{
	if (CTrigger_Manager::Get_Instance()->Get_CurrentSpot() != TS_CescoMap)
	{
		Kill();
	}
	CAMERA_STATE CamState = CCamera_Manager::Get_Instance()->Get_CameraState();

	if (CamState == CS_INVEN or CamState==CS_SKILLBOOK)
	{
		return;
	}
	if (m_pGameInstance->Key_Down(DIK_7, InputChannel::UI))
	{
		m_ePreviousPhase = Phase3;
	}
	if (m_pGameInstance->Key_Down(DIK_8, InputChannel::UI))
	{
		m_ePreviousPhase = Phase_End;
		m_eCurrentPhase = Phase_Buff;
	}
	if (m_pGameInstance->Key_Down(DIK_9))
	{
		Kill();
		return;
	}

	Init_Phase(fTimeDelta);

	switch (m_eCurrentPhase)
	{
	case Client::CCescoGame::Phase1:
		if (m_fTimeLimit < 180.f)
		{
			m_eCurrentPhase = Phase_Buff;
			return;
		}
		Tick_Phase1(fTimeDelta);
		break;
	case Client::CCescoGame::Phase2:
		if (m_fTimeLimit < 90.f)
		{
			m_eCurrentPhase = Phase_Buff;
			return;
		}
		Tick_Phase2(fTimeDelta);
		break;
	case Client::CCescoGame::Phase3:
		if (m_fTimeLimit <= 0.f)
		{
			Kill();
		}
		Tick_Phase3(fTimeDelta);
		break;
	case Client::CCescoGame::Phase_Buff:
		Tick_Phase_Buff(fTimeDelta);
		return;
	}
	//½ÇÆÐ Á¶°Ç
	if (m_Monsters.size() > m_iMonsterLimit)
	{
		for (auto& pMonster : m_Monsters)
		{
			pMonster->Set_Damage(pMonster->Get_HP());
		}

		for (auto& Pair : m_Logs)
		{
			Pair.second->Set_Damage(Pair.second->Get_HP());
		}

		for (auto& Pair : m_Hives)
		{
			Pair.second->Set_Damage(Pair.second->Get_HP());
		}
	}

	for (auto& pMonster : m_Monsters)
	{
		pMonster->Tick(fTimeDelta);
	}

	_uint iNumHasPlayedSound{};
	for (auto& pMonster : m_Monsters)
	{
		if (pMonster->Get_SoundChannel() != -1)
		{
			iNumHasPlayedSound++;
		}
	}
	_bool IsPlaySound{};
	for (auto& pMonster : m_Monsters)
	{
		IsPlaySound = pMonster->Get_IsPlaySound();
		if (IsPlaySound && ++iNumHasPlayedSound <= 5)
		{
			pMonster->Play_Sound(IsPlaySound);
		}
	}

	for (auto& Pair : m_Logs)
	{
		Pair.second->Tick(fTimeDelta);
	}

	for (auto& Pair : m_Hives)
	{
		Pair.second->Tick(fTimeDelta);
	}

	Release_DeadObjects();
}

void CCescoGame::Late_Tick(_float fTimeDelta)
{
	if (m_pTimeBar)
	{
		m_pTimeBar->Late_Tick(fTimeDelta);
	}
	for (auto& pBuffCard : m_vecBuffCard)
	{
		pBuffCard->Late_Tick(fTimeDelta);
	}

	for (auto& pMonster : m_Monsters)
	{
		pMonster->Late_Tick(fTimeDelta);
	}

	for (auto& pHook : m_vecHooks)
	{
		pHook->Late_Tick(fTimeDelta);
	}

	for (auto& Pair : m_Logs)
	{
		Pair.second->Late_Tick(fTimeDelta);
	}

	for (auto& Pair : m_Hives)
	{
		Pair.second->Late_Tick(fTimeDelta);
	}
}

void CCescoGame::Init_Phase(_float fTimeDelta)
{
	if (m_eCurrentPhase != m_ePreviousPhase)
	{
		switch (m_eCurrentPhase)
		{
		case Client::CCescoGame::Phase1:
		{
			m_iNumSpawnLarva = 2;

		}
		break;
		case Client::CCescoGame::Phase2:
		{
			m_iNumSpawnLarva = 5;

			for (_uint i = 0; i < m_SpawnPositions.size(); i++)
			{
				Create_Log(i);
			}

			//¹úÁý °¹¼ö +1
			m_IsSpawnHives.push_back(false);
		}
		break;
		case Client::CCescoGame::Phase3:
		{
			m_iNumSpawnLarva = 5;

			for (_uint i = 0; i < m_SpawnPositions.size(); i++)
			{
				Create_Log(i);
			}

			//¹úÁý °¹¼ö +1
			m_IsSpawnHives.push_back(false);
		}
		break;
		case Client::CCescoGame::Phase_Buff:
		{
			m_bBuffSelected = false;
			switch (m_ePreviousPhase)
			{
			case Phase_End:
			{

				BUFFCARD_DESC Buff_Desc{};
				Buff_Desc.eBuff = Buff::Buff_MaxHp;
				Buff_Desc.vPos = _vec2(320.f, 360.f);
				CBuff_Card* pBuff = reinterpret_cast<CBuff_Card*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Buff_Card"), &Buff_Desc));
				m_vecBuffCard.push_back(pBuff);

				Buff_Desc.eBuff = Buff::Buff_CoolDown;
				Buff_Desc.vPos = _vec2(640.f, 360.f);
				pBuff = reinterpret_cast<CBuff_Card*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Buff_Card"), &Buff_Desc));
				m_vecBuffCard.push_back(pBuff);

				Buff_Desc.eBuff = Buff::Buff_PoisonImmune;
				Buff_Desc.vPos = _vec2(960.f, 360.f);
				pBuff = reinterpret_cast<CBuff_Card*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Buff_Card"), &Buff_Desc));
				m_vecBuffCard.push_back(pBuff);
				m_eNextPhase = Phase1;

			}
			break;
			case Phase1:
			{
				BUFFCARD_DESC Buff_Desc{};
				Buff_Desc.eBuff = Buff::Buff_MpRegen;
				Buff_Desc.vPos = _vec2(320.f, 360.f);
				CBuff_Card* pBuff = reinterpret_cast<CBuff_Card*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Buff_Card"), &Buff_Desc));
				m_vecBuffCard.push_back(pBuff);

				Buff_Desc.eBuff = Buff::Buff_MonRegenDown;
				Buff_Desc.vPos = _vec2(640.f, 360.f);
				pBuff = reinterpret_cast<CBuff_Card*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Buff_Card"), &Buff_Desc));
				m_vecBuffCard.push_back(pBuff);

				Buff_Desc.eBuff = Buff::Buff_Speed;
				Buff_Desc.vPos = _vec2(960.f, 360.f);
				pBuff = reinterpret_cast<CBuff_Card*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Buff_Card"), &Buff_Desc));
				m_vecBuffCard.push_back(pBuff);
				m_eNextPhase = Phase2;
			}
			break;
			case Phase2:
			{
				BUFFCARD_DESC Buff_Desc{};
				Buff_Desc.eBuff = Buff::Buff_HpRegen;
				Buff_Desc.vPos = _vec2(320.f, 360.f);
				CBuff_Card* pBuff = reinterpret_cast<CBuff_Card*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Buff_Card"), &Buff_Desc));
				m_vecBuffCard.push_back(pBuff);

				Buff_Desc.eBuff = Buff::Buff_Attack;
				Buff_Desc.vPos = _vec2(640.f, 360.f);
				pBuff = reinterpret_cast<CBuff_Card*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Buff_Card"), &Buff_Desc));
				m_vecBuffCard.push_back(pBuff);

				Buff_Desc.eBuff = Buff::Buff_BloodDrain;
				Buff_Desc.vPos = _vec2(960.f, 360.f);
				pBuff = reinterpret_cast<CBuff_Card*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Buff_Card"), &Buff_Desc));
				m_vecBuffCard.push_back(pBuff);
				m_eNextPhase = Phase3;
			}
			break;
			}
		}
		break;

		}

		m_ePreviousPhase = m_eCurrentPhase;
	}
}

void CCescoGame::Tick_Phase1(_float fTimeDelta)
{
	View_Time(fTimeDelta);
	m_fMonsterSpawnTime += fTimeDelta * m_iMonsterSpawnSpeed;

#pragma region SpawnMonster

	if (m_fMonsterSpawnTime >= 1.f)
	{
		_vec3 vSpawnPos = m_SpawnPositions[0];
		vSpawnPos.z -= 1.f;
		if (FAILED(Create_CommonMonster(TEXT("Prototype_VTFModel_Scorpion"), vSpawnPos, TEXT("Prototype_GameObject_Scorpion"))))
			return;

		vSpawnPos = m_SpawnPositions[1];
		vSpawnPos.z += 1.f;
		if (FAILED(Create_CommonMonster(TEXT("Prototype_VTFModel_Redant"), vSpawnPos, TEXT("Prototype_GameObject_RedAnt"))))
			return;

		m_iMonsterSpawnCount++;
		m_fMonsterSpawnTime = 0.f;
	}

#pragma endregion

#pragma region SpawnLarva

	if (m_iMonsterSpawnCount % 10 == 1 && m_fMonsterSpawnTime == 0.f)
	{
		for (_uint i = 0; i < m_iNumSpawnLarva; i++)
		{
			Create_Larva();
		}
	}

#pragma endregion
}

void CCescoGame::Tick_Phase2(_float fTimeDelta)
{
	Tick_Phase1(fTimeDelta);
#pragma region SpawnLog

	for (_uint i = 0; i < m_SpawnPositions.size(); i++)
	{
		auto& Pair = m_Logs.find(i);
		if (Pair == m_Logs.end())
		{
			m_fLogSpawnTimes[i] += fTimeDelta;
			if (m_fLogSpawnTimes[i] >= 30.f)
			{
				Create_Log(i);
				m_fLogSpawnTimes[i] = 0.f;
			}
		}
	}

#pragma endregion

#pragma region SpawnHive

	_uint iNumReadySpawnHive{};
	for (size_t i = 0; i < m_IsSpawnHives.size(); i++)
	{
		if (m_IsSpawnHives[i])
		{
			iNumReadySpawnHive++;
		}
	}
	if (m_Hives.size() + static_cast<size_t>(iNumReadySpawnHive) < m_IsSpawnHives.size())
	{
		_uint iNumSpawnHive = static_cast<_uint>(m_IsSpawnHives.size() - (m_Hives.size() + static_cast<size_t>(iNumReadySpawnHive)));

		while (iNumSpawnHive)
		{
			_uint iSpawnHiveIndex{};
			while (m_IsSpawnHives[iSpawnHiveIndex])
			{
				++iSpawnHiveIndex;
			}
			m_IsSpawnHives[iSpawnHiveIndex] = true;

			--iNumSpawnHive;
		}
	}

	for (size_t i = 0; i < m_IsSpawnHives.size(); i++)
	{
		if (m_IsSpawnHives[i])
		{
			m_fHiveSpawnTimes[i] += fTimeDelta;
			if (m_fHiveSpawnTimes[i] >= 8.f)
			{
				Create_Hive();
				m_IsSpawnHives[i] = false;
				m_fHiveSpawnTimes[i] = 0.f;
			}
		}
	}

#pragma endregion

#pragma region SpawnWasp

	for (size_t i = 0; i < m_HiveSpawnPositions.size(); i++)
	{
		auto& iter = m_Hives.find(i);
		if (iter != m_Hives.end())
		{
			m_fWaspSpawnTimes[i] += fTimeDelta;
			if (m_fWaspSpawnTimes[i] >= 8.f)
			{
				_vec3 vSpawnPos = m_HiveSpawnPositions[i];
				vSpawnPos.y -= 3.f;
				if (Create_CommonMonster(TEXT("Prototype_VTFModel_Wasp"), vSpawnPos, TEXT("Prototype_GameObject_Wasp")))
					return;

				m_fWaspSpawnTimes[i] = 0.f;
			}
		}
	}

#pragma endregion
}

void CCescoGame::Tick_Phase3(_float fTimeDelta)
{
	Tick_Phase2(fTimeDelta);

#pragma region EyeBombSpawn
	if (m_fEyeBombSpawnTime >= 3.5f)
	{
		_vec3 vSpawnPos = m_pPlayerTransform->Get_CenterPos();
		_randFloat RandomCountNum(-300.f, 300.f);

		_float fRandomX = RandomCountNum(m_RandomNumber);
		_float fRandomZ = RandomCountNum(m_RandomNumber);
		fRandomX /= 100.f;
		fRandomZ /= 100.f;
		vSpawnPos.x += fRandomX;
		vSpawnPos.z += fRandomZ;
		vSpawnPos.y = 0.f;
		if (FAILED(Create_CommonMonster(TEXT("Prototype_VTFModel_EyeBomb"), vSpawnPos, TEXT("Prototype_GameObject_EyeBomb"))))
		{
			return;
		}
		m_fEyeBombSpawnTime = 0.f;

	}
	m_fEyeBombSpawnTime += fTimeDelta;

#pragma endregion

#pragma region SpawnHook

	if (m_fHookSpawnTime >= 7.f)
	{
		Create_Hook();
		m_fHookSpawnTime = 0.f;
	}
	m_fHookSpawnTime += fTimeDelta;

#pragma endregion

#pragma region HookTick

	for (auto& pHook : m_vecHooks)
	{
		pHook->Tick(fTimeDelta);
	}
	m_bHadDragging = false;

	_bool bDrag{};
	_bool bCollision{};

	for (auto& pHooks : m_vecHooks)
	{
		if (pHooks->Get_Dragging())
		{
			bDrag = true;
			break;
		}
	}

	if (!bDrag)
	{
		for (auto& pHooks : m_vecHooks)
		{

			if (pHooks->Get_HadCollision())
			{
				m_pCurrent_DraggingHook = pHooks;
				pHooks->Set_Dragging(true);
				break;
			}
		}
	}

	if (m_pCurrent_DraggingHook)
	{
		if ((CUI_Manager::Get_Instance()->Get_Hp().x) > 0)
		{
			if (m_pGameInstance->Key_Down(DIK_SPACE, InputChannel::UI))
			{
				m_iDragging_EscapeCount++;
			}
			if (m_iDragging_EscapeCount >= 5)
			{
				m_iDragging_EscapeCount = 0;
				m_vecHooks.erase(remove(m_vecHooks.begin(), m_vecHooks.end(), m_pCurrent_DraggingHook), m_vecHooks.end());
				Safe_Release(m_pCurrent_DraggingHook);
				m_pCurrent_DraggingHook = nullptr;
				m_pGameInstance->Attack_Player(nullptr, 0, MonAtt_Hook_End);
				return;
			}
			m_fHookAttTime += fTimeDelta;
			if (m_fHookAttTime >= 0.5f)
			{
				m_pGameInstance->Attack_Player(nullptr, rand() % 20 + 25, MonAtt_Hook);
				m_fHookAttTime = 0.f;
			}
			m_pPlayerTransform->Set_Position(_vec3(m_pCurrent_DraggingHook->Get_Position()));
		}

	}

#pragma endregion
}

void CCescoGame::Tick_Phase_Buff(_float fTimeDelta)
{
	if (!m_bBuffSelected)
	{
		for (auto& Buffcard : m_vecBuffCard)
		{
			Buffcard->Tick(fTimeDelta);
			if (Buffcard->Get_IsSelect())
			{
				m_bBuffSelected = true;
				Buff eBuff = Buffcard->Get_Buff();
				CPlayer::PLAYER_STATUS* eState = CUI_Manager::Get_Instance()->Set_ExtraStatus();
				switch (eBuff)
				{
				case Client::Buff_MaxHp:
					eState->Max_Hp += _int(Buffcard->Get_Status());
					break;
				case Client::Buff_HpRegen:
					eState->HpRegenAmount = Buffcard->Get_Status();
					break;
				case Client::Buff_MpRegen:
					eState->MpRegenAmount = Buffcard->Get_Status();
					break;
				case Client::Buff_Attack:
					eState->Attack += _int(Buffcard->Get_Status());
					break;
				case Client::Buff_Speed:
					eState->Speed += Buffcard->Get_Status();
					break;
				case Client::Buff_CoolDown:
					eState->CoolDownTime = Buffcard->Get_Status();
					break;
				case Client::Buff_BloodDrain:
					eState->BloodDrain = Buffcard->Get_Status();
					break;
				case Client::Buff_PoisonImmune:
					eState->PoisonImmune = (_bool)Buffcard->Get_Status();
					break;
				case Client::Buff_MonRegenDown:
					m_iMonsterSpawnSpeed = 0.7f;
					break;
				}
			}

		}
	}

	if (m_bBuffSelected)
	{
		for (auto& Buffcard : m_vecBuffCard)
		{
			Buffcard->Tick(fTimeDelta);
			Buffcard->Set_Fade();
		}
	}

	_bool isEnd{};
	for (auto& Buffcard : m_vecBuffCard)
	{

		if (Buffcard->Get_IsSelectEnd())
		{
			isEnd = true;
		}
	}

	if (isEnd)
	{
		for (auto& Buffcard : m_vecBuffCard)
		{
			Safe_Release(Buffcard);
		}
		m_vecBuffCard.clear();
		m_eCurrentPhase = m_eNextPhase;
	}
}

void CCescoGame::View_Time(_float fTimeDelta)
{
	m_fTimeLimit -= fTimeDelta;
	m_iMinute = static_cast<_int>(m_fTimeLimit) / 60;
	m_iSec = static_cast<_int>(m_fTimeLimit) % 60;

	wstring strMin = to_wstring(m_iMinute);
	wstring strSec = to_wstring(m_iSec);
	wstring strText = strMin + TEXT(" : ") + strSec;
	_vec4 vColor{};

	if (m_iMinute < 1 && m_iSec <= 30)
	{
		vColor = _vec4(1.f, 0.f, 0.f, 1.f);
	}
	else
	{
		vColor = _vec4(1.f, 1.f, 1.f, 1.f);
	}
	m_pGameInstance->Render_Text(L"Font_Malang", strText, _vec2(static_cast<_float>(g_ptCenter.x), 38.f), 0.8f, vColor);

	m_pTimeBar->Set_Text(strText);

}

HRESULT CCescoGame::Create_CommonMonster(const wstring& strModelTag, _vec3 SpawnPosition, const wstring& strPrototypeTag)
{
	CVTFMonster::VTFMONSTER_DESC VTFMonsterDesc{};
	VTFMonsterDesc.strModelTag = strModelTag;
	VTFMonsterDesc.vPosition = SpawnPosition;
	VTFMonsterDesc.pPlayerTransform = m_pPlayerTransform;
	CVTFMonster* pVTFMonster = reinterpret_cast<CVTFMonster*>(m_pGameInstance->Clone_Object(strPrototypeTag, &VTFMonsterDesc));
	m_Monsters.push_back(pVTFMonster);

	return S_OK;
}

HRESULT CCescoGame::Create_Hook()
{
	CHook::HOOK_DESC HookDesc{};
	_randInt RandomDir(0, 3);
	_randInt RandomCount(1, 3);
	_randInt RandomCountNum(1, 8);


	HookDesc.WorldMatrix = _mat::CreateScale(2.f, 2.f, 1.5f);



	for (int i = 0; i < 2; i++)
	{
		_int iDirNum = RandomDir(m_RandomNumber);
		vector<int> vecHookPos;
		_int iCountNum = RandomCount(m_RandomNumber);
		_vec3 vSpawnPos = m_SpawnPositions[iDirNum];

		switch (iDirNum)
		{
		case 0:	//À§
		{
			while (iCountNum < 5)
			{
				_vec3 vHookPos = vSpawnPos;
				_int iPosNum = RandomCountNum(m_RandomNumber);
				vector<int>::iterator it = std::find(vecHookPos.begin(), vecHookPos.end(), iPosNum);
				if (it == vecHookPos.end())
				{
					vecHookPos.push_back(iPosNum);
					vHookPos.x = vSpawnPos.x - 20.f + 6.f * iPosNum;
					vHookPos.y += 1.f;
					HookDesc.WorldMatrix.Position_vec3(vHookPos);
					HookDesc.vLookat = _vec4(0.f, 0.f, -1.f, 1.f);
					CHook* pHook = dynamic_cast<CHook*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Hook"), &HookDesc));
					m_vecHooks.push_back(pHook);
					iCountNum++;
				}
			}
			break;
		}
		case 1:	//¾Æ·¡
		{
			while (iCountNum < 5)
			{
				_int iPosNum = RandomCountNum(m_RandomNumber);
				_vec3 vHookPos = vSpawnPos;
				vector<int>::iterator it = std::find(vecHookPos.begin(), vecHookPos.end(), iPosNum);
				if (it == vecHookPos.end())
				{
					vecHookPos.push_back(iPosNum);
					vHookPos.x = vSpawnPos.x - 20.f + 6.f * iPosNum;
					vHookPos.y += 1.f;
					HookDesc.WorldMatrix.Position_vec3(vHookPos);
					HookDesc.vLookat = _vec4(0.f, 0.f, 1.f, 1.f);
					CHook* pHook = dynamic_cast<CHook*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Hook"), &HookDesc));
					m_vecHooks.push_back(pHook);
					iCountNum++;
				}
			}
		}
		break;
		case 2:	//¿À¸¥ÂÊ
		{
			while (iCountNum < 5)
			{
				_int iPosNum = RandomCountNum(m_RandomNumber);
				_vec3 vHookPos = vSpawnPos;
				vector<int>::iterator it = std::find(vecHookPos.begin(), vecHookPos.end(), iPosNum);
				if (it == vecHookPos.end())
				{
					vecHookPos.push_back(iPosNum);
					vHookPos.z = vSpawnPos.z - 20.f + 5.f * iPosNum;
					vHookPos.y += 1.f;
					HookDesc.WorldMatrix.Position_vec3(vHookPos);
					HookDesc.vLookat = _vec4(-1.f, 0.f, 0.f, 1.f);
					CHook* pHook = dynamic_cast<CHook*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Hook"), &HookDesc));
					m_vecHooks.push_back(pHook);
					iCountNum++;
				}
			}
			break;
		}
		case 3:	//¿ÞÂÊ		
		{
			while (iCountNum < 5)
			{
				_int iPosNum = RandomCountNum(m_RandomNumber);
				_vec3 vHookPos = vSpawnPos;
				vector<int>::iterator it = std::find(vecHookPos.begin(), vecHookPos.end(), iPosNum);
				if (it == vecHookPos.end())
				{
					vecHookPos.push_back(iPosNum);
					vHookPos.z = vSpawnPos.z - 20.f + 6.f * iPosNum;
					vHookPos.y += 1.f;
					HookDesc.WorldMatrix.Position_vec3(vHookPos);
					HookDesc.vLookat = _vec4(1.f, 0.f, 0.f, 1.f);
					CHook* pHook = dynamic_cast<CHook*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Hook"), &HookDesc));
					m_vecHooks.push_back(pHook);
					iCountNum++;
				}
			}
			break;
		}
		}
	}

	return S_OK;
}

HRESULT CCescoGame::Create_Larva()
{
	CVTFMonster::VTFMONSTER_DESC VTFMonsterDesc{};
	VTFMonsterDesc.strModelTag = TEXT("Prototype_VTFModel_Larva");
	VTFMonsterDesc.pPlayerTransform = m_pPlayerTransform;

	_randInt RandomDir(0, 3);
	_randInt RandomPos(5, 20);
	_randInt RandomSymbol(0, 1);

	while (true)
	{
		_int iSymbol = RandomSymbol(m_RandomNumber);
		if (iSymbol == 0)
		{
			iSymbol = -1;
		}

		_vec3 vPos{};
		_bool HasPosition{};

		_int iRandom = RandomDir(m_RandomNumber);
		if (iRandom <= 1)
		{
			vPos = m_SpawnPositions[iRandom];
			vPos.x += RandomPos(m_RandomNumber) * iSymbol;
			for (auto& Pair : m_LarvaPositions)
			{
				if (Pair.second == vPos)
				{
					HasPosition = true;
					break;
				}
			}
		}
		else
		{
			vPos = m_SpawnPositions[iRandom];
			vPos.z += RandomPos(m_RandomNumber) * iSymbol;
			for (auto& Pair : m_LarvaPositions)
			{
				if (Pair.second == vPos)
				{
					HasPosition = true;
					break;
				}
			}
		}

		if (m_LarvaPositions.size() == 128)
		{
			break;
		}

		if (HasPosition)
			continue;

		VTFMonsterDesc.vPosition = vPos;
		CVTFMonster* pMonster = reinterpret_cast<CVTFMonster*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Larva"), &VTFMonsterDesc));
		m_Monsters.push_back(pMonster);
		m_LarvaPositions.emplace(pMonster->Get_ID(), vPos);
		return S_OK;
	}

	return S_OK;
}

HRESULT CCescoGame::Create_Log(_uint SpawnPositionIndex)
{
	if (SpawnPositionIndex >= m_SpawnPositions.size())
	{
		return S_OK;
	}

	CLog::LOG_DESC LogDesc{};

	LogDesc.WorldMatrix = _mat::CreateScale(3.f, 3.f, 10.f);
	if (SpawnPositionIndex <= 1)
	{
		LogDesc.WorldMatrix *= _mat::CreateRotationY(XMConvertToRadians(90.f));
	}

	_vec3 vSpawnPos = m_SpawnPositions[SpawnPositionIndex];
	vSpawnPos.y = 18.f;
	LogDesc.WorldMatrix.Position_vec3(vSpawnPos);

	CLog* pLog = reinterpret_cast<CLog*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Log"), &LogDesc));
	m_Logs.emplace(SpawnPositionIndex, pLog);

	return S_OK;

}


HRESULT CCescoGame::Create_Hive()
{
	CHive::HIVE_DESC HiveDesc{};

	while (true)
	{
		_uint iHiveSpawnPosSize = static_cast<_uint>(m_HiveSpawnPositions.size() - 1);
		_randInt RandomPosIndex(0, iHiveSpawnPosSize);

		_int PosIndex = RandomPosIndex(m_RandomNumber);
		auto iter = m_Hives.find(PosIndex);
		if (iter != m_Hives.end())
		{
			continue;
		}

		HiveDesc.vPosition = m_HiveSpawnPositions[PosIndex];

		CHive* pHive = reinterpret_cast<CHive*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Hive"), &HiveDesc));

		m_Hives.emplace(PosIndex, pHive);
		return S_OK;
	}

	return S_OK;
}

void CCescoGame::Release_DeadObjects()
{
	for (auto& it = m_Monsters.begin(); it != m_Monsters.end();)
	{
		if ((*it)->isDead())
		{
			if ((*it)->Get_ModelTag() == TEXT("Prototype_VTFModel_Larva"))
			{
				m_LarvaPositions.erase((*it)->Get_ID());
			}
			Safe_Release(*it);
			it = m_Monsters.erase(it);
		}
		else
		{
			++it;
		}
	}

	for (auto& it = m_vecHooks.begin(); it != m_vecHooks.end();)
	{
		if ((*it)->isDead())
		{
			if (m_pCurrent_DraggingHook == *it)
			{
				m_pCurrent_DraggingHook = nullptr;
				m_pGameInstance->Attack_Player(nullptr, 0, MonAtt_Hook_End);
			}
			Safe_Release(*it);
			it = m_vecHooks.erase(it);
		}
		else
		{
			++it;
		}
	}

	for (_uint i = 0; i < m_SpawnPositions.size(); i++)
	{
		auto& Pair = m_Logs.find(i);
		if (Pair == m_Logs.end())
		{
			continue;
		}

		if (Pair->second->isDead())
		{
			Safe_Release(Pair->second);
			m_Logs.erase(i);
		}
	}

	for (size_t i = 0; i < m_HiveSpawnPositions.size(); i++)
	{
		auto& Pair = m_Hives.find(i);
		if (Pair == m_Hives.end())
		{
			continue;
		}

		if (Pair->second->isDead())
		{
			Safe_Release(Pair->second);
			m_Hives.erase(i);
			m_fWaspSpawnTimes[i] = 0.f;
		}
	}
}

CCescoGame* CCescoGame::Create(_dev pDevice, _context pContext)
{
	CCescoGame* pInstance = new CCescoGame(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CCescoGame");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCescoGame::Clone(void* pArg)
{
	CCescoGame* pInstance = new CCescoGame(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CCescoGame");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCescoGame::Free()
{
	CCamera_Manager::Get_Instance()->Set_RidingZoom(false);
	__super::Free();

	for (auto& Buffcard : m_vecBuffCard)
	{
		Safe_Release(Buffcard);
	}
	m_vecBuffCard.clear();

	for (auto& pMonster : m_Monsters)
	{
		Safe_Release(pMonster);
	}
	m_Monsters.clear();

	for (auto& pHook : m_vecHooks)
	{
		Safe_Release(pHook);
	}
	m_vecHooks.clear();

	for (auto& Pair : m_Logs)
	{
		Safe_Release(Pair.second);
	}
	m_Logs.clear();

	for (auto& Pair : m_Hives)
	{
		Safe_Release(Pair.second);
	}
	m_Hives.clear();

	m_LarvaPositions.clear();

	Safe_Release(m_pTimeBar);
	Safe_Release(m_pPlayerTransform);
}
