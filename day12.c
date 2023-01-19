#include "advent_of_code.h"

void next_step(FuriString* grid, bitset_t visited, array_int_t next, char hc, int n) {
    if(!bitset_get(visited, n) && hc <= furi_string_get_char(grid, n) + 1) {
        bitset_set_at(visited, n, true);
        array_int_push_back(next, n);
    }
}

void solve_day12(AdventOfCode* aoc, Stream* file_stream) {
    FuriString* line = furi_string_alloc();

    int p1 = 0;
    int p2 = 0;

    FuriString* grid = furi_string_alloc();

    int w = 0, h = 0;
    while(true) {
        if(!stream_read_line(file_stream, line)) break;
        furi_string_trim(line);
        w = furi_string_size(line);
        furi_string_cat(grid, line);
        h++;
    }

    size_t start = furi_string_search_char(grid, 'S');
    size_t end = furi_string_search_char(grid, 'E');
    furi_string_set_char(grid, start, 'a');
    furi_string_set_char(grid, end, 'z');

    bitset_t visited;
    bitset_init(visited);
    bitset_resize(visited, w * h);

    array_int_t current;
    array_int_init(current);
    array_int_t next;
    array_int_init(next);

    bitset_set_at(visited, end, true);
    array_int_push_back(next, end);

    for(; !array_int_empty_p(next); p1++) {
        array_int_move(current, next);
        array_int_init(next);
        while(!array_int_empty_p(current)) {
            int c;
            array_int_pop_back(&c, current);

            char hc = furi_string_get_char(grid, c);
            if(hc == 'a' && p2 == 0) {
                p2 = p1;
            }

            if(c == (int)start) {
                array_int_reset(next);
                break;
            }

            if(c % w > 0) next_step(grid, visited, next, hc, c - 1);
            if((c + 1) % w > 0) next_step(grid, visited, next, hc, c + 1);
            if((c / w) > 0) next_step(grid, visited, next, hc, c - w);
            if((c / w) < h - 1) next_step(grid, visited, next, hc, c + w);
        }
    }
    p1--; // gets incremented an extra time after breaking out of inner loop

    FuriString* part_1 = furi_string_alloc_printf("%d", p1);
    FuriString* part_2 = furi_string_alloc_printf("%d", p2);

    update_results(aoc, part_1, part_2);

    array_int_clear(current);
    array_int_clear(next);
    bitset_clear(visited);

    furi_string_free(part_2);
    furi_string_free(part_1);
    furi_string_free(grid);
    furi_string_free(line);
}
