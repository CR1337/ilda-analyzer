#include "analyze.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

const int8_t magicNumber[4] = { 'I', 'L', 'D', 'A' };
const int32_t NO_RECORD_INDEX = -1;

const char *errorCategoryStrings[] = {
    "Information",
    "Warning",
    "Error"
};

const char *errorStrings[] = {
    //Information
    "There are multiple projectors used.",

    // Warnings
    "There is an indexed record but no color palette for it.",
    "There is no projector with index 0.",
    "There are colors specified during a blanking interval.",

    // Errors
    "The format code is invalid. It must be one of (0, 1, 2, 4, 5).",
    "The magic number is invalid. It must be \"ILDA\".",
    "Reserved bits in the header are not set to zero.",
    "The color palette has an invalid length.",
    "The total frames amount is not set to zero for color palette.",
    "The status code of the last record doesn't indicate that it is the last record.",
    "Reserved bits in the status code are not set to zero.",
    "The last header has records. It must not have records.",
    "The file is corrupted."
};

const char unknownFormatString[] = "UNKNOWN";

const char *formatStrings[] = {
    "Indexed 3D",
    "Indexed 2D",
    "Color Palette",
    unknownFormatString,
    "True Color 3D",
    "True Color 2D",
};

void _printError(enum ErrorCategory errorCategory, enum ErrorType errorType, int32_t recordIndex) {
    printf("%s: %s\n", errorCategoryStrings[errorCategory], errorStrings[errorType]);
    if (recordIndex != NO_RECORD_INDEX) {
        printf("\tin record #%d\n", recordIndex);
    }
    putchar('\n');
}

void _printHeader(int64_t sectionIndex, header_t *header) {
    printf("------------------------------------------------\n");
    printf("Section Index:                 %ld\n", sectionIndex);
    printf("Magic Number:                  %.4s\n", header->magic);
    printf("Format Code:                   %d\n", header->formatCode);
    const char *formatString;
    if (header->formatCode >= INDEXED_2D && header->formatCode <= TRUE_COLOR_3D) {
        formatString = formatStrings[header->formatCode];
    } else {
        formatString = unknownFormatString;
    }
    printf("Format:                        %s\n", formatString);
    printf("Frame or Color Palette Name:   %.8s\n", header->frameOrColorPaletteName);
    printf("Company Name:                  %.8s\n", header->companyName);
    printf("Number of Records:             %d\n", header->numberOfRecords);
    printf("Frame Or Color Palette Number: %d\n", header->frameOrColorPaletteNumber);
    printf("Total Frames:                  %d\n", header->totalFrames);
    printf("Projector Number:              %d\n", header->projectorNumber);
    putchar('\n');
}

bool _analyzeAndPrintHeader(FILE *file, header_t *header, int64_t sectionAmount) {
    if (!fread(header, sizeof(header_t), 1, file)) {
        _printError(ERROR, FILE_CORRUPTED, NO_RECORD_INDEX);
        return false;
    }

    _printHeader(sectionAmount, header);

    if (memcmp(header->magic, magicNumber, 4)) {
        _printError(ERROR, INVALID_MAGIC_NUMBER, NO_RECORD_INDEX);
    }
    if (header->reserved0[0] || header->reserved0[1] || header->reserved0[2] || header->reserved1) {
        _printError(ERROR, RESERVED_HEADER_BITS_NOT_ZERO, NO_RECORD_INDEX);
    }
    return true;
}

uint16_t _readIndexed2dRecords(FILE *file, uint16_t numberOfRecords, record_t *records) {
    for (int i = 0; i < numberOfRecords; ++i) {
        indexedPoint2d_t record;
        if (!fread(&record, sizeof(record), 1, file)) {
            return i;
        }
        records[i] = (record_t){
            .x = record.x,
            .y = record.y,
            .z = 0,
            .statusCode = record.statusCode,
            .colorIndex = record.colorIndex,
            .r = 0,
            .g = 0,
            .b = 0
        };
    }
    return numberOfRecords;
}

uint16_t _readIndexed3dRecords(FILE *file, uint16_t numberOfRecords, record_t *records) {
    for (int i = 0; i < numberOfRecords; ++i) {
        indexedPoint3d_t record;
        if (!fread(&record, sizeof(record), 1, file)) {
            return i;
        }
        records[i] = (record_t){
            .x = record.x,
            .y = record.y,
            .z = record.z,
            .statusCode = record.statusCode,
            .colorIndex = record.colorIndex,
            .r = 0,
            .g = 0,
            .b = 0
        };
    }
    return numberOfRecords;
}

uint16_t _readColorPaletteRecord(FILE *file, uint16_t numberOfRecords, record_t *records) {
    for (int i = 0; i < numberOfRecords; ++i) {
        colorPalette_t record;
        if (!fread(&record, sizeof(record), 1, file)) {
            return i;
        }
        records[i] = (record_t){
            .x = 0,
            .y = 0,
            .z = 0,
            .statusCode = 0,
            .colorIndex = 0,
            .r = record.r,
            .g = record.g,
            .b = record.b
        };
    }
    return numberOfRecords;
}

uint16_t _readTrueColor2dRecord(FILE *file, uint16_t numberOfRecords, record_t *records) {
    for (int i = 0; i < numberOfRecords; ++i) {
        trueColorPoint2d_t record;
        if (!fread(&record, sizeof(record), 1, file)) {
            return i;
        }
        records[i] = (record_t){
            .x = record.x,
            .y = record.y,
            .z = 0,
            .statusCode = record.statusCode,
            .colorIndex = 0,
            .r = record.r,
            .g = record.g,
            .b = record.b
        };
    }
    return numberOfRecords;
}

uint16_t _readTrueColor3dRecord(FILE *file, uint16_t numberOfRecords, record_t *records) {
    for (int i = 0; i < numberOfRecords; ++i) {
        trueColorPoint3d_t record;
        if (!fread(&record, sizeof(record), 1, file)) {
            return i;
        }
        records[i] = (record_t){
            .x = record.x,
            .y = record.y,
            .z = record.z,
            .statusCode = record.statusCode,
            .colorIndex = 0,
            .r = record.r,
            .g = record.g,
            .b = record.b
        };
    }
    return numberOfRecords;
}

bool _readRecords(FILE *file, record_t *records, header_t *header) {
    uint32_t recordsRead = 0;
    switch (header->formatCode) {
        case INDEXED_2D:
            recordsRead = _readIndexed2dRecords(file, header->numberOfRecords, records);
            break;
        case INDEXED_3D:
            recordsRead = _readIndexed3dRecords(file, header->numberOfRecords, records);
            break;
        case COLOR_PALETTE:
            if (header->numberOfRecords < 2 || header->numberOfRecords > 256) {
                _printError(ERROR, INVALID_COLOR_PALETTE_LENGTH, NO_RECORD_INDEX);
            }
            if (header->totalFrames) {
                _printError(ERROR, TOTAL_FRAMES_FOR_COLOR_PALETTE_NOT_ZERO, NO_RECORD_INDEX);
            }
            recordsRead = _readColorPaletteRecord(file, header->numberOfRecords, records);
            break;
        case TRUE_COLOR_2D:
            recordsRead = _readTrueColor2dRecord(file, header->numberOfRecords, records);
            break;
        case TRUE_COLOR_3D:
            recordsRead = _readTrueColor3dRecord(file, header->numberOfRecords, records);
            break;
        default:
            _printError(ERROR, INVALID_FORMAT_CODE, NO_RECORD_INDEX);
            break;
    }
    if (recordsRead != header->numberOfRecords) {
        _printError(ERROR, FILE_CORRUPTED, recordsRead + 1);
        return false;
    }
    return true;
}

void analyze(FILE *file) {
    bool projectorZeroFound = false;
    bool projectorsFound[0xff] = { false };
    bool colorPaletteFound = false;

    header_t header = { 0 };
    for (int64_t sectionAmount = 0; fgetc(file) != EOF; ++sectionAmount) {
        fseek(file, -1, SEEK_CUR);
        if (!_analyzeAndPrintHeader(file, &header, sectionAmount)) return;

        projectorZeroFound |= header.projectorNumber == 0;
        projectorsFound[header.projectorNumber] = true;
        colorPaletteFound |= header.formatCode == COLOR_PALETTE;

        record_t *records = (record_t*)malloc(
            header.numberOfRecords * sizeof(record_t)
        );

        if (!_readRecords(file, records, &header)) return;

        switch (header.formatCode) {
            case INDEXED_2D:
            case INDEXED_3D:
                if (!colorPaletteFound) {
                    _printError(WARNING, INDEXED_RECORDS_BUT_NO_COLOR_PALETTE, NO_RECORD_INDEX);
                }

            case TRUE_COLOR_2D:
            case TRUE_COLOR_3D:
                if (!header.numberOfRecords) break;
                for (int i = 0; i < header.numberOfRecords; ++i) {
                    if (records[i].statusCode & BLANKING && (records[i].r || records[i].g || records[i].b)) {
                        _printError(WARNING, RGB_NOT_ZERO_DURING_BLANK, i);
                    }
                    if (records[i].statusCode & RESERVED_STATUS_BITS) {
                        _printError(ERROR, RESERVED_STATUS_BITS_NOT_ZERO, NO_RECORD_INDEX);
                    }
                }
                if (!(records[header.numberOfRecords - 1].statusCode & LAST_POINT)) {
                    _printError(ERROR, INVALID_LAST_POINT_STATUS_BIT, NO_RECORD_INDEX);
                }
                break;
        }

        free(records);
    }

    if (header.numberOfRecords) {
        _printError(ERROR, LAST_HEADER_HAS_RECORDS, NO_RECORD_INDEX);
    }

    printf("------------------------------------------------\n");
    printf("Global Errors:\n");

    for (int i = 0, projectorCount = 0; i < 0xff; ++i) {
        projectorCount += projectorsFound[i];
        if (projectorCount > 1) {
            _printError(INFORMATION, MULTIPLE_PROJECTORS, NO_RECORD_INDEX);
            break;
        }
    }

    if (!projectorZeroFound) {
        _printError(WARNING, NO_PROJECTOR_ZERO, NO_RECORD_INDEX);
    }
}
