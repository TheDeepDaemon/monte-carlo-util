#ifndef MONTE_CARLO_NODE_H
#define MONTE_CARLO_NODE_H
#include<iostream>
#include<list>
#include"Assert.h"
#include"GameState.h"
#include"random_util.h"


using std::vector;
using std::list;
using std::pair;
using std::cout;


inline double ucb1(const int totalValue, const int sims, const int totalSimsOfParent, const double constant = 2.0) {
	if (sims == 0) {
		return std::numeric_limits<double>::infinity();
	}
	return ((double)totalValue / (double)sims) + (constant * sqrt(log((double)totalSimsOfParent) / (double)sims));
}


template<typename GameState_t, typename Move_t>
class MonteCarloNode {
private:

	const GameState* curGameState;
	MonteCarloNode<GameState_t, Move_t>* parent = nullptr;
	vector<pair<Move_t, MonteCarloNode<GameState_t, Move_t>*>> nextStates;
	bool expanded = false;

	unsigned p1Wins = 0;
	unsigned p2Wins = 0;
	unsigned numRollouts = 0;
	bool terminal = false;
	const int playerToMove;

	MonteCarloNode(const GameState* gameState, MonteCarloNode<GameState_t, Move_t>* parent_) :
		curGameState(gameState), parent(parent_), playerToMove(curGameState->getPlayerToMove()) {
		ASSERT(curGameState != nullptr, "Game state cannot be null.");
		if (curGameState != nullptr && curGameState->getWinValue() != 0) {
			terminal = true;
			expanded = true;
		}
	}

public:

	~MonteCarloNode() {
		for (auto it = nextStates.begin(); it != nextStates.end(); ++it) {
			MonteCarloNode<GameState_t, Move_t>* child = it->second;
			if (child != nullptr && child->parent == this) {
				delete child;
			}
		}
		if (curGameState != nullptr) {
			delete curGameState;
		}
	}

	static MonteCarloNode<GameState_t, Move_t>* provideRootNode() {
		GameState* startingGameState = new GameState_t();
		startingGameState->setAtBeginningState();
		return new MonteCarloNode<GameState_t, Move_t>(startingGameState, nullptr);
	}

	void detachFromParent() {
		parent = nullptr;
	}

	void expand() {
		if (!expanded) {
			expanded = true;
			list<Move_t> moves = curGameState->getPossibleMoves();

			nextStates.reserve(moves.size());
			for (const Move_t& move : moves) {
				const GameState_t* ptr = (GameState_t*)curGameState;
				MonteCarloNode<GameState_t, Move_t>* newNode = new MonteCarloNode<GameState_t, Move_t>(new GameState_t(*ptr, move), this);
				nextStates.push_back(pair<Move_t, MonteCarloNode<GameState_t, Move_t>*>(move, newNode));
			}

			Random::shuffle(nextStates);
		}
	}

	void print() {
		curGameState->print();
	}

	MonteCarloNode<GameState_t, Move_t>* getNextState(const Move_t& move) {
		expand();
		for (auto it = nextStates.begin(); it != nextStates.end(); ++it) {
			if (it->first == move) {
				return it->second;
			}
		}
		return nullptr;
	}

	GameState* getGameState() {
		return curGameState;
	}

	bool isExpanded() {
		return expanded;
	}

	bool isTerminal() {
		return terminal;
	}

	int getWinValue() {
		return curGameState->getWinValue();
	}

	double getUCB1() {
		ASSERT(parent != nullptr, "Can't get UCB1 value of a root node.");

		int numWins = 0;
		if (playerToMove == 1) {
			numWins = p1Wins;
		}
		else if (playerToMove == -1) {
			numWins = p2Wins;
		}

		return ucb1(numWins, numRollouts, parent->numRollouts);
	}

	pair<Move_t, MonteCarloNode<GameState_t, Move_t>*> getHighestUCB1() {
		if (!terminal) {
			pair<Move_t, MonteCarloNode<GameState_t, Move_t>*> highestChild = nextStates[0];
			double highestValue = highestChild.second->getUCB1();
			for (size_t i = 0; i < nextStates.size(); i++) {
				pair<Move_t, MonteCarloNode<GameState_t, Move_t>*> curChild = nextStates[i];
				double curValue = curChild.second->getUCB1();
				if (curValue > highestValue) {
					highestValue = curValue;
					highestChild = curChild;
				}
			}
			return highestChild;
		}
		else {
			return pair<Move_t, MonteCarloNode<GameState_t, Move_t>*>(Move_t(), this);
		}
	}

	int rollout(const int maxDepth) {
		if (terminal) {
			return curGameState->getWinValue();
		}
		GameState_t gameState = *curGameState;
		for (int i = 0; i < maxDepth; i++) {
			list<Move_t> moves = gameState.getPossibleMoves();
			if (moves.size() > 0) {
				Move_t nextMove = gameState.pickMove();
				gameState.makeMove(nextMove);

				int winValue = gameState.getWinValue();
				if (winValue != 0) {
					return winValue;
				}

			}
		}
		return 0;
	}

	void backprop(const int winValue) {
		if (winValue == 1) {
			p1Wins++;
		}
		else if (winValue == -1) {
			p2Wins++;
		}
		numRollouts++;
		if (parent != nullptr) {
			parent->backprop(winValue);
		}
	}

};

#endif // !MONTE_CARLO_NODE_H