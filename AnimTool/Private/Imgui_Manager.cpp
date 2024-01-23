#include "imgui.h"
#include "ImGuizmo.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

#include "Imgui_Manager.h"
#include "GameInstance.h"
#include "PipeLine.h"
#include "GameObject.h"
#include "Animation.h"
#include "Bone.h"
#include "Layer.h"
#include "Effect_Dummy.h"

#include "Player.h"

#include <commdlg.h>

CImgui_Manager::CImgui_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:m_pDevice(pDevice)
	, m_pContext(pContext)
	, m_pGameInstance(CGameInstance::Get_Instance())
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pGameInstance);
}

HRESULT CImgui_Manager::Initialize_Prototype(const GRAPHIC_DESC& GraphicDesc)
{
	m_hWnd = GraphicDesc.hWnd;
	m_iWinSizeX = GraphicDesc.iWinSizeX;
	m_iWinSizeY = GraphicDesc.iWinSizeY;
	//몬스터 모델
	string strInputFilePath = "../../Client/Bin/Resources/AnimMesh/Monster/";
	_uint iNumMonsterModels{};
	for (const auto& entry : std::filesystem::recursive_directory_iterator(strInputFilePath))
	{
		if (entry.is_regular_file())
		{
			if (entry.path().extension().string() != ".hyuntraanimmesh")
			{
				continue;
			}
			iNumMonsterModels++;
			m_MonsterNames.push_back(entry.path().stem().string());
		}
	}

	m_szMonsterNames = new const _char * [iNumMonsterModels];

	for (_uint i = 0; i < iNumMonsterModels; i++)
	{
		m_szMonsterNames[i] = m_MonsterNames[i].c_str();
	}
	//이펙트
	strInputFilePath = "../../Client/Bin/EffectData/";
	_uint iNumEffectFiles{};
	for (const auto& entry : std::filesystem::recursive_directory_iterator(strInputFilePath))
	{
		if (entry.is_regular_file())
		{
			if (entry.path().extension().string() != ".effect")
			{
				continue;
			}
			iNumEffectFiles++;
			m_EffectFiles.push_back(entry.path().stem().string());
		}
	}

	m_szEffectFiles = new const _char * [iNumEffectFiles];

	for (_uint i = 0; i < iNumEffectFiles; i++)
	{
		m_szEffectFiles[i] = m_EffectFiles[i].c_str();
	}
	//사운드
	strInputFilePath = "../../Client/Bin/Resources/Sound/";
	_uint iNumSoundFiles{};
	for (const auto& entry : std::filesystem::recursive_directory_iterator(strInputFilePath))
	{
		if (entry.is_regular_file())
		{
			iNumSoundFiles++;
			m_SoundFiles.push_back(entry.path().stem().string());
		}
	}

	m_szSoundFiles = new const _char * [iNumSoundFiles];

	for (_uint i = 0; i < iNumSoundFiles; i++)
	{
		m_szSoundFiles[i] = m_SoundFiles[i].c_str();
	}

	return S_OK;
}

void CImgui_Manager::Tick(_float fTimeDelta)
{
	GetCursorPos(&m_ptMouse);
	ScreenToClient(m_hWnd, &m_ptMouse);

#pragma region CreateObject

	if (m_IsCreateModel)
	{
		if (not m_pPlayer)
		{
			m_pPlayer = (CPlayer*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Player"));
			CTransform* pTargetTransform = (CTransform*)(m_pPlayer->Find_Component(TEXT("Com_Transform")));
			m_vPreScale = pTargetTransform->Get_Scale();
			m_vPreRight = pTargetTransform->Get_State(State::Right);
			m_vPreUp = pTargetTransform->Get_State(State::Up);
			m_vPreLook = pTargetTransform->Get_State(State::Look);
			m_vPrePosition = pTargetTransform->Get_State(State::Pos);
		}
	}
#pragma endregion

	if (m_pGameInstance->Key_Down(DIK_X))
	{
		m_fTimeDelta = 0.f;
	}
	else if (m_pGameInstance->Key_Down(DIK_C))
	{
		m_fTimeDelta = fTimeDelta;
	}

	if (m_pPlayer)
	{
		m_pPlayer->Set_ModelType((CPlayer::TYPE)m_eModelType);
		m_pPlayer->Set_ModelIndex(m_iCurrentModelIndex);
		m_pPlayer->Tick(m_fTimeDelta);
	}
}

void CImgui_Manager::Late_Tick(_float fTimeDelta)
{
	if (m_pPlayer)
	{
		m_pPlayer->Late_Tick(m_fTimeDelta);
	}
}

HRESULT CImgui_Manager::Render()
{
	bool bDemo = true;
	ImGui::ShowDemoWindow(&bDemo);
	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoTitleBar;
	window_flags |= ImGuiWindowFlags_NoMove;
	window_flags |= ImGuiWindowFlags_NoResize;

	ImGuiMenu();

	ImGuizmo::SetOrthographic(false);
	ImGuizmo::SetDrawlist(ImGui::GetBackgroundDrawList());
	ImGuizmo::SetRect(0, 0, (_float)(m_iWinSizeX), (_float)(m_iWinSizeY));
	ImGuizmoMenu();

	ImGui::Render();

	return S_OK;
}

HRESULT CImgui_Manager::ImGuiMenu()
{
#pragma endregion
	ImGui::Begin("MENU");

	ImGui::RadioButton("MONSTER", &m_eModelType, TYPE_MONSTER); ImGui::SameLine();
	ImGui::RadioButton("PLAYER", &m_eModelType, TYPE_PLAYER);

	if (m_eModelType == TYPE_MONSTER)
	{
		static const char* szCurrentModel = m_szMonsterNames[0];

		if (m_ePreModelType != m_eModelType)
		{
			m_ePreModelType = m_eModelType;
			m_iCurrentModelIndex = 0;
			szCurrentModel = m_szMonsterNames[0];
		}

		if (ImGui::BeginCombo("LIST", szCurrentModel))
		{
			for (_uint i = 0; i < m_MonsterNames.size(); i++)
			{
				_bool bSelectedModel = (szCurrentModel == m_MonsterNames[i]);
				if (ImGui::Selectable(m_szMonsterNames[i], bSelectedModel))
				{
					szCurrentModel = m_szMonsterNames[i];
					m_iCurrentModelIndex = i;
					m_iCurTriggerIndex = 0;
				}
			}
			ImGui::EndCombo();
		}

	}
	else if (m_eModelType == TYPE_PLAYER)
	{
		const char* szModelTag[4] = { "Select_Priest","Select_Rogue","Select_Sorceress","Select_Warrior" };
		static const char* szCurrentModel = "Select_Priest";
		if (m_ePreModelType != m_eModelType)
		{
			m_ePreModelType = m_eModelType;
			m_iCurrentModelIndex = 0;
			szCurrentModel = "Select_Priest";
		}

		if (ImGui::BeginCombo("LIST", szCurrentModel))
		{
			for (_uint i = 0; i < IM_ARRAYSIZE(szModelTag); i++)
			{
				_bool bSelectedModel = (szCurrentModel == szModelTag[i]);
				if (ImGui::Selectable(szModelTag[i], bSelectedModel))
				{
					szCurrentModel = szModelTag[i];
					m_iCurrentModelIndex = i;
					m_iCurTriggerIndex = 0;
				}
			}
			ImGui::EndCombo();
		}
	}

	ImGui::SeparatorText("TRIGGER");
	ImGui::RadioButton("EFFECT", &m_eTriggerType, TRIGGER_EFFECT); ImGui::SameLine();
	ImGui::RadioButton("SOUND", &m_eTriggerType, TRIGGER_SOUND);
	if (m_ePreTriggerType != m_eTriggerType)
	{
		m_ePreTriggerType = m_eTriggerType;
		m_iCurTriggerIndex = 0;
		m_iSelectFile = 0;
	}

	if (m_pPlayer)
	{
		if (ImGui::Button("SAVE##1"))
		{
			SaveFile();
		}
		ImGui::SameLine();
		if (ImGui::Button("LOAD##1"))
		{
			LoadFile();
		}

		if (ImGui::Button("ADD"))
		{	//트리거 정보 저장
			CModel* pCurModel = m_pPlayer->Get_CurrentModel();
			_uint iCurrentAnimPos = static_cast<_uint>(pCurModel->Get_CurrentAnimPos());
			if (m_eTriggerType == TRIGGER_EFFECT)
			{
				_uint iSelectEffectFile = m_iSelectFile;
				_tchar szEffectName[MAX_PATH]{};
				MultiByteToWideChar(CP_UTF8, 0, m_szEffectFiles[iSelectEffectFile], (_int)strlen(m_szEffectFiles[iSelectEffectFile]), szEffectName, MAX_PATH);

				TRIGGEREFFECT_DESC EffectDesc{};
				EffectDesc.strEffectName = szEffectName;
				EffectDesc.iStartAnimIndex = m_AnimDesc.iAnimIndex;
				EffectDesc.fStartAnimPos = static_cast<_float>(iCurrentAnimPos);
				EffectDesc.iBoneIndex = m_iCurrentBoneIndex;
				EffectDesc.IsFollow = true;
				pCurModel->Add_TriggerEffect(EffectDesc);
			}
			else if (m_eTriggerType == TRIGGER_SOUND)
			{
				_uint iSelectSoundFile = m_iSelectFile;
				_tchar szSoundName[MAX_PATH]{};
				MultiByteToWideChar(CP_UTF8, 0, m_szSoundFiles[iSelectSoundFile], (_int)strlen(m_szSoundFiles[iSelectSoundFile]), szSoundName, MAX_PATH);

				TRIGGERSOUND_DESC SoundDesc{};
				SoundDesc.strSoundName = szSoundName;
				SoundDesc.iStartAnimIndex = m_AnimDesc.iAnimIndex;
				SoundDesc.fStartAnimPos = static_cast<_float>(iCurrentAnimPos);
				SoundDesc.fVolume = 0.5f;
				pCurModel->Add_TriggerSound(SoundDesc);
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("DELETE"))
		{	//트리거 정보 삭제
			CModel* pCurModel = m_pPlayer->Get_CurrentModel();
			if (m_eTriggerType == TRIGGER_EFFECT)
			{
				pCurModel->Delete_TriggerEffect(m_iCurTriggerIndex);
				if (m_iCurTriggerIndex != 0)
				{
					m_iCurTriggerIndex--;
				}
			}
			else if (m_eTriggerType == TRIGGER_SOUND)
			{
				pCurModel->Delete_TriggerSound(m_iCurTriggerIndex);
				if (m_iCurTriggerIndex != 0)
				{
					m_iCurTriggerIndex--;
				}
			}
		}
	}

	ImGui::End();

	//ImGuizmo
#pragma region Animation Menu
	if (m_pPlayer)
	{
		ImGui::Begin("ANIMATION MENU");
		ImGui::PushItemWidth(270.f);

		ImGui::RadioButton("STATE", &m_iManipulateType, TYPE_STATE); ImGui::SameLine();
		ImGui::RadioButton("RESET", &m_iManipulateType, TYPE_RESET);
		m_eManipulateType = (MANIPULATETYPE)(m_iManipulateType);
		if (ImGui::Button("SCALE"))
		{
			if (m_eManipulateType == TYPE_STATE)
			{
				m_eStateType = ImGuizmo::OPERATION::SCALE;
			}
			else if (m_eManipulateType == TYPE_RESET)
			{
				CTransform* pTargetTransform = (CTransform*)(m_pPlayer->Find_Component(TEXT("Com_Transform")));
				pTargetTransform->Set_Scale(m_vPreScale);
				m_vCurrentScale = m_vPreScale;
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("ROTATION"))
		{
			if (m_eManipulateType == TYPE_STATE)
			{
				m_eStateType = ImGuizmo::OPERATION::ROTATE;
			}
			else if (m_eManipulateType == TYPE_RESET)
			{
				CTransform* pTargetTransform = (CTransform*)(m_pPlayer->Find_Component(TEXT("Com_Transform")));
				pTargetTransform->Set_State(State::Right, XMVector3Normalize(XMLoadFloat4(&m_vPreRight)) * m_vCurrentScale.x);
				pTargetTransform->Set_State(State::Up, XMVector3Normalize(XMLoadFloat4(&m_vPreUp)) * m_vCurrentScale.y);
				pTargetTransform->Set_State(State::Look, XMVector3Normalize(XMLoadFloat4(&m_vPreLook)) * m_vCurrentScale.z);
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("POSITION"))
		{
			if (m_eManipulateType == TYPE_STATE)
			{
				m_eStateType = ImGuizmo::OPERATION::TRANSLATE;
			}
			else if (m_eManipulateType == TYPE_RESET)
			{
				CTransform* pTargetTransform = (CTransform*)(m_pPlayer->Find_Component(TEXT("Com_Transform")));
				pTargetTransform->Set_State(State::Pos, XMLoadFloat4(&m_vPrePosition));
			}
		}
		if (m_eManipulateType == TYPE_RESET)
		{
			ImGui::SameLine();
			if (ImGui::Button("ALL"))
			{
				CTransform* pTargetTransform = (CTransform*)(m_pPlayer->Find_Component(TEXT("Com_Transform")));
				pTargetTransform->Set_State(State::Right, XMLoadFloat4(&m_vPreRight));
				pTargetTransform->Set_State(State::Up, XMLoadFloat4(&m_vPreUp));
				pTargetTransform->Set_State(State::Look, XMLoadFloat4(&m_vPreLook));
				pTargetTransform->Set_State(State::Pos, XMLoadFloat4(&m_vPrePosition));
				m_vCurrentScale = m_vPreScale;
			}
		}

		CModel* pCurrentModel = m_pPlayer->Get_CurrentModel();
		if (pCurrentModel != nullptr)
		{
			_uint iNumAnimations = pCurrentModel->Get_NumAnim();
			vector<CAnimation*> pAnimations = pCurrentModel->Get_Animations();

			m_AnimationNames.clear();

			auto iter = pAnimations.begin();
			for (_uint i = 0; i < iNumAnimations; i++)
			{
				m_AnimationNames.push_back((*iter)->Get_Name());
				++iter;
			}

			static int iCurrentAnimIndex = 0;
			if (m_AnimationNames.size() != 0)
			{
				iCurrentAnimIndex = pCurrentModel->Get_CurrentAnimationIndex();
				if (ImGui::ListBox("ANIMATION", &iCurrentAnimIndex, m_AnimationNames.data(), m_AnimationNames.size()))
				{
					m_AnimDesc.iAnimIndex = iCurrentAnimIndex;
					m_AnimDesc.bSkipInterpolation = false;
					pCurrentModel->Set_Animation(m_AnimDesc);
				}
			}

			_int iCurrentAnimPos = static_cast<_int>(pCurrentModel->Get_CurrentAnimPos());
			iter = pAnimations.begin();
			for (_int i = 0; i < iCurrentAnimIndex; i++)
			{
				++iter;
			}

			if (ImGui::SliderInt("ANIMPOS", &iCurrentAnimPos, 0, static_cast<_int>((*iter)->Get_Duration())))
			{
				(*iter)->Set_CurrentAnimPos(static_cast<_float>(iCurrentAnimPos));
			}

			if (ImGui::InputInt("ANIMP0S", &iCurrentAnimPos, 1))
			{
				if (iCurrentAnimPos > static_cast<_int>((*iter)->Get_Duration()))
				{
					iCurrentAnimPos = static_cast<_int>((*iter)->Get_Duration());
				}
				else if (iCurrentAnimPos < 0)
				{
					iCurrentAnimPos = 0;
				}
				(*iter)->Set_CurrentAnimPos(static_cast<_float>(iCurrentAnimPos));
			}
		}

		ImGui::PopItemWidth();
		ImGui::End();
	}
#pragma endregion

	if (m_pPlayer)
	{
		CModel* pCurModel = m_pPlayer->Get_CurrentModel();

#pragma region Effect_Trigger
		if (pCurModel->Get_NumTriggerEffect() != 0 && m_eTriggerType == TRIGGER_EFFECT)
		{
			ImGui::Begin("MATRIX MENU");

			CModel* pCurModel = m_pPlayer->Get_CurrentModel();
			if (pCurModel != nullptr)
			{
				TRIGGEREFFECT_DESC* pEffectDesc = pCurModel->Get_TriggerEffect(m_iCurTriggerIndex);

				_uint iNumBones = pCurModel->Get_NumBones();
				vector<CBone*> Bones = pCurModel->Get_Bones();

				m_BoneNames.clear();

				auto iter = Bones.begin();
				for (_uint i = 0; i < iNumBones; i++)
				{
					m_BoneNames.push_back((*iter)->Get_BoneName());
					++iter;
				}
				if (m_BoneNames.size() != 0)
				{
					m_iCurrentBoneIndex = pEffectDesc->iBoneIndex;
					ImGui::PushItemWidth(270.f);
					if (ImGui::ListBox("BONE", &m_iCurrentBoneIndex, m_BoneNames.data(), m_BoneNames.size()))
					{
						pEffectDesc->iBoneIndex = m_iCurrentBoneIndex;
					}
					ImGui::PopItemWidth();
				}

				string strNumBones = "ALLBONES : " + to_string(iNumBones);
				ImGui::Text(strNumBones.c_str()); ImGui::SameLine();
				string strCurBone = "CURRENTBONE : " + to_string(m_iCurrentBoneIndex);
				ImGui::Text(strCurBone.c_str());


				ImGui::PushItemWidth(90.f);
				_vec3 vScale{}, vPosition{};
				vScale.x = pEffectDesc->OffsetMatrix.Right().Length();
				vScale.y = pEffectDesc->OffsetMatrix.Up().Length();
				vScale.z = pEffectDesc->OffsetMatrix.Look().Length();
				vPosition = pEffectDesc->OffsetMatrix.Position();

				ImGui::SeparatorText("SIZE");
				ImGui::InputFloat("X##1", &vScale.x, 0.01f, 0.f, "%.2f"); ImGui::SameLine();
				ImGui::InputFloat("Y##1", &vScale.y, 0.01f, 0.f, "%.2f"); ImGui::SameLine();
				ImGui::InputFloat("Z##1", &vScale.z, 0.01f, 0.f, "%.2f");
				ImGui::SeparatorText("OFFSET");
				ImGui::InputFloat("X##2", &vPosition.x, 0.01f, 0.f, "%.2f"); ImGui::SameLine();
				ImGui::InputFloat("Y##2", &vPosition.y, 0.01f, 0.f, "%.2f"); ImGui::SameLine();
				ImGui::InputFloat("Z##2", &vPosition.z, 0.01f, 0.f, "%.2f");/*
				ImGui::SeparatorText("AXIS");
				ImGui::InputFloat("X##3", &vRotation.x, 1.f, 0.f, "%.1f"); ImGui::SameLine();
				ImGui::InputFloat("Y##3", &vRotation.y, 1.f, 0.f, "%.1f"); ImGui::SameLine();
				ImGui::InputFloat("Z##3", &vRotation.z, 1.f, 0.f, "%.1f");
				ImGui::InputFloat("ANGLE", &vRotation.w, 1.f, 0.f, "%.1f");*/
				ImGui::PopItemWidth();
				pEffectDesc->OffsetMatrix.Right(pEffectDesc->OffsetMatrix.Right().Get_Normalized() * vScale.x);
				pEffectDesc->OffsetMatrix.Up(pEffectDesc->OffsetMatrix.Up().Get_Normalized() * vScale.y);
				pEffectDesc->OffsetMatrix.Look(pEffectDesc->OffsetMatrix.Look().Get_Normalized() * vScale.z);
				pEffectDesc->OffsetMatrix.Position(vPosition);

				ImGui::SeparatorText("OFFSET");
				if (ImGui::Button("SAVE##2"))
				{
					m_OffsetMatrix = pEffectDesc->OffsetMatrix;
				}
				ImGui::SameLine();
				if (ImGui::Button("LOAD##2"))
				{
					pEffectDesc->OffsetMatrix = m_OffsetMatrix;
				}
			}

			ImGui::End();

			ImGui::Begin("TRIGGER MENU");
			ImGui::PushItemWidth(150.f);

			//이펙트 이름 띄우기
			vector<TRIGGEREFFECT_DESC> EffectDescs = pCurModel->Get_TriggerEffects();
			_char** ppEffectNameList = new _char * [EffectDescs.size()] {};

			for (size_t i = 0; i < EffectDescs.size(); i++)
			{
				ppEffectNameList[i] = new _char[MAX_PATH];
				int bufferSize = WideCharToMultiByte(CP_UTF8, 0, EffectDescs[i].strEffectName.c_str(), -1, nullptr, 0, nullptr, nullptr);
				WideCharToMultiByte(CP_UTF8, 0, EffectDescs[i].strEffectName.c_str(), -1, ppEffectNameList[i], bufferSize, nullptr, nullptr);
			}
			if (ImGui::ListBox("EFFECT##2", &m_iCurTriggerIndex, ppEffectNameList, EffectDescs.size()))
			{
			}
			//릭 제거
			for (size_t i = 0; i < EffectDescs.size(); i++)
			{
				Safe_Delete_Array(ppEffectNameList[i]);
			}
			Safe_Delete_Array(ppEffectNameList);
			//
			TRIGGEREFFECT_DESC* pEffectDesc = m_pPlayer->Get_CurrentModel()->Get_TriggerEffect(m_iCurTriggerIndex);
			if (ImGui::Button("START"))
			{
				if (pEffectDesc->iStartAnimIndex != pEffectDesc->iEndAnimIndex ||
					pEffectDesc->iStartAnimIndex == pEffectDesc->iEndAnimIndex &&
					pEffectDesc->fEndAnimPos > m_pPlayer->Get_CurrentAnim()->Get_CurrentAnimPos())
				{
					pEffectDesc->iStartAnimIndex = m_pPlayer->Get_CurrentModel()->Get_CurrentAnimationIndex();
					_uint iCurrentAnimPos = static_cast<_uint>(m_pPlayer->Get_CurrentAnim()->Get_CurrentAnimPos());
					pEffectDesc->fStartAnimPos = static_cast<_float>(iCurrentAnimPos);
				}
			}
			ImGui::SameLine();
			if (ImGui::Button("END"))
			{
				if (pEffectDesc->iStartAnimIndex != pEffectDesc->iEndAnimIndex ||
					pEffectDesc->iStartAnimIndex == pEffectDesc->iEndAnimIndex &&
					pEffectDesc->fStartAnimPos < m_pPlayer->Get_CurrentAnim()->Get_CurrentAnimPos())
				{
					pEffectDesc->iEndAnimIndex = m_pPlayer->Get_CurrentModel()->Get_CurrentAnimationIndex();
					_uint iCurrentAnimPos = static_cast<_uint>(m_pPlayer->Get_CurrentAnim()->Get_CurrentAnimPos());
					pEffectDesc->fEndAnimPos = static_cast<_float>(iCurrentAnimPos);
				}
			}

			ImGui::SeparatorText("ANIMINDEX");
			string strStartEffectIndex = "START : " + to_string(pEffectDesc->iStartAnimIndex);
			ImGui::Text(strStartEffectIndex.c_str()); ImGui::SameLine();
			string strEndEffectIndex = "END : " + to_string(pEffectDesc->iEndAnimIndex);
			ImGui::Text(strEndEffectIndex.c_str());
			ImGui::SeparatorText("ANIMPOS");
			string strStartEffectPos = "START : " + to_string(static_cast<_int>(pEffectDesc->fStartAnimPos));
			ImGui::Text(strStartEffectPos.c_str()); ImGui::SameLine();
			string strEndEffectPos = "END : " + to_string(static_cast<_int>(pEffectDesc->fEndAnimPos));
			ImGui::Text(strEndEffectPos.c_str());

			if (ImGui::Button("FOLLOW"))
			{
				if (pEffectDesc->IsFollow == false)
				{
					pEffectDesc->IsFollow = true;
				}
				else if (pEffectDesc->IsFollow == true)
				{
					pEffectDesc->IsFollow = false;
				}
			}
			ImGui::SameLine();
			if (pEffectDesc->IsFollow)
			{
				ImGui::Text("TRUE");
			}
			else
			{
				ImGui::Text("FALSE");
			}

			if (ImGui::Button("INIT ROTATE"))
			{
				if (pEffectDesc->IsInitRotateToBone == false)
				{
					pEffectDesc->IsInitRotateToBone = true;
				}
				else if (pEffectDesc->IsInitRotateToBone == true)
				{
					pEffectDesc->IsInitRotateToBone = false;
				}
			}
			ImGui::SameLine();
			if (pEffectDesc->IsInitRotateToBone)
			{
				ImGui::Text("TRUE");
			}
			else
			{
				ImGui::Text("FALSE");
			}

			if (ImGui::Button("DELETE ROTATE"))
			{
				if (pEffectDesc->IsDeleteRotateToBone == false)
				{
					pEffectDesc->IsDeleteRotateToBone = true;
				}
				else if (pEffectDesc->IsDeleteRotateToBone == true)
				{
					pEffectDesc->IsDeleteRotateToBone = false;
				}
			}
			ImGui::SameLine();
			if (pEffectDesc->IsDeleteRotateToBone)
			{
				ImGui::Text("TRUE");
			}
			else
			{
				ImGui::Text("FALSE");
			}

			ImGui::PopItemWidth();
			ImGui::End();
		}
#pragma endregion

#pragma region Sound_Trigger
		if (pCurModel->Get_NumTriggerSound() != 0 && m_eTriggerType == TRIGGER_SOUND)
		{
			ImGui::Begin("TRIGGER MENU");
			ImGui::PushItemWidth(150.f);

			CModel* pCurModel = m_pPlayer->Get_CurrentModel();

			//사운드 이름 띄우기
			vector<TRIGGERSOUND_DESC> SoundDescs = pCurModel->Get_TriggerSounds();
			_char** ppSoundNameList = new _char * [SoundDescs.size()] {};

			for (size_t i = 0; i < SoundDescs.size(); i++)
			{
				ppSoundNameList[i] = new _char[MAX_PATH];
				int bufferSize = WideCharToMultiByte(CP_UTF8, 0, SoundDescs[i].strSoundName.c_str(), -1, nullptr, 0, nullptr, nullptr);
				WideCharToMultiByte(CP_UTF8, 0, SoundDescs[i].strSoundName.c_str(), -1, ppSoundNameList[i], bufferSize, nullptr, nullptr);
			}
			if (ImGui::ListBox("EFFECT##2", &m_iCurTriggerIndex, ppSoundNameList, SoundDescs.size()))
			{
			}
			//릭 제거
			for (size_t i = 0; i < SoundDescs.size(); i++)
			{
				Safe_Delete_Array(ppSoundNameList[i]);
			}
			Safe_Delete_Array(ppSoundNameList);

			TRIGGERSOUND_DESC* pSoundDesc = m_pPlayer->Get_CurrentModel()->Get_TriggerSound(m_iCurTriggerIndex);
			if (ImGui::Button("START"))
			{
				pSoundDesc->iStartAnimIndex = m_pPlayer->Get_CurrentModel()->Get_CurrentAnimationIndex();
				_uint iCurrentAnimPos = static_cast<_uint>(m_pPlayer->Get_CurrentAnim()->Get_CurrentAnimPos());
				pSoundDesc->fStartAnimPos = static_cast<_float>(iCurrentAnimPos);
			}

			ImGui::SeparatorText("ANIMINDEX");
			string strStartSoundIndex = "START : " + to_string(pSoundDesc->iStartAnimIndex);
			ImGui::Text(strStartSoundIndex.c_str()); ImGui::SameLine();
			ImGui::SeparatorText("ANIMPOS");
			string strStartSoundPos = "START : " + to_string(static_cast<_int>(pSoundDesc->fStartAnimPos));
			ImGui::Text(strStartSoundPos.c_str()); ImGui::SameLine();

			//pSoundDesc->fVolume
			ImGui::SeparatorText("CHANNEL");
			string strSoundChannel = to_string(pSoundDesc->iChannel);
			ImGui::Text(strSoundChannel.c_str());
			ImGui::SeparatorText("VOLUME");
			string strSoundVolume = to_string(pSoundDesc->fVolume);

			ImGui::PopItemWidth();
			ImGui::End();
		}
#pragma endregion

	}

	ImGui::Begin("TRIGGER DATAFILE");
	ImGui::PushItemWidth(150.f);

	if (m_eTriggerType == TRIGGER_EFFECT)
	{
		if (ImGui::ListBox("EFFECT##1", &m_iSelectFile, m_szEffectFiles, m_EffectFiles.size()))
		{
		}
	}
	else if (m_eTriggerType == TRIGGER_SOUND)
	{
		if (ImGui::ListBox("SOUND##1", &m_iSelectFile, m_szSoundFiles, m_SoundFiles.size()))
		{

		}
	}

	ImGui::PopItemWidth();
	ImGui::End();

	return S_OK;
}

HRESULT CImgui_Manager::ImGuizmoMenu()
{
	_mat ViewMatrix = m_pGameInstance->Get_Transform(TransformType::View);
	_mat ProjMatrix = m_pGameInstance->Get_Transform(TransformType::Proj);

	if (m_pGameInstance->Key_Down(DIK_1))
	{
		m_eSelect = SELECT_PLAYER;
	}
	else if (m_pGameInstance->Key_Down(DIK_2))
	{
		m_eSelect = SELECT_EFFECT;
	}

	if (m_pPlayer && m_eSelect == SELECT_PLAYER)
	{
		CTransform* pTargetTransform = (CTransform*)(m_pPlayer->Find_Component(TEXT("Com_Transform")));
		if (pTargetTransform != nullptr)
		{
			_mat TargetMatrix = pTargetTransform->Get_World_Matrix();
			_mat PreMatrix = pTargetTransform->Get_World_Matrix();
			ImGuizmo::Manipulate(&ViewMatrix.m[0][0], &ProjMatrix.m[0][0], m_eStateType, ImGuizmo::MODE::WORLD, &TargetMatrix.m[0][0]);
			if (ImGuizmo::IsUsing())
			{
				if (m_eStateType == ImGuizmo::OPERATION::SCALE)
				{
					m_vCurrentScale = _vec3(TargetMatrix.Right().Length(),
						TargetMatrix.Up().Length(),
						TargetMatrix.Backward().Length());

					_vec4 Right = PreMatrix.Right();
					Right.Normalize();
					_vec4 Up = PreMatrix.Up();
					Up.Normalize();
					_vec4 Look = PreMatrix.Look();
					Look.Normalize();

					pTargetTransform->Set_State(State::Right, Right * m_vCurrentScale.x);
					pTargetTransform->Set_State(State::Up, Up * m_vCurrentScale.y);
					pTargetTransform->Set_State(State::Look, Look * m_vCurrentScale.z);
				}
				else
				{
					pTargetTransform->Set_State(State::Right, TargetMatrix.Right());
					pTargetTransform->Set_State(State::Up, TargetMatrix.Up());
					pTargetTransform->Set_State(State::Look, TargetMatrix.Look());
					pTargetTransform->Set_State(State::Pos, TargetMatrix.Position());
				}
			}
		}
	}

	if (m_pPlayer)
	{
		CModel* pCurModel = m_pPlayer->Get_CurrentModel();
		if (pCurModel->Get_NumTriggerEffect() != 0)
		{
			if (m_eSelect == SELECT_EFFECT)
			{
				TRIGGEREFFECT_DESC* pEffectDesc = pCurModel->Get_TriggerEffect(m_iCurTriggerIndex);
				_mat TargetMatrix = pEffectDesc->OffsetMatrix;
				_mat PreMatrix = pEffectDesc->OffsetMatrix;

				ImGuizmo::Manipulate(&ViewMatrix.m[0][0], &ProjMatrix.m[0][0], m_eStateType, ImGuizmo::MODE::WORLD, &TargetMatrix.m[0][0]);
				if (ImGuizmo::IsUsing())
				{
					if (m_eStateType == ImGuizmo::OPERATION::SCALE)
					{
						m_vCurrentScale = _vec3(TargetMatrix.Right().Length(),
							TargetMatrix.Up().Length(),
							TargetMatrix.Backward().Length());

						_vec4 Right = PreMatrix.Right();
						Right.Normalize();
						_vec4 Up = PreMatrix.Up();
						Up.Normalize();
						_vec4 Look = PreMatrix.Look();
						Look.Normalize();

						pEffectDesc->OffsetMatrix.Right(Right * m_vCurrentScale.x);
						pEffectDesc->OffsetMatrix.Up(Up * m_vCurrentScale.y);
						pEffectDesc->OffsetMatrix.Look(Look * m_vCurrentScale.z);
					}
					else
					{
						pEffectDesc->OffsetMatrix.Right(TargetMatrix.Right());
						pEffectDesc->OffsetMatrix.Up(TargetMatrix.Up());
						pEffectDesc->OffsetMatrix.Look(TargetMatrix.Look());
						pEffectDesc->OffsetMatrix.Position(TargetMatrix.Position());
					}
				}
			}
		}
	}

	return S_OK;
}


HRESULT CImgui_Manager::SaveFile()
{
	CModel* pCurrentModel = m_pPlayer->Get_CurrentModel();

	_char szFilePath[MAX_PATH] = "";
	_char szDirectory[MAX_PATH] = "";
	_char szFileName[MAX_PATH] = "";
	_splitpath_s(pCurrentModel->Get_FilePath(), nullptr, 0, szDirectory, MAX_PATH, szFileName, MAX_PATH, nullptr, 0);

	_char szExt[MAX_PATH] = ".effecttrigger";
	strcpy_s(szFilePath, MAX_PATH, szDirectory);
	strcat_s(szFilePath, MAX_PATH, szFileName);
	strcat_s(szFilePath, MAX_PATH, szExt);

	ofstream Fileout(szFilePath, ios::binary);

	if (Fileout.is_open())
	{
		CModel* pCurModel = m_pPlayer->Get_CurrentModel();
		for (_uint i = 0; i < pCurModel->Get_NumTriggerEffect(); i++)
		{
			vector<TRIGGEREFFECT_DESC> EffectDescs = pCurModel->Get_TriggerEffects();
			_uint iNumTriggerEffect = EffectDescs.size();
			Fileout.write(reinterpret_cast<_char*>(&iNumTriggerEffect), sizeof(_uint));
			for (_uint i = 0; i < iNumTriggerEffect; i++)
			{
				_int iStartAnimIndex = EffectDescs[i].iStartAnimIndex;
				Fileout.write(reinterpret_cast<_char*>(&iStartAnimIndex), sizeof(_int));
				_float fStartAnimPos = EffectDescs[i].fStartAnimPos;
				Fileout.write(reinterpret_cast<_char*>(&fStartAnimPos), sizeof(_float));
				_int iEndAnimIndex = EffectDescs[i].iEndAnimIndex;
				Fileout.write(reinterpret_cast<_char*>(&iEndAnimIndex), sizeof(_int));
				_float fEndAnimPos = EffectDescs[i].fEndAnimPos;
				Fileout.write(reinterpret_cast<_char*>(&fEndAnimPos), sizeof(_float));
				_bool IsFollow = EffectDescs[i].IsFollow;
				Fileout.write(reinterpret_cast<_char*>(&IsFollow), sizeof(_bool));

				size_t iNameSize{};
				iNameSize = (EffectDescs[i].strEffectName.size() + 1) * sizeof(_tchar);
				Fileout.write(reinterpret_cast<const _char*>(&iNameSize), sizeof size_t);
				Fileout.write(reinterpret_cast<const _char*>(EffectDescs[i].strEffectName.data()), iNameSize);

				_uint iBoneIndex = EffectDescs[i].iBoneIndex;
				Fileout.write(reinterpret_cast<_char*>(&iBoneIndex), sizeof(_uint));
				_mat OffsetMatrix = EffectDescs[i].OffsetMatrix;
				Fileout.write(reinterpret_cast<_char*>(&OffsetMatrix), sizeof(_mat));
				_bool IsInitRotateToBone = EffectDescs[i].IsInitRotateToBone;
				Fileout.write(reinterpret_cast<_char*>(&IsInitRotateToBone), sizeof(_bool));
				_bool IsDeleteRotateToBone = EffectDescs[i].IsDeleteRotateToBone;
				Fileout.write(reinterpret_cast<_char*>(&IsDeleteRotateToBone), sizeof(_bool));
			}
		}

		MessageBox(g_hWnd, L"파일 저장 완료", L"파일 저장", MB_OK);
	}

	return S_OK;
}

HRESULT CImgui_Manager::LoadFile()
{
	CModel* pCurrentModel = m_pPlayer->Get_CurrentModel();

	_char szFilePath[MAX_PATH] = "";
	_char szDirectory[MAX_PATH] = "";
	_char szFileName[MAX_PATH] = "";
	_splitpath_s(pCurrentModel->Get_FilePath(), nullptr, 0, szDirectory, MAX_PATH, szFileName, MAX_PATH, nullptr, 0);

	_char szExt[MAX_PATH] = ".effecttrigger";
	strcpy_s(szFilePath, MAX_PATH, szDirectory);
	strcat_s(szFilePath, MAX_PATH, szFileName);
	strcat_s(szFilePath, MAX_PATH, szExt);

	ifstream Filein(szFilePath, ios::binary);

	if (Filein.is_open())
	{
		CModel* pCurModel = m_pPlayer->Get_CurrentModel();

		pCurModel->Reset_TriggerEffects();

		_uint iNumTriggerEffect = { 0 };
		Filein.read(reinterpret_cast<char*>(&iNumTriggerEffect), sizeof _uint);
		for (_uint i = 0; i < iNumTriggerEffect; i++)
		{
			TRIGGEREFFECT_DESC EffectDesc{};
			Filein.read(reinterpret_cast<_char*>(&EffectDesc.iStartAnimIndex), sizeof(_int));
			Filein.read(reinterpret_cast<_char*>(&EffectDesc.fStartAnimPos), sizeof(_float));
			Filein.read(reinterpret_cast<_char*>(&EffectDesc.iEndAnimIndex), sizeof(_int));
			Filein.read(reinterpret_cast<_char*>(&EffectDesc.fEndAnimPos), sizeof(_float));
			Filein.read(reinterpret_cast<_char*>(&EffectDesc.IsFollow), sizeof(_bool));

			size_t iNameSize{};
			_tchar* pBuffer{};
			Filein.read(reinterpret_cast<_char*>(&iNameSize), sizeof size_t);
			pBuffer = new _tchar[iNameSize / sizeof(_tchar)];
			Filein.read(reinterpret_cast<_char*>(pBuffer), iNameSize);
			EffectDesc.strEffectName = pBuffer;
			Safe_Delete_Array(pBuffer);

			Filein.read(reinterpret_cast<_char*>(&EffectDesc.iBoneIndex), sizeof(_uint));
			Filein.read(reinterpret_cast<_char*>(&EffectDesc.OffsetMatrix), sizeof(_mat));
			Filein.read(reinterpret_cast<_char*>(&EffectDesc.IsInitRotateToBone), sizeof(_bool));
			Filein.read(reinterpret_cast<_char*>(&EffectDesc.IsDeleteRotateToBone), sizeof(_bool));

			pCurModel->Add_TriggerEffect(EffectDesc);
		}

		MessageBox(g_hWnd, L"파일 로드 완료", L"파일 로드", MB_OK);
	}

	return S_OK;
}

CImgui_Manager* CImgui_Manager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const GRAPHIC_DESC& GraphicDesc)
{
	CImgui_Manager* pInstance = new CImgui_Manager(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(GraphicDesc)))
	{
		MSG_BOX("Failed to Created : CImgui_Manager");
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CImgui_Manager::Free()
{
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pGameInstance);

	Safe_Release(m_pPlayer);

	Safe_Delete_Array(m_szMonsterNames);
	Safe_Delete_Array(m_szEffectFiles);
	Safe_Delete_Array(m_szSoundFiles);

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}
