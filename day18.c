#include "advent_of_code.h"

static inline int pt_to_id(uint8_t x, uint8_t y, uint8_t z) {
    return x + (1 << 5) * y + (1 << 10) * z;
}

uint8_t status(uint8_t* cubes, int id) {
    uint8_t c = cubes[id];
    if(c > 0) return c;

    array_int_t pts;
    array_int_init(pts);
    array_int_t done;
    array_int_init(done);

    array_int_push_back(pts, id);

    static const int d[6] = {-1, 1, -(1 << 5), 1 << 5, -(1 << 10), 1 << 10};
    while(!array_int_empty_p(pts)) {
        array_int_pop_back(&id, pts);
        array_int_push_back(done, id);
        for(int i = 0; i < 6; i++) {
            int n = id + d[i];
            c = cubes[n];
            if(c == 0) {
                array_int_push_back(pts, n);
                cubes[n] = 2;
            }
            if(c > 2) break;
        }
        if(c > 2) break;
    }

    if(c < 3) c = 3;

    while(!array_int_empty_p(done)) {
        array_int_pop_back(&id, done);
        cubes[id] = c;
    }
    while(!array_int_empty_p(pts)) {
        array_int_pop_back(&id, pts);
        cubes[id] = c;
    }

    array_int_clear(done);
    array_int_clear(pts);

    return c;
}

void solve_day18(AdventOfCode* aoc, Stream* file_stream) {
    FuriString* line = furi_string_alloc();

    int p1 = 0;
    int p2 = 0;

    uint8_t* cubes;
    cubes = malloc(1 << 15);
    for(int i = 0; i < 1 << 15; i++) cubes[i] = 0;

    for(uint8_t x = 0; x < 1 << 5; x += (1 << 5) - 1)
        for(uint8_t y = 0; y < 1 << 5; y++)
            for(uint8_t z = 0; z < 1 << 5; z++) cubes[pt_to_id(x, y, z)] = 4;
    for(uint8_t y = 0; y < 1 << 5; y += (1 << 5) - 1)
        for(uint8_t x = 0; x < 1 << 5; x++)
            for(uint8_t z = 0; z < 1 << 5; z++) cubes[pt_to_id(x, y, z)] = 4;
    for(uint8_t z = 0; z < 1 << 5; z += (1 << 5) - 1)
        for(uint8_t x = 0; x < 1 << 5; x++)
            for(uint8_t y = 0; y < 1 << 5; y++) cubes[pt_to_id(x, y, z)] = 4;

    while(true) {
        if(!stream_read_line(file_stream, line)) break;
        furi_string_trim(line);

        char* ptr = (char*)furi_string_get_cstr(line);
        uint8_t x = strtol(ptr, &ptr, 10) + 1;
        uint8_t y = strtol(ptr + 1, &ptr, 10) + 1;
        uint8_t z = strtol(ptr + 1, &ptr, 10) + 1;

        cubes[pt_to_id(x, y, z)] = 1;
    }

    stream_rewind(file_stream);
    while(true) {
        if(!stream_read_line(file_stream, line)) break;
        furi_string_trim(line);

        char* ptr = (char*)furi_string_get_cstr(line);
        uint8_t x = strtol(ptr, &ptr, 10) + 1;
        uint8_t y = strtol(ptr + 1, &ptr, 10) + 1;
        uint8_t z = strtol(ptr + 1, &ptr, 10) + 1;

        int id = pt_to_id(x, y, z);

        uint8_t c;
        static const int d[6] = {-1, 1, -(1 << 5), 1 << 5, -(1 << 10), 1 << 10};
        for(int i = 0; i < 6; i++) {
            c = status(cubes, id + d[i]);
            if(c > 2) p1++;
            if(c == 4) p2++;
        }
    }

    FuriString* part_1 = furi_string_alloc_printf("%d", p1);
    FuriString* part_2 = furi_string_alloc_printf("%d", p2);

    update_results(aoc, part_1, part_2);

    free(cubes);
    furi_string_free(part_2);
    furi_string_free(part_1);
    furi_string_free(line);
}
