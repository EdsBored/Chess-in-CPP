#include <iostream>
#include "Chess_backend.h"

void initializeBoard(Board& board) {
    // Place white pieces
    board.placePiece(new Rook("white", 0, 0), 0, 0);
    board.placePiece(new Knight("white", 1, 0), 1, 0);
    board.placePiece(new Bishop("white", 2, 0), 2, 0);
    board.placePiece(new Queen("white", 3, 0), 3, 0);
    board.placePiece(new King("white", 4, 0), 4, 0);
    board.placePiece(new Bishop("white", 5, 0), 5, 0);
    board.placePiece(new Knight("white", 6, 0), 6, 0);
    board.placePiece(new Rook("white", 7, 0), 7, 0);
    // Place white pawns
    for (int x = 0; x < 8; ++x) {
        board.placePiece(new Pawn("white", x, 1), x, 1);
    }

    // Place black pieces
    board.placePiece(new Rook("black", 0, 7), 0, 7);
    board.placePiece(new Knight("black", 1, 7), 1, 7);
    board.placePiece(new Bishop("black", 2, 7), 2, 7);
    board.placePiece(new Queen("black", 3, 7), 3, 7);
    board.placePiece(new King("black", 4, 7), 4, 7);
    board.placePiece(new Bishop("black", 5, 7), 5, 7);
    board.placePiece(new Knight("black", 6, 7), 6, 7);
    board.placePiece(new Rook("black", 7, 7), 7, 7);

    // Place black pawns
    for (int x = 0; x < 8; ++x) {
        board.placePiece(new Pawn("black", x, 6), x, 6);
    }
}
bool parseInput(const std::string& input, int& x1, int& y1, int& x2, int& y2) {
    if (input.length() != 5 || input[2] != ' ') {
        return false;
    }
    // Convert columns ('a' to 'h') to 0-7
    x1 = input[0] - 'a';
    x2 = input[3] - 'a';
    
    // Convert rows ('1' to '8') to 0-7
    y1 = input[1] - '1';
    y2 = input[4] - '1';

    return x1 >= 0 && x1 < 8 && y1 >= 0 && y1 < 8 && x2 >= 0 && x2 < 8 && y2 >= 0 && y2 < 8;
}

void game(Board& board) {
    std::string input;
    bool isWhiteTurn = true;
    while (true) {
        board.printBoard();
        std::cout << (isWhiteTurn ? "White's" : "Black's") << " move: ";

        // Get player input
        std::getline(std::cin, input);

        int x1, y1, x2, y2;
        if (parseInput(input, x1, y1, x2, y2)) {
            Piece* piece = board.getPiecePlacement(x1, y1);
            
            // Check if there is a piece at the selected position
            if (piece == nullptr) {
                std::cout << "No piece at the selected square.\n";
                continue;
            }

            // Ensure the piece belongs to the current player
            if (isWhiteTurn && piece->getColor() != "white") {
                std::cout << "That is not a white piece!\n";
                continue;
            } 
            else if (!isWhiteTurn && piece->getColor() != "black") {
                std::cout << "That is not a black piece!\n";
                continue;
            }

            // Try to move the piece
            
            if (piece->move(x2, y2, board) && !board.isOccupied(x1, y1)) {
                // Check if the move places the opponent's king in check (for future implementation)
                // If successful, switch turns
                isWhiteTurn = !isWhiteTurn;
                
            } else {
                std::cout << "An error occurred.\n";
            }
        } 
        
        else {
            std::cout << "Invalid input. Please use format like 'e2 e4'.\n";
        }
        
        // Implement check, checkmate, and stalemate detection here
    }
}

int main() {
    // Create the board
    Board board;
    initializeBoard(board);
    //Game loop IF THERE ARE ISSUES GO TO THE GAME FUNCTION, NOT HERE, IF THAT WASN'T OBVIOUS ALREADY NOW IT IS
    //game function is located above, NOT in Chess_backend.h
    game(board);
    
    return 0;
}