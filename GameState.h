#ifndef GAME_STATE_H
#define GAME_STATE_H
#include<list>

/**
* What to implement when implementing this class:
* -> The virtual functions (obviously)
* -> The constructor that takes GameState and Move
* -> The copy constructor and = operator
*/

class Move;

class GameState
{
public:

	virtual std::list<Move> getPossibleMoves() const = 0;

	virtual void setAtBeginningState() = 0;

	virtual void print() const = 0;

	virtual int getWinValue() const = 0;

	virtual Move pickMove() const = 0;

	virtual void makeMove(const Move& move) = 0;

	virtual int getPlayerToMove() const = 0;

};

#endif // !GAME_STATE_H