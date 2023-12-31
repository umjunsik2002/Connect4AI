#include "connect_four.h"
#include <cstdlib>
#include <limits>
#include <iostream>
#include <string>

// 1  Player
// 2  AI

bool AI_mode = true;
bool player_first = true;
int player_number = player_first ? 1 : 2;
int AI_number = player_first ? 2 : 1;

c4::c4()
{
}

c4::~c4()
{
}

//
// make an X or an O
//
Bit* c4::PieceForPlayer(const int playerNumber)
{
    // depending on playerNumber load the "x.png" or the "o.png" graphic
    Bit *bit = new Bit();
    // should possibly be cached from player class?
    bit->LoadTextureFromFile(playerNumber == 1 ? "red.png" : "yellow.png");
    bit->setOwner(getCurrentPlayer());
    return bit;
}

void c4::setUpBoard()
{
    setNumberOfPlayers(2);
    for (int y=0; y<6; y++) {
        for (int x=0; x<7; x++) {
            _grid[y][x].initHolder(ImVec2(100*x + 100, 100*y + 100), "square.png", x, y);
        }
    }
    startGame();
}


//
// scan for mouse is temporarily in the actual game class
// this will be moved to a higher up class when the squares have a heirarchy
// we want the event loop to be elsewhere and calling this class, not called by this class
// but this is fine for tic-tac-toe
//
void c4::scanForMouse()
{
    ImVec2 mousePos = ImGui::GetMousePos();
    mousePos.x -= ImGui::GetWindowPos().x;
    mousePos.y -= ImGui::GetWindowPos().y;
    int highlightedColumn = -1; // Initialize to an invalid value.

    if (getCurrentPlayer()->playerNumber() == player_number-1) {
        for (int x=0; x<7; x++) {
            for (int y=0; y<6; y++) {
                if (_grid[y][x].isMouseOver(mousePos)) {
                    if (ImGui::IsMouseClicked(0)) {
                        for (int z=5; z>=0; z--) {
                            if (actionForEmptyHolder(&_grid[z][x])) {
                                endTurn();
                                break;
                            }
                        }
                    }
                    else {
                        highlightedColumn = x;
                        break;
                    }
                }
            }
        }
    }
    else if (AI_mode) {
        updateAI();
    }
    else {
        for (int x=0; x<7; x++) {
            for (int y=0; y<6; y++) {
                if (_grid[y][x].isMouseOver(mousePos)) {
                    if (ImGui::IsMouseClicked(0)) {
                        for (int z=5; z>=0; z--) {
                            if (actionForEmptyHolder(&_grid[z][x])) {
                                endTurn();
                                break;
                            }
                        }
                    }
                    else {
                        highlightedColumn = x;
                        break;
                    }
                }
            }
        }
    }

    for (int y=0; y<6; y++) {
        for (int x=0; x<7; x++) {
            if (x == highlightedColumn) {
                _grid[y][x].setHighlighted(true);
            } 
            else {
                _grid[y][x].setHighlighted(false);
            }
        }
    }
}

c4ai* c4::clone() {
    c4ai* newGameAI = new c4ai();
    std::string gamestate = stateString();
    for (int y=0; y<6; y++) {
        for (int x=0; x<7; x++) {
            int index = y*7 + x;
            int playerNumber = gamestate[index] - '0';
            newGameAI->_grid[y][x] = playerNumber;
            newGameAI->_depthSearches = 0;
        }
    }
    return newGameAI;
}


void c4::updateAI() {
    int bestVal = INT_MIN;
    int bestColumn = -1;

    for (int x=0; x<7; x++) {
        if (!_grid[0][x].bit()) {
            c4ai* newState = this->clone();
            for (int y=5; y>=0; y--) {
                if (!_grid[y][x].bit()) {
                    newState->_grid[y][x] = AI_number;
                    int moveVal = newState->minimax(newState, 4, true, -1000, 1000);
                    newState->_depthSearches += 1;
                    newState->_grid[y][x] = 0;
                    std::cout << "x: " << x << ", bestVal: " << bestVal << std::endl;
                    if (moveVal > bestVal) {
                        bestColumn = x;
                        bestVal = moveVal;
                    }
                    break;
                }
            }
            delete newState;
        }
    }
    if (bestColumn != -1) {
        for (int y=5; y>=0; y--) {
            if (actionForEmptyHolder(&_grid[y][bestColumn])) {
                endTurn();
                break;
            }
        }
    }
}

int c4ai::ownerAt(int index) const
{
    return _grid[index/7][index%7];
}

int c4ai::AICheckForWinner()
{
    static const int kWinningCombinations[69][4] = {
        {0, 1, 2, 3}, {1, 2, 3, 4}, {2, 3, 4, 5}, {3, 4, 5, 6},
        {7, 8, 9, 10}, {8, 9, 10, 11}, {9, 10, 11, 12}, {10, 11, 12, 13},
        {14, 15, 16, 17}, {15, 16, 17, 18}, {16, 17, 18, 19}, {17, 18, 19, 20}, 
        {21, 22, 23, 24}, {22, 23, 24, 25}, {23, 24, 25, 26}, {24, 25, 26, 27},
        {28, 29, 30, 31}, {29, 30, 31, 32}, {30, 31, 32, 33}, {31, 32, 33, 34},
        {35, 36, 37, 38}, {36, 37, 38, 39}, {37, 38, 39, 40}, {38, 39, 40, 41},
        {0, 7, 14, 21}, {7, 14, 21, 28}, {14, 21, 28, 35},
        {1, 8, 15, 22}, {8, 15, 22, 29}, {15, 22, 29, 36},
        {2, 9, 16, 23}, {9, 16, 23, 30}, {16, 23, 30, 37},
        {3, 10, 17, 24}, {10, 17, 24, 31}, {17, 24, 31, 38},
        {4, 11, 18, 25}, {11, 18, 25, 32}, {18, 25, 32, 39},
        {5, 12, 19, 26}, {12, 19, 26, 33}, {19, 26, 33, 40},
        {6, 13, 20, 27}, {13, 20, 27, 34}, {20, 27, 34, 41},
        {0, 8, 16, 24}, {1, 9, 17, 25}, {2, 10, 18, 26}, {3, 11, 19, 27},
        {7, 15, 23, 31}, {8, 16, 24, 32}, {9, 17, 25, 33}, {10, 18, 26, 34},
        {14, 22, 30, 38}, {15, 23, 31, 39}, {16, 24, 32, 40}, {17, 25, 33, 41},
        {3, 9, 15, 21}, {4, 10, 16, 22}, {5, 11, 17, 23}, {6, 12, 18, 24},
        {10, 16, 22, 28}, {11, 17, 23, 29}, {12, 18, 24, 30}, {13, 19, 25, 31},
        {17, 23, 29, 35}, {18, 24, 30, 36}, {19, 25, 31, 37}, {20, 26, 32, 38}
    };
    for( int i=0; i<69; i++ ) {
        const int *combination = kWinningCombinations[i];
        int p = ownerAt(combination[0]);
        if( p != 0 && p == ownerAt(combination[1]) && p == ownerAt(combination[2]) && p == ownerAt(combination[3]))
            return p;
    }
    return -1;
}

int c4ai::evaluateBoard() {
    int score = 0;
    int winner = AICheckForWinner();
    if (winner == player_number) {score -= -1000;}
    if (winner == AI_number) {score += 1000;}
    static const int frequency[42] = {
        3, 4, 5, 7, 5, 4, 3,
        5, 7, 9, 11, 9, 7, 5,
        7, 10, 13, 15, 13, 10, 7,
        8, 11, 14, 16, 14, 11, 8,
        8, 10, 12, 14, 12, 10, 8,
        8, 9, 10, 12, 10, 9, 8
    };
    for (int i=0; i<42; i++) {
        if (ownerAt(i) == player_number) {score += frequency[i];}
        if (ownerAt(i) == AI_number) {score -= frequency[i];}
    }
    static const int kWinningCombinations[69][4] = {
        {0, 1, 2, 3}, {1, 2, 3, 4}, {2, 3, 4, 5}, {3, 4, 5, 6},
        {7, 8, 9, 10}, {8, 9, 10, 11}, {9, 10, 11, 12}, {10, 11, 12, 13},
        {14, 15, 16, 17}, {15, 16, 17, 18}, {16, 17, 18, 19}, {17, 18, 19, 20}, 
        {21, 22, 23, 24}, {22, 23, 24, 25}, {23, 24, 25, 26}, {24, 25, 26, 27},
        {28, 29, 30, 31}, {29, 30, 31, 32}, {30, 31, 32, 33}, {31, 32, 33, 34},
        {35, 36, 37, 38}, {36, 37, 38, 39}, {37, 38, 39, 40}, {38, 39, 40, 41},
        {0, 7, 14, 21}, {7, 14, 21, 28}, {14, 21, 28, 35},
        {1, 8, 15, 22}, {8, 15, 22, 29}, {15, 22, 29, 36},
        {2, 9, 16, 23}, {9, 16, 23, 30}, {16, 23, 30, 37},
        {3, 10, 17, 24}, {10, 17, 24, 31}, {17, 24, 31, 38},
        {4, 11, 18, 25}, {11, 18, 25, 32}, {18, 25, 32, 39},
        {5, 12, 19, 26}, {12, 19, 26, 33}, {19, 26, 33, 40},
        {6, 13, 20, 27}, {13, 20, 27, 34}, {20, 27, 34, 41},
        {0, 8, 16, 24}, {1, 9, 17, 25}, {2, 10, 18, 26}, {3, 11, 19, 27},
        {7, 15, 23, 31}, {8, 16, 24, 32}, {9, 17, 25, 33}, {10, 18, 26, 34},
        {14, 22, 30, 38}, {15, 23, 31, 39}, {16, 24, 32, 40}, {17, 25, 33, 41},
        {3, 9, 15, 21}, {4, 10, 16, 22}, {5, 11, 17, 23}, {6, 12, 18, 24},
        {10, 16, 22, 28}, {11, 17, 23, 29}, {12, 18, 24, 30}, {13, 19, 25, 31},
        {17, 23, 29, 35}, {18, 24, 30, 36}, {19, 25, 31, 37}, {20, 26, 32, 38}
    };
    int row3 = 250;
    for(int i=0; i<69; i++) {
        const int *combination = kWinningCombinations[i];
        int p = ownerAt(combination[0]);
        if( p == player_number && p == ownerAt(combination[1]) && p == ownerAt(combination[2])) {
            score -= row3;
        }
        if( p == AI_number && p == ownerAt(combination[1]) && p == ownerAt(combination[2])) {
            score += row3;
        }
        if( p == player_number && p == ownerAt(combination[2]) && p == ownerAt(combination[3])) {
            score -= row3;
        }
        if( p == AI_number && p == ownerAt(combination[2]) && p == ownerAt(combination[3])) {
            score += row3;
        }
        if (p == player_number && p == ownerAt(combination[1])) {
            score -= 50;
        }
        if (p == AI_number && p == ownerAt(combination[1])) {
            score += 50;
        }
        int q = ownerAt(combination[3]);
        if( q == player_number && p == ownerAt(combination[0]) && p == ownerAt(combination[1])) {
            score -= row3;
        }
        if( q == AI_number && p == ownerAt(combination[0]) && p == ownerAt(combination[1])) {
            score += row3;
        }
        if( q == player_number && p == ownerAt(combination[1]) && p == ownerAt(combination[2])) {
            score -= row3;
        }
        if( q == AI_number && p == ownerAt(combination[1]) && p == ownerAt(combination[2])) {
            score += row3;
        }
        if (q == player_number && p == ownerAt(combination[2])) {
            score -= 50;
        }
        if (q == AI_number && p == ownerAt(combination[2])) {
            score += 50;
        }
        if (ownerAt(combination[1]) == player_number && ownerAt(combination[1]) == ownerAt(combination[2])) {
            score -= 50;
        }
        if (ownerAt(combination[1]) == AI_number && ownerAt(combination[1]) == ownerAt(combination[2])) {
            score += 50;
        }
    }
    return score;
}

bool c4ai::isBoardFull() const
{
    for (int y=0; y<6; y++) {
        for (int x=0; x<7; x++) {
            if (!_grid[y][x]) {
                return false;
            }
        }
    }
    return true;    
}

int c4ai::minimax(c4ai* state, int depth, bool isMaximizingPlayer, int alpha, int beta) {
    int score = state->evaluateBoard();
    // std::cout << "depth: " << depth << std::endl;

    // Check for terminal states or depth limit
    /*
    if (score >= 1000) {
        return score - depth * 100;
    }
    else if (score <= -1000) {
        return score + depth * 100;
    }
    */
    std::cout << "score: " << score << std::endl;
    if (state->isBoardFull() || depth == 0) {
        return score;
    }

    if (isMaximizingPlayer) {
        int bestVal = INT_MIN;
        for (int x = 0; x < 7; x++) {
            for (int y = 5; y >= 0; y--) {
                if (!state->_grid[y][x]) {
                    state->_grid[y][x] = AI_number;  // Make the move
                    int value = minimax(state, depth - 1, false, alpha, beta);  // Minimize opponent's score
                    state->_grid[y][x] = 0;  // Undo the move

                    bestVal = std::max(bestVal, value);
                    alpha = std::max(alpha, bestVal);

                    if (beta <= alpha) {
                        // return bestVal;  // Prune the tree
                    }
                    break;
                }
            }
        }
        return bestVal;
    }
    else {
        int bestVal = INT_MAX;
        for (int x = 0; x < 7; x++) {
            for (int y = 5; y >= 0; y--) {
                if (!state->_grid[y][x]) {
                    state->_grid[y][x] = player_number;  // Make the move
                    int value = minimax(state, depth - 1, true, alpha, beta);  // Maximize player's score
                    state->_grid[y][x] = 0;  // Undo the move

                    bestVal = std::min(bestVal, value);
                    beta = std::min(beta, bestVal);

                    if (beta <= alpha) {
                        // return bestVal;  // Prune the tree
                    }
                    break;
                }
            }
        }
        return bestVal;
    }
}


//
// draw the board and then the pieces
// this will also go somewhere else when the heirarchy is set up
//
void c4::drawFrame() 
{
    scanForMouse();

    for (int y=0; y<6; y++) {
        for (int x=0; x<7; x++) {
            _grid[y][x].paintSprite();
            if (_grid[y][x].bit()) {
                _grid[y][x].bit()->paintSprite();
            }
        }
    }
}

//
// about the only thing we need to actually fill out for tic-tac-toe
//
bool c4::actionForEmptyHolder(BitHolder *holder)
{
    if (holder->bit()) {
        return false;
    }
    Bit *bit = PieceForPlayer(getCurrentPlayer()->playerNumber());
    if (bit) {
        bit->setPosition(holder->getPosition());
        holder->setBit(bit);
        return true;
    }   
    return false;
}

bool c4::canBitMoveFrom(Bit *bit, BitHolder *src)
{
    // you can't move anything in tic tac toe
    return false;
}

bool c4::canBitMoveFromTo(Bit* bit, BitHolder*src, BitHolder*dst)
{
    // you can't move anything in tic tac toe
    return false;
}

//
// free all the memory used by the game on the heap
//
void c4::stopGame()
{
    for (int y=0; y<6; y++) {
        for (int x=0; x<7; x++) {
            _grid[y][x].destroyBit();
        }
    }
}

//
// helper function for the winner check
//
Player* c4::ownerAt(int index )
{
    int x = index%7;
    int y = index/7;
    if (!_grid[y][x].bit()) {
        return nullptr;
    }
    return _grid[y][x].bit()->getOwner();
}



Player* c4::checkForWinner()
{
    static const int kWinningCombinations[69][4] = {
        {0, 1, 2, 3}, {1, 2, 3, 4}, {2, 3, 4, 5}, {3, 4, 5, 6},
        {7, 8, 9, 10}, {8, 9, 10, 11}, {9, 10, 11, 12}, {10, 11, 12, 13},
        {14, 15, 16, 17}, {15, 16, 17, 18}, {16, 17, 18, 19}, {17, 18, 19, 20}, 
        {21, 22, 23, 24}, {22, 23, 24, 25}, {23, 24, 25, 26}, {24, 25, 26, 27},
        {28, 29, 30, 31}, {29, 30, 31, 32}, {30, 31, 32, 33}, {31, 32, 33, 34},
        {35, 36, 37, 38}, {36, 37, 38, 39}, {37, 38, 39, 40}, {38, 39, 40, 41},
        {0, 7, 14, 21}, {7, 14, 21, 28}, {14, 21, 28, 35},
        {1, 8, 15, 22}, {8, 15, 22, 29}, {15, 22, 29, 36},
        {2, 9, 16, 23}, {9, 16, 23, 30}, {16, 23, 30, 37},
        {3, 10, 17, 24}, {10, 17, 24, 31}, {17, 24, 31, 38},
        {4, 11, 18, 25}, {11, 18, 25, 32}, {18, 25, 32, 39},
        {5, 12, 19, 26}, {12, 19, 26, 33}, {19, 26, 33, 40},
        {6, 13, 20, 27}, {13, 20, 27, 34}, {20, 27, 34, 41},
        {0, 8, 16, 24}, {1, 9, 17, 25}, {2, 10, 18, 26}, {3, 11, 19, 27},
        {7, 15, 23, 31}, {8, 16, 24, 32}, {9, 17, 25, 33}, {10, 18, 26, 34},
        {14, 22, 30, 38}, {15, 23, 31, 39}, {16, 24, 32, 40}, {17, 25, 33, 41},
        {3, 9, 15, 21}, {4, 10, 16, 22}, {5, 11, 17, 23}, {6, 12, 18, 24},
        {10, 16, 22, 28}, {11, 17, 23, 29}, {12, 18, 24, 30}, {13, 19, 25, 31},
        {17, 23, 29, 35}, {18, 24, 30, 36}, {19, 25, 31, 37}, {20, 26, 32, 38}
    };
    for( int i=0; i<69; i++ ) {
        const int *combination = kWinningCombinations[i];
        Player *p = ownerAt(combination[0]);
        if( p && p == ownerAt(combination[1]) && p == ownerAt(combination[2]) && p == ownerAt(combination[3]))
            return p;
    }
    return nullptr;
}

//
// state strings
//
std::string c4::initialStateString()
{
    return "000000000000000000000000000000000000000000";
}

//
// this still needs to be tied into imguis init and shutdown
// we will read the state string and store it in each turn object
//
std::string c4::stateString()
{
    std::string s;
    for (int y=0; y<6; y++) {
        for (int x=0; x<7; x++) {
            Bit *bit = _grid[y][x].bit();
            if (bit) {
                s += std::to_string(bit->getOwner()->playerNumber()+1);
            } else {
                s += "0";
            }
        }
    }
    return s;
}

//
// this still needs to be tied into imguis init and shutdown
// when the program starts it will load the current game from the imgui ini file and set the game state to the last saved state
//
void c4::setStateString(const std::string &s)
{
    for (int y=0; y<6; y++) {
        for (int x=0; x<7; x++) {
            int index = y*6 + x;
            int playerNumber = s[index] - '0';
            if (playerNumber) {
                _grid[y][x].setBit( PieceForPlayer(playerNumber-1) );
            } else {
                _grid[y][x].setBit( nullptr );
            }
        }
    }
}

