#include "advent_of_code.h"

ARRAY_DEF(array_ll, int64_t)

typedef struct {
    array_ll_t items;
    char op;
    int step; // 0 for item itself
    int test;
    size_t dest_true;
    size_t dest_false;
    int inspections;
} Monkey;

void inspect(Monkey* monkeys[], size_t i, int mod, bool panic_mode) {
    Monkey* m = monkeys[i];
    array_ll_it_t it;
    for(array_ll_it(it, m->items); !array_ll_end_p(it); array_ll_next(it)) {
        int64_t item = *array_ll_cref(it);
        if(m->op == '+') {
            item += m->step > 0 ? m->step : item;
        } else {
            item *= m->step > 0 ? m->step : item;
        }
        if(!panic_mode) item /= 3;
        item %= mod;
        array_ll_push_back(
            monkeys[item % m->test == 0 ? m->dest_true : m->dest_false]->items, item);
    }
    m->inspections += array_ll_size(m->items);
    array_ll_reset(m->items);
}

void parse_monkey(Stream* file_stream, FuriString* line, Monkey* monkey, int* mod) {
    stream_read_line(file_stream, line);
    char* end;
    array_ll_push_back(monkey->items, strtol(furi_string_get_cstr(line) + 18, &end, 10));
    while(*end == ',') {
        array_ll_push_back(monkey->items, strtol(end + 1, &end, 10));
    }

    stream_read_line(file_stream, line);
    monkey->op = furi_string_get_char(line, 23);
    monkey->step = strtol(furi_string_get_cstr(line) + 25, &end, 10);

    stream_read_line(file_stream, line);
    monkey->test = strtol(furi_string_get_cstr(line) + 21, &end, 10);
    *mod *= monkey->test;

    stream_read_line(file_stream, line);
    monkey->dest_true = strtol(furi_string_get_cstr(line) + 29, &end, 10);

    stream_read_line(file_stream, line);
    monkey->dest_false = strtol(furi_string_get_cstr(line) + 30, &end, 10);

    monkey->inspections = 0;

    stream_read_line(file_stream, line); // consume empty line
}

int64_t monkey_business(Stream* file_stream, bool panic_mode) {
    stream_rewind(file_stream);

    FuriString* line = furi_string_alloc();

    int num_rounds = panic_mode ? 10000 : 20;

    size_t num = 0;
    Monkey* monkeys[16] = {NULL}; // 8 in my input. surely same for all, or at least not too many?!
    int mod = 1;

    while(true) {
        if(!stream_read_line(file_stream, line)) break;
        furi_string_trim(line);

        Monkey* m = malloc(sizeof(Monkey));
        array_ll_init(m->items);
        parse_monkey(file_stream, line, m, &mod);
        monkeys[num++] = m;
    }

    for(int i = 0; i < num_rounds; i++)
        for(size_t m = 0; m < num; m++) inspect(monkeys, m, mod, panic_mode);

    int64_t m1 = 0, m2 = 0;
    for(size_t m = 0; m < num; m++) {
        int i = monkeys[m]->inspections;
        if(i > m1) {
            m2 = m1;
            m1 = i;
        } else if(i > m2)
            m2 = i;
    }

    for(size_t i = 0; i < num; i++) {
        array_ll_clear(monkeys[i]->items);
        free(monkeys[i]);
        monkeys[i] = NULL;
    }
    furi_string_free(line);

    return m1 * m2;
}

void solve_day11(AdventOfCode* aoc, Stream* file_stream) {
    FuriString* part_1 = furi_string_alloc_printf("%lld", monkey_business(file_stream, false));
    update_results(aoc, part_1, NULL);

    FuriString* part_2 = furi_string_alloc_printf("%lld", monkey_business(file_stream, true));
    update_results(aoc, part_1, part_2);

    furi_string_free(part_2);
    furi_string_free(part_1);
}
