#include "advent_of_code.h"

uint64_t parse_fuel(const char* c) {
    uint64_t f = 0;
    for(; *c != 0; c++) {
        f *= 5;
        switch(*c) {
        default:
            f += *c - '0';
            break;
        case '=':
            f -= 2;
            break;
        case '-':
            f--;
            break;
        case 0:
            break;
        }
    }
    return f;
}

void print_fuel(FuriString* str, uint64_t f) {
    furi_string_reset(str);
    char c;
    while(f > 0) {
        switch(f % 5) {
        default:
            c = '0' + f % 5;
            f -= f % 5;
            break;
        case 3:
            c = '=';
            f += 3;
            break;
        case 4:
            c = '-';
            f += 4;
            break;
        }
        f /= 5;
        furi_string_push_back(str, c);
    }
    size_t l = furi_string_size(str);
    char* cstr = (char*)furi_string_get_cstr(str);
    for(size_t i = 0; i < l / 2; i++) {
        M_SWAP(char, cstr[i], cstr[l - 1 - i]);
    }
}

void solve_day25(AdventOfCode* aoc, Stream* file_stream) {
    FuriString* line = furi_string_alloc();

    uint64_t tot = 0;
    while(true) {
        if(!stream_read_line(file_stream, line)) break;
        furi_string_trim(line);
        tot += parse_fuel(furi_string_get_cstr(line));
    }

    print_fuel(line, tot);

    FuriString* overflow = furi_string_alloc_set(line);
    furi_string_left(line, 10);
    furi_string_right(overflow, 10);

    update_results(aoc, line, overflow);

    furi_string_free(overflow);
    furi_string_free(line);
}
