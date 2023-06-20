#include "./mcts.hpp"
#include "../state/state.hpp"
#include <fstream>
#include <iostream>
#include <random>

#define NEG_INF -1000000
#define POS_INF 1000000

MCTNode::MCTNode(State *state, Move from, MCTNode *parent) {
    this->state = state;
    this->from = from;
    this->parent = parent;
    if (state->legal_actions.empty()) state->get_legal_actions();
}

MCTNode *MCTNode::getBestChild() {
    MCTNode *bestChild = nullptr;
    double bestScore = NEG_INF;
    for (auto &child : this->children) {
        float UCB = child->calculateUCB();
        if (UCB > bestScore) {
            bestScore = UCB;
            bestChild = child;
        }
    }
    return bestChild;
}

MCTNode *MCTNode::select() {
    MCTNode *node = this;
    // accumulate visit count here to avoid divide by zero at the first time
    while (!node->isLeaf()) {
        if (node->isFullyExpanded()) {
            node = node->getBestChild();
        } else {
            break;
        }
    }
    return node;
}

MCTNode *MCTNode::expand() {
    auto action = this->state->legal_actions[this->children.size()];
    auto newChild = new MCTNode(this->state->next_state(action), action, this);
    this->children.push_back(newChild);
    return newChild;
}

void MCTNode::update(float result, int asPlayer) {
    this->visit_count++;
    if (this->state->player == asPlayer) result = 1 - result;
    this->win_score += result;
}

void MCTNode::backPropagate(float result, int asPlayer) {
    MCTNode *node = this;
    while (node != nullptr) {
        node->update(result, asPlayer);
        node = node->parent;
    }
}

int MCTS::evaluateAB(State *state, int asPlayer, int depth, int alpha, int beta) {
    if (state->game_state != GameState::NONE) {
        if (state->game_state == GameState::WIN) {
            return state->player == asPlayer ? POS_INF : NEG_INF;
        } else {
            return 0;
        }
    }
    if (depth == 0) return state->evaluatePSS() * (state->player == asPlayer ? 1 : -1);
    state->get_legal_actions();
    if (state->legal_actions.empty()) return state->evaluatePSS() * (state->player == asPlayer ? 1 : -1);
    if (state->player == asPlayer) {
        int maxScore = NEG_INF;
        for (auto &a : state->legal_actions) {
            auto nextState = state->next_state(a);
            int score = evaluateAB(nextState, asPlayer, depth - 1, alpha, beta);
            maxScore = std::max(maxScore, score);
            alpha = std::max(alpha, score);
            delete nextState;
            if (beta <= alpha) break;
        }
        return maxScore;
    } else {
        int minScore = POS_INF;
        for (auto &a : state->legal_actions) {
            auto nextState = state->next_state(a);
            int score = evaluateAB(nextState, asPlayer, depth - 1, alpha, beta);
            minScore = std::min(minScore, score);
            beta = std::min(beta, score);
            delete nextState;
            if (beta <= alpha) break;
        }
        return minScore;
    }
}

#define SIM_POLICY 0
#define SIM_DEPTH 200

static std::random_device rd;
static std::mt19937 gen(rd());
static std::uniform_real_distribution<> dis(0, 1);

/**
 * @brief simulate a game until it ends
 * @return 0 if draw, positive if win, negative if lose
 */
float MCTS::simulate(State *_state, int asPlayer, int depth = SIM_DEPTH) {
    State *state = new State(_state);
    state->get_legal_actions();
    while (state->game_state != GameState::WIN && depth--) {
        if (state->legal_actions.empty()) state->get_legal_actions();
#if SIM_POLICY == 0   // random
        auto action = state->legal_actions[dis(gen) * state->legal_actions.size()];
        auto *nextState = state->next_state(action);
        delete state;
        state = nextState;
#elif SIM_POLICY == 1   // greedy
        State *bestNextState = nullptr;
        int bestScore = NEG_INF;
        for (auto &a : state->legal_actions) {
            auto nextState = state->next_state(a);
            int score = -nextState->evaluatePSS();
            if (score > bestScore) {
                bestScore = score;
                if (bestNextState) delete bestNextState;
                bestNextState = nextState;
            } else {
                delete nextState;
            }
        }
        delete state;
        state = bestNextState;
#elif SIM_POLICY == 2   // alpha-beta
        auto bestNextState = state->next_state(state->legal_actions[0]);
        int maxScore = NEG_INF;
        for (auto &a : state->legal_actions) {
            auto nextState = state->next_state(a);
            int score = evaluateAB(nextState, state->player, 2, NEG_INF, POS_INF);
            if (score > maxScore) {
                maxScore = score;
                delete bestNextState;
                bestNextState = nextState;
            } else {
                delete nextState;
            }
        }
        delete state;
        state = bestNextState;
#endif
    }
    int winner;
    if (state->game_state == GameState::WIN) {
        winner = state->player;
    } else {
        int score = state->evaluatePSS();
        if (score == 0) return 0.5;
        winner = score > 0 ? state->player : 1 - state->player;
    }
    delete state;
    return winner == asPlayer ? 1.0 : 0.0;

    // float score = (float)state->evaluatePSS() / 100.0;
    // score = state->player == asPlayer ? score : -score;
    // delete state;
    // return score;
}

void PrintTree(MCTNode *root, int indent = 0) {
    if (indent >= 2) return;
    for (int i = 0; i < indent; i++) std::cout << "  ";
    std::cout << "(" << root->from.first.first << "," << root->from.first.second << " -> " << root->from.second.first << "," << root->from.second.second << ")";
    std::cout << " " << root->win_score << "/" << root->visit_count;
    if (root->parent) std::cout << " " << root->calculateUCB();
    std::cout << std::endl;
    for (auto &child : root->children) {
        PrintTree(child, indent + 1);
    }
}

Move MCTS::getBestMove(const MCTNode *root) {
    Move bestMove;
    float best_score = NEG_INF;
    for (auto &child : root->children) {
        auto score = child->visit_count;
        if (score > best_score) {
            best_score = score;
            bestMove = child->from;
        }
    }
    return bestMove;
}

Move MCTS::get_move(State *state, int iteration, int generation, std::ofstream &fout) {
    State *_state = new State(state->board, state->player);
    MCTNode *root = new MCTNode(_state);
    int asPlayer = state->player;

    while (generation--) {
        int i = iteration;
        while (i--) {
            MCTNode *node = root->select();                   // selection
            if (!node->isTerminal()) node = node->expand();   // expansion
            float result = simulate(node->state, asPlayer);   // simulation
            node->backPropagate(result, asPlayer);                      // back propagation
        }

        // PrintTree(root);

        // get best move
        auto bestMove = getBestMove(root);
        // write to file
        fout << bestMove.first.first << " " << bestMove.first.second << " "
             << bestMove.second.first << " " << bestMove.second.second << std::endl;
        fout.flush();
    }
    auto bestMove = getBestMove(root);
    delete root;
    return bestMove;
}