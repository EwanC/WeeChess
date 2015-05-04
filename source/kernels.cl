
// Bittable needed for poping bits from the Bitboard
int constant BitTable[64] = {
  63, 30, 3, 32, 25, 41, 22, 33, 15, 50, 42, 13, 11, 53, 19, 34, 61, 29, 2,
  51, 21, 43, 45, 10, 18, 47, 1, 54, 9, 57, 0, 35, 62, 31, 40, 4, 49, 5, 52,
  26, 60, 6, 23, 44, 46, 27, 56, 16, 7, 39, 48, 24, 59, 14, 12, 55, 38, 28,
  58, 20, 37, 17, 36, 8
};

//Piece Tables
int constant PieceSqMasks[13][64] = { 
                                  { // All
                                    0, 0, 0, 0, 0, 0, 0, 0, 
                                    0, 0, 0, 0, 0, 0, 0, 0,
                                    0, 0, 0, 0, 0, 0, 0, 0,
                                    0, 0, 0, 0, 0, 0, 0, 0, 
                                    0, 0, 0, 0, 0, 0, 0, 0,
                                    0, 0, 0, 0, 0, 0, 0, 0,
                                    0, 0, 0, 0, 0, 0, 0, 0,
                                    0, 0, 0, 0, 0, 0, 0, 0
                                  },
                                  { // White Pawn Table
                                     0,  0,  0,   0,  0,  0,  0,  0,  
                                    10, 10,  0, -10,-10,  0, 10, 10,
                                     5,  0,  0,   5,  5,  0,  0,  5, 
                                     0,  0, 10,  20, 20, 10,  0,  0,
                                     5,  5,  5,  10, 10,  5,  5,  5, 
                                    10, 10, 10,  20, 20, 10, 10, 10,
                                    20, 20, 20,  30, 30, 20, 20, 20,
                                     0,  0,  0,   0,  0,  0,  0,  0
                                  },
                                  { // White Knight Table
                                     0, -10,  0,  0,  0,  0, -10, 0,
                                     0,   0,  0,  5,  5,  0,   0, 0,
                                     0,   0, 10, 10, 10, 10,   0, 0,
                                     0,   0, 10, 20, 20, 10,   5, 0,
                                     5,  10, 15, 20, 20, 15,  10, 5,
                                     5,  10, 10, 20, 20, 10,  10, 5,
                                     0,   0,  5, 10, 10,  5,   0, 0,
                                     0,   0,  0,  0,  0,  0,   0, 0
                                  },
                                  { // White Bishop Table
                                    0,  0, -10,   0,  0, -10,   0,  0, 
                                    0,  0,   0,  10, 10,   0,   0,  0,
                                    0,  0,  10,  15, 15,  10,   0,  0,
                                    0, 10,  15,  20, 20,  15,  10,  0,
                                    0, 10,  15,  20, 20,  15,  10,  0,
                                    0,  0,  10,  15, 15,  10,   0,  0,
                                    0,  0,   0,  10, 10,   0,   0,  0, 
                                    0,  0,   0,   0,  0,   0,   0,  0
                                  },
                                  { // White Rook Table
                                    0,   0,   5,  10, 10,  5,   0,   0,
                                    0,   0,   5,  10, 10,  5,   0,   0,
                                    0,   0,   5,  10, 10,  5,   0,   0, 
                                    0,   0,   5,  10, 10,  5,   0,   0,
                                    0,   0,   5,  10, 10,  5,   0,   0, 
                                    0,   0,   5,  10, 10,  5,   0,   0,
                                    25,  25,  25,  25, 25, 25,  25,  25,
                                    0,   0,   5,  10, 10,  5,  0,  0
                                  },
                                  { // White Queen Table
                                    0,   0,  5,  10, 10, 5,   0,  0,
                                    0,   0,  5,  10, 10, 5,   0,  0, 
                                    0,   0,  5,  15, 15, 5,   0,  0,
                                    0,  10, 15,  25, 25, 15, 10,  0, 
                                    0,  10, 15,  25, 25, 15, 10,  0, 
                                    0,   5, 10,  20, 20, 10,  5,  0,
                                   25,  25, 25,  30, 30, 25, 25, 25, 
                                    0,   0,  5,  10, 10,  5,  0,  0
                                  },
                                  { // White King Opening Table
                                      0,   5,   5, -10, -10,   0,  10,   5,
                                    -30, -30, -30, -30, -30, -30, -30, -30, 
                                    -50, -50, -50, -50, -50, -50, -50, -50, 
                                    -70, -70, -70, -70, -70, -70, -70, -70,
                                    -70, -70, -70, -70, -70, -70, -70, -70, 
                                    -70, -70, -70, -70, -70, -70, -70, -70, 
                                    -70, -70, -70, -70, -70, -70, -70, -70, 
                                    -70, -70, -70, -70, -70, -70, -70, -70
                                  },
                                  { // Black Pawn Table
                                      0,   0,   0,   0,   0,   0,   0,   0,
                                    -20, -20, -20, -30, -30, -20, -20, -20, 
                                    -10, -10, -10, -20, -20, -10, -10, -10,
                                     -5,  -5,  -5, -10, -10,  -5,  -5,  -5,
                                      0,   0, -10, -20, -20, -10,   0,   0,
                                     -5,   0,   0,  -5,  -5,   0,   0,  -5,
                                    -10, -10,   0,  10,  10,   0, -10, -10,
                                      0,   0,   0,   0,   0,   0,   0,   0
                                  },
                                  { // Black Knight Table 
                                    0,   0,   0,   0,   0,   0,   0,  0,
                                    0,   0,  -5, -10, -10,  -5,   0,  0,
                                   -5, -10, -10, -20, -20, -10, -10, -5,
                                   -5, -10, -15, -20, -20, -15, -10, -5,
                                    0,   0, -10, -20, -20, -10,  -5,  0,
                                    0,   0, -10, -10, -10, -10,   0,  0,
                                    0,   0,   0,  -5,  -5,   0,   0,  0,
                                    0,  10,   0,   0,   0,   0,  10,  0
                                 },  
                                 { // Black Bishop Table
                                  0,   0,  0,    0,   0,   0,   0, 0,
                                  0,   0,  0,  -10, -10,   0,   0, 0,
                                  0,   0, -10, -15, -15, -10,   0, 0,
                                  0, -10, -15, -20, -20, -15, -10, 0,
                                  0, -10, -15, -20, -20, -15, -10, 0,
                                  0,   0, -10, -15, -15, -10,   0, 0,
                                  0,   0,   0, -10, -10,   0,   0, 0,
                                  0,   0,  10,   0,   0,  10,   0, 0
                                 },
                                  { // Black Rook Table
                                    0,   0,  -5, -10, -10,  -5,   0,   0,
                                  -25, -25, -25, -25, -25, -25, -25, -25,
                                    0,   0,  -5, -10, -10,  -5,   0,   0,
                                    0,   0,  -5, -10, -10,  -5,   0,   0,
                                    0,   0,  -5, -10, -10,  -5,   0,   0,
                                    0,   0,  -5, -10, -10,  -5,   0,   0,
                                    0,   0,  -5, -10, -10,  -5,   0,   0,
                                    0,   0,  -5, -10, -10,  -5,   0,   0
                                 },
                                  { // Black Queen Table
                                    0,    0,  -5,  -10, -10,  -5,   0,   0,
                                  -25,  -25, -25,  -30, -30, -25, -25, -25,
                                    0,   -5, -10,  -20, -20, -10, -5,   0,
                                    0,  -10, -15,  -25, -25, -15, -10,   0, 
                                    0,  -10, -15,  -25, -25, -15, -10,   0, 
                                    0,    0,  -5,  -15, -15,  -5,   0,   0,
                                    0,    0,  -5,  -10, -10,  -5,   0,   0, 
                                    0,    0,  -5,  -10, -10,  -5,   0,   0,
                                  },
                                  { // Black King Opening Table
                                    70, 70, 70, 70, 70, 70, 70, 70, 
                                    70, 70, 70, 70, 70, 70, 70, 70,
                                    70, 70, 70, 70, 70, 70, 70, 70,
                                    70, 70, 70, 70, 70, 70, 70, 70, 
                                    70, 70, 70, 70, 70, 70, 70, 70,
                                    50, 50, 50, 50, 50, 50, 50, 50, 
                                    30, 30, 30, 30, 30, 30, 30, 30, 
                                     0, -5, -5, 10, 10,  0,-10, -5
                                  }
                                };

unsigned long constant conditionMasks[13][64] = {

                         { // ALL
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff
                        },
                        {   // Pawn Isolated masks
                            0x0000000002020202, 0x0000000005050505, 0x000000000a0a0a0a, 0x0000000014141414,
                            0x0000000028282828, 0x0000000050505050, 0xffffffffa0a0a0a0, 0x0000000040404040,
                            0x0000000002020202, 0x0000000005050505, 0x000000000a0a0a0a, 0x0000000014141414,
                            0x0000000028282828, 0x0000000050505050, 0xffffffffa0a0a0a0, 0x0000000040404040,
                            0x0000000002020202, 0x0000000005050505, 0x000000000a0a0a0a, 0x0000000014141414,
                            0x0000000028282828, 0x0000000050505050, 0xffffffffa0a0a0a0, 0x0000000040404040,
                            0x0000000002020202, 0x0000000005050505, 0x000000000a0a0a0a, 0x0000000014141414,
                            0x0000000028282828, 0x0000000050505050, 0xffffffffa0a0a0a0, 0x0000000040404040,
                            0x0000000002020202, 0x0000000005050505, 0x000000000a0a0a0a, 0x0000000014141414,
                            0x0000000028282828, 0x0000000050505050, 0xffffffffa0a0a0a0, 0x0000000040404040,
                            0x0000000002020202, 0x0000000005050505, 0x000000000a0a0a0a, 0x0000000014141414,
                            0x0000000028282828, 0x0000000050505050, 0xffffffffa0a0a0a0, 0x0000000040404040,
                            0x0000000002020202, 0x0000000005050505, 0x000000000a0a0a0a, 0x0000000014141414,
                            0x0000000028282828, 0x0000000050505050, 0xffffffffa0a0a0a0, 0x0000000040404040,
                            0x0000000002020202, 0x0000000005050505, 0x000000000a0a0a0a, 0x0000000014141414,
                            0x0000000028282828, 0x0000000050505050, 0xffffffffa0a0a0a0, 0x0000000040404040
                        },
                       {    // W Knight
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff
                        },
                        {   // W Bishop
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff
                        },
                        {   // W Rook
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff
                        },
                        {   // W Queen
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff
                        },
                        {   // W King
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff
                        },
                        {   // B Pawn Isolated
                            0x0000000002020202, 0x0000000005050505, 0x000000000a0a0a0a, 0x0000000014141414,
                            0x0000000028282828, 0x0000000050505050, 0xffffffffa0a0a0a0, 0x0000000040404040,
                            0x0000000002020202, 0x0000000005050505, 0x000000000a0a0a0a, 0x0000000014141414,
                            0x0000000028282828, 0x0000000050505050, 0xffffffffa0a0a0a0, 0x0000000040404040,
                            0x0000000002020202, 0x0000000005050505, 0x000000000a0a0a0a, 0x0000000014141414,
                            0x0000000028282828, 0x0000000050505050, 0xffffffffa0a0a0a0, 0x0000000040404040,
                            0x0000000002020202, 0x0000000005050505, 0x000000000a0a0a0a, 0x0000000014141414,
                            0x0000000028282828, 0x0000000050505050, 0xffffffffa0a0a0a0, 0x0000000040404040,
                            0x0000000002020202, 0x0000000005050505, 0x000000000a0a0a0a, 0x0000000014141414,
                            0x0000000028282828, 0x0000000050505050, 0xffffffffa0a0a0a0, 0x0000000040404040,
                            0x0000000002020202, 0x0000000005050505, 0x000000000a0a0a0a, 0x0000000014141414,
                            0x0000000028282828, 0x0000000050505050, 0xffffffffa0a0a0a0, 0x0000000040404040,
                            0x0000000002020202, 0x0000000005050505, 0x000000000a0a0a0a, 0x0000000014141414,
                            0x0000000028282828, 0x0000000050505050, 0xffffffffa0a0a0a0, 0x0000000040404040,
                            0x0000000002020202, 0x0000000005050505, 0x000000000a0a0a0a, 0x0000000014141414,
                            0x0000000028282828, 0x0000000050505050, 0xffffffffa0a0a0a0, 0x0000000040404040
                        },
                        {   // B Knight
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff
                        },
                        {   // B Bishop
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff
                        },
                        {   // B Rook
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff
                        },
                        {   // B Queen
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff
                        },
                        {   // B King
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                            0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff
                        },

                    };

int constant adjustment[13] = {  
                        0,  // ALL
                      -10,  // Wp isolated
                        0,  // Wn
                        0,  // Wb
                        0,  // WR
                        0,  // WQ
                        0,  // Wk
                       10,  // Bp isolated
                        0,  // Bk
                        0,  // Bb
                        0,  // br
                        0,  // Bq
                        0   // Bk
                    };



inline int popBit( unsigned long* bb)
 {
  unsigned long b = *bb ^ (*bb - 1);
  unsigned int fold = (unsigned int) ((b & 0xffffffff) ^ (b >> 32));
  *bb &= (*bb - 1);
  return BitTable[(fold * 0x783a9b23) >> 26];
}


__kernel void evalKernel(
                         __global unsigned long* bitboards,
                         __global int* score,
                         __local int* local_score
                        )
{
    int group_id = get_group_id(0);
    int local_id = get_local_id(0);

    unsigned long board = bitboards[group_id*13 + local_id];
    const unsigned long const_board = board;
    local_score[0] = 0;
    int priv_score = 0;
    barrier(CLK_LOCAL_MEM_FENCE);

    while(board != 0)
    {
        int sq64 = popBit(&board);
        priv_score += PieceSqMasks[local_id][sq64];
        //if ((conditionMasks[local_id][sq64] & const_board) == 0)
        //    priv_score += adjustment[local_id];

    }
   
    atomic_add(local_score, priv_score);
    barrier(CLK_LOCAL_MEM_FENCE);

    if(local_id == 0)
      score[group_id] = local_score[0];
}