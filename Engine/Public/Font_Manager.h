#pragma once
#include "Base.h"

BEGIN(Engine)

class CFont_Manager final : public CBase
{
private:
	CFont_Manager(_dev pDevice, _context pContext);
	virtual ~CFont_Manager() = default;

public:
	HRESULT Init();
	HRESULT Add_Font(const wstring& strFontTag, const wstring& strFilePath);
	HRESULT Render(const wstring& strFontTag, const wstring& strText, const _float2& vPosition, _float fScale = 1.f, _fvector vColor = Colors::White, _float fRotation = 0.f);

private:
	_dev m_pDevice{ nullptr };
	_context m_pContext{ nullptr };

private:
	map<const wstring, class CMyFont*> m_Fonts{};

private:
	class CMyFont* Find_Font(const wstring& strFontTag);

public:
	static CFont_Manager* Create(_dev pDevice, _context pContext);
	virtual void Free() override;
};

END