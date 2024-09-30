#include <iostream>
#include <cmath>
#include <string>
class Board; // Start it here so Piece can use Board

class Piece {
    protected:
        std::string color;
        int posX, posY;

    public:
        Piece(std::string pieceColor, int initialX, int initialY)
        : color(pieceColor), posX(initialX), posY(initialY) {}

        

        int getPosX()  { return posX; }
        int getPosY()  { return posY; }

        void setPos(int x, int y) {
            posX = x;
            posY = y;
        }

        std::string getColor() const { return color; }

        bool isOpponent(const Piece* otherPiece) {
            return otherPiece != nullptr && otherPiece->getColor() != this->getColor();
        }
        

        

        virtual bool move(int newX, int newY, Board& board) = 0;
       
        virtual bool canAttack(int targetX, int targetY, Board& board) = 0;
        
        virtual void getPiece() const {
            std::cout << "Piece: " << color << " is currently at position: " << posX << ", " << posY;
        }
        
        virtual ~Piece() {} //Destructor for proper cleanup
        virtual char getSymbol() const = 0; //pure virtual function
};

class Board {
    private: 
        Piece* board [8][8]; //2D array representing board

    public:
        Board() {
            for (int i = 0; i < 8; i++) {
                for (int j = 0; j < 8; j++) {
                    board[i][j] = nullptr;
                }
            }

        }

        
        
        //Placing a piece on the board
        void placePiece(Piece* piece, int x, int y) {
            board[x][y] = piece;
        }
        
        //Checking if the square is being used
        bool isUsed(int x, int y) const {
            return board[x][y] != nullptr;
        }

        //Get piece at a specific square
        Piece* getPiecePlacement(int x, int y) const {
            return board[x][y];
        }

        bool isOutOfBounds(int x, int y)  {
            return (x > 7 || y > 7 || x < 0 || y < 0); //checks if out of bounds
        }

        bool isOccupied(int x, int y)  {
        return !isOutOfBounds(x, y) && board[x][y] != nullptr; // Check if square is occupied
    }

        //Remove the piece from a specific square
        void removePiece(int x, int y) {
            if(!isOutOfBounds(x, y))
            board[x][y] = nullptr;
        }
        //Moves a piece and handles captures
        void movePiece(Piece* piece, int updX, int updY) {
            // Clear the piece's old position (using its current coordinates)
            int oldX = piece->getPosX();
            int oldY = piece->getPosY();

            // Remove the old piece from the board
            board[oldX][oldY] = nullptr;

            // Check if the target square is occupied and remove the piece if necessary
            if (isOccupied(updX, updY)) {
                removePiece(updX, updY); // Ensure you are removing the opponent's piece correctly
            }

            // Update the piece's position
            piece->setPos(updX, updY);

            // Place the piece on the new square
            board[updX][updY] = piece;

            
        }

        
        
        bool isPathClear(int startX, int startY, int endX, int endY)  {
        int changeX = (endX - startX);
        int changeY = (endY - startY);
        
        // Normalize the direction
        int stepX = (changeX == 0) ? 0 : (changeX > 0 ? 1 : -1);
        int stepY = (changeY == 0) ? 0 : (changeY > 0 ? 1 : -1);

        // Move from the starting position to the ending position
        int x = startX + stepX;
        int y = startY + stepY;

        while (x != endX || y != endY) {
            if (isOccupied(x, y)) {
                return false; // Path is blocked by another piece
            }
            x += stepX; // Move in the X direction
            y += stepY; // Move in the Y direction
        }
    return false;
}

        

        /*Checks if a square is currently "under attack" by any opponent piece. 
        Exclusively used for the King to not move into check*/
        bool isSquareAttacked(int x, int y, const std::string&attackerColor)  {
            for (int i = 0; i < 8; i++) {
                for (int j = 0; j < 8; j++) {
                    Piece* piece = board[i][j];
                    if(piece != nullptr && piece->getColor() == attackerColor) {
                            if(piece->canAttack(x, y, *this)) {
                                return true; // Square is under attack
                            }
                    }
                }
            }
        return false;
    }

        std::pair<int, int> lastTwoSquareMove; // Stores the (x, y) coordinates of the pawn that last moved two squares

            void printBoard() {
        for (int y = 7; y >= 0; --y) {
            for (int x = 0; x < 8; ++x) {
                if (board[x][y] != nullptr) {
                    std::cout << board[x][y]->getSymbol() << " "; // Each piece will have a symbol
                } else {
                    std::cout << ". "; // Empty square
                }
            }
            std::cout << std::endl;
        }
    
    }
};

class Rook : public Piece {
    public:
    bool hasMoved;
        Rook(std::string color, int initialX, int initialY)
            : Piece(color, initialX, initialY), hasMoved(false) {}

            char getSymbol() const override {
                return (color == "white") ? 'R' : 'r';
            }

            bool move(int updX, int updY, Board& board) override {
            int oldX;
            int oldY;
                if (board.isOutOfBounds(updX, updY)) {
                //Piece is out of bounds, illegal move.
                return false;
            }


                if ((updX != posX && updY == posY) || (updX == posX && updY != posY)) {
                    if(board.isPathClear(posX, posY, updX, updY)){
                        Piece* target = board.getPiecePlacement(updX, updY);

                        //If there is an opponent's piece at the destination, capture it.
                        if(target != nullptr && isOpponent(target)) {
                            board.movePiece(this, updX, updY);

                            hasMoved = true;
                            return true;
                        }
                        else if(target == nullptr) {
                            oldX = posX;
                            oldY = posY;
                            posX = updX;
                            posY = updY;
                            board.movePiece(this, posX, posY);
                            board.removePiece(oldX, oldY);
                            hasMoved = true;
                            return true;
                        }
                        else { return false; } //Trying to capture your own piece is illegal
                    } else {return false; } //Path is blocked
                }
                else { return false; } //Other illegal move
            }

        bool canAttack(int updX, int updY, Board& board) override {
        // Check if the target position is in the same row or column
        if (posX == updX || posY == updY) {
            // Check if there are any pieces in between
            if (board.isPathClear(updX, updY, updX, updY)) {
                Piece* targetPiece = board.getPiecePlacement(updX, updY);
                // If there's a piece at the target position and it's of the opposite color, can attack
                return targetPiece != nullptr && targetPiece->getColor() != color;
            }
        }
        return false; // Cannot attack if not in the same line or if path is blocked
    }

};

class King : public Piece {
    private: 
        bool isInCheck;
        bool hasMoved; //check if King has moved before initiating a castle
    public: 
        King(std::string color, int initialX, int initialY)
            : Piece(color, initialX, initialY), hasMoved(false) {}

            char getSymbol() const override {
                return (color == "white") ? 'K' : 'k';
            }

            void updateCheckStatus(Board& board)  {
                isInCheck = board.isSquareAttacked(posX, posY, (color == "white") ? "black" : "white");
            }
            bool canCastle(Board& board, Rook* rook) {
                if(hasMoved || rook ->hasMoved) return false;

                int direction = (rook->getPosX() > posX) ? 1 : -1; // 1 is King Side Castle, -1 is Queen side castle

                for(int x = posX + direction; x != rook->getPosX(); x += direction) {
                    if(board.isOccupied(x, posY)) {
                        return false; //Blocked by piece
                    }
                }

                //Check if squares the king passes over and/or lands on are not attacked
                for(int i = -1; i <= 1; ++i) {
                    if (board.isSquareAttacked(posX + (2 * direction), posY, (color == "white") ? "black" : "white")) {
                        return false; //square under attack
                    }

                }

                return true; //Conditions have been met, you can castle
            }

            bool castle(Board& board, Rook* rook) {
                if(!canCastle(board, rook)) { return false; } //Self explanatory, checks if you can castle or not before allowing you to try it

                //Going through with castling:
                int direction = (rook->getPosX() > posX) ? 1 : -1; //Must determine which way we're castling, don't we?
                int rookNewPosX = posX + (direction == 1 ? 1 : -1); //Rook goes to the square next to the King

                //Move king
                board.movePiece(this, posX + (2 * direction), posY);
                //Move rook
                board.movePiece(rook, rookNewPosX, posY);

                hasMoved = true; //I'm gonna stop putting notes on self explanatory things, figure it out!
                rook->hasMoved = true; //Maybe I lied.

                return true; //Castling is a success!
            }

            bool canMove(Board& board) {
        // Check all possible moves (1 square in any direction)
        for (int dx = -1; dx <= 1; ++dx) {
            for (int dy = -1; dy <= 1; ++dy) {
                // Skip moving to the current position
                if (dx == 0 && dy == 0) continue;

                int newX = posX + dx;
                int newY = posY + dy;

                // Check if the new position is within bounds
                if (!board.isOutOfBounds(newX, newY)) {
                    Piece* target = board.getPiecePlacement(newX, newY);

                    // Simulate the move: Check if it would be in check after the move
                    if (target == nullptr || isOpponent(target)) {
                        // Temporarily move the King to the new position
                        board.movePiece(this, newX, newY);
                        if (!board.isSquareAttacked(newX, newY, (color == "white") ? "black" : "white")) {
                            // If the King is not in check after the move, it can move
                            board.movePiece(this, posX, posY); // Move back
                            return true; // Can move
                        }
                        board.movePiece(this, posX, posY); // Move back
                    }
                }
            }
        }
        return false; // No legal moves available
    }

        bool isCheckmate(Board& board) {
            updateCheckStatus(board);
            return isInCheck && !canMove(board);
        }

        bool move(int updX, int updY, Board& board) override {
            int oldX;
            int oldY;
        if (board.isOutOfBounds(updX, updY)) {
            return false; // Move is out of bounds
        }

            if(abs(updX - posX) <= 1 && abs (updY - posY) <= 1) {
                if(board.isSquareAttacked(updX, updY, (color == "white") ? "black" : "white"))
                {
                    return false; //Move would put the king in check, illegal move
                }

                Piece* target = board.getPiecePlacement(updX, updY);

                //Destination is the opponents piece? Capture it.
                if(target != nullptr && isOpponent(target)) {
                    oldX = posX;
                    oldY = posY;
                    posX = updX;
                    posY = updY;
                    board.movePiece(this, posX, posY);
                    board.removePiece(oldX, oldY);
                    hasMoved = true;
                    return true;
                }
                if(canMove(board)){
                    posX = updX;
                    posY = updY;
                    board.movePiece(this, posX, posY);
                    hasMoved = true;
                    return true;
                }
                //No piece there, just move it.
                
            }
            
        return false;

    }

    bool canAttack(int targetX, int targetY, Board& board) override {
        // Check if the target is within one square in any direction
        if (abs(targetX - posX) <= 1 && abs(targetY - posY) <= 1) {
            Piece* targetPiece = board.getPiecePlacement(targetX, targetY);
            // If there's a piece at the target position and it's of the opposite color, can attack
            return targetPiece != nullptr && targetPiece->getColor() != color;
        }
        return false; // Cannot attack if not within one square
    }
};



class Bishop : public Piece {
public:
    Bishop(std::string color, int initialX, int initialY)
        : Piece(color, initialX, initialY) {}

        char getSymbol() const override {
            return (color == "white") ? 'B' : 'b';
        }

    bool move(int updX, int updY, Board& board) override {
            int oldX;
            int oldY;
        // Check if the move is diagonal
        if (abs(updX - posX) == abs(updY - posY) && !board.isOutOfBounds(updX, updY)) {
            // Check for pieces in the way
            if (board.isPathClear(posX, posY, updX, updY)) {
                Piece* target = board.getPiecePlacement(updX, updY);
                if (target == nullptr || isOpponent(target)) {
                    // Move the piece
                    oldX = posX;
                    oldY = posY;
                    posX = updX;
                    posY = updY;
                    board.movePiece(this, posX, posY);
                    board.removePiece(oldX, oldY);
                    return true;
                }
            }
        }
        return false; // Illegal move
    }

    bool canAttack(int targetX, int targetY, Board& board) override {
        int changeX = abs(targetX - posX);
        int changeY = abs(targetY - posY);

        // Bishop can attack only on diagonal lines
        return changeX == changeY && board.isPathClear(posX, posY, targetX, targetY) && board.isOccupied(targetX, targetY) && board.getPiecePlacement(targetX, targetY)->isOpponent(this);
    }
};

class Knight : public Piece {
public:
    Knight(std::string color, int initialX, int initialY)
        : Piece(color, initialX, initialY) {}
        char getSymbol() const override {
            return (color == "white") ? 'N' : 'n';
        }
     // Override the move function for the Knight
        bool move(int updX, int updY, Board& board) override {
            int oldX;
            int oldY;
            int changeX = abs(updX - posX);
            int changeY = abs(updY - posY);

        // Check if the move is in the "L" shape (2 squares in one direction and 1 square in the other)
            if ((changeX == 2 && changeY == 1) || (changeX == 1 && changeY == 2)) {
                // Check if the target square is either empty or occupied by an opponent's piece
                if (!board.isOccupied(updX, updY) || board.getPiecePlacement(updX, updY)->isOpponent(this)) {
                    oldX = posX;
                    oldY = posY;
                    posX = updX;
                    posY = updY;
                    board.movePiece(this, posX, posY);
                    board.removePiece(oldX, oldY);
                    return true;
                }
            }

            return false; // Invalid move
        }

            bool canAttack(int targetX, int targetY, Board& board) override {
                if(!board.isOutOfBounds(targetX, targetY)){
                    int changeX = abs(targetX - posX);
                    int changeY = abs(targetY - posY);

                    // Check if the target square is in the "L" shape (same as move function)
                    return (changeX == 2 && changeY == 1) || (changeX == 1 && changeY == 2);
                }
        return false;
    }
};

class Queen : public Piece {
public:
    Queen(std::string color, int initialX, int initialY)
        : Piece(color, initialX, initialY) {}

    char getSymbol() const override {
            return (color == "white") ? 'Q' : 'q';
        }

    bool move(int updX, int updY, Board& board) override {
            int oldX;
            int oldY;
        // Check if the move is straight or diagonal and legitimate
        if ((abs(updX - posX) == 0 || abs(updY - posY) == 0 || abs(updX - posX) == abs(updY - posY)) && 
            !board.isOutOfBounds(updX, updY) && board.isPathClear(posX, posY, updX, updY)) {
                Piece* target = board.getPiecePlacement(updX, updY);
                if (target == nullptr || isOpponent(target)) {
                    // Move the piece
                    oldX = posX;
                    oldY = posY;
                    posX = updX;
                    posY = updY;
                    board.movePiece(this, posX, posY);
                    board.removePiece(oldX, oldY);

                return true;
                    return true;
                }
        }
        return false; // Illegal move
    }
    
        bool canAttack(int targetX, int targetY, Board& board) override {
        int changeX = abs(targetX - posX);
        int changeY = abs(targetY - posY);

        // Queen can attack horizontally, vertically, or diagonally
        if ((changeX == 0 || changeY == 0 || changeX == changeY) && board.isPathClear(posX, posY, targetX, targetY)) {
            return true; // Can attack if path is clear
        }
        return false; // Cannot attack if path is blocked or invalid movement
    }
};

class Pawn : public Piece {
public:
    bool hasMoved; // Track if the pawn has moved before (for 2-square movement)
    int moves; //Tracks how many times the pawn moved for en passant
    Pawn(std::string color, int initialX, int initialY)
        : Piece(color, initialX, initialY), hasMoved(false) {}
        
        char getSymbol() const override {
            return (color == "white") ? 'P' : 'p';
        }
    // Override the move function for the Pawn
        
        bool move(int updX, int updY, Board& board) override {
            int oldX;
            int oldY;
            int changeX = abs(updX - posX);

            int direction = (color == "white") ? 1 : -1; // White pawns move up, black pawns move down
            

        // Forward movement (1 square forward)
        if (updX == posX && updY == posY + direction) {
            if (!board.isOccupied(updX, updY)) {
                oldX = posX;
                oldY = posY;
                posX = updX;
                posY = updY;
                board.movePiece(this, posX, posY);
                board.removePiece(oldX, oldY);
                hasMoved = true;
                return true;
            }
        }

        // First move (2 squares forward)
        if (!hasMoved && updX == posX && updY == posY + 2 * direction) {
            if (!board.isOccupied(updX, updY) && !board.isOccupied(updX, posY + direction)) {
                oldX = posX;
                oldY = posY;
                posX = updX;
                posY = updY;
                board.movePiece(this, posX, posY);
                board.removePiece(oldX, oldY);
                hasMoved = true;

                // Track that this pawn moved two squares
                board.lastTwoSquareMove = {posX, posY};

                return true;
            }
        }

        // Diagonal capture
        if (changeX == 1 && updY == posY + direction) {
            if (board.isOccupied(updX, updY) && board.getPiecePlacement(updX, updY)->isOpponent(this)) {
                oldX = posX;
                oldY = posY;
                posX = updX;
                posY = updY;
                board.movePiece(this, posX, posY);
                
                hasMoved = true;
                return true;
            }

            // Check for En Passant
            if (board.lastTwoSquareMove.first == updX && board.lastTwoSquareMove.second == posY) {
                // En Passant is possible
                oldX = posX;
                oldY = posY;
                posX = updX;
                posY = updY;
                board.removePiece(updX, updY);
                board.movePiece(this, posX, posY);
                board.removePiece(oldX, oldY);
                hasMoved = true;
                return true;
            }
        }

        return false; // Invalid move
    }


    // Pawn cannot attack in the same way it moves, so we override canAttack
    bool canAttack(int targetX, int targetY, Board& board) override {
        int direction = (color == "white") ? 1 : -1;
        int changeX = abs(targetX - posX);
        int changeY = targetY - posY;

        // Pawn can attack diagonally (1 square)
        return changeX == 1 && changeY == direction && board.isOccupied(targetX, targetY) && board.getPiecePlacement(targetX, targetY)->isOpponent(this);
    }
    
    // Pawn promotion - to be implemented later
    bool canPromote() {
        return (color == "white" && posY == 7) || (color == "black" && posY == 0);
    }
};