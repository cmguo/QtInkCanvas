#ifndef DISPATCHER_H
#define DISPATCHER_H

#include "InkCanvas_global.h"

#include <functional>

class QThread;
class QAbstractEventDispatcher;

INKCANVAS_BEGIN_NAMESPACE

class Dispatcher
{
public:
    void VerifyAccess();

    bool CheckAccess();

    void BeginInvoke(std::function<void(void*)> func, void* data);

    static Dispatcher* from(QThread* thread);

private:
    Dispatcher(QThread* thread);
    QAbstractEventDispatcher* eventDispatche_;
};

INKCANVAS_END_NAMESPACE

#endif // DISPATCHER_H
