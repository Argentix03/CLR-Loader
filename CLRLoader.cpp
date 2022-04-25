#define _CRT_SECURE_NO_WARNINGS
#include <metahost.h>
#include <stdio.h>
#include "atlstr.h"
#include "comdef.h"
#pragma comment(lib, "mscoree.lib")

int wmain(int argc, wchar_t** argv)
{
    // Sanity check
    if (argc < 7) {
        printf("Usage: CLRLoader.exe <vRuntime Version> <Path to CLR Assemly> <Namespace> <Class> <(public)Method> <\"Arg arg arg...\">\n");
        printf("Example: HKLM:\\SOFTWARE\\Microsoft\\NET Framework Setup\\NDP\\v4\\Full -> Verion: 4.8.0375 (or C:\\Windows\\Microsoft.NET\\Framework64\\v4.0.30319)\n");
        printf("CLRLoader.exe v4.0.30319 C:\\Users\\Public\\SpoolFool.exe SpoolFool Program CommandEntry -dll add_user.dll -printer 'My Printer'\n");
        return 0;
    }

    wchar_t* version = argv[1];
    wchar_t* filepath = argv[2];
    wchar_t* nameSpace = argv[3];
    wchar_t* className = argv[4];
    wchar_t* methodName = argv[5];
    wchar_t* arguments = argv[6];
    //wchar_t* args[20];

    // Convert arguments to char* (https://docs.microsoft.com/en-us/cpp/text/how-to-convert-between-various-string-types?view=msvc-170)
    /*
    wchar_t* orig = arguments;
    size_t origsize = wcslen(orig) + 1;
    size_t convertedChars = 0;
    char strConcat[] = " (char *)";
    size_t strConcatsize = (strlen(strConcat) + 1) * 2;
    const size_t newsize = origsize * 2;
    char* nstring = new char[newsize + strConcatsize];
    wcstombs_s(&convertedChars, nstring, newsize, orig, _TRUNCATE);
    _mbscat_s((unsigned char*)nstring, newsize + strConcatsize, (unsigned char*)strConcat);
    */

    // Parse arguments. Will probably crash if too many or too long.
    /*
    char* arg;
    arg = strtok(nstring, ",");
    int i = 0;
    while (arg != NULL) {
        args[++i] = arg;
        arg = strtok(NULL, ",");
    }
    */

    // --- Create Runtime ---
    ICLRMetaHost* CLRMetaHost = NULL;
    ICLRRuntimeInfo* CLRRuntimeInfo = NULL;
    ICLRRuntimeHost* CLRRuntimeHost = NULL;
    HRESULT hRes;

    // Create instance of CLR
    if ((hRes = CLRCreateInstance(CLSID_CLRMetaHost, IID_ICLRMetaHost, (LPVOID*)&CLRMetaHost)) != S_OK) {
        _com_error err(hRes);
        LPCTSTR errMsg = err.ErrorMessage();
        printf("[!] CLRCreateInstance()\n");
        printf("%ls\n", errMsg);
        return 0;
    }
    else {
        printf("[+] CLRCreateInstance()\n");
    }

    // Get the v4.0.30319 Runtime
    if ((hRes = CLRMetaHost->GetRuntime(version, IID_ICLRRuntimeInfo, (LPVOID*)&CLRRuntimeInfo)) != S_OK) {
        _com_error err(hRes);
        LPCTSTR errMsg = err.ErrorMessage();
        printf("[!] ICLRMetaHost.GetRuntime()\n");
        printf("%ls\n", errMsg);
        return 0;
    }
    else {
        printf("[+] ICLRRuntimeInfo.GetInterface()\n");
    }

    // Load CLR into current process
    if ((hRes = CLRRuntimeInfo->GetInterface(CLSID_CLRRuntimeHost, IID_ICLRRuntimeHost, (LPVOID*)&CLRRuntimeHost)) != S_OK) {
        _com_error err(hRes);
        LPCTSTR errMsg = err.ErrorMessage();
        printf("[!] ICLRRuntimeInfo.GetInterface()\n");
        printf("%ls\n", errMsg);
        return 0;
    }
    else {
        printf("[+] ICLRRuntimeInfo.GetInterface()\n");
    }

    // Initialise te CLR in current process
    if ((hRes = CLRRuntimeHost->Start()) != S_OK) {
        _com_error err(hRes);
        LPCTSTR errMsg = err.ErrorMessage();
        printf("[!] ICLRRuntimeHost.Start()\n");
        printf("%ls\n", errMsg);
        return 0;
    }
    else {
        printf("[+] ICLRRuntimeHost.Start()()\n");
    }

    DWORD pReturnValue;
    wchar_t pwzTypeName[40];
    wcscpy(pwzTypeName, nameSpace);
    wcscat(pwzTypeName, L".");
    wcscat(pwzTypeName, className);
    printf("Calling ExecuteInDefaultAppDomain\npwzAssemblyPath: %ls\npwzTypeName: %ls\npwzMethodName: %ls\npwzArguments: %ls\n", filepath, pwzTypeName, methodName, arguments);
    if ((hRes = CLRRuntimeHost->ExecuteInDefaultAppDomain(filepath, pwzTypeName, methodName, arguments, &pReturnValue)) != S_OK) {
        _com_error err(hRes);
        LPCTSTR errMsg = err.ErrorMessage();
        printf("[!] ICLRRuntimeHost.ExecuteInDefaultAppDomain()\n");
        printf("%ls\n", errMsg);
    }
    else {
        printf("Returned result: %d\n", pReturnValue);
    }

    // Safely Release() everything
    CLRRuntimeInfo->Release();
    CLRMetaHost->Release();
    CLRRuntimeHost->Release();
    return 0;
}