#include "advent_of_code.h"

void solve_day10(AdventOfCode* aoc, Stream* file_stream) {
    FuriString* line = furi_string_alloc();

    int p1 = 0;

    int cycle = 0;
    int x = 1;
    int interest = 20;
    int step = 40;
    int stop = 220;

    uint8_t screen[1 + 240 / 8] = {0};
    Image img;
    img.width = 40;
    img.height = 6;
    img.data = screen;

    while(true) {
        if(!stream_read_line(file_stream, line)) break;
        furi_string_trim(line);

        if(abs(x - cycle % step) <= 1) {
            screen[1 + cycle / 8] |= 1 << (cycle % 8);
        }
        cycle++;

        const char* l = furi_string_get_cstr(line);
        if(l[0] == 'n') {
            continue;
        }

        if(abs(x - cycle % step) <= 1) {
            screen[1 + cycle / 8] |= 1 << (cycle % 8);
        }
        cycle++;

        if(cycle >= interest && interest <= stop) {
            p1 += interest * x;
            interest += step;
        }

        x += strtol(l + 5, NULL, 10);
    }

    FuriString* part_1 = furi_string_alloc_printf("%d", p1);

    update_results(aoc, part_1, NULL);
    update_images(aoc, NULL, &img);

    furi_string_free(part_1);
    furi_string_free(line);
}
