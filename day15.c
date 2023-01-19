#include "advent_of_code.h"

DICT_SET_DEF(dict_int, int)

typedef struct {
    int x, y, d;
} Sensor;

ARRAY_DEF(sensors, Sensor, M_POD_OPLIST)

typedef struct {
    int x0, x1;
} Segment;

static int cmp(const Segment* a, const Segment* b) {
    int ax = a->x0;
    int bx = b->x0;
    return (ax > bx) - (bx > ax);
}

ARRAY_DEF(segments, Segment, M_POD_OPLIST)

int min_steps(sensors_t sensors, int x, int y) {
    int steps = 0;
    sensors_it_t it;
    for(sensors_it(it, sensors); !sensors_end_p(it); sensors_next(it)) {
        Sensor sr = *sensors_ref(it);
        int s = 1 + sr.d - abs(x - sr.x) - abs(y - sr.y);
        if(s > steps) steps = s;
    }
    return steps;
}

void solve_day15(AdventOfCode* aoc, Stream* file_stream) {
    FuriString* line = furi_string_alloc();

    int p1 = 0;
    int64_t p2 = 0;

    int target_y = 2000000;

    sensors_t sensors;
    sensors_init(sensors);

    segments_t segments;
    segments_init(segments);

    dict_int_t beacons;
    dict_int_init(beacons);

    while(true) {
        if(!stream_read_line(file_stream, line)) break;
        furi_string_trim(line);

        char* end;
        int sx = strtol(furi_string_get_cstr(line) + 12, &end, 10);
        int sy = strtol(end + 4, &end, 10);
        int bx = strtol(end + 25, &end, 10);
        int by = strtol(end + 4, &end, 10);

        // pretty sure that will accurately identify if we're running the example case
        if(abs(sx) + abs(sy) + abs(bx) + abs(by) < 200) target_y = 10;

        if(by == target_y) {
            dict_int_push(beacons, bx);
        }

        int d = abs(bx - sx) + abs(by - sy);
        sensors_push_back(sensors, (Sensor){sx, sy, d});

        d -= abs(target_y - sy);
        if(d < 0) continue;

        segments_push_back(segments, (Segment){sx - d, sx + d});
    }
    p1 -= dict_int_size(beacons);

    segments_special_sort(segments, cmp);

    int x0 = segments_front(segments)->x0;
    int x1 = segments_front(segments)->x1;
    segments_it_t it;
    segments_it(it, segments);
    segments_next(it);
    for(; !segments_end_p(it); segments_next(it)) {
        Segment seg = *segments_ref(it);
        if(seg.x0 > x1 + 1) {
            p1 += x1 - x0 + 1;
            x0 = seg.x0;
        }
        x1 = M_MAX(x1, seg.x1);
    }
    p1 += x1 - x0 + 1;

    sensors_it_t it1, it2;
    for(sensors_it(it1, sensors); !sensors_end_p(it1) && p2 == 0; sensors_next(it1)) {
        Sensor si = *sensors_ref(it1);
        for(sensors_it_set(it2, it1), sensors_next(it2); !sensors_end_p(it2); sensors_next(it2)) {
            Sensor sj = *sensors_ref(it2);
            if(abs(sj.x - si.x) + abs(sj.y - si.y) != si.d + sj.d + 2) continue;

            x0 = M_MAX(M_MIN(si.x, sj.x), si.x > sj.x ? si.x - si.d - 1 : sj.x - sj.d - 1);
            x1 = M_MIN(M_MAX(si.x, sj.x), si.x > sj.x ? sj.x + sj.d + 1 : si.x + si.d + 1);
            int y0 = M_MAX(M_MIN(si.y, sj.y), si.y > sj.y ? si.y - si.d - 1 : sj.y - sj.d - 1);
            int y1 = M_MIN(M_MAX(si.y, sj.y), si.y > sj.y ? sj.y + sj.d + 1 : si.y + si.d + 1);

            int x = x0;
            int y = y0;
            int dy = 1;
            if((si.x < sj.x) == (si.y < sj.y)) {
                y = y1;
                dy = -1;
            }

            int step = 1;
            for(; x <= x1; x += step, y += dy * step) {
                step = (1 + min_steps(sensors, x, y)) / 2;
                if(step == 0) break;
            }
            if(step == 0) {
                p2 = 4000000 * (int64_t)x + (int64_t)y;
                break;
            }
        }
    }

    FuriString* part_1 = furi_string_alloc_printf("%d", p1);
    FuriString* part_2 = furi_string_alloc_printf("%lld", p2);

    update_results(aoc, part_1, part_2);

    dict_int_clear(beacons);
    segments_clear(segments);
    sensors_clear(sensors);
    furi_string_free(part_2);
    furi_string_free(part_1);
    furi_string_free(line);
}
