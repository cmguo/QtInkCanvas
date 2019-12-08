#ifndef SERIALIZATIONFLAGS_H
#define SERIALIZATIONFLAGS_H

/// <summary>
/// Specifies how to persist the ink.
/// </summary>
enum class PersistenceFormat
{

    /// <summary>
    /// Native ink serialization format.
    /// </summary>
    InkSerializedFormat = 0,

    /// <summary>
    /// GIF serialization format.
    /// </summary>
    Gif = 1,
};

/// <summary>
/// Specifies how to compress the ink.
/// </summary>
enum class CompressionMode
{
    /// <summary>
    /// Compressed
    /// </summary>
    Compressed = 0,


    /// <summary>
    /// NoCompression
    /// </summary>
    NoCompression = 2
};

#endif // SERIALIZATIONFLAGS_H
