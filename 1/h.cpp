#include "h.h"

//���ߣ�HappyDay1234567890
//���ӣ�https ://www.imooc.com/article/269474
//��Դ��Ľ����
HRESULT __stdcall DXTraceW(const WCHAR* strFile, DWORD dwLine, HRESULT hr, const WCHAR* strMsg, bool bPopMsgBox) {
    WCHAR strBufferFile[MAX_PATH];    WCHAR strBufferLine[128];    WCHAR strBufferError[300];    WCHAR strBufferMsg[1024];    WCHAR strBufferHR[40];    WCHAR strBuffer[3000];

    swprintf_s(strBufferLine, 128, L"%lu", dwLine);
    if (strFile)
    {
        swprintf_s(strBuffer, 3000, L"%ls(%ls): ", strFile, strBufferLine);
        OutputDebugStringW(strBuffer);
    }
    size_t nMsgLen = (strMsg) ? wcsnlen_s(strMsg, 1024) : 0;
    if (nMsgLen > 0)
    {
        OutputDebugStringW(strMsg);
        OutputDebugStringW(L" ");
    }
    // Windows SDK 8.0��DirectX�Ĵ�����Ϣ�Ѿ����ɽ��������У�����ͨ��FormatMessageW��ȡ������Ϣ�ַ���
    // ����Ҫ�����ַ����ڴ�
    FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, nullptr, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        strBufferError, 256, nullptr);
    WCHAR* errorStr = wcsrchr(strBufferError, L'\r');
    if (errorStr)
    {
        errorStr[0] = L'\0';    // ����FormatMessageW�����Ļ��з�(��\r\n��\r�û�Ϊ\0����)
    }

    swprintf_s(strBufferHR, 40, L" (0x%0.8x)", hr);
    wcscat_s(strBufferError, strBufferHR);
    swprintf_s(strBuffer, 3000, L"�����뺬�壺%ls", strBufferError);
    OutputDebugStringW(strBuffer);

    OutputDebugStringW(L"\n");    if (bPopMsgBox)
    {
        wcscpy_s(strBufferFile, MAX_PATH, L"");
        if (strFile)
            wcscpy_s(strBufferFile, MAX_PATH, strFile);

        wcscpy_s(strBufferMsg, 1024, L"");
        if (nMsgLen > 0)
            swprintf_s(strBufferMsg, 1024, L"��ǰ���ã�%ls\n", strMsg);

        swprintf_s(strBuffer, 3000, L"�ļ�����%ls\n�кţ�%ls\n�����뺬�壺%ls\n%ls����Ҫ���Ե�ǰӦ�ó�����",
            strBufferFile, strBufferLine, strBufferError, strBufferMsg);
        int nResult = MessageBoxW(GetForegroundWindow(), strBuffer, L"����", MB_YESNO | MB_ICONERROR);
        if (nResult == IDYES)
            DebugBreak();
    }
    return hr;
}