#pragma once

#define BEGIN(NAME) namespace NAME {
#define END			}

#ifdef ENGINE_EXPORTS
#define ENGINE_DLL _declspec(dllexport)
#else
#define ENGINE_DLL _declspec(dllimport)
#endif

// ���콺 Ŀ�� ��ġ�� ������ ������ ��ũ�η� �����.
#define GET_CURSOR_POINT(ptName)\
POINT ptName;\
GetCursorPos(&ptName);\
ScreenToClient(g_hWnd, &ptName)

#define MSG_BOX(message)								\
::MessageBox(nullptr, TEXT(message), L"error", MB_OK);	\
DebugBreak()

#define NO_COPY(ClassName)								\
ClassName(const ClassName&) = delete;					\
ClassName& operator=(const ClassName&) = delete;

#define DECLARE_SINGLETON(ClassName)					\
		NO_COPY(ClassName)								\
public :												\
	static ClassName* Get_Instance();					\
	static unsigned long Destroy_Instance();			\
private:												\
	static ClassName* m_pInstance;

#define IMPLEMENT_SINGLETON(ClassName)					\
ClassName* ClassName::m_pInstance = nullptr;			\
ClassName* ClassName::Get_Instance()					\
{														\
	if (nullptr == m_pInstance)							\
		m_pInstance = new ClassName;					\
	return m_pInstance;									\
}														\
unsigned long ClassName::Destroy_Instance()				\
{														\
	unsigned long	dwRefCnt = 0;						\
	if(nullptr != m_pInstance)							\
	{													\
		dwRefCnt = m_pInstance->Release();				\
		if(0 == dwRefCnt)								\
			m_pInstance = nullptr;						\
	}													\
	return dwRefCnt;									\
}

static XMFLOAT4 LIGHT_RANGE_7 = XMFLOAT4(7.f, 1.0f, 0.7f, 1.8f);
static XMFLOAT4 LIGHT_RANGE_13 = XMFLOAT4(13.f, 1.0f, 0.35f, 0.44f);
static XMFLOAT4 LIGHT_RANGE_20 = XMFLOAT4(20.f, 1.0f, 0.22f, 0.20f);
static XMFLOAT4 LIGHT_RANGE_32 = XMFLOAT4(32.f, 1.0f, 0.14f, 0.07f);
static XMFLOAT4 LIGHT_RANGE_50 = XMFLOAT4(50.f, 1.0f, 0.09f, 0.032f);
static XMFLOAT4 LIGHT_RANGE_65 = XMFLOAT4(65.f, 1.0f, 0.07f, 0.017f);
static XMFLOAT4 LIGHT_RANGE_100 = XMFLOAT4(100.f, 1.0f, 0.045f, 0.0075f);
static XMFLOAT4 LIGHT_RANGE_160 = XMFLOAT4(160.f, 1.0f, 0.027f, 0.0028f);
static XMFLOAT4 LIGHT_RANGE_200 = XMFLOAT4(200.f, 1.0f, 0.022f, 0.0019f);
static XMFLOAT4 LIGHT_RANGE_325 = XMFLOAT4(325.f, 1.0f, 0.014f, 0.0007f);
static XMFLOAT4 LIGHT_RANGE_600 = XMFLOAT4(600.f, 1.0f, 0.007f, 0.0002f);
static XMFLOAT4 LIGHT_RANGE_3250 = XMFLOAT4(3250.f, 1.0f, 0.0014f, 0.000007f);