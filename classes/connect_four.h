#pragma once
#include "Game.h"
#include "Square.h"
#include <string>

class c4ai {
public:
    int     _grid[6][7];
    int     _depthSearches;
    bool    isBoardFull() const;
    int     evaluateBoard();
    int     minimax(c4ai* state, int depth, bool isMaximizingPlayer, int alpha, int beta);
    int     ownerAt(int index) const;
    int     AICheckForWinner();
};

class c4 : public Game
{
public:
    c4();
    ~c4();

    // set up the board
    void    setUpBoard() override;
    void    drawFrame() override;

    Player* checkForWinner() override;

    std::string initialStateString() override;
    std::string stateString() override;
    void    setStateString(const std::string &s) override;
    bool    actionForEmptyHolder(BitHolder *holder) override;
    bool    canBitMoveFrom(Bit*bit, BitHolder *src) override;
    bool    canBitMoveFromTo(Bit* bit, BitHolder*src, BitHolder*dst) override;
    void    stopGame() override;

    c4ai*   clone();
	void    updateAI();

private:
    Bit*    PieceForPlayer(const int playerNumber);
    Player* ownerAt(int index );
    void    scanForMouse();

    Square   _grid[6][7];
};
