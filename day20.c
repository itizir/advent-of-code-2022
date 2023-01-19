#include "advent_of_code.h"

typedef struct {
    int16_t val;
    uint16_t p, n;
} Node;

int64_t decrypt(Stream* file_stream, const int64_t key, const int num_rounds);

void solve_day20(AdventOfCode* aoc, Stream* file_stream) {
    int64_t p1 = decrypt(file_stream, 1, 1);
    FuriString* part_1 = furi_string_alloc_printf("%lld", p1);
    update_results(aoc, part_1, NULL);

    stream_rewind(file_stream);
    int64_t p2 = decrypt(file_stream, 811589153, 10);
    FuriString* part_2 = furi_string_alloc_printf("%lld", p2);

    update_results(aoc, part_1, part_2);

    furi_string_free(part_2);
    furi_string_free(part_1);
}

int64_t decrypt(Stream* file_stream, const int64_t key, const int num_rounds) {
    FuriString* line = furi_string_alloc();

    Node* nodes;
    nodes = malloc(sizeof(Node) * 5000); // assume all inputs are same (max) size...

    stream_read_line(file_stream, line);
    furi_string_trim(line);

    Node *n = nodes, *p;
    n->val = strtol(furi_string_get_cstr(line), NULL, 10);
    n->p = 0;
    n->n = 0;
    int i = 1;
    int start = 0;
    p = n;

    while(true) {
        if(!stream_read_line(file_stream, line)) break;
        furi_string_trim(line);

        n++;
        n->val = strtol(furi_string_get_cstr(line), NULL, 10);
        if(n->val == 0) start = i;
        n->p = i - 1;
        n->n = p->n;
        (nodes + p->n)->p = i;
        p->n = i;
        p = n;
        i++;
    }

    const int size = i;

    for(int k = 0; k < num_rounds; k++)
        for(int i = 0; i < size; i++) {
            n = nodes + i;

            int steps = n->val;
            while(steps < 0) steps += size - 1;
            steps %= size - 1;
            steps *= (key % (size - 1));
            steps %= size - 1;
            if(steps > size / 2) steps -= size;

            if(steps == 0) continue;
            int j = i;
            while(steps > 0) {
                j = (nodes + j)->n;
                steps--;
            }
            while(steps < 0) {
                j = (nodes + j)->p;
                steps++;
            }
            p = nodes + j;

            (nodes + n->p)->n = n->n;
            (nodes + n->n)->p = n->p;

            n->p = j;
            n->n = p->n;
            (nodes + p->n)->p = i;
            p->n = i;
        }

    int64_t retval = 0;
    for(int j = 0; j < 3; j++) {
        for(int i = 0; i < 1000; i++) start = (nodes + start)->n;
        retval += (nodes + start)->val;
    }

    free(nodes);
    furi_string_free(line);

    return retval * key;
}
