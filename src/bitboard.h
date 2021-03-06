#pragma once
#include "types.h"

void bb_init();

extern bitboard_t Rank[NB_RANK], File[NB_FILE];
extern bitboard_t PAttacks[NB_COLOR][NB_SQUARE], NAttacks[NB_SQUARE], KAttacks[NB_SQUARE];
extern bitboard_t BPseudoAttacks[NB_SQUARE], RPseudoAttacks[NB_SQUARE];
extern bitboard_t Segment[NB_SQUARE][NB_SQUARE];
extern bitboard_t Ray[NB_SQUARE][NB_SQUARE];

bitboard_t bb_battacks(int s, bitboard_t occ);
bitboard_t bb_rattacks(int s, bitboard_t occ);

bool bb_test(bitboard_t b, int s);
void bb_clear(bitboard_t *b, int s);
void bb_set(bitboard_t *b, int s);
bitboard_t bb_shift(bitboard_t b, int i);

int bb_lsb(bitboard_t b);
int bb_msb(bitboard_t b);
int bb_pop_lsb(bitboard_t *b);

bool bb_several(bitboard_t b);
int bb_count(bitboard_t b);

void bb_print(bitboard_t b);
