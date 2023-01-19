#include "advent_of_code.h"

void solve_day01(AdventOfCode* aoc, Stream* file_stream) {
    FuriString* line = furi_string_alloc();

    int max[3] = {0};
    int elf = 0;

    while(true) {
        if(!stream_read_line(file_stream, line)) break;
        furi_string_trim(line);

        if(furi_string_empty(line)) {
            if(elf > max[0]) {
                max[2] = max[1];
                max[1] = max[0];
                max[0] = elf;
            } else if(elf > max[1]) {
                max[2] = max[1];
                max[1] = elf;
            } else if(elf > max[2]) {
                max[2] = elf;
            }
            elf = 0;
            continue;
        }
        unsigned long n = strtoul(furi_string_get_cstr(line), NULL, 10);
        elf += n;
    }

    if(elf > max[0]) {
        max[2] = max[1];
        max[1] = max[0];
        max[0] = elf;
    } else if(elf > max[1]) {
        max[2] = max[1];
        max[1] = elf;
    } else if(elf > max[2]) {
        max[2] = elf;
    }

    FuriString* part_1 = furi_string_alloc_printf("%d", max[0]);
    FuriString* part_2 = furi_string_alloc_printf("%d", max[0] + max[1] + max[2]);

    update_results(aoc, part_1, part_2);

    furi_string_free(part_2);
    furi_string_free(part_1);
    furi_string_free(line);
}
