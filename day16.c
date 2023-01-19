#include "advent_of_code.h"

typedef struct {
    uint16_t visited;
    uint16_t f, h;
    int8_t t, v;
    int8_t te, ve;
} Step;

typedef struct {
    Step* steps;
    int n, cap;
} StepPQ;

static inline int cmp_step(const Step a, const Step b) {
    return a.h > b.h ? -1 : a.h < b.h;
}

void step_pq_push(StepPQ* pq, Step step) {
    int i = pq->n >= pq->cap ? pq->n - 1 : pq->n++;
    pq->steps[i] = step;
    while(i > 0 && cmp_step(pq->steps[(i - 1) / 2], pq->steps[i]) > 0) {
        M_SWAP(Step, pq->steps[(i - 1) / 2], pq->steps[i]);
        i = (i - 1) / 2;
    }
}

void step_pq_pop(StepPQ* pq, Step* step) {
    *step = pq->steps[0];
    M_SWAP(Step, pq->steps[0], pq->steps[pq->n - 1]);
    pq->n--;
    int i = 0;
    while(2 * i + 1 < pq->n) {
        if(2 * i + 2 < pq->n && cmp_step(pq->steps[2 * i + 1], pq->steps[2 * i + 2]) > 0 &&
           cmp_step(pq->steps[i], pq->steps[2 * i + 2]) > 0) {
            M_SWAP(Step, pq->steps[i], pq->steps[2 * i + 2]);
            i = 2 * i + 2;
        } else if(cmp_step(pq->steps[i], pq->steps[2 * i + 1]) > 0) {
            M_SWAP(Step, pq->steps[i], pq->steps[2 * i + 1]);
            i = 2 * i + 1;
        } else
            break;
    }
}

int find_max_flow(
    StepPQ* steps,
    int** dst,
    int* flows,
    int num_valves,
    int cutoff,
    bool greedy,
    bool elephant);

void solve_day16(AdventOfCode* aoc, Stream* file_stream) {
    FuriString* line = furi_string_alloc();
    FuriString* tmp = furi_string_alloc();

    int p1 = 0;
    int p2 = 0;

    dict_string_int_t names;
    dict_string_int_init(names);

    int num_caves = 1;
    int num_valves = 0;
    while(true) {
        if(!stream_read_line(file_stream, line)) break;
        furi_string_trim(line);

        int flow = strtol(furi_string_get_cstr(line) + 23, NULL, 10);
        furi_string_mid(line, 6, 2);

        if(furi_string_equal(line, "AA")) {
            dict_string_int_set_at(names, line, 1000);
            continue;
        }
        if(flow > 0) {
            dict_string_int_set_at(names, line, num_valves);
            num_valves++;
        } else {
            dict_string_int_set_at(names, line, 1000 + num_caves - num_valves);
        }
        num_caves++;
    }

    dict_string_int_it_t it;
    for(dict_string_int_it(it, names); !dict_string_int_end_p(it); dict_string_int_next(it)) {
        struct dict_string_int_pair_s* kv = dict_string_int_ref(it);
        if(kv->value >= 1000) kv->value += num_valves - 1000;
    }
    stream_rewind(file_stream);

    int** dst;
    dst = malloc(num_caves * sizeof(int*));
    for(int i = 0; i < num_caves; i++) {
        dst[i] = malloc(num_caves * sizeof(int));
        for(int j = 0; j < num_caves; j++) dst[i][j] = i == j ? 0 : 9999;
    }

    int* flows;
    flows = malloc(num_valves * sizeof(int));

    while(true) {
        if(!stream_read_line(file_stream, line)) break;
        furi_string_trim(line);

        furi_string_set_n(tmp, line, 6, 2);
        int from = *dict_string_int_get(names, tmp);

        char* end;
        int flow = strtol(furi_string_get_cstr(line) + 23, &end, 10);
        if(flow > 0) flows[from] = flow;
        end += 23;
        if(*end == ' ') end--;
        furi_string_set(line, end);

        while(!furi_string_empty(line)) {
            furi_string_set_n(tmp, line, 2, 2);
            furi_string_right(line, 4);

            int to = *dict_string_int_get(names, tmp);
            dst[from][to] = 1;
            dst[to][from] = 1;
        }
    }

    for(int k = 0; k < num_caves; k++)
        for(int i = 0; i < num_caves - 1; i++)
            for(int j = i + 1; j < num_caves; j++) {
                if(dst[k][i] + dst[k][j] < dst[i][j]) {
                    dst[i][j] = dst[k][i] + dst[k][j];
                    dst[j][i] = dst[i][j];
                }
            }

    for(int i = 0; i <= num_valves - 1; i++)
        for(int j = i + 1; j <= num_valves; j++) {
            dst[i][j]++;
            dst[j][i]++;
        }

    StepPQ steps;
    steps.steps = malloc(5000 * sizeof(Step));
    steps.cap = 5000;
    steps.n = 0;

    int cutoff = find_max_flow(&steps, dst, flows, num_valves, 0, true, false);
    p1 = find_max_flow(&steps, dst, flows, num_valves, cutoff, false, false);
    FuriString* part_1 = furi_string_alloc_printf("%d", p1);
    update_results(aoc, part_1, NULL);

    cutoff = find_max_flow(&steps, dst, flows, num_valves, 0, true, true);
    p2 = find_max_flow(&steps, dst, flows, num_valves, cutoff, false, true);
    FuriString* part_2 = furi_string_alloc_printf("%d", p2);
    update_results(aoc, part_1, part_2);

    free(steps.steps);
    free(flows);
    for(int j = 0; j < num_caves; j++) free(dst[j]);
    free(dst);

    dict_string_int_clear(names);
    furi_string_free(part_2);
    furi_string_free(part_1);
    furi_string_free(tmp);
    furi_string_free(line);
}

int find_max_flow(
    StepPQ* steps,
    int** dst,
    int* flows,
    int num_valves,
    int cutoff,
    bool greedy,
    bool elephant) {
    steps->n = 0;
    Step prev, next, best_step;
    int maxflow = cutoff;

    next.t = elephant ? 26 : 30;
    next.v = num_valves;
    next.te = elephant ? 26 : 0;
    next.ve = elephant ? num_valves : 0;
    next.f = 0;
    next.h = cutoff;
    next.visited = 0;
    step_pq_push(steps, next);

    while(steps->n > 0) {
        step_pq_pop(steps, &prev);

        if(prev.f > maxflow) maxflow = prev.f;
        if(prev.h < maxflow) continue;

        if(prev.te > prev.t) {
            M_SWAP(int8_t, prev.te, prev.t);
            M_SWAP(int8_t, prev.ve, prev.v);
        }

        int maxh = 0;
        bool opened_all = true;
        for(int i = 0; i < num_valves; i++) {
            if(prev.visited & (1 << i)) continue;
            opened_all = false;

            for(int j = 0; j < num_valves; j++) {
                if(prev.ve == num_valves && j < i) continue; // break the symmetry
                if(prev.visited & (1 << j) || j == i) continue;

                next.f = prev.f;
                next.v = i;
                next.t = prev.t - dst[prev.v][i];
                if(next.t > 0) next.f += next.t * flows[i];

                next.ve = j;
                if(prev.te > 0)
                    next.te = prev.te - dst[prev.ve][j];
                else {
                    next.te = 0;
                    j = num_valves;
                }
                if(next.te > 0) next.f += next.te * flows[j];

                if(next.t <= 0 && next.te <= 0) continue;

                next.h = next.f;
                next.visited = prev.visited | (1 << i) | (1 << j);
                for(int k = 0; k < num_valves; k++) {
                    if(next.visited & (1 << k)) continue;
                    next.h += M_MAX(0, M_MAX(next.t - dst[i][k], next.te - dst[j][k])) * flows[k];
                }

                if(greedy) {
                    if(maxh < next.h) {
                        maxh = next.h;
                        best_step = next;
                    }
                    continue;
                }

                if(next.h > maxflow) step_pq_push(steps, next);
            }
        }

        if(opened_all) {
            maxflow = prev.f;
            break;
        }

        if(greedy && maxh > 0) step_pq_push(steps, best_step);
    }

    return maxflow;
}
