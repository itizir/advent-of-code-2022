#include "advent_of_code.h"

// slightly more convenient that furi_string
ARRAY_DEF(array_char, char)

void solve_day05(AdventOfCode* aoc, Stream* file_stream) {
    FuriString* line = furi_string_alloc();

    FuriString* part_1 = furi_string_alloc();
    FuriString* part_2 = furi_string_alloc();

    // don't want to bother making this dynamic. my input needs 9.
    // maybe some inputs need more? guessing 16 is enough...
    // would it even fit on screen? ha!
    array_char_t stacks_1[16];
    for(int i = 0; i < 16; i++) array_char_init(stacks_1[i]);
    // one extra array for convenience (use as tmp one)
    array_char_t stacks_2[17];
    for(int i = 0; i < 17; i++) array_char_init(stacks_2[i]);
    int num_stacks = 0;

    while(true) {
        if(!stream_read_line(file_stream, line)) break;

        if(num_stacks == 0) {
            num_stacks = furi_string_size(line) / 4;
            if(num_stacks > 16) {
                break;
            }
            if(num_stacks < 2 || furi_string_size(line) % 4 != 0) {
                break;
            }
        }

        if(furi_string_size(line) == 0 || furi_string_get_char(line, 0) != '[') {
            stream_read_line(file_stream, line); // throw away one more line
            break;
        }

        for(size_t i = 0; i < furi_string_size(line); i++) {
            char c = furi_string_get_char(line, i);
            if(c != '[') continue;
            c = furi_string_get_char(line, ++i);
            array_char_push_at(stacks_1[i / 4], 0, c);
            array_char_push_at(stacks_2[i / 4], 0, c);
        }
    }

    while(true) {
        if(!stream_read_line(file_stream, line)) break;
        furi_string_trim(line);

        char* l;
        unsigned long num = strtoul(furi_string_get_cstr(line) + 5, &l, 10);
        unsigned long from = strtoul(l + 6, &l, 10) - 1;
        unsigned long to = strtoul(l + 4, &l, 10) - 1;

        char c = 0;
        for(; num > 0; num--) {
            array_char_pop_back(&c, stacks_1[from]);
            array_char_push_back(stacks_1[to], c);

            array_char_pop_back(&c, stacks_2[from]);
            array_char_push_back(stacks_2[16], c);
        }
        while(array_char_size(stacks_2[16]) > 0) {
            array_char_pop_back(&c, stacks_2[16]);
            array_char_push_back(stacks_2[to], c);
        }
    }

    for(int i = 0; i < num_stacks; i++) {
        if(array_char_size(stacks_1[i]) == 0) continue;
        char c = 0;
        array_char_pop_back(&c, stacks_1[i]);
        furi_string_push_back(part_1, c);
    }
    for(int i = 0; i < num_stacks; i++) {
        if(array_char_size(stacks_2[i]) == 0) continue;
        char c = 0;
        array_char_pop_back(&c, stacks_2[i]);
        furi_string_push_back(part_2, c);
    }

    for(int i = 0; i < 16; i++) array_char_clear(stacks_1[i]);
    for(int i = 0; i < 17; i++) array_char_clear(stacks_2[i]);

    update_results(aoc, part_1, part_2);

    furi_string_free(part_2);
    furi_string_free(part_1);
    furi_string_free(line);
}
