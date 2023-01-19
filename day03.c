#include "advent_of_code.h"

void solve_day03(AdventOfCode* aoc, Stream* file_stream) {
    FuriString* line = furi_string_alloc();

    int p1 = 0;
    int p2 = 0;
    int b = 0;
    uint64_t badge = 0;

    while(true) {
        if(!stream_read_line(file_stream, line)) break;
        furi_string_trim(line);

        size_t len = furi_string_size(line);
        if(len == 0 || len % 2 == 1) {
            break;
        }
        size_t mid = len / 2;

        const char* l = furi_string_get_cstr(line);

        uint64_t first = 0;
        uint64_t second = 0;

        for(size_t i = 0; i < mid; i++) {
            char c = l[i];
            char d = l[i + mid];

            if(c >= 'a')
                c += 1 - 'a';
            else
                c += 27 - 'A';
            if(d >= 'a')
                d += 1 - 'a';
            else
                d += 27 - 'A';

            first |= (uint64_t)1 << (uint64_t)(c - 1);
            second |= (uint64_t)1 << (uint64_t)(d - 1);
        }

        uint64_t overlap = first & second;
        int p = 0;
        while(overlap > 0) {
            p++;
            overlap >>= 1;
        }
        p1 += p;

        if(b % 3 == 0)
            badge = first | second;
        else
            badge &= first | second;
        if(b % 3 == 2) {
            p = 0;
            while(badge > 0) {
                p++;
                badge >>= 1;
            }
            p2 += p;
        }
        b++;
    }

    FuriString* part_1 = furi_string_alloc_printf("%d", p1);
    FuriString* part_2 = furi_string_alloc_printf("%d", p2);

    update_results(aoc, part_1, part_2);

    furi_string_free(part_2);
    furi_string_free(part_1);
    furi_string_free(line);
}
