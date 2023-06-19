#pragma once
#include "../state/state.hpp"
#include <cmath>

#define UCB_C 1.3862943611198906   // C^2

class MCTNode {
public:
    State *state;
    MCTNode *parent;
    std::vector<MCTNode *> children;
    int visit_count = 0;
    float win_score = 0;
    Move from;

    MCTNode(State *state, Move from = Move(), MCTNode *parent = nullptr);
    ~MCTNode() {
        for (auto &child : children) delete child;
        delete state;
    }

    MCTNode *getBestChild();
    MCTNode *select();
    MCTNode *expand();
    void update(float result);
    void backPropagate(float result);

    // win or draw
    bool isTerminal() { return this->state->game_state != GameState::NONE; }
    // have no children
    bool isLeaf() { return this->children.empty(); }
    // have all possible children
    bool isFullyExpanded() { return this->children.size() == this->state->legal_actions.size(); }

    float calculateUCB() {
        return this->win_score / this->visit_count +
               sqrt(UCB_C * log(this->parent->visit_count) / this->visit_count);
    }
};

/**
 * @brief Minimax + Piece Score Sum
 */
class MCTS {
    static int evaluateAB(State *state, int asPlayer, int depth, int alpha, int beta);
    static float simulate(State *state, int asPlayer, int depth);
    static Move getBestMove(const MCTNode *root);

public:
    static Move get_move(State *state, int iteration, int generation, std::ofstream &fout);
};