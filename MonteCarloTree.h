#ifndef MONTE_CARLO_TREE_H
#define MONTE_CARLO_TREE_H
#include"MonteCarloNode.h"


template<typename GameState_t, typename Move_t>
class MonteCarloTree {
public:

	MonteCarloNode<GameState_t, Move_t>* root = nullptr;

	MonteCarloTree() {
		root = MonteCarloNode<GameState_t, Move_t>::provideRootNode();
	}

	void printCurrentGameState() {
		root->print();
	}

	void makeMove(const Move_t& move) {
		MonteCarloNode<GameState_t, Move_t>* nextNode = root->getNextState(move);
		if (nextNode != nullptr) {
			nextNode->detachFromParent();
			delete root;
			root = nextNode;
		}
	}

	bool mctsStep(const int rolloutDepth) {
		MonteCarloNode<GameState_t, Move_t>* selectedNode = root;

		while (selectedNode->isExpanded()) {
			if (selectedNode->isTerminal()) {
				int winValue = selectedNode->getWinValue();
				selectedNode->backprop(winValue);
				return true; // early exit returns true
			}
			selectedNode = selectedNode->getHighestUCB1().second;
		}

		selectedNode->expand();
		int rolloutResult = selectedNode->rollout(rolloutDepth);
		selectedNode->backprop(rolloutResult);
		return false; // normal exit returns false
	}

	Move_t monteCarloTreeSearch(const int numIterations, const int rolloutDepth) {
		for (int i = 0; i < numIterations; i++) {
			if (mctsStep(rolloutDepth)) {
				break;
			}
		}

		return root->getHighestUCB1().first;
	}

};

#endif // !MONTE_CARLO_TREE_H