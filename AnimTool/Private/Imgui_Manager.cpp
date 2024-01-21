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
			m_FBXDataName.push_back(entry.path().stem().string());
		}
	}

	m_szFBXDataName = new const _char * [iNumMonsterModels];

	for (size_t i = 0; i < iNumMonsterModels; i++)
	{
		m_szFBXDataName[i] = m_FBXDataName[i].c_str();
	}

	strInputFilePath = "../../Client/Bin/EffectData/";
	_uint iNumEffects{};
	for (const auto& entry : std::filesystem::recursive_directory_iterator(strInputFilePath))
	{
		if (entry.is_regular_file())
		{
			if (entry.path().extension().string() != ".effect")
			{
				continue;
			}
			iNumEffects++;
			m_EffectNames.push_back(entry.path().stem().string());
		}
	}

	m_szEffectNames = new const _char * [iNumEffects];

	for (size_t i = 0; i < iNumEffects; i++)
	{
		m_szEffectNames[i] = m_EffectNames[i].c_str();
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
		m_pPlayer->Set_ModelType((CPlayer::TYPE)m_eType);
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

	ImGui::RadioButton("MONSTER", &m_eType, TYPE_MONSTER); ImGui::SameLine();
	ImGui::RadioButton("PLAYER", &m_eType, TYPE_PLAYER);

	if (m_eType == TYPE_MONSTER)
	{
		static const char* szCurrentModel = m_szFBXDataName[0];

		if (m_ePreType != m_eType)
		{
			m_ePreType = m_eType;
			m_iCurrentModelIndex = 0;
			szCurrentModel = m_szFBXDataName[0];
		}

		if (ImGui::BeginCombo("LIST", szCurrentModel))
		{
			for (_uint i = 0; i < m_FBXDataName.size(); i++)
			{
				_bool bSelectedModel = (szCurrentModel == m_FBXDataName[i]);
				if (ImGui::Selectable(m_szFBXDataName[i], bSelectedModel))
				{
					szCurrentModel = m_szFBXDataName[i];
					m_iCurrentModelIndex = i;
					m_iCurrentEffect = 0;
				}
			}
			ImGui::EndCombo();
		}

	}
	else if (m_eType == TYPE_PLAYER)
	{
		const char* szModelTag[4] = { "Select_Priest","Select_Rogue","Select_Sorceress","Select_Warrior" };
		static const char* szCurrentModel = "Select_Priest";
		if (m_ePreType != m_eType)
		{
			m_ePreType = m_eType;
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
					m_iCurrentEffect = 0;
				}
			}
			ImGui::EndCombo();
		}
	}

	if (m_pPlayer)
	{
		ImGui::SeparatorText("TRIGGER");

		if (ImGui::Button("SAVE"))
		{
			SaveFile();
		}
		ImGui::SameLine();
		if (ImGui::Button("LOAD"))
		{
			LoadFile();
		}

		ImGui::SeparatorText("EFFECT");

		if (ImGui::Button("ADD"))
		{	//이펙트 디스크립션 저장
			TRIGGEREFFECT_DESC EffectDesc{};
			_uint iSelectEffectFile = m_iSelectEffectFile;
			_tchar szEffectName[MAX_PATH]{};
			MultiByteToWideChar(CP_UTF8, 0, m_szEffectNames[iSelectEffectFile], (_int)strlen(m_szEffectNames[iSelectEffectFile]), szEffectName, MAX_PATH);
			EffectDesc.strEffectName = szEffectName;
			CModel* pCurModel = m_pPlayer->Get_CurrentModel();
			EffectDesc.IsFollow = true;
			EffectDesc.iStartAnimIndex = m_AnimDesc.iAnimIndex;
			pCurModel->Add_TriggerEffect(EffectDesc);
		}
		ImGui::SameLine();
		if (ImGui::Button("DELETE"))
		{
			CModel* pCurModel = m_pPlayer->Get_CurrentModel();
			pCurModel->Delete_TriggerEffect(m_iCurrentEffect);
			if (m_iCurrentEffect != 0)
			{
				m_iCurrentEffect--;
			}
		}
	}

	ImGui::SameLine();

	ImGui::End();

	//ImGuizmo
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

		_tchar szComName[MAX_PATH] = TEXT("");
		const wstring& strMonsterComName = TEXT("Com_Model_Monster%d");
		const wstring& strPlayerComName = TEXT("Com_Model_Player%d");
		if (m_eType == TYPE_MONSTER)
		{
			wsprintf(szComName, strMonsterComName.c_str(), m_iCurrentModelIndex);
		}
		else if (m_eType == TYPE_PLAYER)
		{
			wsprintf(szComName, strPlayerComName.c_str(), m_iCurrentModelIndex);
		}
		wstring strFinalComName = szComName;
		CModel* pCurrentModel = (CModel*)m_pPlayer->Find_Component(strFinalComName);
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

			_int iCurrentAnimPos = (_int)pCurrentModel->Get_CurrentAnimPos();
			iter = pAnimations.begin();
			for (_int i = 0; i < iCurrentAnimIndex; i++)
			{
				++iter;
			}

			if (ImGui::SliderInt("ANIMPOS", &iCurrentAnimPos, 0, (_int)(*iter)->Get_Duration()))
			{
				(*iter)->Set_CurrentAnimPos((_float)iCurrentAnimPos);
			}

			if (ImGui::InputInt("ANIMP0S", &iCurrentAnimPos, 1))
			{
				if (iCurrentAnimPos > (_int)(*iter)->Get_Duration())
				{
					iCurrentAnimPos = (_int)(*iter)->Get_Duration();
				}
				else if (iCurrentAnimPos < 0)
				{
					iCurrentAnimPos = 0;
				}
				(*iter)->Set_CurrentAnimPos((_float)iCurrentAnimPos);
			}

			/*if (ImGui::Button("ADD TRIGGER"))
			{
				(*iter)->Add_Trigger((*iter)->Get_CurrentAnimPos());
			}

			vector<_float> Triggers = (*iter)->Get_Triggers();
			auto Trigger = Triggers.begin();
			string* strTrigger = new string[(*iter)->Get_NumTrigger()];
			for (size_t i = 0; i < (*iter)->Get_NumTrigger(); i++)
			{
				strTrigger[i] = to_string((*Trigger));
				m_TriggerTimes.push_back(strTrigger[i].c_str());
				++Trigger;
			}
			ImGui::SameLine();
			static int iTrigger = 0;
			if (ImGui::ListBox("TRIGGER", &iTrigger, m_TriggerTimes.data(), m_TriggerTimes.size()))
			{
			}
			m_TriggerTimes.clear();
			Safe_Delete_Array(strTrigger);*/
		}

		ImGui::PopItemWidth();
		ImGui::End();
	}
	if (m_pPlayer)
	{
		CModel* pCurModel = m_pPlayer->Get_CurrentModel();
		if (pCurModel->Get_NumTriggerEffect() != 0)
		{
			//이펙트 디스크립션 이름 저장(메뉴로 보여주기 위해)
			vector<TRIGGEREFFECT_DESC> EffectDescs = pCurModel->Get_TriggerEffects();
			_char** ppEffectNameList = new _char * [EffectDescs.size()] {};

			for (size_t i = 0; i < EffectDescs.size(); i++)
			{
				ppEffectNameList[i] = new _char[MAX_PATH];
				int bufferSize = WideCharToMultiByte(CP_UTF8, 0, EffectDescs[i].strEffectName.c_str(), -1, nullptr, 0, nullptr, nullptr);
				std::string str(bufferSize, 0);
				WideCharToMultiByte(CP_UTF8, 0, EffectDescs[i].strEffectName.c_str(), -1, ppEffectNameList[i], bufferSize, nullptr, nullptr);
			}
			//
			ImGui::Begin("EFFECTDESC MENU");

			CModel* pCurModel = m_pPlayer->Get_CurrentModel();
			if (pCurModel != nullptr)
			{
				TRIGGEREFFECT_DESC* pEffectDesc = pCurModel->Get_TriggerEffect(m_iCurrentEffect);

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
					m_iCurrentBone = pEffectDesc->iBoneIndex;
					ImGui::PushItemWidth(270.f);
					if (ImGui::ListBox("BONE", &m_iCurrentBone, m_BoneNames.data(), m_BoneNames.size()))
					{
						pEffectDesc->iBoneIndex = m_iCurrentBone;
					}
					ImGui::PopItemWidth();
				}

				string strNumBones = "ALLBONES : " + to_string(iNumBones);
				ImGui::Text(strNumBones.c_str()); ImGui::SameLine();
				string strCurBone = "CURRENTBONE : " + to_string(m_iCurrentBone);
				ImGui::Text(strCurBone.c_str());


				ImGui::PushItemWidth(90.f);
				_vec3 vScale{}, vPosition{};
				vScale.x = pEffectDesc->OffsetMatrix.Right().Length();
				vScale.y = pEffectDesc->OffsetMatrix.Up().Length();
				vScale.z = pEffectDesc->OffsetMatrix.Look().Length();
				vPosition = pEffectDesc->OffsetMatrix.Position();

				ImGui::SeparatorText("SIZE");
				ImGui::InputFloat("X##1", &vScale.x, 0.1f, 0.f, "%.1f"); ImGui::SameLine();
				ImGui::InputFloat("Y##1", &vScale.y, 0.1f, 0.f, "%.1f"); ImGui::SameLine();
				ImGui::InputFloat("Z##1", &vScale.z, 0.1f, 0.f, "%.1f");
				ImGui::SeparatorText("OFFSET");
				ImGui::InputFloat("X##2", &vPosition.x, 0.1f, 0.f, "%.1f"); ImGui::SameLine();
				ImGui::InputFloat("Y##2", &vPosition.y, 0.1f, 0.f, "%.1f"); ImGui::SameLine();
				ImGui::InputFloat("Z##2", &vPosition.z, 0.1f, 0.f, "%.1f");/*
				ImGui::SeparatorText("AXIS");
				ImGui::InputFloat("X##3", &vRotation.x, 1.f, 0.f, "%.1f"); ImGui::SameLine();
				ImGui::InputFloat("Y##3", &vRotation.y, 1.f, 0.f, "%.1f"); ImGui::SameLine();
				ImGui::InputFloat("Z##3", &vRotation.z, 1.f, 0.f, "%.1f");
				ImGui::InputFloat("ANGLE", &vRotation.w, 1.f, 0.f, "%.1f");*/
				ImGui::PopItemWidth();
				pEffectDesc->OffsetMatrix.Right(pEffectDesc->OffsetMatrix.Right().Get_Normalized()* vScale.x);
				pEffectDesc->OffsetMatrix.Up(pEffectDesc->OffsetMatrix.Up().Get_Normalized()* vScale.y);
				pEffectDesc->OffsetMatrix.Look(pEffectDesc->OffsetMatrix.Look().Get_Normalized()* vScale.z);
				pEffectDesc->OffsetMatrix.Position(vPosition);
			}

			ImGui::End();

			ImGui::Begin("EFFECT MENU");
			ImGui::PushItemWidth(150.f);

			if (ImGui::ListBox("EFFECT##2", &m_iCurrentEffect, ppEffectNameList, EffectDescs.size()))
			{
			}
			//릭 제거
			for (size_t i = 0; i < EffectDescs.size(); i++)
			{
				Safe_Delete_Array(ppEffectNameList[i]);
			}
			Safe_Delete_Array(ppEffectNameList);
			//
			TRIGGEREFFECT_DESC* pEffectDesc = m_pPlayer->Get_CurrentModel()->Get_TriggerEffect(m_iCurrentEffect);
			if (ImGui::Button("START"))
			{
				if (pEffectDesc->iStartAnimIndex != pEffectDesc->iEndAnimIndex ||
					pEffectDesc->iStartAnimIndex == pEffectDesc->iEndAnimIndex &&
					pEffectDesc->fEndAnimPos > m_pPlayer->Get_CurrentAnim()->Get_CurrentAnimPos())
				{
					pEffectDesc->iStartAnimIndex = m_pPlayer->Get_CurrentModel()->Get_CurrentAnimationIndex();
					pEffectDesc->fStartAnimPos = m_pPlayer->Get_CurrentAnim()->Get_CurrentAnimPos();
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
					pEffectDesc->fEndAnimPos = m_pPlayer->Get_CurrentAnim()->Get_CurrentAnimPos();
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

			ImGui::PopItemWidth();
			ImGui::End();
		}
	}

	ImGui::Begin("EFFECT DATAFILE");
	ImGui::PushItemWidth(150.f);

	if (ImGui::ListBox("EFFECT##1", &m_iSelectEffectFile, m_szEffectNames, m_EffectNames.size()))
	{
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
				TRIGGEREFFECT_DESC* pEffectDesc = pCurModel->Get_TriggerEffect(m_iCurrentEffect);
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

	Safe_Delete_Array(m_szFBXDataName);
	Safe_Delete_Array(m_szEffectNames);

	m_EffectDescNames.clear();

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}
