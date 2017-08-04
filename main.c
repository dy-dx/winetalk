
#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN

#include <stdio.h>
#include <windows.h>
#include <mmsystem.h>   // includes multimedia extensions
// required for _setmode
#include <io.h>
#include <fcntl.h>

typedef struct TTS_PHONEME_TAG
{
    DWORD dwPhoneme;
    DWORD dwPhonemeSampleNumber;
    DWORD dwPhonemeDuration;
    DWORD dwReserved;
} TTS_PHONEME_T;

typedef TTS_PHONEME_T * LPTTS_PHONEME_T;

typedef struct TTS_INDEX_TAG
{
    DWORD dwIndexValue;
    DWORD dwIndexSampleNumber;
    DWORD dwReserved;
} TTS_INDEX_T;

typedef TTS_INDEX_T * LPTTS_INDEX_T;

typedef struct TTS_BUFFER_TAG
{
    LPSTR lpData;
    LPTTS_PHONEME_T lpPhonemeArray;
    LPTTS_INDEX_T lpIndexArray;

    DWORD dwMaximumBufferLength;
    DWORD dwMaximumNumberOfPhonemeChanges;
    DWORD dwMaximumNumberOfIndexMarks;
    DWORD dwBufferLength;
    DWORD dwNumberOfPhonemeChanges;
    DWORD dwNumberOfIndexMarks;
    DWORD dwReserved;
} TTS_BUFFER_T;

typedef TTS_BUFFER_T * LPTTS_BUFFER_T;


typedef UINT (WINAPI TextToSpeechStartLangFn)(LPSTR lang);
typedef BOOL (WINAPI TextToSpeechSelectLangFn)(VOID *handle, UINT lang);
typedef MMRESULT (WINAPI TextToSpeechSyncFn)(VOID *handle);
typedef MMRESULT (WINAPI TextToSpeechShutdownFn)(VOID *handle);
typedef MMRESULT (WINAPI TextToSpeechSpeakFn)(VOID *handle, LPSTR msg, UINT flags);
typedef MMRESULT (WINAPI TextToSpeechAddBufferFn)(VOID *handle, TTS_BUFFER_T *buffer);
typedef MMRESULT (WINAPI TextToSpeechOpenInMemoryFn)(VOID *handle, UINT format);
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
static TextToSpeechAddBufferFn* TextToSpeechAddBuffer;
static TextToSpeechOpenInMemoryFn* TextToSpeechOpenInMemory;

void link_that_shit_homie(HMODULE h)
{
    TextToSpeechStartLang    = (TextToSpeechStartLangFn*)GetProcAddress(h, "TextToSpeechStartLangA");
    TextToSpeechSelectLang   = (TextToSpeechSelectLangFn*)GetProcAddress(h, "TextToSpeechSelectLang");
    TextToSpeechStartupEx    = (TextToSpeechStartupExFn*)GetProcAddress(h, "TextToSpeechStartupEx");
    TextToSpeechShutdown     = (TextToSpeechShutdownFn*)GetProcAddress(h, "TextToSpeechShutdown");
    TextToSpeechSpeak        = (TextToSpeechSpeakFn*)GetProcAddress(h, "TextToSpeechSpeakA");
    TextToSpeechSync         = (TextToSpeechSyncFn*)GetProcAddress(h, "TextToSpeechSync");
    TextToSpeechAddBuffer    = (TextToSpeechAddBufferFn*)GetProcAddress(h, "TextToSpeechAddBuffer");
    TextToSpeechOpenInMemory = (TextToSpeechOpenInMemoryFn*)GetProcAddress(h, "TextToSpeechOpenInMemory");
}

UINT UiIndexMsg;
UINT UiErrorMsg;
UINT UiBufferMsg;
UINT UiPhonemeMsg;

void register_window_messages()
{
    UiIndexMsg = RegisterWindowMessage("DECtalkIndexMessage");
    UiErrorMsg = RegisterWindowMessage("DECtalkErrorMessage");
    UiBufferMsg = RegisterWindowMessage("DECtalkBufferMessage");
    UiPhonemeMsg = RegisterWindowMessage("DECtalkVisualMessage");
}

static UINT speakFlagNormal = 0;
static UINT speakFlagForce = 1;
// signed 16bit little endian 11025hz mono PCM
static UINT WaveFormat_1M16 = 0x00000004;

UINT uiDeviceNumber = 0xFFFFFFFF;
UINT dwDeviceOptions = 0;

VOID *_handle = NULL;
TTS_BUFFER_T _buffer = {};
#define BUFFER_SIZE 16384
char daBytes[BUFFER_SIZE];

void ResetBuffer()
{
    _buffer.dwBufferLength = 0;
    _buffer.dwNumberOfPhonemeChanges = 0;
    _buffer.dwNumberOfIndexMarks = 0;
    // I don't think I need to do this but whatevs
    memset(daBytes, 0, sizeof(daBytes));
}

void TTSCallbackRoutine(LONG lParam1, LONG lParam2, DWORD dwInstanceParam, UINT uiMsg)
{
    if (uiMsg == UiBufferMsg)
    {
        UINT length = _buffer.dwBufferLength;
        if(fwrite(daBytes, sizeof(char), length, stdout) != length)
        {
            perror("fwrite");
        }
        ResetBuffer();
        TextToSpeechAddBuffer(_handle, &_buffer);
    }
}

int main(int argc, char* argv[])
{
    _setmode(_fileno(stdout), _O_BINARY);
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
    register_window_messages();

    _buffer.dwMaximumBufferLength = sizeof(daBytes);
    _buffer.lpData = daBytes;

    MMRESULT mmStatus;

    unsigned int langid = TextToSpeechStartLang("US");
    TextToSpeechSelectLang(NULL, langid);
    mmStatus = TextToSpeechStartupEx(&_handle, uiDeviceNumber, dwDeviceOptions, TTSCallbackRoutine, 0);
    mmStatus = TextToSpeechOpenInMemory(_handle, WaveFormat_1M16);
    mmStatus = TextToSpeechAddBuffer(_handle, &_buffer);
    mmStatus = TextToSpeechSpeak(_handle, msg, speakFlagForce);
    mmStatus = TextToSpeechSync(_handle);
    mmStatus = TextToSpeechShutdown(_handle);

    FreeLibrary(dllHandle);
    return 0;
}
