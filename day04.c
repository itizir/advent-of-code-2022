#include "advent_of_code.h"

void solve_day04(AdventOfCode* aoc, Stream* file_stream) {
    FuriString* line = furi_string_alloc();

    int p1 = 0;
    int p2 = 0;

    while(true) {
        if(!stream_read_line(file_stream, line)) break;
        furi_string_trim(line);

        char* end;
        long a = strtol(furi_string_get_cstr(line), &end, 10);
        long b = strtol(end + 1, &end, 10);
        long c = strtol(end + 1, &end, 10);
        long d = strtol(end + 1, &end, 10);
        if((a <= c && b >= d) || (c <= a && d >= b)) p1++;
        if((a <= c && c <= b) || (c <= a && a <= d)) p2++;
    }

    FuriString* part_1 = furi_string_alloc_printf("%d", p1);
    FuriString* part_2 = furi_string_alloc_printf("%d", p2);

    update_results(aoc, part_1, part_2);

    furi_string_free(part_2);
    furi_string_free(part_1);
    furi_string_free(line);
}
