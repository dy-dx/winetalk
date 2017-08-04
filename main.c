
#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN

#include <stdio.h>
#include <windows.h>
#include <mmsystem.h>   // includes multimedia extensions

typedef UINT (WINAPI TextToSpeechStartLangFn)(LPSTR lang);
typedef BOOL (WINAPI TextToSpeechSelectLangFn)(VOID *handle, UINT lang);
typedef MMRESULT (WINAPI TextToSpeechSyncFn)(VOID *handle);
typedef MMRESULT (WINAPI TextToSpeechShutdownFn)(VOID *handle);
typedef MMRESULT (WINAPI TextToSpeechSpeakFn)(VOID *handle, LPSTR msg, UINT flags);
typedef MMRESULT (WINAPI TextToSpeechStartupExFn)(
    VOID **handle,
    UINT uiDeviceNumber,
    UINT dwDeviceOptions,
    VOID (*DtCallbackRoutine)(),
    LONG dwCallbackParameter
);

static TextToSpeechStartLangFn* TextToSpeechStartLang;
static TextToSpeechSelectLangFn* TextToSpeechSelectLang;
static TextToSpeechStartupExFn* TextToSpeechStartupEx;
static TextToSpeechShutdownFn* TextToSpeechShutdown;
static TextToSpeechSpeakFn* TextToSpeechSpeak;
static TextToSpeechSyncFn* TextToSpeechSync;

static UINT speakFlagNormal = 0;
static UINT speakFlagForce = 1;

void link_that_shit_homie(HMODULE h) {
    TextToSpeechStartLang  = (TextToSpeechStartLangFn*)GetProcAddress(h, "TextToSpeechStartLangA");
    TextToSpeechSelectLang = (TextToSpeechSelectLangFn*)GetProcAddress(h, "TextToSpeechSelectLang");
    TextToSpeechStartupEx  = (TextToSpeechStartupExFn*)GetProcAddress(h, "TextToSpeechStartupEx");
    TextToSpeechShutdown   = (TextToSpeechShutdownFn*)GetProcAddress(h, "TextToSpeechShutdown");
    TextToSpeechSpeak      = (TextToSpeechSpeakFn*)GetProcAddress(h, "TextToSpeechSpeakA");
    TextToSpeechSync       = (TextToSpeechSyncFn*)GetProcAddress(h, "TextToSpeechSync");
}

// might need this later
void TTSCallbackRoutine(LONG lParam1, LONG lParam2, DWORD dwInstanceParam, UINT uiMsg)
{
}

int main(int argc, char* argv[])
{
    char *msg = "hello";
    if (argc >= 2)
    {
        msg = argv[1];
    }

    HMODULE dllHandle = LoadLibrary(TEXT("FonixTalk.dll"));
    if (dllHandle == NULL)
    {
        return 1;
    }

    link_that_shit_homie(dllHandle);
    VOID *_handle = NULL;
    UINT uiDeviceNumber = 0xFFFFFFFF;
    UINT dwDeviceOptions = 0;
    MMRESULT mmStatus;

    unsigned int langid = TextToSpeechStartLang("US");
    TextToSpeechSelectLang(NULL, langid);

    mmStatus = TextToSpeechStartupEx(&_handle, uiDeviceNumber, dwDeviceOptions, NULL, 0);

    mmStatus = TextToSpeechSpeak(_handle, msg, speakFlagForce);

    mmStatus = TextToSpeechSync(_handle);
    mmStatus = TextToSpeechShutdown(_handle);

    FreeLibrary(dllHandle);
    return 0;
}
