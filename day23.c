#include "advent_of_code.h"

ARRAY_DEF(array_u16, uint16_t)

static inline uint16_t ptu(uint8_t x, uint8_t y) {
    return (y << 8) | x;
}

static inline uint8_t utx(uint16_t u) {
    return u & 0xFF;
}

static inline uint8_t uty(uint16_t u) {
    return utx(u >> 8);
}

static inline uint16_t add(uint16_t p, uint16_t q) {
    return ptu(utx(p) + utx(q), uty(p) + uty(q));
}

void solve_day23(AdventOfCode* aoc, Stream* file_stream) {
    int p1 = 0;
    int p2 = 0;

    int8_t* grid;
    grid = malloc(1 + UINT16_MAX);
    memset(grid, -1, 1 + UINT16_MAX);
    array_u16_t elves;
    array_u16_init(elves);

    FuriString* line = furi_string_alloc();
    uint8_t offset = 0;
    for(uint8_t i = 128;; i++) {
        if(!stream_read_line(file_stream, line)) break;
        furi_string_trim(line);
        if(offset == 0) {
            offset = furi_string_size(line) / 2;
            i -= offset;
        }
        for(uint8_t j = 0; j < furi_string_size(line); j++)
            if(furi_string_get_char(line, j) == '#') {
                uint16_t elf = ptu(j + 128 - offset, i);
                grid[elf] = 0;
                array_u16_push_back(elves, elf);
            }
    }
    furi_string_free(line);

    const uint16_t steps[8] = {
        0xFFFF, // NW
        0xFF00, // N
        0xFF01, // NE
        0x0001, // E
        0x0101, // SE
        0x0100, // S
        0x01FF, // SW
        0x00FF, // W
    };
    const int dirs[4] = {1, 5, 7, 3};

    int first_dir = 0;
    array_u16_it_t it;
    for(p2 = 1;; p2++) {
        for(array_u16_it(it, elves); !array_u16_end_p(it); array_u16_next(it)) {
            uint16_t elf = *array_u16_ref(it);

            bool alone = true;
            int mv = 0;
            for(int i = first_dir; i < first_dir + 4 && (alone || mv == 0); i++) {
                int d = dirs[i % 4];
                bool free = true;
                for(int j = d - 1; j <= d + 1; j++) {
                    if(grid[add(elf, steps[j % 8])] >= 0) {
                        alone = false;
                        free = false;
                        break;
                    }
                }
                if(free && mv == 0) mv = d;
                if(free && !alone) break;
            }

            if(alone || mv == 0) {
                continue;
            }

            uint16_t n = add(elf, steps[mv]);
            int8_t other = grid[add(n, steps[mv])];
            if(other >= 0 && abs(mv - other) == 4)
                grid[add(n, steps[mv])] = 0;
            else
                grid[elf] = mv;
        }

        int moved = 0;
        for(array_u16_it(it, elves); !array_u16_end_p(it); array_u16_next(it)) {
            uint16_t* elf = array_u16_ref(it);
            int8_t d = grid[*elf];

            if(d == 0) continue;
            moved++;

            grid[*elf] = -1;
            *elf = add(*elf, steps[d]);
            grid[*elf] = 0;
        }
        if(moved == 0) break;

        first_dir++;
        first_dir %= 4;

        if(p2 != 10) continue;

        int minx = 0xFF, maxx = 0;
        int miny = 0xFF, maxy = 0;
        for(array_u16_it(it, elves); !array_u16_end_p(it); array_u16_next(it)) {
            uint16_t elf = *array_u16_ref(it);
            uint8_t x = utx(elf), y = uty(elf);
            if(x < minx) minx = x;
            if(x > maxx) maxx = x;
            if(y < miny) miny = y;
            if(y > maxy) maxy = y;
        }

        p1 = (maxx - minx + 1) * (maxy - miny + 1) - array_u16_size(elves);
        FuriString* part_1 = furi_string_alloc_printf("%d", p1);
        update_results(aoc, part_1, NULL);
        furi_string_free(part_1);
    }

    FuriString* part_2 = furi_string_alloc_printf("%d", p2);
    update_results(aoc, NULL, part_2);
    furi_string_free(part_2);

    array_u16_clear(elves);
    free(grid);
}
