#include "move.hpp"
#include "board.hpp"

 Move::Move():m_move(0),m_score(0)
 {

 }

 std::string Move::moveString() const
 {
    std::string move_str;
	
	int ff = Board::FilesBrd[FROMSQ(m_move)];
	int rf = Board::RanksBrd[FROMSQ(m_move)];
	int ft = Board::FilesBrd[TOSQ(m_move)];
	int rt = Board::RanksBrd[TOSQ(m_move)];
	
	move_str.push_back('a'+ff);
    move_str.push_back('1'+rf);
	move_str.push_back('a'+ft);
	move_str.push_back('1'+rt);
	
	int promoted = PROMOTED(m_move);
	
	if(promoted) {
		char pchar = 'q';
		if(Board::PieceKnight[promoted]) {
			pchar = 'n';
		} else if(Board::PieceRookQueen[promoted] && !Board::PieceBishopQueen[promoted])  {
			pchar = 'r';
		} else if(!Board::PieceRookQueen[promoted] && Board::PieceBishopQueen[promoted])  {
			pchar = 'b';
		}
		
    	move_str.push_back(pchar);
	} 
	
	return move_str;
 }  
