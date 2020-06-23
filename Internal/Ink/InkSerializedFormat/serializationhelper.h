#ifndef SERIALIZATIONHELPER_H
#define SERIALIZATIONHELPER_H

#include "InkCanvas_global.h"

class QIODevice;

INKCANVAS_BEGIN_NAMESPACE

/// <summary>
/// Summary description for HelperMethods.
/// </summary>
class SerializationHelper
{
public:
    /// <summary>
    /// returns the no of byte it requires to mutlibyte encode a quint32 value
    /// </summary>
    /// <param name="Value"></param>
    /// <returns></returns>
    static quint32 VarSize(quint32 Value);


    /// <summary>
    /// MultiByte Encodes an quint32 Value into the stream
    /// </summary>
    /// <param name="strm"></param>
    /// <param name="Value"></param>
    /// <returns></returns>
    static quint32 Encode(QIODevice& strm, quint32 Value);


    /// <summary>
    /// Multibyte encodes a unsinged long value in the stream
    /// </summary>
    /// <param name="strm"></param>
    /// <param name="ulValue"></param>
    /// <returns></returns>
    static quint32 EncodeLarge(QIODevice& strm, quint64 ulValue);


    /// <summary>
    /// Multibyte encodes a signed integer value into a stream. Use 1's complement to
    /// store signed values.  This means both 00 and 01 are actually 0, but we don't
    /// need to encode all negative numbers as 64 bit values.
    /// </summary>
    /// <param name="strm"></param>
    /// <param name="Value"></param>
    /// <returns></returns>
    // Use 1's complement to store signed values.  This means both 00 and 01 are
    // actually 0, but we don't need to encode all negative numbers as 64 bit values.
    static quint32 SignEncode(QIODevice& strm, int Value);

    /// <summary>
    /// Decodes a multi byte encoded unsigned integer from the stream
    /// </summary>
    /// <param name="strm"></param>
    /// <param name="dw"></param>
    /// <returns></returns>
    static quint32 Decode(QIODevice& strm, quint32& dw);


    /// <summary>
    /// Decodes a multibyte encoded unsigned long from the stream
    /// </summary>
    /// <param name="strm"></param>
    /// <param name="ull"></param>
    /// <returns></returns>
    static quint32 DecodeLarge(QIODevice& strm, quint64& ull);


    /// <summary>
    /// Decodes a multibyte encoded signed integer from the stream
    /// </summary>
    /// <param name="strm"></param>
    /// <param name="i"></param>
    /// <returns></returns>
    static quint32 SignDecode(QIODevice& strm, int& i);

    /// <summary>
    /// Converts the CLR type information into a COM-compatible type enumeration
    /// </summary>
    /// <param name="type">The CLR type information of the object to convert</param>
    /// <param name="throwOnError">Throw an exception if unknown type is used</param>
    /// <returns>The COM-compatible type enumeration</returns>
    /// <remarks>Only supports the types of data that are supported in ISF ExtendedProperties</remarks>
    //static VarEnum ConvertToVarEnum(Type type, bool throwOnError);
};

INKCANVAS_END_NAMESPACE

#endif // SERIALIZATIONHELPER_H
