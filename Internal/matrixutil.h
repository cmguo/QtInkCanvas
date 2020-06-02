#ifndef MATRIXUTIL_H
#define MATRIXUTIL_H

#include "InkCanvas_global.h"

INKCANVAS_BEGIN_NAMESPACE

enum class MatrixTypes
{
    TRANSFORM_IS_IDENTITY = 0,
    TRANSFORM_IS_TRANSLATION = 1,
    TRANSFORM_IS_SCALING     = 2,
    TRANSFORM_IS_SCALING_TRANSLATION     = 3,
    TRANSFORM_IS_UNKNOWN     = 4
};

inline MatrixTypes operator |(MatrixTypes l, MatrixTypes r)
{
    return static_cast<MatrixTypes>(static_cast<int>(l) | static_cast<int>(r));
}

inline int operator &(MatrixTypes l, MatrixTypes r)
{
    return static_cast<int>(l) & static_cast<int>(r);
}

class Matrix;
class Rect;

class MatrixUtil
{
public:
    /// <summary>
    /// TransformRect - Internal helper for perf
    /// </summary>
    /// <param name="rect"> The Rect to transform. </param>
    /// <param name="matrix"> The Matrix with which to transform the Rect. </param>
    static void TransformRect(Rect & rect, Matrix const & matrix);

    /// <summary>
    /// Multiplies two transformations, where the behavior is matrix1 *= matrix2.
    /// This code exists so that we can efficient combine matrices without copying
    /// the data around, since each matrix is 52 bytes.
    /// To reduce duplication and to ensure consistent behavior, this is the
    /// method which is used to implement Matrix * Matrix as well.
    /// </summary>
    static void MultiplyMatrix(Matrix & matrix1, Matrix const & matrix2);

    /// <summary>
    /// Applies an offset to the specified matrix in place.
    /// </summary>
    static void PrependOffset(
        Matrix & matrix,
        double offsetX,
        double offsetY);
};

INKCANVAS_END_NAMESPACE

#endif // MATRIXUTIL_H
