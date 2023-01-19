#include "advent_of_code.h"

int cmp(const char*, const char*);

static int cmp_packets(FuriString* const* a, FuriString* const* b) {
    return cmp(furi_string_get_cstr(*a), furi_string_get_cstr(*b));
}

void solve_day13(AdventOfCode* aoc, Stream* file_stream) {
    FuriString* a = furi_string_alloc();
    FuriString* b = furi_string_alloc();

    int p1 = 0;
    int p2 = 0;
    int i = 1;

    array_string_t packets;
    array_string_init(packets);

    furi_string_set(a, "[[2]]");
    array_string_push_back(packets, a);
    furi_string_set(a, "[[6]]");
    array_string_push_back(packets, a);

    while(true) {
        if(!stream_read_line(file_stream, a)) break;
        furi_string_trim(a);
        if(!stream_read_line(file_stream, b)) break;
        furi_string_trim(b);

        if(cmp_packets(&a, &b) == -1) p1 += i;

        array_string_push_back(packets, a);
        array_string_push_back(packets, b);

        if(!stream_read_line(file_stream, a)) break;
        i++;
    }

    // could link against FW qsort, but symbol not exported
    // so would need to edit api_symbols.csv and flash custom build. boo.
    // alternatively use `special_stable_sort`, but looks like it's buggy:
    // crashing when using on string array directly (RAM issue or bug?)
    // but giving incorrect sort when using intermediate 'pointer/id' array.
    // and i'm not going to write my own sort function either...
    // anyway: ended up bringing qsort.c along for the ride!
    array_string_special_sort(packets, cmp_packets);

    // could do binary search if huge input. beh...
    int num = array_string_size(packets);
    for(int i = 0; i < num; i++) {
        FuriString* packet = *array_string_get(packets, i);
        if(furi_string_equal(packet, "[[2]]"))
            p2 = i + 1;
        else if(furi_string_equal(packet, "[[6]]")) {
            p2 *= i + 1;
            break; // we know [[6]] has to come after [[2]]
        }
    }

    furi_string_printf(a, "%d", p1);
    furi_string_printf(b, "%d", p2);

    update_results(aoc, a, b);

    array_string_clear(packets);
    furi_string_free(b);
    furi_string_free(a);
}

int cmp(const char* c1, const char* c2) {
    while(*c1 != 0 && *c2 != 0) {
        while(*c1 == '[' && *c2 == '[') {
            c1++;
            c2++;
        }
        while(*c1 == ']' && *c2 == ']') {
            c1++;
            c2++;
        }
        if(*c1 == ']' && *c2 != ']') {
            return -1;
        }
        if(*c1 != ']' && *c2 == ']') {
            return 1;
        }
        if(*c1 == ',' && *c2 == ',') {
            c1++;
            c2++;
            continue;
        }

        // assume valid input: at this point either both are number,
        // of one is a number and the other '['

        int l1 = 0;
        while(*c1 == '[') {
            c1++;
            l1++;
        }
        if(*c1 == ']') return -1;

        int l2 = 0;
        while(*c2 == '[') {
            c2++;
            l2++;
        }
        if(*c2 == ']') return 1;

        char* end;
        int v1 = strtol(c1, &end, 10);
        c1 = end;
        int v2 = strtol(c2, &end, 10);
        c2 = end;

        if(v1 != v2) {
            return v1 < v2 ? -1 : 1;
        }

        while(l1 > 0) {
            if(*c1 != ']') return 1;
            c1++;
            l1--;
        }
        while(l2 > 0) {
            if(*c2 != ']') return -1;
            c2++;
            l2--;
        }
    }

    return *c1 == 0 ? (*c2 == 0 ? 0 : -1) : 1;
}
