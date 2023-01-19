#include "advent_of_code.h"

#define MAGIC 0xdead // hoping this is unlikely enough to collide...

typedef union {
    int64_t val;
    struct {
        uint16_t magic;
        uint16_t l, r;
        char op;
    } calc;
} Monkey;

DICT_DEF2(monkey_ids, uint32_t, uint16_t)

int64_t eval(Monkey* mks, uint16_t id, bool resolve_humn);
int64_t inverse(Monkey* mks, uint16_t id, int64_t goal);

void solve_day21(AdventOfCode* aoc, Stream* file_stream) {
    FuriString* line = furi_string_alloc();
    uint32_t tmp = 0;

    int64_t p1 = 0;
    int64_t p2 = 0;

    Monkey* mks;
    // 2500 should be plenty
    mks = malloc(sizeof(Monkey) * 2500);

    monkey_ids_t ids;
    monkey_ids_init(ids);

    memcpy(&tmp, "root", 4);
    monkey_ids_set_at(ids, tmp, 0);

    memcpy(&tmp, "humn", 4);
    monkey_ids_set_at(ids, tmp, 1);

    int i = 2;
    while(true) {
        if(!stream_read_line(file_stream, line)) break;
        furi_string_trim(line);

        char* ptr = (char*)furi_string_get_cstr(line);

        memcpy(&tmp, ptr, 4);
        uint16_t id;
        uint16_t* idp = monkey_ids_get(ids, tmp);
        if(idp == NULL) {
            id = i;
            monkey_ids_set_at(ids, tmp, i++);
        } else {
            id = *idp;
        }

        ptr += 6;
        Monkey* m = &mks[id];
        if('0' <= *ptr && *ptr <= '9') {
            m->val = strtol(ptr, NULL, 10);
        } else {
            m->calc.magic = MAGIC;
            m->calc.op = ptr[5];

            memcpy(&tmp, ptr, 4);
            idp = monkey_ids_get(ids, tmp);
            if(idp == NULL) {
                m->calc.l = i;
                monkey_ids_set_at(ids, tmp, i++);
            } else {
                m->calc.l = *idp;
            }

            memcpy(&tmp, ptr + 7, 4);
            idp = monkey_ids_get(ids, tmp);
            if(idp == NULL) {
                m->calc.r = i;
                monkey_ids_set_at(ids, tmp, i++);
            } else {
                m->calc.r = *idp;
            }
        }
    }

    char root_op = mks[0].calc.op;
    mks[0].calc.op = '-';
    eval(mks, 0, false);
    p2 = inverse(mks, 0, 0);

    mks[0].calc.op = root_op;
    p1 = eval(mks, 0, true);

    FuriString* part_1 = furi_string_alloc_printf("%lld", p1);
    FuriString* part_2 = furi_string_alloc_printf("%lld", p2);

    update_results(aoc, part_1, part_2);

    monkey_ids_clear(ids);
    free(mks);
    furi_string_free(part_2);
    furi_string_free(part_1);
    furi_string_free(line);
}

int64_t eval(Monkey* mks, uint16_t id, bool resolve_humn) {
    if(mks[id].calc.magic != MAGIC) return mks[id].val;

    array_int_t stack;
    array_int_init(stack);

    array_int_push_back(stack, id);

    while(!array_int_empty_p(stack)) {
        int i = *array_int_back(stack);

        int64_t lv = 0;
        uint16_t l = mks[i].calc.l;
        if(mks[l].calc.magic != MAGIC)
            lv = mks[l].val;
        else if((mks[l].calc.op & 0x80) == 0) {
            array_int_push_back(stack, l);
            continue;
        }
        int64_t rv = 0;
        uint16_t r = mks[i].calc.r;
        if(mks[r].calc.magic != MAGIC)
            rv = mks[r].val;
        else if((mks[r].calc.op & 0x80) == 0) {
            array_int_push_back(stack, r);
            continue;
        }

        array_int_pop_back(NULL, stack);

        if((((mks[l].calc.op | mks[r].calc.op) & 0x80) != 0 || l == 1 || r == 1) &&
           !resolve_humn) {
            mks[i].calc.op |= 0x80;
            continue;
        }

        switch(mks[i].calc.op) {
        case '+':
            mks[i].val = lv + rv;
            break;
        case '-':
            mks[i].val = lv - rv;
            break;
        case '*':
            mks[i].val = lv * rv;
            break;
        case '/':
            mks[i].val = lv / rv;
            break;
        }
    }

    array_int_clear(stack);

    return mks[id].val;
}

int64_t inverse(Monkey* mks, uint16_t id, int64_t goal) {
    uint16_t l, r;
    bool swap;

    while(true) {
        l = mks[id].calc.l;
        r = mks[id].calc.r;
        mks[id].calc.op &= ~0x80;

        swap = false;
        if(mks[r].calc.magic == MAGIC) {
            swap = true;
            M_SWAP(uint16_t, l, r);
        }

        switch(mks[id].calc.op) {
        case '+':
            goal -= mks[r].val;
            break;
        case '-':
            if(swap)
                goal = mks[r].val - goal;
            else
                goal += mks[r].val;
            break;
        case '*':
            goal /= mks[r].val;
            break;
        case '/':
            if(swap) {
                goal = mks[r].val / goal;
            } else
                goal *= mks[r].val;
        }

        if(l == 1) return goal;

        id = l;
    }
}
