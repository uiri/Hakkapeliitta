#ifndef HASH_H
#define HASH_H

#include "defs.h"

extern array<uint64_t, Squares> pieceHash[2][6];
extern array<uint64_t, 8> materialHash[2][6];
extern array<uint64_t, 16> castlingRightsHash;
extern array<uint64_t, Squares> enPassantHash;
extern uint64_t turnHash;

void initializeHash();

#endif
