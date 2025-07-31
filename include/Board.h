#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <cctype>
#include <cmath>

class Board {
public:
    Board() {
        LoadPieceImages();
        SetupInitialPosition();
        currentTurn = 'w';
        selectedPiece = "";
    }

    ~Board() {
        for (auto& pair : pieceSurfaces) {
            SDL_FreeSurface(pair.second);
        }
    }

    void StartNewGame() {
        std::cout << "Starting a new game" << std::endl;
        SetupInitialPosition();
        currentTurn = 'w';
        selectedPiece = "";
        possibleMoves.clear();
    }

    void HandleEvent(SDL_Event& e, SDL_Surface* surface) {
        if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
            int boardSize = 800;
            int tileSize = boardSize / 8;
            int boardX = (surface->w - boardSize) / 2;
            int boardY = (surface->h - boardSize) / 2;

            int mouseX = e.button.x;
            int mouseY = e.button.y;

            if (mouseX >= boardX && mouseX < boardX + boardSize &&
                mouseY >= boardY && mouseY < boardY + boardSize) {

                int x = (mouseX - boardX) / tileSize;
                int y = (mouseY - boardY) / tileSize;

                std::string clickedPiece = GetPieceAtPosition(x, y);

                if (selectedPiece.empty()) {
                    if (!clickedPiece.empty() && clickedPiece[0] == currentTurn) {
                        selectedPiece = clickedPiece;
                        possibleMoves = GetLegalMoves(selectedPiece);
                        std::cout << "Selected piece: " << selectedPiece << std::endl;
                    }
                } else {
                    // Check if clicked square is a possible move
                    if (IsInPossibleMoves(x, y)) {
                        MovePiece(selectedPiece, x, y);
                        selectedPiece = "";
                        possibleMoves.clear();
                        currentTurn = (currentTurn == 'w') ? 'b' : 'w';
                        std::cout << ((currentTurn == 'w') ? "White" : "Black") << "'s turn\n";
                    } else if (!clickedPiece.empty() && clickedPiece[0] == currentTurn) {
                        // Select a different piece of the current player
                        selectedPiece = clickedPiece;
                        possibleMoves = GetLegalMoves(selectedPiece);
                        std::cout << "Selected piece: " << selectedPiece << std::endl;
                    } else {
                        // Deselect if clicked empty or invalid square
                        selectedPiece = "";
                        possibleMoves.clear();
                    }
                }
            }
        }
    }

    void Render(SDL_Surface* surface) {
        int boardSize = 800;
        int tileSize = boardSize / 8;
        int boardX = (surface->w - boardSize) / 2;
        int boardY = (surface->h - boardSize) / 2;

        // Draw board tiles
        for (int y = 0; y < 8; ++y) {
            for (int x = 0; x < 8; ++x) {
                SDL_Rect tileRect = { boardX + x * tileSize, boardY + y * tileSize, tileSize, tileSize };
                bool isWhite = (x + y) % 2 == 0;
                SDL_FillRect(surface, &tileRect, SDL_MapRGB(surface->format, isWhite ? 240 : 100, isWhite ? 217 : 100, isWhite ? 181 : 100));
            }
        }

        // Highlight possible moves
        for (const auto& pos : possibleMoves) {
            SDL_Rect highlightRect = {
                boardX + pos.first * tileSize,
                boardY + pos.second * tileSize,
                tileSize,
                tileSize
            };
            Uint32 highlightColor = SDL_MapRGBA(surface->format, 0, 255, 0, 128); // semi-transparent green
            SDL_SetSurfaceBlendMode(surface, SDL_BLENDMODE_BLEND);
            SDL_FillRect(surface, &highlightRect, highlightColor);
            SDL_SetSurfaceBlendMode(surface, SDL_BLENDMODE_NONE);
        }

        // Highlight selected piece square
        if (!selectedPiece.empty()) {
            auto pos = piecePositions[selectedPiece];
            SDL_Rect highlightRect = {
                boardX + pos.first * tileSize,
                boardY + pos.second * tileSize,
                tileSize,
                tileSize
            };
            Uint32 highlightColor = SDL_MapRGBA(surface->format, 255, 255, 0, 128); // semi-transparent yellow
            SDL_SetSurfaceBlendMode(surface, SDL_BLENDMODE_BLEND);
            SDL_FillRect(surface, &highlightRect, highlightColor);
            SDL_SetSurfaceBlendMode(surface, SDL_BLENDMODE_NONE);
        }

        // Draw pieces
        for (const auto& pair : piecePositions) {
            const std::string& pieceKey = pair.first;
            int x = pair.second.first;
            int y = pair.second.second;

            std::string basePiece = pieceKey;
            while (!basePiece.empty() && isdigit(basePiece.back())) {
                basePiece.pop_back();
            }

            auto it = pieceSurfaces.find(basePiece);
            if (it != pieceSurfaces.end() && it->second) {
                SDL_Rect destRect = {
                    boardX + x * tileSize,
                    boardY + y * tileSize,
                    tileSize,
                    tileSize
                };
                SDL_BlitScaled(it->second, nullptr, surface, &destRect);
            }
        }
    }

private:
    std::map<std::string, SDL_Surface*> pieceSurfaces;
    std::map<std::string, std::pair<int, int>> piecePositions;
    std::string selectedPiece = "";
    char currentTurn = 'w'; // 'w' or 'b'
    std::vector<std::pair<int, int>> possibleMoves;

    std::string GetPieceAtPosition(int x, int y) {
        for (const auto& pair : piecePositions) {
            if (pair.second.first == x && pair.second.second == y) {
                return pair.first;
            }
        }
        return "";
    }

    void MovePiece(const std::string& pieceKey, int newX, int newY) {
        // Capture any piece at destination
        std::string targetPiece = GetPieceAtPosition(newX, newY);
        if (!targetPiece.empty() && targetPiece != pieceKey) {
            piecePositions.erase(targetPiece);
            std::cout << "Captured " << targetPiece << "!\n";
        }
        piecePositions[pieceKey] = {newX, newY};
        std::cout << "Moved " << pieceKey << " to (" << newX << "," << newY << ")\n";
    }

    bool IsLegalMove(const std::string& pieceKey, int newX, int newY) {
        auto it = piecePositions.find(pieceKey);
        if (it == piecePositions.end()) return false;
        int oldX = it->second.first;
        int oldY = it->second.second;

        char color = pieceKey[0];
        std::string basePiece = pieceKey;
        while (!basePiece.empty() && isdigit(basePiece.back())) {
            basePiece.pop_back();
        }
        char pieceType = basePiece[1]; // K,Q,R,B,N,P

        // Check board boundaries
        if (newX < 0 || newX > 7 || newY < 0 || newY > 7) return false;

        // Check if moving to square occupied by same color piece
        std::string destPiece = GetPieceAtPosition(newX, newY);
        if (!destPiece.empty() && destPiece[0] == color) return false;

        int dx = newX - oldX;
        int dy = newY - oldY;

        switch (pieceType) {
            case 'P': return IsLegalPawnMove(color, oldX, oldY, newX, newY);
            case 'N': return IsLegalKnightMove(dx, dy);
            case 'B': return IsLegalBishopMove(oldX, oldY, newX, newY);
            case 'R': return IsLegalRookMove(oldX, oldY, newX, newY);
            case 'Q': return IsLegalQueenMove(oldX, oldY, newX, newY);
            case 'K': return IsLegalKingMove(dx, dy);
            default:
                std::cout << "Move check not implemented for piece: " << basePiece << std::endl;
                return false;
        }
    }

    std::vector<std::pair<int,int>> GetLegalMoves(const std::string& pieceKey) {
        std::vector<std::pair<int,int>> moves;
        for (int x = 0; x < 8; ++x) {
            for (int y = 0; y < 8; ++y) {
                if (IsLegalMove(pieceKey, x, y)) {
                    moves.emplace_back(x,y);
                }
            }
        }
        return moves;
    }

    bool IsLegalPawnMove(char color, int oldX, int oldY, int newX, int newY) {
        int direction = (color == 'w') ? -1 : 1;
        int startRow = (color == 'w') ? 6 : 1;

        int dx = newX - oldX;
        int dy = newY - oldY;

        std::string destPiece = GetPieceAtPosition(newX, newY);

        // Move forward one
        if (dx == 0 && dy == direction && destPiece.empty()) {
            return true;
        }

        // Move forward two from start position
        if (dx == 0 && dy == 2 * direction && oldY == startRow &&
            GetPieceAtPosition(newX, oldY + direction).empty() &&
            destPiece.empty()) {
            return true;
        }

        // Capture diagonally
        if (abs(dx) == 1 && dy == direction && !destPiece.empty() && destPiece[0] != color) {
            return true;
        }

        return false;
    }

    bool IsLegalKnightMove(int dx, int dy) {
        dx = abs(dx);
        dy = abs(dy);
        return (dx == 2 && dy == 1) || (dx == 1 && dy == 2);
    }

    bool IsLegalBishopMove(int oldX, int oldY, int newX, int newY) {
        int dx = newX - oldX;
        int dy = newY - oldY;

        if (abs(dx) != abs(dy)) return false;

        int stepX = (dx > 0) ? 1 : -1;
        int stepY = (dy > 0) ? 1 : -1;

        int x = oldX + stepX;
        int y = oldY + stepY;

        while (x != newX && y != newY) {
            if (!GetPieceAtPosition(x, y).empty()) return false;
            x += stepX;
            y += stepY;
        }
        return true;
    }

    bool IsLegalRookMove(int oldX, int oldY, int newX, int newY) {
        if (oldX != newX && oldY != newY) return false;

        if (oldX == newX) {
            int step = (newY > oldY) ? 1 : -1;
            for (int y = oldY + step; y != newY; y += step) {
                if (!GetPieceAtPosition(oldX, y).empty()) return false;
            }
        } else {
            int step = (newX > oldX) ? 1 : -1;
            for (int x = oldX + step; x != newX; x += step) {
                if (!GetPieceAtPosition(x, oldY).empty()) return false;
            }
        }
        return true;
    }

    bool IsLegalQueenMove(int oldX, int oldY, int newX, int newY) {
        // Queen moves like rook or bishop
        return IsLegalRookMove(oldX, oldY, newX, newY) || IsLegalBishopMove(oldX, oldY, newX, newY);
    }

    bool IsLegalKingMove(int dx, int dy) {
        dx = abs(dx);
        dy = abs(dy);
        return dx <= 1 && dy <= 1;
    }

    void LoadPieceImages() {
        std::string basePath = "../assets/pieces/";
        std::string names[] = {
            "wK", "wQ", "wR", "wB", "wN", "wP",
            "bK", "bQ", "bR", "bB", "bN", "bP"
        };

        for (const std::string& name : names) {
            std::string path = basePath + name + ".png";
            SDL_Surface* img = IMG_Load(path.c_str());
            if (!img) {
                std::cerr << "Failed to load " << path << ": " << IMG_GetError() << "\n";
            } else {
                pieceSurfaces[name] = img;
            }
        }
    }

    void SetupInitialPosition() {
        piecePositions.clear();

        piecePositions["wR"] = {0, 7}; piecePositions["wN"] = {1, 7}; piecePositions["wB"] = {2, 7};
        piecePositions["wQ"] = {3, 7}; piecePositions["wK"] = {4, 7}; piecePositions["wB2"] = {5, 7};
        piecePositions["wN2"] = {6, 7}; piecePositions["wR2"] = {7, 7};
        for (int i = 0; i < 8; ++i) piecePositions["wP" + std::to_string(i)] = {i, 6};

        piecePositions["bR"] = {0, 0}; piecePositions["bN"] = {1, 0}; piecePositions["bB"] = {2, 0};
        piecePositions["bQ"] = {3, 0}; piecePositions["bK"] = {4, 0}; piecePositions["bB2"] = {5, 0};
        piecePositions["bN2"] = {6, 0}; piecePositions["bR2"] = {7, 0};
        for (int i = 0; i < 8; ++i) piecePositions["bP" + std::to_string(i)] = {i, 1};
    }

    bool IsInPossibleMoves(int x, int y) const {
        for (const auto& pos : possibleMoves) {
            if (pos.first == x && pos.second == y) {
                return true;
            }
        }
        return false;
    }
};
