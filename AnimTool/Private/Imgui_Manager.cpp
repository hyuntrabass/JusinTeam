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
	_char* szFileName = nullptr;
	for (const auto& entry : std::filesystem::recursive_directory_iterator(strInputFilePath))
	{
		if (entry.is_regular_file())
		{
			if (entry.path().extension().string() != ".hyuntraanimmesh")
			{
				continue;
			}
			szFileName = new _char[MAX_PATH];
			_splitpath_s(entry.path().string().c_str(), nullptr, 0, nullptr, 0, szFileName, MAX_PATH, nullptr, 0);
			iNumMonsterModels++;
			m_FBXDataName.push_back(szFileName);
		}
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
			szFileName = new _char[MAX_PATH];
			_splitpath_s(entry.path().string().c_str(), nullptr, 0, nullptr, 0, szFileName, MAX_PATH, nullptr, 0);
			iNumEffects++;
			m_EffectNames.push_back(szFileName);
		}
	}

	return S_OK;
}

void CImgui_Manager::Tick(_float fTimeDelta)
{
	GetCursorPos(&m_ptMouse);
	ScreenToClient(m_hWnd, &m_ptMouse);

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
		m_pPlayer->Tick(m_fTimeDelta);
	}

	if (not m_Effects.empty())
	{
		for (auto& iter = m_Effects.begin(); iter != m_Effects.end();)
		{
			(*iter)->Tick(m_fTimeDelta);
			if ((*iter)->isDead())
			{
				Safe_Release((*iter));
				iter = m_Effects.erase(iter);
			}
			else
			{
				++iter;
			}
		}
	}
}

void CImgui_Manager::Late_Tick(_float fTimeDelta)
{
	if (m_pPlayer)
	{
		m_pPlayer->Late_Tick(m_fTimeDelta);
	}

	if (not m_Effects.empty())
	{
		for (auto& pEffect : m_Effects)
			pEffect->Late_Tick(m_fTimeDelta);
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
		static const char* szCurrentModel = m_FBXDataName[0];

		if (m_ePreType != m_eType)
		{
			m_ePreType = m_eType;
			m_iCurrentModelIndex = 0;
			szCurrentModel = m_FBXDataName[0];
		}

		if (ImGui::BeginCombo("LIST", szCurrentModel))
		{
			for (_uint i = 0; i < m_FBXDataName.size(); i++)
			{
				_bool bSelectedModel = (szCurrentModel == m_FBXDataName[i]);
				if (ImGui::Selectable(m_FBXDataName[i], bSelectedModel))
				{
					szCurrentModel = m_FBXDataName[i];
					m_iCurrentModelIndex = i;
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
				}
			}
			ImGui::EndCombo();
		}
	}

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
 		MultiByteToWideChar(CP_UTF8, 0, m_EffectNames[iSelectEffectFile], (_int)strlen(m_EffectNames[iSelectEffectFile]), szEffectName, MAX_PATH);
		EffectDesc.strEffectName = szEffectName;
		CAnimation* pCurrentAnim = m_pPlayer->Get_CurrentAnim();
		pCurrentAnim->Add_TriggerEffect(EffectDesc);
		//이펙트 생성
		EffectInfo EffectInfo = CEffect_Manager::Get_Instance()->Get_EffectInformation(szEffectName);
		CEffect_Dummy* pEffect = CEffect_Manager::Get_Instance()->Clone_Effect(&EffectInfo);
		m_Effects.push_back(pEffect);
		//이펙트 디스크립션 이름 저장(메뉴로 보여주기 위해)
		m_EffectDescNames.push_back(m_EffectNames[iSelectEffectFile]);
	}
	ImGui::SameLine();
	if (ImGui::Button("DELETE"))
	{

	}

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
	if (not m_Effects.empty())
	{
		ImGui::Begin("EFFECTDESC MENU");

		CModel* pCurrentModel = m_pPlayer->Get_CurrentModel();
		if (pCurrentModel != nullptr)
		{
			_uint iNumBones = pCurrentModel->Get_NumBones();
			vector<CBone*> Bones = pCurrentModel->Get_Bones();

			m_BoneNames.clear();

			auto iter = Bones.begin();
			for (_uint i = 0; i < iNumBones; i++)
			{
				m_BoneNames.push_back((*iter)->Get_BoneName());
				++iter;
			}
			if (m_BoneNames.size() != 0)
			{
				CAnimation* pCurAnim = m_pPlayer->Get_CurrentAnim();
				TRIGGEREFFECT_DESC* pEffectDesc = pCurAnim->Get_TriggerEffect(m_iCurrentEffect);
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

			CAnimation* pCurAnim = m_pPlayer->Get_CurrentAnim();
			CTransform* pPlayerTransform = reinterpret_cast<CTransform*>(m_pPlayer->Find_Component(TEXT("Com_Transform")));

			ImGui::PushItemWidth(90.f);
			TRIGGEREFFECT_DESC* pEffectDesc = pCurAnim->Get_TriggerEffect(m_iCurrentEffect);
			ImGui::SeparatorText("OFFSET");
			ImGui::InputFloat("X##1", &pEffectDesc->vPosOffset.x, 0.1f, 0.f, "%.1f"); ImGui::SameLine();
			ImGui::InputFloat("Y##1", &pEffectDesc->vPosOffset.y, 0.1f, 0.f, "%.1f"); ImGui::SameLine();
			ImGui::InputFloat("Z##1", &pEffectDesc->vPosOffset.z, 0.1f, 0.f, "%.1f");
			ImGui::SeparatorText("AXIS");
			ImGui::InputFloat("X##2", &pEffectDesc->vRotationAxis.x, 1.f, 0.f, "%.1f"); ImGui::SameLine();
			ImGui::InputFloat("Y##2", &pEffectDesc->vRotationAxis.y, 1.f, 0.f, "%.1f"); ImGui::SameLine();
			ImGui::InputFloat("Z##2", &pEffectDesc->vRotationAxis.z, 1.f, 0.f, "%.1f");
			ImGui::InputFloat("ANGLE", &pEffectDesc->fAngle, 1.f, 0.f, "%.1f");
			ImGui::SeparatorText("SIZE");
			ImGui::InputFloat("X##3", &pEffectDesc->vSize.x, 0.1f, 0.f, "%.1f"); ImGui::SameLine();
			ImGui::InputFloat("Y##3", &pEffectDesc->vSize.y, 0.1f, 0.f, "%.1f"); ImGui::SameLine();
			ImGui::InputFloat("Z##3", &pEffectDesc->vSize.z, 0.1f, 0.f, "%.1f");
			ImGui::PopItemWidth();

			_uint iEffectIndex{};
			for (auto& pEffect : m_Effects)
			{
				pEffectDesc = pCurAnim->Get_TriggerEffect(iEffectIndex++);
				_mat WorldMatrix = _mat::CreateFromAxisAngle(pEffectDesc->vRotationAxis, XMConvertToRadians(pEffectDesc->fAngle)) * *Bones[pEffectDesc->iBoneIndex]->Get_CombinedMatrix() * pCurrentModel->Get_PivotMatrix() * pPlayerTransform->Get_World_Matrix();
				WorldMatrix.Position() += pEffectDesc->vPosOffset;
				pEffect->Set_WorldMatrix(WorldMatrix);
			}
		}

		ImGui::End();
	}

	ImGui::Begin("EFFECT DATAFILE");
	ImGui::PushItemWidth(150.f);

	if (ImGui::ListBox("EFFECT##1", &m_iSelectEffectFile, m_EffectNames.data(), m_EffectNames.size()))
	{
	}

	ImGui::PopItemWidth();
	ImGui::End();

	if (not m_Effects.empty())
	{
		ImGui::Begin("EFFECT MENU");
		ImGui::PushItemWidth(150.f);

		if (ImGui::ListBox("EFFECT##2", &m_iCurrentEffect, m_EffectDescNames.data(), m_EffectDescNames.size()))
		{
		}

		TRIGGEREFFECT_DESC* pEffectDesc = m_pPlayer->Get_CurrentAnim()->Get_TriggerEffect(m_iCurrentEffect);
		if (ImGui::Button("START"))
		{
			if (pEffectDesc->fEndAnimPos > m_pPlayer->Get_CurrentAnim()->Get_CurrentAnimPos() ||
				pEffectDesc->fEndAnimPos == -1.f)
			{
				pEffectDesc->fStartAnimPos = m_pPlayer->Get_CurrentAnim()->Get_CurrentAnimPos();
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("END"))
		{
			if (pEffectDesc->fStartAnimPos < m_pPlayer->Get_CurrentAnim()->Get_CurrentAnimPos())
			{
				pEffectDesc->fEndAnimPos = m_pPlayer->Get_CurrentAnim()->Get_CurrentAnimPos();
			}
		}

		string strStartEffect = "START : " + to_string(static_cast<_int>(pEffectDesc->fStartAnimPos));
		ImGui::Text(strStartEffect.c_str()); ImGui::SameLine();
		string strEndEffect = "END : " + to_string(static_cast<_int>(pEffectDesc->fEndAnimPos));
		ImGui::Text(strEndEffect.c_str());

		ImGui::PopItemWidth();
		ImGui::End();
	}

	//
	if (m_pPlayer)
	{
		m_pPlayer->Set_ModelType((CPlayer::TYPE)m_eType);
		m_pPlayer->Set_ModelIndex(m_iCurrentModelIndex);
	}

	return S_OK;
}

HRESULT CImgui_Manager::ImGuizmoMenu()
{
	_mat ViewMatrix = m_pGameInstance->Get_Transform(TransformType::View);
	_mat ProjMatrix = m_pGameInstance->Get_Transform(TransformType::Proj);

	if (m_pPlayer)
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
		vector<CAnimation*> pAnimations = pCurrentModel->Get_Animations();
		auto iter = pAnimations.begin();
		for (_uint i = 0; i < pCurrentModel->Get_NumAnim(); i++)
		{
			if ((*iter)->Get_NumEffectTrigger() != 0)
			{
				_uint iAnimIndex = i;
				Fileout.write(reinterpret_cast<char*>(&iAnimIndex), sizeof _uint);
				vector<TRIGGEREFFECT_DESC> EffectDescs = (*iter)->Get_TriggerEffects();
				_uint iNumEffectTrigger = EffectDescs.size();
				Fileout.write(reinterpret_cast<_char*>(&iNumEffectTrigger), sizeof(_uint));
				for (_uint i = 0; i < iNumEffectTrigger; i++)
				{
					_float fStartAnimPos = EffectDescs[i].fStartAnimPos;
					Fileout.write(reinterpret_cast<_char*>(&fStartAnimPos), sizeof(_float));
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
					_vec3 vPosOffset = EffectDescs[i].vPosOffset;
					Fileout.write(reinterpret_cast<_char*>(&vPosOffset), sizeof(_vec3));
					_vec3 vRotationAxis = EffectDescs[i].vRotationAxis;
					Fileout.write(reinterpret_cast<_char*>(&vRotationAxis), sizeof(_vec3));
					_float fAngle = EffectDescs[i].fAngle;
					Fileout.write(reinterpret_cast<_char*>(&fAngle), sizeof(_float));
					_vec3 vSize = EffectDescs[i].vSize;
					Fileout.write(reinterpret_cast<_char*>(&vSize), sizeof(_vec3));
				}
			}
			++iter;
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
		vector<CAnimation*> pAnimations = pCurrentModel->Get_Animations();
		auto iter = pAnimations.begin();

		for (_uint i = 0; i < pCurrentModel->Get_NumAnim(); i++)
		{
			(*iter)->Reset_TriggerEffects();
			++iter;
		}

		iter = pAnimations.begin();
		_uint iAnimIndex = { 0 };
		Filein.read(reinterpret_cast<char*>(&iAnimIndex), sizeof _uint);
		for (_uint i = 0; i < iAnimIndex; i++)
		{
			++iter;
		}
		_uint iNumEffectTrigger = { 0 };
		Filein.read(reinterpret_cast<char*>(&iNumEffectTrigger), sizeof _uint);
		for (_uint i = 0; i < iNumEffectTrigger; i++)
		{
			TRIGGEREFFECT_DESC EffectDesc{};
			Filein.read(reinterpret_cast<_char*>(&EffectDesc.fStartAnimPos), sizeof(_float));
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
			Filein.read(reinterpret_cast<_char*>(&EffectDesc.vPosOffset), sizeof(_vec3));
			Filein.read(reinterpret_cast<_char*>(&EffectDesc.vRotationAxis), sizeof(_vec3));
			Filein.read(reinterpret_cast<_char*>(&EffectDesc.fAngle), sizeof(_float));
			Filein.read(reinterpret_cast<_char*>(&EffectDesc.vSize), sizeof(_vec3));
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

	for (size_t i = 0; i < m_Effects.size(); i++)
	{
		Safe_Release(m_Effects[i]);
	}
	m_Effects.clear();

	for (auto& MonsterNames : m_FBXDataName)
	{
		Safe_Delete_Array(MonsterNames);
	}

	for (auto& EffectName : m_EffectNames)
	{
		Safe_Delete_Array(EffectName);
	}

	m_EffectDescNames.clear();

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}
