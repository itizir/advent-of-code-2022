#include "advent_of_code.h"

int find_marker(FuriString* line, size_t size);

void solve_day06(AdventOfCode* aoc, Stream* file_stream) {
    FuriString* line = furi_string_alloc();

    int p1 = 0;
    int p2 = 0;

    while(true) {
        if(!stream_read_line(file_stream, line)) break;
        furi_string_trim(line);

        p1 = find_marker(line, 4);
        p2 = find_marker(line, 14);
    }

    FuriString* part_1 = furi_string_alloc_printf("%d", p1);
    FuriString* part_2 = furi_string_alloc_printf("%d", p2);

    update_results(aoc, part_1, part_2);

    furi_string_free(part_2);
    furi_string_free(part_1);
    furi_string_free(line);
}

int find_marker(FuriString* line, size_t size) {
    int dupes = 0;
    int chars[26] = {0};

    for(size_t i = 0; i < furi_string_size(line); i++) {
        size_t c = furi_string_get_char(line, i) - 'a';
        if(chars[c]++ > 0) dupes++;
        if(i > size - 1) {
            c = furi_string_get_char(line, i - size) - 'a';
            if(chars[c]-- > 1) dupes--;
        }
        if(i >= size - 1 && dupes == 0) {
            return i + 1;
        }
    }

    return -1;
}
