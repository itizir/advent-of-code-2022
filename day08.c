#include "advent_of_code.h"

void solve_day08(AdventOfCode* aoc, Stream* file_stream) {
    FuriString* line = furi_string_alloc();

    int p1 = 0;
    int p2 = 0;

    FuriString* forest = furi_string_alloc();

    size_t w = 0;
    size_t h = 0;
    while(true) {
        if(!stream_read_line(file_stream, line)) break;
        furi_string_trim(line);
        w = furi_string_size(line);
        h++;
        furi_string_cat(forest, line);
    }
    size_t area = w * h;

    bitset_t visible;
    bitset_init(visible);
    bitset_resize(visible, area);

    array_int_t scores;
    array_int_init(scores);
    array_int_resize(scores, area);
    for(size_t i = 0; i < area; i++) array_int_set_at(scores, i, 1);

    char a, b, c;
    int last_a[10];
    int last_b[10];

    for(size_t y = 0; y < h; y++) {
        a = '0' - 1;
        b = '0' - 1;
        for(size_t i = 0; i < 10; i++) {
            last_a[i] = 0;
            last_b[i] = w - 1;
        }
        for(size_t x = 0; x < w; x++) {
            size_t i = x + (y * w);
            size_t j = (w - 1 - x) + (y * w);

            c = furi_string_get_char(forest, i);
            if(c > a) {
                a = c;
                if(!bitset_get(visible, i)) {
                    p1++;
                    bitset_set_at(visible, i, true);
                }
            }

            int closest = w;
            for(size_t k = c - '0'; k < 10; k++) {
                int dst = x - last_a[k];
                if(dst < closest) {
                    closest = dst;
                    if(dst <= 1) break;
                }
            }
            last_a[c - '0'] = x;
            *array_int_get(scores, i) *= closest;

            c = furi_string_get_char(forest, j);
            if(c > b) {
                b = c;
                if(!bitset_get(visible, j)) {
                    p1++;
                    bitset_set_at(visible, j, true);
                }
            }

            closest = w;
            for(size_t k = c - '0'; k < 10; k++) {
                int dst = last_b[k] + 1 + x - w;
                if(dst < closest) {
                    closest = dst;
                    if(dst <= 1) break;
                }
            }
            last_b[c - '0'] = w - 1 - x;
            *array_int_get(scores, j) *= closest;
        }
    }
    for(size_t x = 0; x < w; x++) {
        a = '0' - 1;
        b = '0' - 1;
        for(size_t i = 0; i < 10; i++) {
            last_a[i] = 0;
            last_b[i] = h - 1;
        }
        for(size_t y = 0; y < h; y++) {
            size_t i = x + (y * w);
            size_t j = x + ((h - 1 - y) * w);

            c = furi_string_get_char(forest, i);
            if(c > a) {
                a = c;
                if(!bitset_get(visible, i)) {
                    p1++;
                    bitset_set_at(visible, i, true);
                }
            }

            int closest = h;
            for(size_t k = c - '0'; k < 10; k++) {
                int dst = y - last_a[k];
                if(dst < closest) {
                    closest = dst;
                    if(dst <= 1) break;
                }
            }
            last_a[c - '0'] = y;
            *array_int_get(scores, i) *= closest;

            c = furi_string_get_char(forest, j);
            if(c > b) {
                b = c;
                if(!bitset_get(visible, j)) {
                    p1++;
                    bitset_set_at(visible, j, true);
                }
            }

            closest = h;
            for(size_t k = c - '0'; k < 10; k++) {
                int dst = last_b[k] + 1 + y - h;
                if(dst < closest) {
                    closest = dst;
                    if(dst <= 1) break;
                }
            }
            last_b[c - '0'] = h - 1 - y;
            *array_int_get(scores, j) *= closest;
        }
    }

    for(size_t i = 0; i < area; i++) {
        int score = *array_int_get(scores, i);
        if(score > p2) p2 = score;
    }

    array_int_clear(scores);
    bitset_clear(visible);
    furi_string_free(forest);

    FuriString* part_1 = furi_string_alloc_printf("%d", p1);
    FuriString* part_2 = furi_string_alloc_printf("%d", p2);

    update_results(aoc, part_1, part_2);

    furi_string_free(part_2);
    furi_string_free(part_1);
    furi_string_free(line);
}
