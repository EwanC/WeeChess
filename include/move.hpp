#ifndef MOVE_H
#define MOVE_H

#include <stdint.h> //Standard Integer Types
#include <string>
#include <vector>

/* GAME MOVE 
  
   0000 0000 0000 0000 0000 0111 1111 -> From [0x7F]
   0000 0000 0000 0011 1111 1000 0000 -> To [>> 7, 0x7F]
   0000 0000 0011 1100 0000 0000 0000 -> Captured Piece [>> 14, 0xF]
   0000 0000 0100 0000 0000 0000 0000 -> En Passent capture? [0x40000]
   0000 0000 1000 0000 0000 0000 0000 -> Pawn Start? [0x80000]
   0000 1111 0000 0000 0000 0000 0000 -> Piece promoted to[>> 20, 0xF]
   0001 0000 0000 0000 0000 0000 0000 -> Castle? [0x1000000]

*/

#define FROMSQ(m) ((m) & 0x7F)
#define TOSQ(m) (((m)>>7) & 0x7F)
#define CAPTURED(m) (((m)>>14) & 0xF)
#define PROMOTED(m) (((m)>>20) & 0xF)

#define MFLAGEP 0x40000    // Move flag En Pass
#define MFLAGPS 0x80000    // Move flag pawn start
#define MFLAGCA 0x1000000  // Move flag castle

#define MFLAGCAP 0x7C000   // Move flag captured
#define MFLAGPROM 0xF00000 // Move flag promoted

class Move {

public:
 Move();

 uint32_t m_move;
 int m_score;

 std::string moveString() const;  

};

class MoveList {
	std::vector<Move> m_move_vec;
};


#endif //MOVE_H