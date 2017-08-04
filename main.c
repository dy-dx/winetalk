// jordan why
// #define WIN32_LEAN_AND_MEAN
// #define VC_EXTRALEAN

#include <stdio.h>
#include <windows.h>
#include <mmsystem.h>   // includes multimedia extensions

// I guess I don't need these

// #include <string.h>     // includes string functions
// #include <commdlg.h>    // includes common dialog functionality
// #include <dlgs.h>       // includes common dialog template defines
// #include <cderr.h>      // includes the common dialog error codes
// #include <ctype.h>

static UINT (*TextToSpeechStartLang)(LPSTR lang);
static BOOL (*TextToSpeechSelectLang)(VOID *handle, UINT lang);
static MMRESULT (*TextToSpeechSync)(VOID *handle);
static MMRESULT (*TextToSpeechShutdown)(VOID *handle);
static MMRESULT (*TextToSpeechSpeak)(VOID *handle, LPSTR msg, UINT flags);
static MMRESULT (*TextToSpeechStartupEx)(
    VOID **handle,
    UINT uiDeviceNumber,
    UINT dwDeviceOptions,
    VOID (*DtCallbackRoutine)(),
    LONG dwCallbackParameter
);

static UINT speakFlagNormal = 0;
static UINT speakFlagForce = 1;

void link_that_shit_homie(HMODULE h) {
    TextToSpeechStartLang = (void *)GetProcAddress(h, "TextToSpeechStartLangA");
    TextToSpeechSelectLang = (void *)GetProcAddress(h, "TextToSpeechSelectLang");
    TextToSpeechStartupEx = (void *)GetProcAddress(h, "TextToSpeechStartupEx");
    TextToSpeechShutdown = (void *)GetProcAddress(h, "TextToSpeechShutdown");
    TextToSpeechSpeak = (void *)GetProcAddress(h, "TextToSpeechSpeakA");
    TextToSpeechSync = (void *)GetProcAddress(h, "TextToSpeechSync");
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

    HMODULE dllHandle = LoadLibrary("FonixTalk.dll");
    if (dllHandle == NULL)
    {
        exit(1);
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
    exit(0); // idk it breaks if I don't do this
    return 0;
}
