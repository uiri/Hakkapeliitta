#ifndef HASH_CPP
#define HASH_CPP

#include "hash.h"
#include "random.h"

array<uint64_t, Squares> pieceHash[2][6];
array<uint64_t, 8> materialHash[2][6];
array<uint64_t, 16> castlingRightsHash;
array<uint64_t, Squares> enPassantHash;
uint64_t turnHash;

void initializeHash()
{
	WELL512 rng(119582769);

	for (int i = White; i <= Black; i++)
	{
		for (int j = Pawn; j <= King; j++)
		{
			for (int k = A1; k <= H8; k++)
			{
				pieceHash[i][j][k] = (uint64_t(rng.rand()) << 32) | uint64_t(rng.rand());
			}
			for (int l = 0; l < 8; l++)
			{
				materialHash[i][j][l] = (uint64_t(rng.rand()) << 32) | uint64_t(rng.rand());
			}
		}
	}

	for (int i = A1; i <= H8; i++)
	{
		enPassantHash[i] = (uint64_t(rng.rand()) << 32) | uint64_t(rng.rand());
	}

	for (int i = 0; i < 16; i++)
	{
		castlingRightsHash[i] = (uint64_t(rng.rand()) << 32) | uint64_t(rng.rand());
	}

	turnHash = (uint64_t(rng.rand()) << 32) | uint64_t(rng.rand());
}

#endif