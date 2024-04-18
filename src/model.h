#ifndef __MODEL_H__
#define __MODEL_H__

#include <stdint.h>

#define BIG_ENDIAN_ORDER __attribute__((packed, scalar_storage_order("big-endian")))

enum FormatCode {
    INDEXED_3D = 0,
    INDEXED_2D = 1,
    COLOR_PALETTE = 2,
    TRUE_COLOR_3D = 4,
    TRUE_COLOR_2D = 5
};

enum StatusCode {
    LAST_POINT = 0b10000000,
    BLANKING = 0b01000000,
    RESERVED_STATUS_BITS = 0b00111111
};

enum ErrorCategory {
    INFORMATION,
    WARNING,
    ERROR
};

enum ErrorType {
    // Information
    MULTIPLE_PROJECTORS,

    // Warnings
    INDEXED_RECORDS_BUT_NO_COLOR_PALETTE,
    NO_PROJECTOR_ZERO,
    RGB_NOT_ZERO_DURING_BLANK,

    // Errors
    INVALID_FORMAT_CODE,
    INVALID_MAGIC_NUMBER,
    RESERVED_HEADER_BITS_NOT_ZERO,
    INVALID_COLOR_PALETTE_LENGTH,
    TOTAL_FRAMES_FOR_COLOR_PALETTE_NOT_ZERO,
    INVALID_LAST_POINT_STATUS_BIT,
    RESERVED_STATUS_BITS_NOT_ZERO,
    LAST_HEADER_HAS_RECORDS,
    FILE_CORRUPTED
};

typedef struct BIG_ENDIAN_ORDER {
    int8_t magic[4];
    uint8_t reserved0[3];
    uint8_t formatCode;
    int8_t frameOrColorPaletteName[8];
    int8_t companyName[8];
    uint16_t numberOfRecords;
    uint16_t frameOrColorPaletteNumber;
    uint16_t totalFrames;
    uint8_t projectorNumber;
    uint8_t reserved1;
} header_t;

typedef struct BIG_ENDIAN_ORDER {
    uint16_t x;
    uint16_t y;
    uint16_t z;
    uint8_t statusCode;
    uint8_t colorIndex;
} indexedPoint3d_t;

typedef struct BIG_ENDIAN_ORDER {
    uint16_t x;
    uint16_t y;
    uint8_t statusCode;
    uint8_t colorIndex;
} indexedPoint2d_t;

typedef struct BIG_ENDIAN_ORDER {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} colorPalette_t;

typedef struct BIG_ENDIAN_ORDER {
    uint16_t x;
    uint16_t y;
    uint16_t z;
    uint8_t statusCode;
    uint8_t r;
    uint8_t g;
    uint8_t b;
} trueColorPoint3d_t;

typedef struct BIG_ENDIAN_ORDER {
    uint16_t x;
    uint16_t y;
    uint8_t statusCode;
    uint8_t r;
    uint8_t g;
    uint8_t b;
} trueColorPoint2d_t;

typedef struct {
    uint16_t x;
    uint16_t y;
    uint16_t z;
    uint8_t statusCode;
    uint8_t colorIndex;
    uint8_t r;
    uint8_t g;
    uint8_t b;
} record_t;

#endif // __MODEL_H__
