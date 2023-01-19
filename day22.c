#include "advent_of_code.h"

ARRAY_DEF(array, int8_t)
ARRAY_DEF(array2, array_t, ARRAY_OPLIST(array))

int solve22(
    FuriString* directions,
    array2_t col_walls[6],
    array2_t row_walls[6],
    int layout[5][5],
    int size,
    bool cube);

void solve_day22(AdventOfCode* aoc, Stream* file_stream) {
    FuriString* line = furi_string_alloc();

    array2_t col_walls[6];
    for(int i = 0; i < 6; i++) array2_init(col_walls[i]);
    array2_t row_walls[6];
    for(int i = 0; i < 6; i++) array2_init(row_walls[i]);

    int size = 50;

    int layout[5][5];
    for(int i = 0; i < 5; i++)
        for(int j = 0; j < 5; j++) layout[i][j] = -1;

    int k = 0;
    for(int i = 0;; i++) {
        if(!stream_read_line(file_stream, line)) break;
        if(furi_string_equal(line, "\n")) break;

        int l = furi_string_size(line) - 1;
        if(l < size) size = 4; // for the example input

        for(int j = 0; j < l; j++) {
            char c = furi_string_get_char(line, j);
            if(i % size == 0 && j % size == 0 && c != ' ') {
                layout[i / size][j / size] = k++;
            }
            int f = layout[i / size][j / size];

            if(c == '#') {
                array_push_back(*array2_safe_get(row_walls[f], i % size), j % size);
                array_push_back(*array2_safe_get(col_walls[f], j % size), i % size);
            }
        }
    }

    stream_read_line(file_stream, line);

    int p1 = solve22(line, col_walls, row_walls, layout, size, false);
    int p2 = solve22(line, col_walls, row_walls, layout, size, true);

    FuriString* part_1 = furi_string_alloc_printf("%d", p1);
    FuriString* part_2 = furi_string_alloc_printf("%d", p2);

    update_results(aoc, part_1, part_2);

    for(int i = 0; i < 6; i++) array2_clear(row_walls[i]);
    for(int i = 0; i < 6; i++) array2_clear(col_walls[i]);

    furi_string_free(part_2);
    furi_string_free(part_1);
    furi_string_free(line);
}

void torus_wrap(int adj_f[6][4], int adj_d[6][4], int layout[5][5]) {
    for(int i = 0; i < 6; i++)
        for(int j = 0; j < 4; j++) {
            adj_f[i][j] = -1;
            adj_d[i][j] = j;
        }

    int k;
    for(int i = 0; i < 5; i++)
        for(int j = 0; j < 5; j++) {
            int f = layout[i][j];
            if(f < 0) continue;

            k = (j + 1) % 5;
            while(layout[i][k] < 0) k = (k + 1) % 5;
            adj_f[f][0] = layout[i][k];

            k = (i + 1) % 5;
            while(layout[k][j] < 0) k = (k + 1) % 5;
            adj_f[f][1] = layout[k][j];

            k = (j + 4) % 5;
            while(layout[i][k] < 0) k = (k + 4) % 5;
            adj_f[f][2] = layout[i][k];

            k = (i + 4) % 5;
            while(layout[k][j] < 0) k = (k + 4) % 5;
            adj_f[f][3] = layout[k][j];
        }
}

void cube_wrap(int adj_f[6][4], int adj_d[6][4], int layout[5][5]) {
    for(int i = 0; i < 6; i++)
        for(int j = 0; j < 4; j++) {
            adj_f[i][j] = -1;
            adj_d[i][j] = j;
        }

    for(int i = 0; i < 5; i++)
        for(int j = 0; j < 5; j++) {
            int f = layout[i][j];
            if(f < 0) continue;

            for(int d = 0; d < 4; d++) {
                if(adj_f[f][d] >= 0) continue;

                int a = 0, b = 0;
                int da = 0, db = 0;
                int pda = 0, pdb = 0;
                switch(d) {
                case 0:
                    db = 1;
                    pda = 1;
                    break;
                case 1:
                    da = 1;
                    pdb = -1;
                    break;
                case 2:
                    db = -1;
                    pda = -1;
                    break;
                case 3:
                    da = -1;
                    pdb = 1;
                    break;
                }

                a = i + da;
                b = j + db;
                if(0 <= a && a < 5 && 0 <= b && b < 5 && layout[a][b] >= 0) {
                    int g = layout[a][b];
                    adj_f[f][d] = g;
                    adj_f[g][(d + 2) % 4] = f;
                    continue;
                }

                a = i + da + pda;
                b = j + db + pdb;
                if(0 <= a && a < 5 && 0 <= b && b < 5 && layout[a][b] >= 0) {
                    int g = layout[a][b];
                    adj_f[f][d] = g;
                    adj_d[f][d] = (d + 1) % 4;
                    adj_f[g][(d + 3) % 4] = f;
                    adj_d[g][(d + 3) % 4] = (d + 2) % 4;
                    continue;
                }

                a = i + da - pda;
                b = j + db - pdb;
                if(0 <= a && a < 5 && 0 <= b && b < 5 && layout[a][b] >= 0) {
                    int g = layout[a][b];
                    adj_f[f][d] = g;
                    adj_d[f][d] = (d + 3) % 4;
                    adj_f[g][(d + 1) % 4] = f;
                    adj_d[g][(d + 1) % 4] = (d + 2) % 4;
                    continue;
                }

                a = i + da + 2 * pda;
                b = j + db + 2 * pdb;
                if(0 <= a && a < 5 && 0 <= b && b < 5 && layout[a][b] >= 0) {
                    int g = layout[a][b];
                    adj_f[f][d] = g;
                    adj_d[f][d] = (d + 2) % 4;
                    adj_f[g][d] = f;
                    adj_d[g][d] = (d + 2) % 4;
                    continue;
                }

                a = i + da - 2 * pda;
                b = j + db - 2 * pdb;
                if(0 <= a && a < 5 && 0 <= b && b < 5 && layout[a][b] >= 0) {
                    int g = layout[a][b];
                    adj_f[f][d] = g;
                    adj_d[f][d] = (d + 2) % 4;
                    adj_f[g][d] = f;
                    adj_d[g][d] = (d + 2) % 4;
                    continue;
                }

                a = i - 3 * da;
                b = j - 3 * db;
                if(0 <= a && a < 5 && 0 <= b && b < 5 && layout[a][b] >= 0) {
                    int g = layout[a][b];
                    adj_f[f][d] = g;
                    adj_f[g][(d + 2) % 4] = f;
                    continue;
                }

                a = i - 2 * da + 2 * pda;
                b = j - 2 * db + 2 * pdb;
                if(0 <= a && a < 5 && 0 <= b && b < 5 && layout[a][b] >= 0 &&
                   layout[i - da][j - db] >= 0 && layout[a - pda][b - pdb] >= 0) {
                    int g = layout[a][b];
                    adj_f[f][d] = g;
                    adj_d[f][d] = (d + 3) % 4;
                    adj_f[g][(d + 1) % 4] = f;
                    adj_d[g][(d + 1) % 4] = (d + 2) % 4;
                    continue;
                }

                a = i - 2 * da - 2 * pda;
                b = j - 2 * db - 2 * pdb;
                if(0 <= a && a < 5 && 0 <= b && b < 5 && layout[a][b] >= 0 &&
                   layout[i - da][j - db] >= 0 && layout[a + pda][b + pdb] >= 0) {
                    int g = layout[a][b];
                    adj_f[f][d] = g;
                    adj_d[f][d] = (d + 1) % 4;
                    adj_f[g][(d + 3) % 4] = f;
                    adj_d[g][(d + 3) % 4] = (d + 2) % 4;
                    continue;
                }

                a = i - 3 * da + pda;
                b = j - 3 * db + pdb;
                // can relax the check a bit as we've already checked some above
                if(0 <= a && a < 5 && 0 <= b && b < 5 && layout[a][b] >= 0) {
                    if(layout[i - da][j - db] < 0 && layout[i - 2 * da][j - 2 * db] < 0) {
                        int g = layout[a][b];
                        adj_f[f][d] = g;
                        adj_d[f][d] = (d + 1) % 4;
                        adj_f[g][(d + 3) % 4] = f;
                        adj_d[g][(d + 3) % 4] = (d + 2) % 4;
                        continue;
                    }
                    if(layout[i - da][j - db] >= 0 && layout[i - 2 * da][j - 2 * db] >= 0) {
                        int g = layout[a][b];
                        adj_f[f][d] = g;
                        adj_d[f][d] = (d + 3) % 4;
                        adj_f[g][(d + 1) % 4] = f;
                        adj_d[g][(d + 1) % 4] = (d + 2) % 4;
                        continue;
                    }
                }

                a = i - 3 * da - pda;
                b = j - 3 * db - pdb;
                if(0 <= a && a < 5 && 0 <= b && b < 5 && layout[a][b] >= 0) {
                    if(layout[i - da][j - db] < 0 && layout[i - 2 * da][j - 2 * db] < 0) {
                        int g = layout[a][b];
                        adj_f[f][d] = g;
                        adj_d[f][d] = (d + 3) % 4;
                        adj_f[g][(d + 1) % 4] = f;
                        adj_d[g][(d + 1) % 4] = (d + 2) % 4;
                        continue;
                    }
                    if(layout[i - da][j - db] >= 0 && layout[i - 2 * da][j - 2 * db] >= 0) {
                        int g = layout[a][b];
                        adj_f[f][d] = g;
                        adj_d[f][d] = (d + 1) % 4;
                        adj_f[g][(d + 3) % 4] = f;
                        adj_d[g][(d + 3) % 4] = (d + 2) % 4;
                        continue;
                    }
                }
            }
        }

    // at most one pair left to connect
    int a = -1, b = -1;
    for(int i = 0; i < 6; i++)
        for(int j = 0; j < 4; j++) {
            if(adj_f[i][j] >= 0) continue;
            if(a < 0) {
                a = i;
                b = j;
            } else {
                adj_f[a][b] = i;
                adj_d[a][b] = (j + 2) % 4;
                adj_f[i][j] = a;
                adj_d[i][j] = (b + 2) % 4;
                i = 5;
                j = 3;
            }
        }
}

bool change_face(
    int* x0,
    int* y0,
    int* f0,
    int* d0,
    array2_t col_walls[6],
    array2_t row_walls[6],
    int adj_f[6][4],
    int adj_d[6][4],
    int size) {
    int x1 = *x0;
    int y1 = *y0;
    int f1 = adj_f[*f0][*d0];
    int d1 = adj_d[*f0][*d0];

    if(*d0 == d1) {
        switch(d1) {
        case 0:
            x1 = 0;
            break;
        case 1:
            y1 = 0;
            break;
        case 2:
            x1 = size - 1;
            break;
        case 3:
            y1 = size - 1;
            break;
        }
    } else if(abs(*d0 - d1) == 2) {
        if(d1 % 2 == 0)
            y1 = size - 1 - y1;
        else
            x1 = size - 1 - x1;
    } else {
        if(*d0 / 2 == d1 / 2) {
            x1 = size - 1 - x1;
            y1 = size - 1 - y1;
        }
        M_SWAP(int, x1, y1);
    }

    int8_t* w = NULL;
    switch(d1) {
    case 0:
        w = array_front(*array2_safe_get(row_walls[f1], y1));
        break;
    case 1:
        w = array_front(*array2_safe_get(col_walls[f1], x1));
        break;
    case 2:
        w = array_back(*array2_safe_get(row_walls[f1], y1));
        break;
    case 3:
        w = array_back(*array2_safe_get(col_walls[f1], x1));
        break;
    }
    // array_back returns UINTPTR_MAX rather than NULL when array empty
    if(w != NULL && w != (int8_t*)UINTPTR_MAX && *w == (d1 < 2 ? 0 : size - 1)) return false;

    *x0 = x1;
    *y0 = y1;
    *f0 = f1;
    *d0 = d1;
    return true;
}

int solve22(
    FuriString* directions,
    array2_t col_walls[6],
    array2_t row_walls[6],
    int layout[5][5],
    int size,
    bool cube) {
    char* ptr = (char*)furi_string_get_cstr(directions);

    int adj_f[6][4];
    int adj_d[6][4];

    if(cube) {
        cube_wrap(adj_f, adj_d, layout);
    } else {
        torus_wrap(adj_f, adj_d, layout);
    }

    int x = 0, y = 0, f = 0, d = 0;

    array_it_t it;
    array_t* a;
    a = array2_safe_get(row_walls[f], y);
    for(array_it(it, *a); !array_end_p(it); array_next(it)) {
        int w = *array_ref(it);
        if(w > x) break;
        x++;
    } // surely the first open spot is on the first face, right??

    while(*ptr != '\n' && *ptr != 0) {
        int steps = strtol(ptr, &ptr, 10);
        while(steps > 0) {
            int* mov = NULL;
            int ref = 0;
            array2_t* walls = NULL;
            int w = -1;

            if(d % 2 == 0) {
                mov = &x;
                ref = y;
                walls = &row_walls[f];
            } else {
                mov = &y;
                ref = x;
                walls = &col_walls[f];
            }

            a = array2_safe_get(*walls, ref);
            // could do binary search. bah.
            if(d < 2) {
                for(array_it(it, *a); !array_end_p(it); array_next(it)) {
                    w = *array_ref(it);
                    if(w > *mov) break;
                }
            } else {
                *mov = size - 1 - *mov;
                for(array_it_last(it, *a); !array_end_p(it); array_previous(it)) {
                    w = size - 1 - *array_ref(it);
                    if(w > *mov) break;
                }
            }

            if(w < *mov) w = *mov + steps + 1;
            *mov = M_MIN(*mov + steps, w - 1);

            if(*mov < size) {
                if(d >= 2) *mov = size - 1 - *mov;
                break;
            }

            steps = *mov - size;
            *mov = size - 1;
            if(d >= 2) *mov = size - 1 - *mov;

            bool changed = change_face(&x, &y, &f, &d, col_walls, row_walls, adj_f, adj_d, size);
            if(!changed) break;
        }

        switch(*ptr) {
        case 'R':
            d = (d + 1) % 4;
            break;
        case 'L':
            d = (d + 3) % 4;
            break;
        }
        ptr++;
    }

    for(int i = 0; i < 5; i++)
        for(int j = 0; j < 5; j++) {
            if(layout[i][j] == f) {
                x += j * size;
                y += i * size;
                // break out of both loops
                i = 4;
                j = 4;
            }
        }
    x++;
    y++;
    return 1000 * y + 4 * x + d;
}
