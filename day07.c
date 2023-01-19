#include "advent_of_code.h"

void solve_day07(AdventOfCode* aoc, Stream* file_stream) {
    FuriString* line = furi_string_alloc();

    int p1 = 0;
    int p2 = 0;

    dict_string_int_t dir_sizes;
    dict_string_int_init(dir_sizes);

    FuriString* pwd = furi_string_alloc();
    array_string_t dirs;
    array_string_init(dirs);

    while(true) {
        if(!stream_read_line(file_stream, line)) break;
        furi_string_trim(line);

        if(furi_string_get_char(line, 0) != '$') {
            char* end;
            unsigned long size = strtoul(furi_string_get_cstr(line), &end, 10);
            furi_string_set(line, end + 1);

            array_string_it_t it;
            for(array_string_it(it, dirs); !array_string_end_p(it); array_string_next(it)) {
                FuriString* dir = *array_string_ref(it);
                int* s = dict_string_int_get(dir_sizes, dir);
                if(s)
                    *s += size;
                else
                    dict_string_int_set_at(dir_sizes, dir, size);
            }

            continue;
        }

        if(furi_string_size(line) < 4) {
            break;
        }
        if(furi_string_size(line) == 4) {
            // should be '$ ls', ignore
            continue;
        }
        furi_string_right(line, 5);

        if(furi_string_equal(line, "..")) {
            array_string_pop_back(NULL, dirs);
            if(array_string_back(dirs)) {
                furi_string_set(pwd, *array_string_back(dirs));
            }
            continue;
        }

        if(!furi_string_equal(line, "/")) {
            furi_string_cat(line, "/");
        }
        furi_string_cat(pwd, line);
        array_string_push_back(dirs, pwd);
    }

    furi_string_set(pwd, "/");
    // at this point just assume it exists...
    p2 = *dict_string_int_get(dir_sizes, pwd);
    int min = p2 - 40000000;

    dict_string_int_it_t it;
    for(dict_string_int_it(it, dir_sizes); !dict_string_int_end_p(it); dict_string_int_next(it)) {
        const struct dict_string_int_pair_s* kv = dict_string_int_cref(it);
        if(kv->value <= 100000) p1 += kv->value;
        if(kv->value >= min && kv->value < p2) p2 = kv->value;
    }

    array_string_clear(dirs);
    furi_string_free(pwd);
    dict_string_int_clear(dir_sizes);

    FuriString* part_1 = furi_string_alloc_printf("%d", p1);
    FuriString* part_2 = furi_string_alloc_printf("%d", p2);

    update_results(aoc, part_1, part_2);

    furi_string_free(part_2);
    furi_string_free(part_1);
    furi_string_free(line);
}
