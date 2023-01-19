#include "advent_of_code.h"

void solve_day02(AdventOfCode* aoc, Stream* file_stream) {
    FuriString* line = furi_string_alloc();

    int p1 = 0;
    int p2 = 0;

    while(true) {
        if(!stream_read_line(file_stream, line)) break;
        furi_string_trim(line);

        size_t len = furi_string_size(line);
        if(len != 3) {
            break;
        }

        const char* l = furi_string_get_cstr(line);
        int opponent = l[0] - 'A';
        int me = l[2] - 'X';

        p1 += 1 + me;
        if(me == opponent) {
            p1 += 3;
        } else if(me - opponent == 1 || (me == 0 && opponent == 2)) {
            p1 += 6;
        }
        p2 += 3 * me;
        p2 += 1 + (opponent + me + 2) % 3;
    }

    FuriString* part_1 = furi_string_alloc_printf("%d", p1);
    FuriString* part_2 = furi_string_alloc_printf("%d", p2);

    update_results(aoc, part_1, part_2);

    furi_string_free(part_2);
    furi_string_free(part_1);
    furi_string_free(line);
}
