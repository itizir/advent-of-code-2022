#include "advent_of_code.h"

DICT_SET_DEF(dict_u16, uint16_t)

static inline uint16_t ptu(int8_t i, int8_t j) {
    return (i << 8) | j;
}

static inline int8_t utj(uint16_t u) {
    return u & 0xFF;
}

static inline int8_t uti(uint16_t u) {
    return utj(u >> 8);
}

static inline bool is_safe(char blz[], int w, int h, int i, int j, int t) {
    return i >= 0 && i < h && j >= 0 && j < w && blz[((i + t) % h) * w + j] != '^' &&
           blz[i * w + (j + t) % w] != '<' && blz[((i + h - (t % h)) % h) * w + j] != 'v' &&
           blz[i * w + (j + w - (t % w)) % w] != '>';
}

int fend_through(char blz[], int w, int h, int t0, int8_t i0, int8_t j0, int8_t i1, int8_t j1);

void solve_day24(AdventOfCode* aoc, Stream* file_stream) {
    FuriString* line = furi_string_alloc();

    int w = 0;
    int h = -2;

    while(true) {
        if(!stream_read_line(file_stream, line)) break;
        furi_string_trim(line);
        w = furi_string_size(line) - 2;
        h++;
    }

    char* blz;
    blz = malloc(h * w);

    int start = 0, end = 0;
    stream_rewind(file_stream);
    for(int i = -1; i <= h; i++) {
        if(!stream_read_line(file_stream, line)) break;
        furi_string_trim(line);

        if(i == -1)
            start = furi_string_search_char(line, '.') - 1;
        else if(i == h)
            end = furi_string_search_char(line, '.') - 1;
        else {
            memcpy(blz + (i * w), furi_string_get_cstr(line) + 1, w);
        }
    }

    int t = 0;
    t = fend_through(blz, w, h, t, 0, start, h - 1, end);
    furi_string_printf(line, "%d", t);
    update_results(aoc, line, NULL);

    t = fend_through(blz, w, h, t, h - 1, end, 0, start);
    t = fend_through(blz, w, h, t, 0, start, h - 1, end);
    furi_string_printf(line, "%d", t);
    update_results(aoc, NULL, line);

    free(blz);
    furi_string_free(line);
}

int fend_through(char blz[], int w, int h, int t0, int8_t i0, int8_t j0, int8_t i1, int8_t j1) {
    dict_u16_t points;
    dict_u16_init(points);
    dict_u16_t next;
    dict_u16_init(next);

    int t = t0 + 1;
    for(;; t++) {
        bool arrived = false;
        // could maybe switch to a* once close enough to fit in memory?
        // bah: it's now working, and it's not about speed...
        dict_u16_it_t it;
        for(dict_u16_it(it, points); !dict_u16_end_p(it); dict_u16_next(it)) {
            uint16_t u = *dict_u16_ref(it);
            int8_t i = uti(u), j = utj(u);
            if(abs(i1 - i) + abs(j1 - j) == 1 && is_safe(blz, w, h, i1, j1, t)) {
                t++;
                arrived = true;
                break;
            }
            if(is_safe(blz, w, h, i, j, t)) dict_u16_push(next, u);
            i--;
            if(is_safe(blz, w, h, i, j, t)) dict_u16_push(next, ptu(i, j));
            i += 2;
            if(is_safe(blz, w, h, i, j, t)) dict_u16_push(next, ptu(i, j));
            i--;
            j--;
            if(is_safe(blz, w, h, i, j, t)) dict_u16_push(next, ptu(i, j));
            j += 2;
            if(is_safe(blz, w, h, i, j, t)) dict_u16_push(next, ptu(i, j));
        }
        if(arrived) break;

        if(is_safe(blz, w, h, i0, j0, t)) dict_u16_push(next, ptu(i0, j0));

        dict_u16_reset(points);
        dict_u16_swap(points, next);
    }

    dict_u16_clear(next);
    dict_u16_clear(points);

    return t;
}
