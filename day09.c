#include "advent_of_code.h"

typedef struct {
    int x, y;
} Point;

int pt_to_i(Point pt, int w) {
    return pt.x + w * pt.y;
}

bool follow(Point* knots, int i, int nb) {
    bool moved = false;
    if(knots[i - 1].y - knots[i].y > 1) {
        knots[i].y++;
        moved = true;
    } else if(knots[i - 1].y - knots[i].y < -1) {
        knots[i].y--;
        moved = true;
    }
    if(moved) {
        if(knots[i - 1].x > knots[i].x)
            knots[i].x++;
        else if(knots[i - 1].x < knots[i].x)
            knots[i].x--;
    } else {
        if(knots[i - 1].x - knots[i].x > 1) {
            knots[i].x++;
            moved = true;
        } else if(knots[i - 1].x - knots[i].x < -1) {
            knots[i].x--;
            moved = true;
        }
        if(moved) {
            if(knots[i - 1].y > knots[i].y)
                knots[i].y++;
            else if(knots[i - 1].y < knots[i].y)
                knots[i].y--;
        }
    }
    if(!moved) return false;
    if(i >= nb - 1) return true;
    return follow(knots, i + 1, nb);
}

int num_visited(Stream* file_stream, Point start, int w, int h, int nb) {
    FuriString* line = furi_string_alloc();
    stream_rewind(file_stream);

    Point knots[nb];
    for(int i = 0; i < nb; i++) knots[i] = start;

    bitset_t visited;
    bitset_init(visited);
    bitset_resize(visited, w * h);
    bitset_set_at(visited, pt_to_i(start, w), true);

    while(true) {
        if(!stream_read_line(file_stream, line)) break;
        furi_string_trim(line);

        char c = furi_string_get_char(line, 0);
        int steps = strtoul(furi_string_get_cstr(line) + 1, NULL, 10);

        for(int i = 0; i < steps; i++) {
            switch(c) {
            case 'U':
                knots[0].y++;
                break;
            case 'D':
                knots[0].y--;
                break;
            case 'L':
                knots[0].x--;
                break;
            case 'R':
                knots[0].x++;
                break;
            }

            if(follow(knots, 1, nb)) {
                bitset_set_at(visited, pt_to_i(knots[nb - 1], w), true);
            }
        }
    }

    int resp = bitset_popcount(visited);

    bitset_clear(visited);
    furi_string_free(line);

    return resp;
}

void solve_day09(AdventOfCode* aoc, Stream* file_stream) {
    FuriString* line = furi_string_alloc();

    Point head = {0};
    int minx = 0, maxx = 0, miny = 0, maxy = 0;

    while(true) {
        if(!stream_read_line(file_stream, line)) break;
        furi_string_trim(line);

        char c = furi_string_get_char(line, 0);
        int steps = strtoul(furi_string_get_cstr(line) + 1, NULL, 10);

        switch(c) {
        case 'U':
            head.y += steps;
            if(head.y > maxy) maxy = head.y;
            break;
        case 'D':
            head.y -= steps;
            if(head.y < miny) miny = head.y;
            break;
        case 'L':
            head.x -= steps;
            if(head.x < minx) minx = head.x;
            break;
        case 'R':
            head.x += steps;
            if(head.x > maxx) maxx = head.x;
            break;
        }
    }

    head.x = -minx;
    head.y = -miny;
    int w = maxx - minx + 1;
    int h = maxy - miny + 1;

    int resp = num_visited(file_stream, head, w, h, 2);

    FuriString* part_1 = furi_string_alloc_printf("%d", resp);
    FuriString* part_2 = furi_string_alloc();

    update_results(aoc, part_1, part_2);

    resp = num_visited(file_stream, head, w, h, 10);
    furi_string_printf(part_2, "%d", resp);

    update_results(aoc, part_1, part_2);

    furi_string_free(part_2);
    furi_string_free(part_1);
    furi_string_free(line);
}
