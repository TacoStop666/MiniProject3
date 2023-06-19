#include <cstdlib>
#include <vector>
#include <utility>
#include <climits>
#include <map>
#include <unordered_map>
#include <iostream>
#include "../state/state.hpp"
#include "./nb.hpp"

using namespace std;

/**
 * @brief Randomly get a legal action
 * 
 * @param state Now state
 * @param depth You may need this for other policy
 * @return Move 
 */

/* original
Move nb::get_move(State *state, int depth){
  map<int, Move> m;
  if(!state->legal_actions.size())
    state->get_legal_actions();
  auto actions = state->legal_actions;
  for(auto it : actions){
    State *next = state->next_state(it);
    int value = next->evaluate();
    m[value] = it;
  }
  int max_value = max(m.begin()->first, m.rbegin()->first);
  return m[max_value];
}
*/

Move nb::get_move(State *state, int depth) {
  map<int, Move> m;
  if (depth == 0 || state->legal_actions.empty())
    state->get_legal_actions();
  auto actions = state->legal_actions;
  int max_value = INT_MIN;
  for (auto it : actions) {
    State *next = state->next_state(it);
    int value = evaluate_next_state(next, depth - 1, max_value, INT_MAX); // 使用Alpha-Beta剪枝
    if (value > max_value) {
      max_value = value;
      m[max_value] = it;
    }
  }
  return m[max_value];
}

int nb::evaluate_next_state(State *state, int depth, int alpha, int beta) {
  if (depth == 0 || state->legal_actions.empty())
    state->get_legal_actions();
  auto actions = state->legal_actions;
  int max_value = INT_MIN;
  for (auto it : actions) {
    State *next = state->next_state(it);
    int value = next->evaluate();
    if (depth > 1) {
      value = evaluate_next_state(next, depth - 1, alpha, beta); // 使用Alpha-Beta剪枝
    }
    if (value > max_value) {
      max_value = value;
    }
    alpha = max(alpha, max_value);
    if (alpha >= beta)
      break; 
  }
  return max_value;
}