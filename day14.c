#include "advent_of_code.h"

// rule no 1: keep it dumb and simple if that's good enough

#define W 400
#define H 200

static int id(int x, int y) {
    return W * y + x;
}

int fall(char* cave, int x, int y) {
    while(y < H - 1 && cave[id(x, y + 1)] == '.') y++;
    if(y == H - 1) return 0;
    if(cave[id(x - 1, y + 1)] == '.') return fall(cave, x - 1, y + 1);
    if(cave[id(x + 1, y + 1)] == '.') return fall(cave, x + 1, y + 1);
    return id(x, y);
}

void solve_day14(AdventOfCode* aoc, Stream* file_stream) {
    FuriString* line = furi_string_alloc();

    int maxy = 0;

    char* cave;
    cave = malloc(W * H);

    for(size_t i = 0; i < W * H; i++) cave[i] = '.';

    while(true) {
        if(!stream_read_line(file_stream, line)) break;
        furi_string_trim(line);

        char* end;
        int x0 = strtol(furi_string_get_cstr(line), &end, 10);
        x0 += W / 2 - 500;
        int y0 = strtol(end + 1, &end, 10);
        if(y0 > maxy) maxy = y0;

        while(*end == ' ') {
            int x1 = strtol(end + 4, &end, 10);
            x1 += W / 2 - 500;
            int y1 = strtol(end + 1, &end, 10);

            int mx = M_MAX(x0, x1);
            int my = M_MAX(y0, y1);
            for(int x = M_MIN(x0, x1); x <= mx; x++)
                for(int y = M_MIN(y0, y1); y <= my; y++) cave[id(x, y)] = '#';

            x0 = x1;
            y0 = y1;
            if(y0 > maxy) maxy = y0;
        }
    }

    int p = 0;

    while(true) {
        int i = fall(cave, W / 2, 0);
        if(i == 0) {
            furi_string_printf(line, "%d", p);
            update_results(aoc, line, NULL);
            for(int x = 0; x < W; x++) cave[id(x, maxy + 2)] = '#';
            continue;
        }
        cave[i] = 'o';
        p++;
        if(i == W / 2) break;
    }

    furi_string_printf(line, "%d", p);
    update_results(aoc, NULL, line);

    free(cave);
    furi_string_free(line);
}
