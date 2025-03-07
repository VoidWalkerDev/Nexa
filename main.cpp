  
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <random>
#include <chrono>
#include <algorithm>
#include <cstdlib>
#include <cmath>

class ChessEngine {
private:
    // Board representation: 8x8 board
    // 0 = empty, 1-6 = white pieces (pawn, knight, bishop, rook, queen, king)
    // 7-12 = black pieces (pawn, knight, bishop, rook, queen, king)
    std::vector<std::vector<int>> board;
    bool whiteToMove;
    bool whiteKingMoved;
    bool blackKingMoved;
    bool whiteKingsideRookMoved;
    bool whiteQueensideRookMoved;
    bool blackKingsideRookMoved;
    bool blackQueensideRookMoved;
    int enPassantCol;
    
    std::mt19937 rng;
    
    void initializeBoard() {
        // Initialize empty board
        board = std::vector<std::vector<int>>(8, std::vector<int>(8, 0));
        
        // Set up pawns
        for (int col = 0; col < 8; col++) {
            board[1][col] = 1; // white pawn
            board[6][col] = 7; // black pawn
        }
        
        // Set up other pieces
        board[0][0] = 4; board[0][7] = 4; // white rooks
        board[0][1] = 2; board[0][6] = 2; // white knights
        board[0][2] = 3; board[0][5] = 3; // white bishops
        board[0][3] = 5; // white queen
        board[0][4] = 6; // white king
        
        board[7][0] = 10; board[7][7] = 10; // black rooks
        board[7][1] = 8;  board[7][6] = 8;  // black knights
        board[7][2] = 9;  board[7][5] = 9;  // black bishops
        board[7][3] = 11; // black queen
        board[7][4] = 12; // black king
        
        whiteToMove = true;
        whiteKingMoved = false;
        blackKingMoved = false;
        whiteKingsideRookMoved = false;
        whiteQueensideRookMoved = false;
        blackKingsideRookMoved = false;
        blackQueensideRookMoved = false;
        enPassantCol = -1;
        
        // Initialize random number generator
        unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
        rng = std::mt19937(seed);
    }
    
    bool isWhitePiece(int piece) {
        return piece >= 1 && piece <= 6;
    }
    
    bool isBlackPiece(int piece) {
        return piece >= 7 && piece <= 12;
    }
    
    bool isEnemyPiece(int piece) {
        if (whiteToMove) {
            return isBlackPiece(piece);
        } else {
            return isWhitePiece(piece);
        }
    }
    
    bool isOwnPiece(int piece) {
        if (whiteToMove) {
            return isWhitePiece(piece);
        } else {
            return isBlackPiece(piece);
        }
    }
    
    // Populates moves with all pseudo–legal moves (note that moves leaving king in check are not removed)
    void getAllLegalMoves(std::vector<std::pair<std::pair<int, int>, std::pair<int, int>>>& moves) {
        moves.clear();
        
        for (int row = 0; row < 8; row++) {
            for (int col = 0; col < 8; col++) {
                int piece = board[row][col];
                
                // Skip empty squares and squares with enemy pieces
                if (piece == 0 || !isOwnPiece(piece)) {
                    continue;
                }
                
                // Normalize piece value (1-6 for both colors)
                int normalizedPiece = (piece > 6) ? piece - 6 : piece;
                
                switch (normalizedPiece) {
                    case 1: // Pawn
                        getPawnMoves(row, col, moves);
                        break;
                    case 2: // Knight
                        getKnightMoves(row, col, moves);
                        break;
                    case 3: // Bishop
                        getBishopMoves(row, col, moves);
                        break;
                    case 4: // Rook
                        getRookMoves(row, col, moves);
                        break;
                    case 5: // Queen (rook + bishop moves)
                        getBishopMoves(row, col, moves);
                        getRookMoves(row, col, moves);
                        break;
                    case 6: // King
                        getKingMoves(row, col, moves);
                        break;
                }
            }
        }
    }
    
    void getPawnMoves(int row, int col, std::vector<std::pair<std::pair<int, int>, std::pair<int, int>>>& moves) {
        int direction = whiteToMove ? 1 : -1;
        int startRow = whiteToMove ? 1 : 6;
        
        // Forward one square
        if (row + direction >= 0 && row + direction < 8 && board[row + direction][col] == 0) {
            moves.push_back({{row, col}, {row + direction, col}});
            
            // Forward two squares from starting position
            if (row == startRow && board[row + 2 * direction][col] == 0) {
                moves.push_back({{row, col}, {row + 2 * direction, col}});
            }
        }
        
        // Captures diagonally
        for (int dc : {-1, 1}) {
            if (col + dc >= 0 && col + dc < 8 && row + direction >= 0 && row + direction < 8) {
                // Normal capture
                if (board[row + direction][col + dc] != 0 && isEnemyPiece(board[row + direction][col + dc])) {
                    moves.push_back({{row, col}, {row + direction, col + dc}});
                }
                
                // En passant capture
                if (enPassantCol == col + dc) {
                    int epRow = whiteToMove ? 5 : 2;
                    if (row == epRow - direction) {
                        moves.push_back({{row, col}, {epRow, col + dc}});
                    }
                }
            }
        }
    }
    
    void getKnightMoves(int row, int col, std::vector<std::pair<std::pair<int, int>, std::pair<int, int>>>& moves) {
        const int knight_moves[8][2] = {
            {-2, -1}, {-2, 1}, {-1, -2}, {-1, 2},
            {1, -2},  {1, 2},  {2, -1},  {2, 1}
        };
        
        for (int i = 0; i < 8; i++) {
            int newRow = row + knight_moves[i][0];
            int newCol = col + knight_moves[i][1];
            
            if (newRow >= 0 && newRow < 8 && newCol >= 0 && newCol < 8) {
                if (board[newRow][newCol] == 0 || isEnemyPiece(board[newRow][newCol])) {
                    moves.push_back({{row, col}, {newRow, newCol}});
                }
            }
        }
    }
    
    void getBishopMoves(int row, int col, std::vector<std::pair<std::pair<int, int>, std::pair<int, int>>>& moves) {
        const int directions[4][2] = {
            {-1, -1}, {-1, 1}, {1, -1}, {1, 1}
        };
        
        for (int i = 0; i < 4; i++) {
            int dr = directions[i][0];
            int dc = directions[i][1];
            
            for (int distance = 1; distance < 8; distance++) {
                int newRow = row + dr * distance;
                int newCol = col + dc * distance;
                
                if (newRow < 0 || newRow >= 8 || newCol < 0 || newCol >= 8) {
                    break;
                }
                
                if (board[newRow][newCol] == 0) {
                    moves.push_back({{row, col}, {newRow, newCol}});
                } else if (isEnemyPiece(board[newRow][newCol])) {
                    moves.push_back({{row, col}, {newRow, newCol}});
                    break;
                } else {
                    break;
                }
            }
        }
    }
    
    void getRookMoves(int row, int col, std::vector<std::pair<std::pair<int, int>, std::pair<int, int>>>& moves) {
        const int directions[4][2] = {
            {-1, 0}, {1, 0}, {0, -1}, {0, 1}
        };
        
        for (int i = 0; i < 4; i++) {
            int dr = directions[i][0];
            int dc = directions[i][1];
            
            for (int distance = 1; distance < 8; distance++) {
                int newRow = row + dr * distance;
                int newCol = col + dc * distance;
                
                if (newRow < 0 || newRow >= 8 || newCol < 0 || newCol >= 8) {
                    break;
                }
                
                if (board[newRow][newCol] == 0) {
                    moves.push_back({{row, col}, {newRow, newCol}});
                } else if (isEnemyPiece(board[newRow][newCol])) {
                    moves.push_back({{row, col}, {newRow, newCol}});
                    break;
                } else {
                    break;
                }
            }
        }
    }
    
    void getKingMoves(int row, int col, std::vector<std::pair<std::pair<int, int>, std::pair<int, int>>>& moves) {
        // Normal king moves
        const int directions[8][2] = {
            {-1, -1}, {-1, 0}, {-1, 1}, {0, -1},
            {0, 1}, {1, -1}, {1, 0}, {1, 1}
        };
        
        for (int i = 0; i < 8; i++) {
            int newRow = row + directions[i][0];
            int newCol = col + directions[i][1];
            
            if (newRow >= 0 && newRow < 8 && newCol >= 0 && newCol < 8) {
                if (board[newRow][newCol] == 0 || isEnemyPiece(board[newRow][newCol])) {
                    moves.push_back({{row, col}, {newRow, newCol}});
                }
            }
        }
        
        // Castling
        if (whiteToMove) {
            if (!whiteKingMoved && row == 0 && col == 4) {
                // Kingside castling
                if (!whiteKingsideRookMoved && board[0][5] == 0 && board[0][6] == 0 && board[0][7] == 4) {
                    moves.push_back({{row, col}, {0, 6}});
                }
                // Queenside castling
                if (!whiteQueensideRookMoved && board[0][3] == 0 && board[0][2] == 0 && board[0][1] == 0 && board[0][0] == 4) {
                    moves.push_back({{row, col}, {0, 2}});
                }
            }
        } else {
            if (!blackKingMoved && row == 7 && col == 4) {
                // Kingside castling
                if (!blackKingsideRookMoved && board[7][5] == 0 && board[7][6] == 0 && board[7][7] == 10) {
                    moves.push_back({{row, col}, {7, 6}});
                }
                // Queenside castling
                if (!blackQueensideRookMoved && board[7][3] == 0 && board[7][2] == 0 && board[7][1] == 0 && board[7][0] == 10) {
                    moves.push_back({{row, col}, {7, 2}});
                }
            }
        }
    }
    
    // Makes a move given by a pair-of-pairs { {fromRow, fromCol}, {toRow, toCol} }
    void makeMove(const std::pair<std::pair<int, int>, std::pair<int, int>>& move) {
        int fromRow = move.first.first;
        int fromCol = move.first.second;
        int toRow = move.second.first;
        int toCol = move.second.second;
        
        int piece = board[fromRow][fromCol];
        
        // Reset en passant
        enPassantCol = -1;
        
        // Check for pawn double move (for en passant)
        if ((piece == 1 || piece == 7) && std::abs(fromRow - toRow) == 2) {
            enPassantCol = fromCol;
        }
        
        // Check for en passant capture
        if ((piece == 1 || piece == 7) && fromCol != toCol && board[toRow][toCol] == 0) {
            // This is an en passant capture
            int capturedRow = fromRow;
            board[capturedRow][toCol] = 0;
        }
        
        // Check for castling
        if (piece == 6 && fromRow == 0 && fromCol == 4) { // White king
            if (toCol == 6) {
                // Kingside castling
                board[0][5] = 4;  // Move rook
                board[0][7] = 0;
            } else if (toCol == 2) {
                // Queenside castling
                board[0][3] = 4;  // Move rook
                board[0][0] = 0;
            }
            whiteKingMoved = true;
        } else if (piece == 12 && fromRow == 7 && fromCol == 4) { // Black king
            if (toCol == 6) {
                // Kingside castling
                board[7][5] = 10;  // Move rook
                board[7][7] = 0;
            } else if (toCol == 2) {
                // Queenside castling
                board[7][3] = 10;  // Move rook
                board[7][0] = 0;
            }
            blackKingMoved = true;
        }
        
        // Update castling rights for rooks
        if (piece == 4) {
            if (fromRow == 0 && fromCol == 0) whiteQueensideRookMoved = true;
            if (fromRow == 0 && fromCol == 7) whiteKingsideRookMoved = true;
        } else if (piece == 10) {
            if (fromRow == 7 && fromCol == 0) blackQueensideRookMoved = true;
            if (fromRow == 7 && fromCol == 7) blackKingsideRookMoved = true;
        }
        
        // Update king moved flag
        if (piece == 6) whiteKingMoved = true;
        if (piece == 12) blackKingMoved = true;
        
        // Move the piece
        board[toRow][toCol] = board[fromRow][fromCol];
        board[fromRow][fromCol] = 0;
        
        // Check for pawn promotion (auto–queen for simplicity)
        if (piece == 1 && toRow == 7) {
            board[toRow][toCol] = 5;  // White queen
        } else if (piece == 7 && toRow == 0) {
            board[toRow][toCol] = 11;  // Black queen
        }
        
        // Switch player
        whiteToMove = !whiteToMove;
    }
    
    // Converts board coordinates (row, col) into algebraic notation (e.g. a1, e4)
    std::string coordToAlgebraic(int row, int col) {
        std::string result;
        result += static_cast<char>('a' + col);
        result += static_cast<char>('1' + row);
        return result;
    }
    
public:
    ChessEngine() {
        initializeBoard();
    }
    
    // Set up position using a very basic FEN parser (only piece placement and side-to-move)
    void setPosition(const std::string& fen) {
        // Reinitialize board first
        initializeBoard();
        
        std::vector<std::string> fenParts;
        std::istringstream iss(fen);
        std::string part;
        
        while (std::getline(iss, part, ' ')) {
            fenParts.push_back(part);
        }
        
        if (fenParts.empty()) return;
        
        // Parse board position
        int row = 7;
        int col = 0;
        
        for (char c : fenParts[0]) {
            if (c == '/') {
                row--;
                col = 0;
            } else if (c >= '1' && c <= '8') {
                col += (c - '0');
            } else {
                int piece = 0;
                switch (c) {
                    case 'P': piece = 1; break;  // White pawn
                    case 'N': piece = 2; break;  // White knight
                    case 'B': piece = 3; break;  // White bishop
                    case 'R': piece = 4; break;  // White rook
                    case 'Q': piece = 5; break;  // White queen
                    case 'K': piece = 6; break;  // White king
                    case 'p': piece = 7; break;  // Black pawn
                    case 'n': piece = 8; break;  // Black knight
                    case 'b': piece = 9; break;  // Black bishop
                    case 'r': piece = 10; break; // Black rook
                    case 'q': piece = 11; break; // Black queen
                    case 'k': piece = 12; break; // Black king
                }
                
                if (piece != 0 && row >= 0 && row < 8 && col >= 0 && col < 8) {
                    board[row][col] = piece;
                }
                col++;
            }
        }
        
        // Parse side to move
        if (fenParts.size() > 1) {
            whiteToMove = (fenParts[1] == "w");
        }
        
        // Castling rights and en passant fields are ignored for simplicity.
    }
    
    // Instead of choosing a random move, we now perform a search.
    // This function uses a basic negamax search with a fixed depth.
    std::string getBestMove() {
        const int SEARCH_DEPTH = 3;  // You can adjust the search depth here.
        std::vector<std::pair<std::pair<int, int>, std::pair<int, int>>> legalMoves;
        getAllLegalMoves(legalMoves);
        
        if (legalMoves.empty()) {
            return "0000";  // No legal moves
        }
        
        int bestScore = -1000000;
        std::pair<std::pair<int, int>, std::pair<int, int>> bestMove = legalMoves[0];
        
        for (const auto& move : legalMoves) {
            ChessEngine child = *this;  // Copy current position
            child.applyMove(move);
            int score = -child.negamax(SEARCH_DEPTH - 1);
            if (score > bestScore) {
                bestScore = score;
                bestMove = move;
            }
        }
        
        std::string fromSquare = coordToAlgebraic(bestMove.first.first, bestMove.first.second);
        std::string toSquare = coordToAlgebraic(bestMove.second.first, bestMove.second.second);
        
        return fromSquare + toSquare;
    }
    
    // Make a move using a string in algebraic coordinate notation, e.g. "e2e4"
    void makeMove(const std::string& moveStr) {
        if (moveStr.length() < 4) return;
        
        int fromCol = moveStr[0] - 'a';
        int fromRow = moveStr[1] - '1';
        int toCol = moveStr[2] - 'a';
        int toRow = moveStr[3] - '1';
        
        makeMove({{fromRow, fromCol}, {toRow, toCol}});
    }
    
    // -------------------------
    // New public helper methods for search
    // -------------------------
    
    // Returns all legal moves as a vector of moves (each move is given as a pair-of-pairs)
    std::vector<std::pair<std::pair<int, int>, std::pair<int, int>>> getLegalMoves() {
        std::vector<std::pair<std::pair<int, int>, std::pair<int, int>>> moves;
        getAllLegalMoves(moves);
        return moves;
    }
    
    // Applies a move (given as a pair-of-pairs) to the board.
    void applyMove(const std::pair<std::pair<int, int>, std::pair<int, int>>& move) {
        makeMove(move);
    }
    
    // Basic evaluation function (material only)
    int evaluateBoard() const {
        int score = 0;
        for (int row = 0; row < 8; row++) {
            for (int col = 0; col < 8; col++) {
                int piece = board[row][col];
                switch (piece) {
                    case 1: score += 100; break;   // White Pawn
                    case 2: score += 320; break;   // White Knight
                    case 3: score += 330; break;   // White Bishop
                    case 4: score += 500; break;   // White Rook
                    case 5: score += 900; break;   // White Queen
                    case 6: score += 20000; break; // White King
                    case 7: score -= 100; break;   // Black Pawn
                    case 8: score -= 320; break;   // Black Knight
                    case 9: score -= 330; break;   // Black Bishop
                    case 10: score -= 500; break;  // Black Rook
                    case 11: score -= 900; break;  // Black Queen
                    case 12: score -= 20000; break; // Black King
                }
            }
        }
        return score;
    }
    
    // Negamax search (without alpha-beta pruning). Returns a score from the point-of-view 
    // of the side to move.
    int negamax(int depth) {
        if (depth == 0) {
            // Evaluate from white's perspective; if it is black’s turn, negate the value.
            int eval = evaluateBoard();
            return whiteToMove ? eval : -eval;
        }
        
        std::vector<std::pair<std::pair<int, int>, std::pair<int, int>>> moves = getLegalMoves();
        if (moves.empty()) {
            // No legal moves (checkmate/stalemate): return a very bad score.
            return -100000;
        }
        
        int bestScore = -1000000;
        for (const auto& move : moves) {
            ChessEngine child = *this;
            child.applyMove(move);
            int score = -child.negamax(depth - 1);
            if (score > bestScore) {
                bestScore = score;
            }
        }
        return bestScore;
    }
};

// This function implements the UCI communication loop.
void uci() {
    ChessEngine engine;
    std::string line;
    
    while (std::getline(std::cin, line)) {
        std::istringstream iss(line);
        std::string token;
        iss >> token;
        
        if (token == "uci") {
            std::cout << "id name RandomChess" << std::endl;
            std::cout << "id author RandomChess Author" << std::endl;
            std::cout << "uciok" << std::endl;
        } else if (token == "isready") {
            std::cout << "readyok" << std::endl;
        } else if (token == "ucinewgame") {
            engine = ChessEngine();
        } else if (token == "position") {
            iss >> token;
            
            if (token == "startpos") {
                engine = ChessEngine();
                
                iss >> token;  // This should be "moves" or end-of-line.
                if (token == "moves") {
                    std::string move;
                    while (iss >> move) {
                        engine.makeMove(move);
                    }
                }
            } else if (token == "fen") {
                std::string fen;
                
                // Read the rest of the FEN string (which may include spaces)
                std::string fenPart;
                while (iss >> fenPart) {
                    if (fenPart == "moves") break;
                    fen += fenPart + " ";
                }
                
                engine.setPosition(fen);
                
                if (fenPart == "moves") {
                    std::string move;
                    while (iss >> move) {
                        engine.makeMove(move);
                    }
                }
            }
        } else if (token == "go") {
            // For simplicity we ignore extra 'go' parameters.
            std::string bestMove = engine.getBestMove();
            std::cout << "bestmove " << bestMove << std::endl;
        } else if (token == "quit") {
            break;
        }
    }
}

int main() {
    uci();
    return 0;
}