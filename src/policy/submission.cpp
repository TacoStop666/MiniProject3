#include <cstdlib>
#include <iostream>
#include <map>
#include <climits>
#include "../state/state.hpp"
#include "./submission.hpp"

using namespace std;

/**
 * @brief Randomly get a legal action
 * 
 * @param state Now state
 * @param depth You may need this for other policy
 * @return Move 
 */
Move submission::get_move(State *state, int depth) {
  auto actions = state->legal_actions;
  int max_value = INT_MIN;
  Move move = actions[0];
  for (auto it : actions) {
    State *next = state->next_state(it);
    int value = minimax(next, depth, INT_MIN, INT_MAX, next->player);
    if (value > max_value) {
      max_value = value;
      move = it;
    }
  }
  return move;
}
int submission::minimax(State *state, int depth, int alpha, int beta, bool player){
    if(depth == 0) return state->evaluate();
    if(player){
        state->get_legal_actions();
        auto actions = state->legal_actions;
        int max_value = INT_MIN;
        for(auto it : actions){
            State *next = state->next_state(it);
            int eval = minimax(next, depth - 1, alpha, beta, !player);
            max_value = max(max_value, eval);
            alpha = max(alpha, eval);
            if(beta <= alpha) break;
        }
        return max_value;
    }
    else{
        int min_value = INT_MAX;
        state->get_legal_actions();
        auto actions = state->legal_actions;
        for(auto it : actions){
            State *next = state->next_state(it);
            int eval = minimax(next, depth - 1, alpha, beta, !player);
            min_value = min(min_value, eval);
            alpha = max(beta, eval);
            if(beta <= alpha) break;
        }
        return min_value;        
    }
}
