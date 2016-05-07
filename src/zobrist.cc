/*
 * Demolito, a UCI chess engine.
 * Copyright 2015 lucasart.
 *
 * Demolito is free software: you can redistribute it and/or modify it under the terms of the GNU
 * General Public License as published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * Demolito is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without
 * even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with this program. If
 * not, see <http://www.gnu.org/licenses/>.
*/
#include "zobrist.h"
#include "bitboard.h"

namespace {

uint64_t Zobrist[NB_COLOR][NB_PIECE][NB_SQUARE];
uint64_t ZobristCastling[NB_SQUARE];
uint64_t ZobristEnPassant[NB_SQUARE+1];
uint64_t ZobristTurn;

uint64_t rotate(uint64_t x, int k)
{
    return (x << k) | (x >> (64 - k));
}

}    // namespace

namespace zobrist {

thread_local History history;

void History::push(uint64_t key)
{
    assert(0 <= idx && idx < MAX_GAME_PLY);
    keys[idx++] = key;
}

void History::pop()
{
    assert(0 < idx && idx <= MAX_GAME_PLY);
    idx--;
}

uint64_t History::back() const
{
    assert(0 < idx && idx <= MAX_GAME_PLY);
    return keys[idx - 1];
}

bool History::repetition(int rule50) const
{
    // 50 move rule
    if (rule50 >= 100)
        return true;

    // TODO: use 3 repetition past root position
    for (int i = 4; i <= rule50 && i < idx; i += 2)
        if (keys[idx-1 - i] == keys[idx-1])
            return true;

    return false;
}

void PRNG::init(uint64_t seed)
{
    a = 0xf1ea5eed;
    b = c = d = seed;

    for (int i = 0; i < 20; ++i)
        rand();
}

uint64_t PRNG::rand()
{
    uint64_t e = a - rotate(b, 7);
    a = b ^ rotate(c, 13);
    b = c + rotate(d, 37);
    c = d + e;
    return d = e + a;
}

void init()
{
    PRNG prng;

    for (Color c = WHITE; c <= BLACK; ++c)
        for (Piece p = KNIGHT; p < NB_PIECE; ++p)
            for (int sq = A1; sq <= H8; ++sq)
                Zobrist[c][p][sq] = prng.rand();

    for (int sq = A1; sq <= H8; ++sq)
        ZobristCastling[sq] = prng.rand();

    for (int sq = 0; sq <= NB_SQUARE; ++sq)
        ZobristEnPassant[sq] = prng.rand();

    ZobristTurn = prng.rand();
}

uint64_t key(Color c, Piece p, int sq)
{
    BOUNDS(c, NB_COLOR);
    BOUNDS(p, NB_PIECE);
    BOUNDS(sq, NB_SQUARE);

    return Zobrist[c][p][sq];
}

uint64_t keys(Color c, Piece p, uint64_t sqs)
{
    BOUNDS(c, NB_COLOR);
    BOUNDS(p, NB_PIECE);

    bitboard_t k = 0;

    while(sqs)
        k ^= key(c, p, bb::pop_lsb(sqs));

    return k;
}

uint64_t castling(bitboard_t castlableRooks)
{
    bitboard_t k = 0;

    while (castlableRooks)
        k ^= ZobristCastling[bb::pop_lsb(castlableRooks)];

    return k;
}

uint64_t en_passant(int sq)
{
    assert(unsigned(sq) <= NB_SQUARE);

    return ZobristEnPassant[sq];
}

uint64_t turn()
{
    return ZobristTurn;
}

}    // namespace zobrist
