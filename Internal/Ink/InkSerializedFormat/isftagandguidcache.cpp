#include "Internal/Ink/InkSerializedFormat/isftagandguidcache.h"

QUuid KnownIdCache::OriginalISFIdTable[] = {
    {0x598a6a8f, 0x52c0, 0x4ba0, 0x93, 0xaf, 0xaf, 0x35, 0x74, 0x11, 0xa5, 0x61},
    {0xb53f9f75, 0x04e0, 0x4498, 0xa7, 0xee, 0xc3, 0x0d, 0xbb, 0x5a, 0x90, 0x11},
    {0x735adb30, 0x0ebb, 0x4788, 0xa0, 0xe4, 0x0f, 0x31, 0x64, 0x90, 0x05, 0x5d},
    {0x6e0e07bf, 0xafe7, 0x4cf7, 0x87, 0xd1, 0xaf, 0x64, 0x46, 0x20, 0x84, 0x18},
    {0x436510c5, 0xfed3, 0x45d1, 0x8b, 0x76, 0x71, 0xd3, 0xea, 0x7a, 0x82, 0x9d},
    {0x78a81b56, 0x0935, 0x4493, 0xba, 0xae, 0x00, 0x54, 0x1a, 0x8a, 0x16, 0xc4},
    {0x7307502d, 0xf9f4, 0x4e18, 0xb3, 0xf2, 0x2c, 0xe1, 0xb1, 0xa3, 0x61, 0x0c},
    {0x6da4488b, 0x5244, 0x41ec, 0x90, 0x5b, 0x32, 0xd8, 0x9a, 0xb8, 0x08, 0x09},
    {0x8b7fefc4, 0x96aa, 0x4bfe, 0xac, 0x26, 0x8a, 0x5f, 0x0b, 0xe0, 0x7b, 0xf5},
    {0xa8d07b3a, 0x8bf0, 0x40b0, 0x95, 0xa9, 0xb8, 0x0a, 0x6b, 0xb7, 0x87, 0xbf},
    {0x0e932389, 0x1d77, 0x43af, 0xac, 0x00, 0x5b, 0x95, 0x0d, 0x6d, 0x4b, 0x2d},
    {0x029123b4, 0x8828, 0x410b, 0xb2, 0x50, 0xa0, 0x53, 0x65, 0x95, 0xe5, 0xdc},
    {0x82dec5c7, 0xf6ba, 0x4906, 0x89, 0x4f, 0x66, 0xd6, 0x8d, 0xfc, 0x45, 0x6c},
    {0x0d324960, 0x13b2, 0x41e4, 0xac, 0xe6, 0x7a, 0xe9, 0xd4, 0x3d, 0x2d, 0x3b},
    {0x7f7e57b7, 0xbe37, 0x4be1, 0xa3, 0x56, 0x7a, 0x84, 0x16, 0x0e, 0x18, 0x93},
    {0x5d5d5e56, 0x6ba9, 0x4c5b, 0x9f, 0xb0, 0x85, 0x1c, 0x91, 0x71, 0x4e, 0x56},
    {0x6a849980, 0x7c3a, 0x45b7, 0xaa, 0x82, 0x90, 0xa2, 0x62, 0x95, 0x0e, 0x89},
    {0x33c1df83, 0xecdb, 0x44f0, 0xb9, 0x23, 0xdb, 0xd1, 0xa5, 0xb2, 0x13, 0x6e},
    {0x5329cda5, 0xfa5b, 0x4ed2, 0xbb, 0x32, 0x83, 0x46, 0x01, 0x72, 0x44, 0x28},
    {0x002df9af, 0xdd8c, 0x4949, 0xba, 0x46, 0xd6, 0x5e, 0x10, 0x7d, 0x1a, 0x8a},
    {0x9d32b7ca, 0x1213, 0x4f54, 0xb7, 0xe4, 0xc9, 0x05, 0x0e, 0xe1, 0x7a, 0x38},
    {0xe71caab9, 0x8059, 0x4c0d, 0xa2, 0xdb, 0x7c, 0x79, 0x54, 0x47, 0x8d, 0x82},
    {0x5c0b730a, 0xf394, 0x4961, 0xa9, 0x33, 0x37, 0xc4, 0x34, 0xf4, 0xb7, 0xeb},
    {0x2812210f, 0x871e, 0x4d91, 0x86, 0x07, 0x49, 0x32, 0x7d, 0xdf, 0x0a, 0x9f},
    {0x8359a0fa, 0x2f44, 0x4de6, 0x92, 0x81, 0xce, 0x5a, 0x89, 0x9c, 0xf5, 0x8f},
    {0x4c4642dd, 0x479e, 0x4c66, 0xb4, 0x40, 0x1f, 0xcd, 0x83, 0x95, 0x8f, 0x00},
    {0xce2d9a8a, 0xe58e, 0x40ba, 0x93, 0xfa, 0x18, 0x9b, 0xb3, 0x90, 0x00, 0xae},
    {0xc3c7480f, 0x5839, 0x46ef, 0xa5, 0x66, 0xd8, 0x48, 0x1c, 0x7a, 0xfe, 0xc1},
    {0xea2278af, 0xc59d, 0x4ef4, 0x98, 0x5b, 0xd4, 0xbe, 0x12, 0xdf, 0x22, 0x34},
    {0xb8630dc9, 0xcc5c, 0x4c33, 0x8d, 0xad, 0xb4, 0x7f, 0x62, 0x2b, 0x8c, 0x79},
    {0x15e2f8e6, 0x6381, 0x4e8b, 0xa9, 0x65, 0x01, 0x1f, 0x7d, 0x7f, 0xca, 0x38},
    {0x7066fbe4, 0x473e, 0x4675, 0x9c, 0x25, 0x00, 0x26, 0x82, 0x9b, 0x40, 0x1f},
    {0xbbc85b9a, 0xade6, 0x4093, 0xb3, 0xbb, 0x64, 0x1f, 0xa1, 0xd3, 0x7a, 0x1a},
    {0x39143d3, 0x78cb, 0x449c, 0xa8, 0xe7, 0x67, 0xd1, 0x88, 0x64, 0xc3, 0x32},
    {0x67743782, 0xee5, 0x419a, 0xa1, 0x2b, 0x27, 0x3a, 0x9e, 0xc0, 0x8f, 0x3d},
    {0xf0720328, 0x663b, 0x418f, 0x85, 0xa6, 0x95, 0x31, 0xae, 0x3e, 0xcd, 0xfa},
    {0xa1718cdd, 0xdac, 0x4095, 0xa1, 0x81, 0x7b, 0x59, 0xcb, 0x10, 0x6b, 0xfb},
    {0x810a74d2, 0x6ee2, 0x4e39, 0x82, 0x5e, 0x6d, 0xef, 0x82, 0x6a, 0xff, 0xc5},
};

uint KnownIdCache::OriginalISFIdTableLength = sizeof(OriginalISFIdTable) / sizeof(OriginalISFIdTable[0]);

// Size of data used by identified by specified QUuid/Id
uint KnownIdCache::OriginalISFIdPersistenceSize[] = {
    sizeof(int),           // X                         0
    sizeof(int),           // Y                         1
    sizeof(int),           // Z                         2
    sizeof(int),           // PACKET_STATUS             3
    2 * sizeof(unsigned int),      // FILETIME : TIMER_TICK     4
    sizeof(unsigned int),          // SERIAL_NUMBER             5
    sizeof(unsigned short),        // NORMAL_PRESSURE           6
    sizeof(unsigned short),        // TANGENT_PRESSURE          7
    sizeof(unsigned short),        // BUTTON_PRESSURE           8
    sizeof(float),         // X_TILT_ORIENTATION        9
    sizeof(float),         // Y_TILT_ORIENTATION        10
    sizeof(float),         // AZIMUTH_ORIENTATION       11
    sizeof(int),           // ALTITUDE_ORIENTATION      12
    sizeof(int),           // TWIST_ORIENTATION         13
    sizeof(unsigned short),        // PITCH_ROTATION            14
    sizeof(unsigned short),        // ROLL_ROTATION             15
    sizeof(unsigned short),        // YAW_ROTATION              16
    sizeof(unsigned short),        // PEN_STYLE                 17
    sizeof(unsigned int),          // COLORREF: COLORREF        18
    sizeof(unsigned int),          // PEN_WIDTH                 19
    sizeof(unsigned int),          // PEN_HEIGHT                20
    sizeof(unsigned char),          // PEN_TIP                   21
    sizeof(unsigned int),          // DRAWING_FLAGS             22
    sizeof(unsigned int),          // CURSORID                  23
    0,                          // WORD_ALTERNATES           24
    0,                          // CHAR_ALTERNATES           25
    5 * sizeof(unsigned int),      // INKMETRICS                26
    3 * sizeof(unsigned int),      // QUuidE_STRUCTURE           27
    8 * sizeof(unsigned short),    // SYSTEMTIME TIME_STAMP     28
    sizeof(unsigned short),        // LANGUAGE                  29
    sizeof(unsigned char),          // TRANSPARENCY              30
    sizeof(unsigned int),          // CURVE_FITTING_ERROR       31
    0,                          // RECO_LATTICE              32
    sizeof(int),           // CURSORDOWN                33
    sizeof(int),           // SECONDARYTIPSWITCH        34
    sizeof(int),           // BARRELDOWN                35
    sizeof(int),           // TABLETPICK                36
    sizeof(int),           // ROP                       37
};

QUuid KnownIdCache::TabletInternalIdTable[] = {
        // Highlighter
    {0x9b6267b8, 0x3968, 0x4048, 0xab, 0x74, 0xf4, 0x90, 0x40, 0x6a, 0x2d, 0xfa},
        // Ink properties
    {0x7fc30e91, 0xd68d, 0x4f07, 0x8b, 0x62, 0x6, 0xf6, 0xd2, 0x73, 0x1b, 0xed},
        // Ink Style Bold
    {0xe02fb5c1, 0x9693, 0x4312, 0xa4, 0x34, 0x0, 0xde, 0x7f, 0x3a, 0xd4, 0x93},
        // Ink Style Italics
    {0x5253b51, 0x49c6, 0x4a04, 0x89, 0x93, 0x64, 0xdd, 0x9a, 0xbd, 0x84, 0x2a},
        // Stroke Timestamp
    {0x4ea66c4, 0xf33a, 0x461b, 0xb8, 0xfe, 0x68, 0x7, 0xd, 0x9c, 0x75, 0x75},
        // Stroke Time Id
    {0x50b6bc8, 0x3b7d, 0x4816, 0x8c, 0x61, 0xbc, 0x7e, 0x90, 0x5b, 0x21, 0x32},
        // Stroke Lattice
    {0x82871c85, 0xe247, 0x4d8c, 0x8d, 0x71, 0x22, 0xe5, 0xd6, 0xf2, 0x57, 0x76},
        // Ink Custom Strokes
    {0x33cdbbb3, 0x588f, 0x4e94, 0xb1, 0xfe, 0x5d, 0x79, 0xff, 0xe7, 0x6e, 0x76},
};

uint KnownIdCache::TabletInternalIdTableLength = sizeof(TabletInternalIdTable) / sizeof(TabletInternalIdTable[0]);

QUuid KnownIdCache::ExtendedISFIdTable[] = {
    // Highlighter
    {0x9b6267b8, 0x3968, 0x4048, 0xab, 0x74, 0xf4, 0x90, 0x40, 0x6a, 0x2d, 0xfa},
    // Ink properties
    {0x7fc30e91, 0xd68d, 0x4f07, 0x8b, 0x62, 0x6, 0xf6, 0xd2, 0x73, 0x1b, 0xed},
    // Ink Style Bold
    {0xe02fb5c1, 0x9693, 0x4312, 0xa4, 0x34, 0x0, 0xde, 0x7f, 0x3a, 0xd4, 0x93},
    // Ink Style Italics
    {0x5253b51, 0x49c6, 0x4a04, 0x89, 0x93, 0x64, 0xdd, 0x9a, 0xbd, 0x84, 0x2a},
    // Stroke Timestamp
    {0x4ea66c4, 0xf33a, 0x461b, 0xb8, 0xfe, 0x68, 0x7, 0xd, 0x9c, 0x75, 0x75},
    // Stroke Time Id
    {0x50b6bc8, 0x3b7d, 0x4816, 0x8c, 0x61, 0xbc, 0x7e, 0x90, 0x5b, 0x21, 0x32},
    // Stroke Lattice
    {0x82871c85, 0xe247, 0x4d8c, 0x8d, 0x71, 0x22, 0xe5, 0xd6, 0xf2, 0x57, 0x76},
    // Ink Custom Strokes
    {0x33cdbbb3, 0x588f, 0x4e94, 0xb1, 0xfe, 0x5d, 0x79, 0xff, 0xe7, 0x6e, 0x76},
};
