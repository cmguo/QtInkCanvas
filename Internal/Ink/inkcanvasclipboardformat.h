#ifndef INKCANVASCLIPBOARDFORMAT_H
#define INKCANVASCLIPBOARDFORMAT_H

#include <QFlags>

// namespace System.Windows.Controls

enum class InkCanvasClipboardFormat
{
    /// <summary>
    /// Ink Serialized Format
    /// </summary>
    InkSerializedFormat = 0,
    /// <summary>
    /// Text Format
    /// </summary>
    Text,
    /// <summary>
    /// Xaml Format
    /// </summary>
    Xaml,
};

// namespace MS.Internal.Ink

// The bits which internally represents the formats supported by InkCanvas
enum class InkCanvasClipboardDataFormat
{
    None            = 0x00,     // None
    XAML            = 0x01,     // XAML
    ISF             = 0x02,     // ISF
};

Q_DECLARE_FLAGS(InkCanvasClipboardDataFormats, InkCanvasClipboardDataFormat)

#endif // INKCANVASCLIPBOARDFORMAT_H
