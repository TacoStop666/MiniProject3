#include <cstdlib>
#include <iostream>
#include "../state/state.hpp"
#include "./minimax.hpp"
#include <climits>
using namespace std;

/**
 * @brief Randomly get a legal action
 * 
 * @param state Now state
 * @param depth You may need this for other policy
 * @return Move 
 */
Move Minimax::get_move(State *state, int depth){
  if(!state->legal_actions.size())
    state->get_legal_actions();
  auto actions = state->legal_actions;
  int max_value = INT_MIN;
  Move move = actions[0];
  for(auto it : actions){
    State *next = state->next_state(it);
    int value = minimax(next, depth, state->player);
    if(value > max_value){
      max_value = value;
      move = it;
    }
  }
  return move;
}

int Minimax::minimax(State *state, int depth, bool player){
  if(depth == 0) return state->evaluate();
  if(player){
    int max_value = INT_MIN;
    state->get_legal_actions();
    auto actions = state->legal_actions;
    for(auto it : actions){
      State *next = state->next_state(it);
      int value = minimax(next, depth - 1, 0);
      max_value = max(max_value, value);
    }
    return max_value;
  }
  else{
    int min_value = INT_MAX;
    state->get_legal_actions();
    auto actions = state->legal_actions;
    for(auto it : actions){
      State *next = state->next_state(it);
      int value = minimax(next, depth - 1, 1);
      min_value = min(min_value, value);
    }
    return min_value;
  }
}