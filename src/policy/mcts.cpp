#include "./mcts.hpp"
#include "../state/state.hpp"
#include <algorithm>
#include <fstream>
#include <iostream>

#define NEG_INF -1000000
#define POS_INF 1000000

MCTNode::MCTNode(State *state, Move from, MCTNode *parent) {
    this->state = state;
    this->from = from;
    this->parent = parent;
    if (state->legal_actions.size() == 0) state->get_legal_actions();
    // randomize legal actions and store it
    // std::random_shuffle(this->state->legal_actions.begin(), this->state->legal_actions.end());
}

MCTNode *MCTNode::getBestChild() {
    MCTNode *bestChild = nullptr;
    double bestScore = NEG_INF;
    for (auto &child : this->children) {
        if (child->UCB > bestScore) {
            bestScore = child->UCB;
            bestChild = child;
        }
    }
    return bestChild;
}

MCTNode *MCTNode::select() {
    MCTNode *node = this;
    // accumulate visit count here to avoid divide by zero at the first time
    ++node->visit_count;
    while (!node->isLeaf()) {
        if (node->isFullyExpanded()) {
            node = node->getBestChild();
            ++node->visit_count;
        } else {
            break;
        }
    }
    return node;
}

MCTNode *MCTNode::expand() {
    if (this->state->legal_actions.empty()) this->state->get_legal_actions();
    auto action = this->state->legal_actions[this->children.size()];
    auto newChild = new MCTNode(this->state->next_state(action), action, this);
    this->children.push_back(newChild);
    ++newChild->visit_count;
    return newChild;
}

void MCTNode::update(float result, int asPlayer) {
    // this->visit_count++;
    this->win_score += result;
    this->calculateUCB(this->state->player != asPlayer);
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
    if (depth == 0) return state->evaluatePSS();
    state->get_legal_actions();
    if (state->legal_actions.empty()) return state->evaluatePSS();
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

#define SIM_POLICY 1
#define SIM_DEPTH 30
/**
 * @brief simulate a game until it ends
 * @return 0 if draw, 1 if win, -1 if lose
 */
float MCTS::simulate(State *_state, int asPlayer, int depth = SIM_DEPTH) {
    State *state = new State(_state);
    state->get_legal_actions();
    while (state->game_state != GameState::WIN && depth--) {
        if (state->legal_actions.empty()) state->get_legal_actions();
#if SIM_POLICY == 0   // random
        auto action = state->legal_actions[rand() % state->legal_actions.size()];
        auto *nextState = state->next_state(action);
        delete state;
        state = nextState;
#elif SIM_POLICY == 1   // greedy
        auto bestNextState = state->next_state(state->legal_actions[0]);
        int maxScore = NEG_INF;
        for (auto &a : state->legal_actions) {
            auto nextState = state->next_state(a);
            int score = nextState->evaluatePSS();
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
    return winner == asPlayer ? 1 : 0;
    // float score = (float)state->evaluatePSS() / 200.0;
    // int winner = score > 0 ? state->player : 1 - state->player;
    // delete state;
    // return winner == asPlayer ? score : -score;
}

/*
void PrintTree(MCTNode *root, int indent = 0) {
    for (int i = 0; i < indent; i++) std::cout << "  ";
    std::cout << "(" << root->from.first.first << "," << root->from.first.second << " -> " << root->from.second.first << "," << root->from.second.second << ")";
    std::cout << " " << root->win_score << "/" << root->visit_count << " " << root->UCB << std::endl;
    for (auto &child : root->children) {
        PrintTree(child, indent + 1);
    }
}
*/

Move MCTS::getBestMove(const MCTNode *root) {
    Move bestMove;
    float best_score = NEG_INF;
    for (auto &child : root->children) {
        auto score = child->win_score / child->visit_count;
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
            // std::cout << "iteration@" << iteration << std::endl;
            // PrintTree(root);

            MCTNode *node = root->select();                   // selection
            if (!node->isTerminal()) node = node->expand();   // expansion
            float result = simulate(node->state, asPlayer);   // simulation
            node->backPropagate(result, asPlayer);            // back propagation
        }
        // get best move
        auto bestMove = getBestMove(root);
        // write to file
        fout << bestMove.first.first << " " << bestMove.first.second << " "
             << bestMove.second.first << " " << bestMove.second.second << std::endl;
    }
    auto bestMove = getBestMove(root);
    delete root;
    return bestMove;
}