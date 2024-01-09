#pragma once
#include "Base.h"

BEGIN(Engine)

class CMyFont final : public CBase
{
private:
	CMyFont(_dev pDevice, _context pContext);
	virtual ~CMyFont() = default;

public:
	HRESULT Init(const wstring& strFilePath);
	HRESULT Render(const wstring& strText, const _float2& vPosition, _float fScale = 1.f, _fvector vColor = Colors::White, _float fRotation = 0.f);

private:
	_dev m_pDevice{ nullptr };
	_context m_pContext{ nullptr };

	SpriteBatch* m_pBatch{ nullptr };
	SpriteFont* m_pFont{ nullptr };

public:
	static CMyFont* Create(_dev pDevice, _context pContext, const wstring& strFilePath);
	virtual void Free() override;
};

END