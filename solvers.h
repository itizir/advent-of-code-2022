#pragma once

#include "advent_of_code.h"

void unimplemented(AdventOfCode* aoc, Stream* file_stream);

void solve_day01(AdventOfCode*, Stream*);
void solve_day02(AdventOfCode*, Stream*);
void solve_day03(AdventOfCode*, Stream*);
void solve_day04(AdventOfCode*, Stream*);
void solve_day05(AdventOfCode*, Stream*);
void solve_day06(AdventOfCode*, Stream*);
void solve_day07(AdventOfCode*, Stream*);
void solve_day08(AdventOfCode*, Stream*);
void solve_day09(AdventOfCode*, Stream*);
void solve_day10(AdventOfCode*, Stream*);
void solve_day11(AdventOfCode*, Stream*);
void solve_day12(AdventOfCode*, Stream*);
void solve_day13(AdventOfCode*, Stream*);
void solve_day14(AdventOfCode*, Stream*);
void solve_day15(AdventOfCode*, Stream*);
void solve_day16(AdventOfCode*, Stream*);
void solve_day17(AdventOfCode*, Stream*);
void solve_day18(AdventOfCode*, Stream*);
void solve_day19(AdventOfCode*, Stream*);
void solve_day20(AdventOfCode*, Stream*);
void solve_day21(AdventOfCode*, Stream*);
void solve_day22(AdventOfCode*, Stream*);
void solve_day23(AdventOfCode*, Stream*);
void solve_day24(AdventOfCode*, Stream*);
void solve_day25(AdventOfCode*, Stream*);

AdventSolver solvers[25] = {
    solve_day01, solve_day02, solve_day03, solve_day04, solve_day05, solve_day06, solve_day07,
    solve_day08, solve_day09, solve_day10, solve_day11, solve_day12, solve_day13, solve_day14,
    solve_day15, solve_day16, solve_day17, solve_day18, solve_day19, solve_day20, solve_day21,
    solve_day22, solve_day23, solve_day24, solve_day25,
};
