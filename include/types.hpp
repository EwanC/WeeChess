#ifndef TYPES_H
#define TYPES_H

#define MAX_GAME_MOVES 256
#define MAX_GAME_PLY 128
#define TOTAL_SQUARES 120
#define MAXDEPTH 64 // Mac depth program will try to search to 

typedef unsigned char uchar;

enum Piece  { EMPTY =0 , wP, wN, wB, wR, wQ, wK, bP, bN, bB, bR, bQ, bK };
enum File   { FILE_A=0, FILE_B, FILE_C, FILE_D, FILE_E, FILE_F, FILE_G, FILE_H, FILE_NONE };
enum Rank   { RANK_1=0, RANK_2, RANK_3, RANK_4, RANK_5, RANK_6, RANK_7, RANK_8, RANK_NONE };
enum Colour { WHITE=0, BLACK, BOTH };

enum Square {
  A1 = 21, B1, C1, D1, E1, F1, G1, H1,
  A2 = 31, B2, C2, D2, E2, F2, G2, H2,
  A3 = 41, B3, C3, D3, E3, F3, G3, H3,
  A4 = 51, B4, C4, D4, E4, F4, G4, H4,
  A5 = 61, B5, C5, D5, E5, F5, G5, H5,
  A6 = 71, B6, C6, D6, E6, F6, G6, H6,
  A7 = 81, B7, C7, D7, E7, F7, G7, H7,
  A8 = 91, B8, C8, D8, E8, F8, G8, H8, NO_SQ, OFFBOARD
};

//Castling flags
#define WKCA 1
#define WQCA 2 
#define BKCA 4 
#define BQCA 8

#endif //TYPES_H
