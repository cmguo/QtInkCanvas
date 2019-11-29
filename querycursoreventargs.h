#ifndef QUERYCURSOREVENTARGS_H
#define QUERYCURSOREVENTARGS_H

#include "mouseeventargs.h"

#include <QCursor>

/// <summary>
///     Provides data for the QueryCursor event.
/// </summary>
class QueryCursorEventArgs : public MouseEventArgs
{
public:
    /// <summary>
    ///     Initializes a new instance of the QueryCursorEventArgs class.
    /// </summary>
    /// <param name="mouse">
    ///     The logical Mouse device associated with this event.
    /// </param>
    /// <param name="timestamp">
    ///     The time when the input occured.
    /// </param>
    QueryCursorEventArgs(MouseDevice* mouse, int timestamp)
        : MouseEventArgs(mouse, timestamp)
    {
    }

    /// <summary>
    ///     Initializes a new instance of the QueryCursorEventArgs class.
    /// </summary>
    /// <param name="mouse">
    ///     The logical Mouse device associated with this event.
    /// </param>
    /// <param name="timestamp">
    ///     The time when the input occured.
    /// </param>
    /// <param name="stylusDevice">
    ///     The stylus pointer that was involved with this event.
    /// </param>
    QueryCursorEventArgs(MouseDevice* mouse, int timestamp, StylusDevice* stylusDevice)
        : MouseEventArgs(mouse, timestamp, stylusDevice)
    {
    }

    /// <summary>
    ///     The cursor to set.
    /// </summary>
    QCursor Cursor()
    {
        return _cursor;
    }
    void SetCursor(QCursor value)
    {
        _cursor = value;
    }

protected:
    /// <summary>
    ///     The mechanism used to call the type-specific handler on the
    ///     target.
    /// </summary>
    /// <param name="genericHandler">
    ///     The generic handler to call in a type-specific way.
    /// </param>
    /// <param name="genericTarget">
    ///     The target to call the handler on.
    /// </param>
    //virtual void InvokeEventHandler(Delegate genericHandler, object genericTarget)

private:
    QCursor _cursor;
};

#endif // QUERYCURSOREVENTARGS_H
