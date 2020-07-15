#include "Internal/debug.h"

#include <assert.h>

#ifdef INKCANVAS_ANDROID
#include <android/log.h>
#endif

INKCANVAS_BEGIN_NAMESPACE

void Debug::Log(const char *message, ...)
{
#ifdef INKCANVAS_ANDROID
    va_list args;
    va_start(args, message);
    __android_log_vprint(ANDROID_LOG_WARN, "InkCanvasAndroid", message, args);
    va_end(args);
#else
    (void) message;
#endif
}

void Debug::_Assert(bool condition, char const * message)
{
#ifdef INKCANVAS_ANDROID
    __android_log_assert(message, "InkCanvasAndroid", nullptr);
#else
    assert(condition);
    (void) message;
#endif
}

INKCANVAS_END_NAMESPACE
