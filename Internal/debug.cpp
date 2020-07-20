#include "Internal/debug.h"

#include <assert.h>

#ifdef INKCANVAS_ANDROID
#include <android/log.h>
#elif defined INKCANVAS_IOS || defined INKCANVAS_MACOS
#include <stdarg.h>
extern "C" void NS_Log(char const * t, char const * m, va_list args);
#endif

#define _STRING(x) #x
#define STRING(x) _STRING(x)

INKCANVAS_BEGIN_NAMESPACE

void Debug::Log(const char *message, ...)
{
#ifdef INKCANVAS_ANDROID
    va_list args;
    va_start(args, message);
    __android_log_vprint(ANDROID_LOG_WARN, STRING(INKCANVAS_NAMESPACE), message, args);
    va_end(args);
#elif defined INKCANVAS_IOS || defined INKCANVAS_MACOS
    va_list args;
    va_start(args, message);
    NS_Log(STRING(INKCANVAS_NAMESPACE), message, args);
    va_end(args);
#else
    (void) message;
#endif
}

void Debug::_Assert(bool condition, char const * message)
{
    if (condition)
        return;
#ifdef INKCANVAS_ANDROID
    __android_log_assert(message, "InkCanvasAndroid", nullptr);
#else
    assert(condition);
    (void) message;
#endif
}

INKCANVAS_END_NAMESPACE
