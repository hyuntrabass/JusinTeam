#pragma once
#include "Base.h"

BEGIN(Engine)

enum class InputChannel
{
	Default,
	UI,
	GamePlay,
	Editor,
	End
};

class CInput_Manager final : public CBase
{
private:
	CInput_Manager();
	virtual ~CInput_Manager() = default;

public:
	_bool Key_Pressing(_uint iKey);
	_bool Key_Down(_uint iKey, InputChannel eInputChannel);
	_bool Key_Up(_uint iKey, InputChannel eInputChannel);

private:
	_bool m_bKeyState[ToIndex(InputChannel::End)][0xff]{};

public:
	static CInput_Manager* Create();
	virtual void Free() override;
};

END