#ifndef DISPATCHER_H
#define DISPATCHER_H

#include <functional>

class QThread;
class QAbstractEventDispatcher;

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

#endif // DISPATCHER_H
