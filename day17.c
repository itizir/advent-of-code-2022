#include "advent_of_code.h"

void pad_string(FuriString* str) {
    int end = furi_string_size(str) - 1;
    int n = 0;
    while(furi_string_get_char(str, end - n) == 0) n++;
    for(int i = 0; i < 4 - n; i++) furi_string_push_back(str, 0);
}

typedef struct {
    uint32_t rocks;
    uint16_t jet;
    uint16_t shape;
} State;

typedef struct {
    uint16_t num_rocks;
    uint16_t height;
} Progress;

DICT_DEF2(chamber_states, State, M_POD_OPLIST, Progress, M_POD_OPLIST)

void solve_day17(AdventOfCode* aoc, Stream* file_stream) {
    uint64_t p1 = 0;
    uint64_t p2 = 0;

    uint64_t num_rocks = 1000000000000;

    // little endian
    const uint32_t shapes[] = {0x0000001E, 0x00081C08, 0x0004041C, 0x10101010, 0x00001818};
    const int num_shapes = 5;

    FuriString* jets = furi_string_alloc();
    stream_read_line(file_stream, jets);
    furi_string_trim(jets);

    const int num_jets = furi_string_size(jets);
    int cj = 0;

    FuriString* chamber = furi_string_alloc_set("\xFF");
    pad_string(chamber);

    chamber_states_t chamber_states;
    chamber_states_init(chamber_states);

    bool found_cycle = false;
    for(uint64_t cs = 0; cs < num_rocks; cs++) {
        if(cs == 2022) p1 = furi_string_size(chamber) - 5;

        char* land = (char*)furi_string_get_cstr(chamber) + furi_string_size(chamber) - 4;
        uint32_t shape = shapes[cs % num_shapes];

        for(int i = 0; i < 3; i++) {
            if(furi_string_get_char(jets, cj) == '<' && (shape & 0x40404040) == 0)
                shape <<= 1;
            else if(furi_string_get_char(jets, cj) == '>' && (shape & 0x01010101) == 0)
                shape >>= 1;
            cj = (cj + 1) % num_jets;
        }

        while(true) {
            uint32_t l = *(uint32_t*)land;
            if(furi_string_get_char(jets, cj) == '<' && (shape & (0x40404040 | (l >> 1))) == 0)
                shape <<= 1;
            else if(furi_string_get_char(jets, cj) == '>' && (shape & (0x01010101 | (l << 1))) == 0)
                shape >>= 1;
            cj = (cj + 1) % num_jets;

            land--;
            if((*(uint32_t*)land & shape) != 0) break;
        }

        land++;
        *(uint32_t*)land |= shape;
        pad_string(chamber);

        if(cs > 2022 && !found_cycle &&
           (*land | *(land + 1) | *(land + 2) | *(land + 3)) == 0x7F) {
            State st = {*(uint32_t*)land, (uint16_t)cj, (uint16_t)(cs % num_shapes)};
            Progress pg = {(uint16_t)cs, (uint16_t)furi_string_size(chamber) - 5};
            Progress* prev = chamber_states_get(chamber_states, st);

            if(prev == NULL)
                chamber_states_set_at(chamber_states, st, pg);
            else {
                found_cycle = true;
                num_rocks -= prev->num_rocks;
                p2 = (uint64_t)(pg.height - prev->height) *
                     (num_rocks / (uint64_t)(pg.num_rocks - prev->num_rocks) - 1);
                num_rocks %= (uint64_t)(pg.num_rocks - prev->num_rocks);
                cs %= (uint64_t)num_shapes;
                num_rocks += cs;
            }
        }
    }

    p2 += furi_string_size(chamber) - 5;

    FuriString* part_1 = furi_string_alloc_printf("%lld", p1);
    FuriString* part_2 = furi_string_alloc_printf("%lld", p2);

    update_results(aoc, part_1, part_2);

    furi_string_free(part_2);
    furi_string_free(part_1);

    chamber_states_clear(chamber_states);
    furi_string_free(chamber);
    furi_string_free(jets);
}
