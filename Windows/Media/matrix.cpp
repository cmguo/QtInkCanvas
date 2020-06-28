#include "matrix.h"
#include "Internal/debug.h"

INKCANVAS_BEGIN_NAMESPACE

Matrix Matrix::s_identity = CreateIdentity();

void Matrix::Debug_CheckType()
{
    switch(_type)
    {
    case MatrixTypes::TRANSFORM_IS_IDENTITY:
        return;
    case MatrixTypes::TRANSFORM_IS_UNKNOWN:
        return;
    case MatrixTypes::TRANSFORM_IS_SCALING:
        Debug::Assert(_m21 == 0);
        Debug::Assert(_m12 == 0);
        Debug::Assert(_offsetX == 0);
        Debug::Assert(_offsetY == 0);
        return;
    case MatrixTypes::TRANSFORM_IS_TRANSLATION:
        Debug::Assert(_m21 == 0);
        Debug::Assert(_m12 == 0);
        Debug::Assert(_m11 == 1);
        Debug::Assert(_m22 == 1);
        return;
    case MatrixTypes::TRANSFORM_IS_SCALING_TRANSLATION:
        Debug::Assert(_m21 == 0);
        Debug::Assert(_m12 == 0);
        return;
    default:
        Debug::Assert(false);
        return;
    }
}

INKCANVAS_END_NAMESPACE
