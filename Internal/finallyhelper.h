#ifndef FINALLYHELPER_H
#define FINALLYHELPER_H

#include <functional>

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

#endif // FINALLYHELPER_H
