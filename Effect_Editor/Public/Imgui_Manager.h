#pragma once
#include "Effect_Define.h"
#include "Base.h"
#include "GameInstance.h"

BEGIN(Effect)

enum Effect_Type
{
	ET_PARTICLE,
	ET_RECT,
	ET_END
};

class CImgui_Manager : public CBase
{
	DECLARE_SINGLETON(CImgui_Manager)
private:
	CImgui_Manager();
	virtual ~CImgui_Manager() = default;

public:
	HRESULT Init(_dev pDevice, _context pContext, vector<string>* pTextureList);
	void Tick(_float fTimeDelta);
	HRESULT Render();

private:
	_dev m_pDevice{ nullptr };
	_context m_pContext{ nullptr };
	CGameInstance* m_pGameInstance{ nullptr };
	CVIBuffer_Rect* m_pVIBuffer_Rect{ nullptr };
	CVIBuffer_Instancing_Point* m_pVIBuffer_Instancing{ nullptr };

private:
	_int m_iCurrect_Type{};
	_int m_iSelected_Texture{};
	const _char** m_pItemList_Texture{ nullptr };
	_int m_iNumTextures{};
	CTexture** m_pTextures{ nullptr };

private:
	HRESULT Ready_Layers();

	HRESULT Load_Data();
	HRESULT Export_Data();

public:
	virtual void Free() override;
};

END