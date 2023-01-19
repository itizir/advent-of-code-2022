#include "advent_of_code.h"

typedef struct {
    uint8_t o, c, s, g;
} Elements;

typedef struct {
    Elements r, s;
    uint8_t t;
} State;

ARRAY_DEF(states, State, M_POD_OPLIST)

int max_geodes(int max_t, int oo, int co, int so, int sc, int go, int gs);

void solve_day19(AdventOfCode* aoc, Stream* file_stream) {
    FuriString* line = furi_string_alloc();

    int p1 = 0;
    int p2 = 1;

    FuriString* part_1 = furi_string_alloc();
    FuriString* part_2 = furi_string_alloc();

    while(true) {
        if(!stream_read_line(file_stream, line)) break;
        furi_string_trim(line);

        char* ptr = (char*)furi_string_get_cstr(line);

        int id = strtol(ptr + 10, &ptr, 10);
        int oo = strtol(ptr + 23, &ptr, 10);
        int co = strtol(ptr + 28, &ptr, 10);
        int so = strtol(ptr + 32, &ptr, 10);
        int sc = strtol(ptr + 9, &ptr, 10);
        int go = strtol(ptr + 30, &ptr, 10);
        int gs = strtol(ptr + 9, &ptr, 10);

        p1 += id * max_geodes(24, oo, co, so, sc, go, gs);
        if(id <= 3)
            p2 *= max_geodes(32, oo, co, so, sc, go, gs);
        else if(id == 4) {
            // assuming here blueprints are in order. seems reasonable
            furi_string_printf(part_2, "%d", p2);
            update_results(aoc, NULL, part_2);
        }
    }

    furi_string_printf(part_1, "%d", p1);
    update_results(aoc, part_1, part_2);

    furi_string_free(part_2);
    furi_string_free(part_1);
    furi_string_free(line);
}

int max_geodes(int tmax, int oo, int co, int so, int sc, int go, int gs) {
    int retval = 0;
    int max_o = M_MAX(M_MAX(oo, co), M_MAX(so, go));

    states_t states;
    states_init(states);

    State st, n;
    st.t = 0;
    st.r = (Elements){1, 0, 0, 0};
    st.s = (Elements){0, 0, 0, 0};
    states_push_back(states, st);

    while(!states_empty_p(states)) {
        states_pop_back(&st, states);
        int t;

        retval = M_MAX(retval, st.s.g + st.r.g * (tmax - st.t));

        if(retval >= st.s.g + st.r.g * (tmax - st.t) + (tmax - st.t) * (tmax - st.t - 1) / 2)
            continue;

        t = st.s.o >= oo ? 1 : 2 + (oo - st.s.o - 1) / st.r.o;
        if(st.t + t < tmax && st.r.o < max_o) {
            n.t = st.t + t;
            n.r = st.r;
            n.r.o++;
            n.s = st.s;
            n.s.o += t * st.r.o - oo;
            n.s.c += t * st.r.c;
            n.s.s += t * st.r.s;
            n.s.g += t * st.r.g;
            states_push_back(states, n);
        }

        t = st.s.o >= co ? 1 : 2 + (co - st.s.o - 1) / st.r.o;
        if(st.t + t < tmax && st.r.c < sc) {
            n.t = st.t + t;
            n.r = st.r;
            n.r.c++;
            n.s = st.s;
            n.s.o += t * st.r.o - co;
            n.s.c += t * st.r.c;
            n.s.s += t * st.r.s;
            n.s.g += t * st.r.g;
            states_push_back(states, n);
        }

        if(st.r.c > 0) {
            t = M_MAX(
                st.s.o >= so ? 1 : 2 + (so - st.s.o - 1) / st.r.o,
                st.s.c >= sc ? 1 : 2 + (sc - st.s.c - 1) / st.r.c);
            if(st.t + t < tmax && st.r.s < gs) {
                n.t = st.t + t;
                n.r = st.r;
                n.r.s++;
                n.s = st.s;
                n.s.o += t * st.r.o - so;
                n.s.c += t * st.r.c - sc;
                n.s.s += t * st.r.s;
                n.s.g += t * st.r.g;
                states_push_back(states, n);
            }
        }

        if(st.r.s > 0) {
            t = M_MAX(
                st.s.o >= go ? 1 : 2 + (go - st.s.o - 1) / st.r.o,
                st.s.s >= gs ? 1 : 2 + (gs - st.s.s - 1) / st.r.s);
            if(st.t + t < tmax) {
                n.t = st.t + t;
                n.r = st.r;
                n.r.g++;
                n.s = st.s;
                n.s.o += t * st.r.o - go;
                n.s.c += t * st.r.c;
                n.s.s += t * st.r.s - gs;
                n.s.g += t * st.r.g;
                states_push_back(states, n);
            }
        }
    }

    states_clear(states);

    return retval;
}
