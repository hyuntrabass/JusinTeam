#include "CescoGame.h"
#include "VTFMonster.h"
#include "Log.h"
#include "Hook.h"

CCescoGame::CCescoGame(_dev pDevice, _context pContext)
	:CGameObject(pDevice, pContext)
{
}

CCescoGame::CCescoGame(const CCescoGame& rhs)
	:CGameObject(rhs)
	, m_pPlayerTransform(rhs.m_pPlayerTransform)
{
}

HRESULT CCescoGame::Init_Prototype()
{
	m_pPlayerTransform = GET_TRANSFORM("Layer_Player", LEVEL_STATIC);
	Safe_AddRef(m_pPlayerTransform);

	return S_OK;
}

HRESULT CCescoGame::Init(void* pArg)
{
	//UP
	m_SpawnPositions.push_back(_vec3(-3000.f, 1.f, 30.f));
	//DOWN
	m_SpawnPositions.push_back(_vec3(-3000.f, 1.f, -30.f));
	//RIGHT
	m_SpawnPositions.push_back(_vec3(-2970.f, 1.f, 0.f));
	//LEFT
	m_SpawnPositions.push_back(_vec3(-3030.f, 1.f, 0.f));

	random_device rand;
	m_RandomNumber = _randNum(rand());

	CLog::LOG_DESC LogDesc{};
	CHook::HOOK_DESC HookDesc{};
	_randInt RandomDir(1, 4);

	for (_uint i = 0; i < m_SpawnPositions.size(); i++)
	{
		HookDesc.WorldMatrix = _mat::CreateScale(1.f, 2.f, 1.f);
		_vec3 vSpawnPos = m_SpawnPositions[i];
		HookDesc.WorldMatrix.Position_vec3(vSpawnPos);
		if (FAILED(m_pGameInstance->Add_Layer(m_pGameInstance->Get_CurrentLevelIndex(), TEXT("Layer_Hook"), TEXT("Prototype_GameObject_Hook_Object"), &HookDesc)))
		{
			return E_FAIL;
		}
	}
	for (_uint i = 0; i < m_SpawnPositions.size(); i++)
	{
		LogDesc.WorldMatrix = _mat::CreateScale(3.f, 3.f, 10.f);
		if (i <= 1)
		{
			LogDesc.WorldMatrix *= _mat::CreateRotationY(XMConvertToRadians(90.f));
		}
		_vec3 vSpawnPos = m_SpawnPositions[i];

		vSpawnPos.y = 18.f;
		LogDesc.WorldMatrix.Position_vec3(vSpawnPos);

		if (FAILED(m_pGameInstance->Add_Layer(m_pGameInstance->Get_CurrentLevelIndex(), TEXT("Layer_Log"), TEXT("Prototype_GameObject_Log_Object"), &LogDesc)))
		{
			return E_FAIL;
		}
	}



	return S_OK;
}

void CCescoGame::Tick(_float fTimeDelta)
{
	if (m_pGameInstance->Key_Down(DIK_9))
	{
		m_fTimeLimit = 0.f;
	}

	m_fTimeLimit -= fTimeDelta;

	if (m_fTimeLimit <= -1.f)
	{
		Kill();
		return;
	}

	m_fMonsterSpawnTime += fTimeDelta;

#pragma region SpawnMonster

	if (m_fMonsterSpawnTime >= 1.f)
	{
		CVTFMonster::VTFMONSTER_DESC VTFMonsterDesc{};
		VTFMonsterDesc.strModelTag = TEXT("Prototype_VTFModel_Scorpion");
		VTFMonsterDesc.vPosition = m_SpawnPositions[0];
		VTFMonsterDesc.pPlayerTransform = m_pPlayerTransform;
		CVTFMonster* pScorpion = reinterpret_cast<CVTFMonster*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Scorpion_Object"), &VTFMonsterDesc));
		m_Monsters.push_back(pScorpion);

		VTFMonsterDesc = {};
		VTFMonsterDesc.strModelTag = TEXT("Prototype_VTFModel_Redant");
		VTFMonsterDesc.vPosition = m_SpawnPositions[1];
		VTFMonsterDesc.pPlayerTransform = m_pPlayerTransform;
		CVTFMonster* pRedAnt = reinterpret_cast<CVTFMonster*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_RedAnt_Object"), &VTFMonsterDesc));
		m_Monsters.push_back(pRedAnt);

		m_iMonsterSpawnCount++;
		m_fMonsterSpawnTime = 0.f;
	}

#pragma endregion

#pragma region SpawnVoid19

	if (m_iMonsterSpawnCount % 10 == 1 && m_fMonsterSpawnTime == 0.f)
	{
		CVTFMonster::VTFMONSTER_DESC VTFMonsterDesc{};
		VTFMonsterDesc.strModelTag = TEXT("Prototype_VTFModel_Larva");
		VTFMonsterDesc.pPlayerTransform = m_pPlayerTransform;

		_int iNumSpawn{};
		_randInt RandomDir(0, 3);
		_randInt RandomPos(5, 20);
		_randInt RandomSymbol(0, 1);

		while (iNumSpawn < 5)
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
						continue;
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
						continue;
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
			CVTFMonster* pMonster = reinterpret_cast<CVTFMonster*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Larva_Object"), &VTFMonsterDesc));
			m_Monsters.push_back(pMonster);
			m_LarvaPositions.emplace(pMonster->Get_ID(), vPos);
			iNumSpawn++;
		}
	}

#pragma endregion
	//실패 조건
	if (m_Monsters.size() > m_iMonsterLimit || m_fTimeLimit <= 0.f)
	{
		for (auto& pMonster : m_Monsters)
		{
			pMonster->Set_Damage(pMonster->Get_HP());
		}
	}

	for (auto& pMonster : m_Monsters)
	{
		pMonster->Tick(fTimeDelta);
	}

	Release_DeadObjects();
}

void CCescoGame::Late_Tick(_float fTimeDelta)
{
	for (auto& pMonster : m_Monsters)
	{
		pMonster->Late_Tick(fTimeDelta);
	}
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
	__super::Free();

	for (auto& pMonster : m_Monsters)
	{
		Safe_Release(pMonster);
	}
	m_Monsters.clear();

	m_LarvaPositions.clear();

	Safe_Release(m_pPlayerTransform);
}
