#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <array>
#include <vector>
#include <cassert>
#include <cstdint> // for int8_t
#include "config.hpp"


/*Board Size, Don't change!*/
#define BOARD_H 6
#define BOARD_W 5


/*State*/
typedef std::pair<size_t, size_t> Point;
typedef std::pair<Point, Point> Move; // Point from, Point to
class Board{
  public:
    // 1: pawn
    // 2: rook
    // 3: knight
    // 4: bishop
    // 5: queen
    // 6: king
    char board[2][BOARD_H][BOARD_W] = {{
      //white
      {0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0},
      {1, 1, 1, 1, 1},
      {2, 3, 4, 5, 6},
    }, {
      //black
      {6, 5, 4, 3, 2},
      {1, 1, 1, 1, 1},
      {0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0},
    }};
};

enum GameState {
  UNKNOWN = 0, // initialization
  WIN,
  DRAW,
  NONE // finding legal movement
};

class State{
  public:
    //You may want to add more property for a state
    GameState game_state = UNKNOWN;
    Board board;
    int player = 0; // 0 white, 1 for black
    std::vector<Move> legal_actions; // every move.first is the same
    
    State(){};
    State(int player): player(player){};
    State(Board board): board(board){};
    State(Board board, int player): board(board), player(player){};
  
    State* next_state(Move move);
    void get_legal_actions();
    std::string encode_output();
    std::string encode_state();
};

/**
 * @brief return next state after the move
 * 
 * @param move 
 * @return State* 
 */
State* State::next_state(Move move){
  Board next = this->board;
  Point from = move.first, to = move.second;
  // move have two point, the first one is from, the second one is to 
  
  int8_t moved = next.board[this->player][from.first][from.second]; // original position
  //promotion for pawn, pawn to queen
  if(moved == 1 && (to.first==BOARD_H-1 || to.first==0)){
    moved = 5;
  }
  // take down opponent's chess
  // opponent's next position board
  if(next.board[1-this->player][to.first][to.second]){ 
    next.board[1-this->player][to.first][to.second] = 0;
  }
  
  next.board[this->player][from.first][from.second] = 0; // remove the chess from the previous position
  next.board[this->player][to.first][to.second] = moved; // put the chess to next position
  
  State* next_state = new State(next, 1-this->player); // take turn : 1-this->player
  
  if(this->game_state != WIN)
    next_state->get_legal_actions();
  return next_state;
}

// 城堡、主教
static const int move_table_rook_bishop[8][7][2] = {
  // rook
  {{0, 1}, {0, 2}, {0, 3}, {0, 4}, {0, 5}, {0, 6}, {0, 7}}, // right
  {{0, -1}, {0, -2}, {0, -3}, {0, -4}, {0, -5}, {0, -6}, {0, -7}}, // left
  {{1, 0}, {2, 0}, {3, 0}, {4, 0}, {5, 0}, {6, 0}, {7, 0}}, // down
  {{-1, 0}, {-2, 0}, {-3, 0}, {-4, 0}, {-5, 0}, {-6, 0}, {-7, 0}}, // up
  // bishop
  {{1, 1}, {2, 2}, {3, 3}, {4, 4}, {5, 5}, {6, 6}, {7, 7}}, // right down
  {{1, -1}, {2, -2}, {3, -3}, {4, -4}, {5, -5}, {6, -6}, {7, -7}}, // left down
  {{-1, 1}, {-2, 2}, {-3, 3}, {-4, 4}, {-5, 5}, {-6, 6}, {-7, 7}}, // right up
  {{-1, -1}, {-2, -2}, {-3, -3}, {-4, -4}, {-5, -5}, {-6, -6}, {-7, -7}}, // left up
};
// 馬
static const int move_table_knight[8][2] = {
  {1, 2}, {1, -2},
  {-1, 2}, {-1, -2},
  {2, 1}, {2, -1},
  {-2, 1}, {-2, -1},
};
static const int move_table_king[8][2] = {
  {1, 0}, {0, 1}, {-1, 0}, {0, -1}, 
  {1, 1}, {1, -1}, {-1, 1}, {-1, -1},
};


/**
 * @brief get all legal actions of now state
 * 
 */
void State::get_legal_actions(){
  // [Optional]
  // This method is not very efficient
  // You can redesign it
  this->game_state = NONE;
  std::vector<Move> all_actions; // store all the available moving direction
  auto self_board = this->board.board[this->player]; // my board
  auto oppn_board = this->board.board[1 - this->player]; // opponent's board
  
  int now_piece, oppn_piece;
  for(int i=0; i<BOARD_H; i+=1){
    for(int j=0; j<BOARD_W; j+=1){
      // assign self_board[i][j] value to now_piece, if now_piece != 0, then execute
      if((now_piece=self_board[i][j])){ 
        // std::cout << this->player << "," << now_piece << ' ';
        switch (now_piece){
          case 1: //pawn
            if(this->player && i<BOARD_H-1){
              //black
              if(!oppn_board[i+1][j] && !self_board[i+1][j])
                // down
                // add a possible move 
                all_actions.push_back(Move(Point(i, j), Point(i+1, j)));
              if(j<BOARD_W-1 && (oppn_piece=oppn_board[i+1][j+1])>0){
                // right down
                all_actions.push_back(Move(Point(i, j), Point(i+1, j+1)));
                if(oppn_piece==6){
                  this->game_state = WIN;
                  this->legal_actions = all_actions;
                  return;
                }
              }
              if(j>0 && (oppn_piece=oppn_board[i+1][j-1])>0){
                // left down
                all_actions.push_back(Move(Point(i, j), Point(i+1, j-1)));
                if(oppn_piece==6){
                  this->game_state = WIN;
                  this->legal_actions = all_actions;
                  return;
                }
              }
            }else if(!this->player && i>0){
              //white
              if(!oppn_board[i-1][j] && !self_board[i-1][j])
                // go up 
                all_actions.push_back(Move(Point(i, j), Point(i-1, j)));
              if(j<BOARD_W-1 && (oppn_piece=oppn_board[i-1][j+1])>0){
                // right up
                all_actions.push_back(Move(Point(i, j), Point(i-1, j+1)));
                if(oppn_piece==6){
                  this->game_state = WIN;
                  this->legal_actions = all_actions;
                  return;
                }
              }
              if(j>0 && (oppn_piece=oppn_board[i-1][j-1])>0){
                // left up
                all_actions.push_back(Move(Point(i, j), Point(i-1, j-1)));
                if(oppn_piece==6){
                  this->game_state = WIN;
                  this->legal_actions = all_actions;
                  return;
                }
              }
            }
            break;
          
          case 2: //rook
          case 4: //bishop
          case 5: //queen
            int st, end;
            switch (now_piece){
              case 2: st=0; end=4; break; // rook
              case 4: st=4; end=8; break; // bishop
              case 5: st=0; end=8; break; // queen
              default: st=0; end=-1;
            }
            for(int part=st; part<end; part+=1){
              auto move_list = move_table_rook_bishop[part];
              for(int k=0; k<std::max(BOARD_H, BOARD_W); k+=1){
                // i and j is current position
                int p[2] = {move_list[k][0] + i, move_list[k][1] + j};   
                
                if(p[0]>=BOARD_H || p[0]<0 || p[1]>=BOARD_W || p[1]<0) break; // out of board
                now_piece = self_board[p[0]][p[1]]; // the position I want to move, my chess
                if(now_piece) break; // encounter same color chess, cant move to the position
                
                all_actions.push_back(Move(Point(i, j), Point(p[0], p[1])));
                
                oppn_piece = oppn_board[p[0]][p[1]]; // the position I want to go, opponent's chess
                if(oppn_piece){
                  if(oppn_piece==6){
                    this->game_state = WIN;
                    this->legal_actions = all_actions;
                    return;
                  }else
                    break;
                };
              }
            }
            break;
          
          case 3: //knight
            for(auto move: move_table_knight){
              int x = move[0] + i;
              int y = move[1] + j;
              
              if(x>=BOARD_H || x<0 || y>=BOARD_W || y<0) continue;
              now_piece = self_board[x][y];
              if(now_piece) continue; // encounter same chess, try to find another possible movement
              all_actions.push_back(Move(Point(i, j), Point(x, y)));
              
              oppn_piece = oppn_board[x][y];
              if(oppn_piece==6){
                this->game_state = WIN;
                this->legal_actions = all_actions;
                return;
              }
            }
            break;
          
          case 6: //king
            for(auto move: move_table_king){
              int p[2] = {move[0] + i, move[1] + j};
              
              if(p[0]>=BOARD_H || p[0]<0 || p[1]>=BOARD_W || p[1]<0) continue;
              now_piece = self_board[p[0]][p[1]];
              if(now_piece) continue;
              
              all_actions.push_back(Move(Point(i, j), Point(p[0], p[1])));
              
              oppn_piece = oppn_board[p[0]][p[1]];
              if(oppn_piece==6){
                this->game_state = WIN;
                this->legal_actions = all_actions;
                return;
              }
            }
            break;
        }
      }
    }
  }
  std::cout << "\n";
  this->legal_actions = all_actions;
}

static std::string y_axis = "654321";
static std::string x_axis = "ABCDE";

void make_seperate_line(std::stringstream& ss){
  ss << "├";
  for(int w=0; w<BOARD_W; w+=1){
    for(int h=0; h<PIECE_STR_LEN; h+=1)
      ss << "─";
    ss << "──┼";
  }
  ss << "─┼───┤\n";
}
void add_axis(std::stringstream& ss){
  ss << "│";
  for(int w=0; w<BOARD_W; w+=1){
    for(int h=0; h<PIECE_STR_LEN/2; h+=1)
      ss << " ";
    ss << " " << x_axis[w] << " ";
    for(int h=0; h<PIECE_STR_LEN/2 - (PIECE_STR_LEN+1)%2; h+=1)
      ss << " ";
    ss << "│";
  }
  ss << " │   │\n";
}
/**
 * @brief encode the output for command line output
 * 
 * @return std::string 
 */
std::string State::encode_output(){
  std::stringstream ss;
  int now_piece;
  ss << "┌";
  for(int w=0; w<BOARD_W; w+=1){
    for(int h=0; h<PIECE_STR_LEN; h+=1)
      ss << "─";
    ss << "──┬";
  }
  ss << "─┬───┐\n";
  
  for(int i=0; i<BOARD_H; i+=1){
    for(int j=0; j<BOARD_W; j+=1){
      ss << "│ ";
      if((now_piece = this->board.board[0][i][j])){
        ss << std::string(PIECE_TABLE[0][now_piece]) << " ";
      }else if((now_piece = this->board.board[1][i][j])){
        ss << std::string(PIECE_TABLE[1][now_piece]) << " ";
      }else{
        ss << std::string(PIECE_TABLE[0][0]) << " ";
      }
    }
    ss << "│ │ " << y_axis[i] << " │\n";
    make_seperate_line(ss);
  }
  make_seperate_line(ss);
  add_axis(ss);
  
  ss << "└";
  for(int w=0; w<BOARD_W; w+=1){
    for(int h=0; h<PIECE_STR_LEN; h+=1)
      ss << "─";
    ss << "──┴";
  }
  ss << "─┴───┘\n";
  return ss.str();
}


/**
 * @brief encode the state to the format for player
 * 
 * @return std::string 
 */
std::string State::encode_state(){
  std::stringstream ss;
  ss << this->player;
  ss << "\n";
  for(int pl=0; pl<2; pl+=1){
    for(int i=0; i<BOARD_H; i+=1){
      for(int j=0; j<BOARD_W; j+=1){
        ss << int(this->board.board[pl][i][j]);
        ss << " ";
      }
      ss << "\n";
    }
    ss << "\n";
  }
  return ss.str();
}


/*game runner part!*/
const std::string file_log = "gamelog.txt";
const std::string file_state = "state";
const std::string file_action = "action";


void launch_executable(std::string filename) {
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
  //filename may has path in it. 
  //like "./build/player.exe" or ".\player.exe"
  size_t pos;
  std::string command = "start /min " + filename + " " + file_state + " " + file_action;
  if((pos = filename.rfind("/"))!=std::string::npos || (pos = filename.rfind("\\"))!=std::string::npos)
    filename = filename.substr(pos+1, std::string::npos);
  std::string kill = "timeout /t " + std::to_string(timeout) + " > NUL && taskkill /im " + filename + " > NUL 2>&1";
  system(command.c_str());
  system(kill.c_str());
#elif __linux__
  std::string command = "timeout " + std::to_string(timeout) + "s " + filename + " " + file_state + " " + file_action;
  system(command.c_str());
#elif __APPLE__
  // May require installing the command by:
  // brew install coreutils
  std::string command = "gtimeout " + std::to_string(timeout) + "s " + filename + " " + file_state + " " + file_action;
  system(command.c_str());
#endif
}


bool valid_move(Move move, std::vector<Move>& legal_moves){
  if(move.first.first>BOARD_H || move.first.second>BOARD_H){
    return false;
  }
  if(move.second.first>BOARD_W || move.second.second>BOARD_W){
    return false;
  }
  // my move : legal move
  for(Move mv: legal_moves){
    if(mv==move){
      return true;
    }
  }
  return false;
}


static const int material_table[7] = {0, 2, 6, 7, 8, 20, 100}; // value of each chess

int main(int argc, char** argv) {
  assert(argc == 3);
  std::ofstream log("gamelog.txt");
  std::string player_filename[3];
  player_filename[1] = argv[1];
  player_filename[2] = argv[2];
  
  std::cout << "Player White File: " << player_filename[1] << std::endl;
  std::cout << "Player Black File: " << player_filename[2] << std::endl;
  
  State game; game.get_legal_actions();
  State *temp;
  std::string data;
  int step=1;
  while (game.game_state == UNKNOWN || game.game_state == NONE) {
    // std::cout << "test\n";
    // Output current state
    std::cout << step << " step" << std::endl;
    log << step << " step" << std::endl;
    data = game.encode_output();
    std::cout << data << std::endl;
    log << data << std::endl;
    
    data = game.encode_state();
    std::ofstream fout(file_state);
    fout << data;
    fout.close();
    // Run external program
    launch_executable(player_filename[game.player+1]);
    // Read action
    std::ifstream fin(file_action);
    Move action(Point(-1, -1), Point(-1, -1));
    int total = 0;
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
    system("cls");
#else
    system("clear");
#endif
    while (true) {
      int x, y, n, m;
      if (!(fin >> x)) break;
      if (!(fin >> y)) break;
      if (!(fin >> n)) break;
      if (!(fin >> m)) break;
      action.first.first = x; // current x
      action.first.second = y; // current y
      action.second.first = n; // next x
      action.second.second = m; // next y
      total ++;
    }
    fin.close();

    if (remove(file_action.c_str()) != 0)
      std::cerr << "Error removing file: " << file_action << "\n";
    // Take action
    if (!valid_move(action, game.legal_actions)){
      // If action is invalid.
      data = game.encode_output();
      std::cout << "Invalid Action\n";
      std::cout << x_axis[action.first.second] << y_axis[action.first.first] << " → " \
                << x_axis[action.second.second] << y_axis[action.second.first] << "\n";
      std::cout << data;
      log << "Invalid Action\n";
      log << x_axis[action.first.second] << y_axis[action.first.first] << " → " \
          << x_axis[action.second.second] << y_axis[action.second.first] << "\n";
      log << data;
      game.player = !game.player;
      game.game_state = WIN;
      break;
    }else{
      temp = game.next_state(action);
      std::cout << "Depth: " << total << std::endl;
      std::cout << x_axis[action.first.second] << y_axis[action.first.first] << " → " \
                << x_axis[action.second.second] << y_axis[action.second.first] << "\n";
      log << "Depth: " << total << std::endl;
      log << x_axis[action.first.second] << y_axis[action.first.first] << " → " \
          << x_axis[action.second.second] << y_axis[action.second.first] << "\n";
    }
    game = *temp;
    
    step += 1;
    if(step>MAX_STEP){
      int white_material = 0;
      int black_material = 0;
      int piece;
      
      for(size_t i=0; i<BOARD_H; i+=1){
        for(size_t j=0; j<BOARD_W; j+=1){
          if((piece=game.board.board[0][i][j])){
            white_material += material_table[piece]; // record the total value of white
          }
          if((piece=game.board.board[1][i][j])){
            black_material += material_table[piece]; // record the total value of black 
          }
        }
      }
      if(white_material<black_material){
        game.player = 1;
        game.game_state = WIN;
      }else if(white_material>black_material){
        game.player = 0;
        game.game_state = WIN;
      }else{
        game.game_state = DRAW;
      }
    }
  }
  
  data = game.encode_output();
  std::cout << data << std::endl;
  log << data << std::endl;
  if(game.game_state == WIN){
    std::cout << "Player" << game.player+1 << " wins\n";
    log << "Player" << game.player+1 << " wins\n";
  }else{
    std::cout << "Draw\n";
    log << "Draw\n";
  }
  
  log.close();
  // Reset state file
  if (remove(file_state.c_str()) != 0)
    std::cerr << "Error removing file: " << file_state << "\n";
  return 0;
}

