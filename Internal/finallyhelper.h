#ifndef FINALLYHELPER_H
#define FINALLYHELPER_H

#include "InkCanvas_global.h"

#include <functional>

INKCANVAS_BEGIN_NAMESPACE

class FinallyHelper
{
public:
    FinallyHelper(std::function<void(void)> finalize) : finalize_(finalize) {}

    ~FinallyHelper() {
        finalize_();
    }

private:
    std::function<void(void)> finalize_;
};

INKCANVAS_END_NAMESPACE

#endif // FINALLYHELPER_H
