#pragma once

#include <dialogs/dialogs.h>
#include <gui/gui.h>
#include <lib/toolbox/stream/buffered_file_stream.h>
#include <lib/toolbox/path.h>

#include "m-array.h"
#include "m-bitset.h"
#include "m-dict.h"

#include <advent_of_code_2022_icons.h>

#define TAG "AdventOfCode2022"

#define ADVENT_OF_CODE_APP_BASE_PATH EXT_PATH("advent/2022")
#define ADVENT_OF_CODE_APP_EXTENSION ".txt"

// used more than once
ARRAY_DEF(array_int, int)
ARRAY_DEF(array_string, FuriString*, FURI_STRING_OPLIST)
DICT_DEF2(dict_string_int, FuriString*, FURI_STRING_OPLIST, int, M_DEFAULT_OPLIST)

typedef struct {
    uint8_t width;
    uint8_t height;
    uint8_t* data;
} Image;

typedef struct {
    int day;

    FuriString* part_1;
    FuriString* part_2;

    Image* img_1;
    Image* img_2;
} AdventOfCodeModel;

typedef struct {
    AdventOfCodeModel* model;
    FuriMutex** model_mutex;

    FuriMessageQueue* input_queue;

    ViewPort* view_port;
    Gui* gui;
} AdventOfCode;

typedef void (*AdventSolver)(AdventOfCode* aoc, Stream* file_stream);

void update_results(AdventOfCode* aoc, FuriString* part_1, FuriString* part_2);
void update_images(AdventOfCode* aoc, Image* img_1, Image* img_2);
