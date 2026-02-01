#include "raylib/include/raylib.h"
#include <stdio.h>
#include <math.h>

// --- GLOBALS ---
Vector2 winStart = {0, 0};
Vector2 winEnd = {0, 0};

// --- HELPER: CENTERED TEXT ---
// Text ko screen ke beech mein likhne ke liye auto-calculation
void DrawTextCentered(const char *text, int y, int fontSize, Color color) {
    int textWidth = MeasureText(text, fontSize);
    DrawText(text, (600 - textWidth) / 2, y, fontSize, color);
}

// --- HELPER: BOLD TEXT ---
// Raylib default font patla hai, isliye hum usse 2-3 baar draw karke mota dikhayenge
void DrawTextBold(const char *text, int x, int y, int fontSize, Color color) {
    DrawText(text, x+2, y+2, fontSize, Fade(BLACK, 0.3f)); // Shadow
    DrawText(text, x+1, y, fontSize, color);
    DrawText(text, x-1, y, fontSize, color);
    DrawText(text, x, y+1, fontSize, color);
    DrawText(text, x, y-1, fontSize, color);
    DrawText(text, x, y, fontSize, color); // Main Top Layer
}

// --- HELPER: CENTERED BOLD TEXT ---
void DrawTextCenteredBold(const char *text, int y, int fontSize, Color color) {
    int textWidth = MeasureText(text, fontSize);
    int x = (600 - textWidth) / 2;
    DrawTextBold(text, x, y, fontSize, color);
}

// --- 1. WIN CHECKER ---
char checkWin(char board[3][3]) {
    // Rows
    for(int i=0; i<3; i++) {
        if(board[i][0] == board[i][1] && board[i][0] == board[i][2] && board[i][0] != ' ') {
            float y = i * 200 + 100; winStart = (Vector2){ 20, y }; winEnd = (Vector2){ 580, y }; return board[i][0];
        }
    }
    // Cols
    for(int i=0; i<3; i++) {
        if(board[0][i] == board[1][i] && board[0][i] == board[2][i] && board[0][i] != ' ') {
            float x = i * 200 + 100; winStart = (Vector2){ x, 20 }; winEnd = (Vector2){ x, 580 }; return board[0][i];
        }
    }
    // Diagonals
    if(board[0][0] == board[1][1] && board[0][0] == board[2][2] && board[0][0] != ' ') {
        winStart = (Vector2){ 20, 20 }; winEnd = (Vector2){ 580, 580 }; return board[0][0];
    }
    if(board[0][2] == board[1][1] && board[0][2] == board[2][0] && board[0][2] != ' ') {
        winStart = (Vector2){ 580, 20 }; winEnd = (Vector2){ 20, 580 }; return board[0][2];
    }
    return ' ';
}

// --- 2. SMART DRAW (PvP ONLY) ---
// YE RAHA FEATURE! MISS NAHI HUA HAI.
int isDrawPossible(char board[3][3]) {
    int lines[8][3][2] = {
        {{0,0},{0,1},{0,2}}, {{1,0},{1,1},{1,2}}, {{2,0},{2,1},{2,2}}, 
        {{0,0},{1,0},{2,0}}, {{0,1},{1,1},{2,1}}, {{0,2},{1,2},{2,2}}, 
        {{0,0},{1,1},{2,2}}, {{0,2},{1,1},{2,0}}
    };
    for(int i=0; i<8; i++) {
        int xFound = 0, oFound = 0;
        for(int j=0; j<3; j++) {
            char cell = board[lines[i][j][0]][lines[i][j][1]];
            if(cell == 'X') xFound = 1;
            if(cell == 'O') oFound = 1;
        }
        if(xFound == 0 || oFound == 0) return 0; // Win possible hai
    }
    return 1; // Smart Draw
}

// --- 3. AI BRAIN (MINIMAX) ---
int isBoardFull(char board[3][3]) {
    for(int i=0; i<3; i++) for(int j=0; j<3; j++) if(board[i][j] == ' ') return 0;
    return 1;
}
int minimax(char board[3][3], int depth, int isAI) {
    char result = checkWin(board);
    if (result == 'O') return 10 - depth; if (result == 'X') return -10 + depth; if (isBoardFull(board)) return 0;
    if (isAI) {
        int bestScore = -1000;
        for(int i=0; i<3; i++) for(int j=0; j<3; j++) if(board[i][j] == ' ') {
            board[i][j] = 'O'; int score = minimax(board, depth + 1, 0); board[i][j] = ' ';
            if(score > bestScore) bestScore = score;
        }
        return bestScore;
    } else {
        int bestScore = 1000;
        for(int i=0; i<3; i++) for(int j=0; j<3; j++) if(board[i][j] == ' ') {
            board[i][j] = 'X'; int score = minimax(board, depth + 1, 1); board[i][j] = ' ';
            if(score < bestScore) bestScore = score;
        }
        return bestScore;
    }
}
void performAIMove(char board[3][3]) {
    int bestScore = -1000; int bestRow = -1, bestCol = -1;
    for(int i=0; i<3; i++) for(int j=0; j<3; j++) if(board[i][j] == ' ') {
        board[i][j] = 'O'; int score = minimax(board, 0, 0); board[i][j] = ' ';
        if(score > bestScore) { bestScore = score; bestRow = i; bestCol = j; }
    }
    if(bestRow != -1) board[bestRow][bestCol] = 'O';
}

// --- MAIN FUNCTION ---
int main() {
    InitWindow(600, 600, "Tic-Tac-Toe: Final Systematic Edition");
    SetTargetFPS(60);

    int gameMode = 0; // 0=Menu, 1=PvP, 2=AI
    char board[3][3] = { {' ',' ',' '}, {' ',' ',' '}, {' ',' ',' '} };
    char currentPlayer = 'X'; char winner = ' '; int moves = 0; int gameOver = 0;
    char statusMsg[50] = "";
    int framesCounter = 0; // Safety Timer

    // Colors
    Color myRed = (Color){ 230, 41, 55, 255 };
    Color myBlue = (Color){ 0, 121, 241, 255 };
    Color myGold = (Color){ 255, 203, 0, 240 };
    Color bgWall = (Color){ 245, 245, 245, 255 };
    Color btnNormal = LIGHTGRAY;
    Color btnHover = (Color){ 200, 200, 225, 255 };

    while (!WindowShouldClose()) {
        Vector2 mouse = GetMousePosition();

        // --- MENU SCREEN ---
        if (gameMode == 0) {
            BeginDrawing();
            ClearBackground(bgWall);
            
            // Bold & Centered Title
            DrawTextCenteredBold("TIC-TAC-TOE", 100, 60, BLACK);
            DrawTextCentered("Choose Your Mode", 170, 25, DARKGRAY);

            Rectangle btnPvP = {150, 250, 300, 60};
            Rectangle btnAI = {150, 350, 300, 60};
            bool hPvP = CheckCollisionPointRec(mouse, btnPvP);
            bool hAI = CheckCollisionPointRec(mouse, btnAI);

            DrawRectangleRec(btnPvP, hPvP ? btnHover : btnNormal);
            DrawRectangleLinesEx(btnPvP, 3, hPvP ? DARKGRAY : GRAY);
            // Centered Text inside Button
            int pvpWidth = MeasureText("Player vs Player", 30);
            DrawTextBold("Player vs Player", 150 + (300-pvpWidth)/2, 265, 30, BLACK);
            
            DrawRectangleRec(btnAI, hAI ? btnHover : btnNormal);
            DrawRectangleLinesEx(btnAI, 3, hAI ? DARKGRAY : GRAY);
            int aiWidth = MeasureText("Player vs AI", 30);
            DrawTextBold("Player vs AI", 150 + (300-aiWidth)/2, 365, 30, BLACK);

            if (hPvP && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) gameMode = 1;
            if (hAI && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) gameMode = 2;
            EndDrawing();
        }
        
        // --- GAMEPLAY SCREEN ---
        else {
            if (!gameOver) {
                int canClick = (gameMode == 1) || (gameMode == 2 && currentPlayer == 'X');
                if (canClick && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    int col = mouse.x / 200; int row = mouse.y / 200;
                    if (row>=0 && row<3 && col>=0 && col<3 && board[row][col] == ' ') {
                        board[row][col] = currentPlayer; moves++;
                        
                        // Check Win
                        winner = checkWin(board);
                        if(winner != ' ') { gameOver = 1; sprintf(statusMsg, "%c WINS!", winner); }
                        else if(isBoardFull(board)) { gameOver = 1; sprintf(statusMsg, "GAME DRAW!"); }
                        
                        // Check Smart Draw (PvP Only)
                        else if (gameMode == 1 && moves > 4 && isDrawPossible(board)) {
                            gameOver = 1;
                            sprintf(statusMsg, "SMART DRAW!");
                        }

                        if (!gameOver) {
                            if (gameMode == 1) currentPlayer = (currentPlayer == 'X') ? 'O' : 'X';
                            else currentPlayer = 'O';
                        }
                    }
                }
                
                // AI Turn
                if (gameMode == 2 && currentPlayer == 'O' && !gameOver) {
                    performAIMove(board); moves++;
                    winner = checkWin(board);
                    if(winner != ' ') { gameOver = 1; sprintf(statusMsg, "AI WINS!"); }
                    else if(isBoardFull(board)) { gameOver = 1; sprintf(statusMsg, "GAME DRAW!"); }
                    currentPlayer = 'X';
                }
            }

            // --- RESTART LOGIC WITH DELAY ---
            if (gameOver) {
                framesCounter++;
                if (framesCounter > 60) { // 1 Second wait
                    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                        for(int i=0; i<3; i++) for(int j=0; j<3; j++) board[i][j] = ' ';
                        currentPlayer = 'X'; winner = ' '; moves = 0; gameOver = 0; framesCounter = 0;
                    }
                    if (IsKeyPressed(KEY_M)) {
                        gameMode = 0; for(int i=0; i<3; i++) for(int j=0; j<3; j++) board[i][j] = ' ';
                        currentPlayer = 'X'; winner = ' '; moves = 0; gameOver = 0; framesCounter = 0;
                    }
                }
            }

            // --- DRAWING ---
            BeginDrawing();
            ClearBackground(bgWall);

            // Thick Grid
            DrawLineEx((Vector2){200, 20}, (Vector2){200, 580}, 6.0f, LIGHTGRAY);
            DrawLineEx((Vector2){400, 20}, (Vector2){400, 580}, 6.0f, LIGHTGRAY);
            DrawLineEx((Vector2){20, 200}, (Vector2){580, 200}, 6.0f, LIGHTGRAY);
            DrawLineEx((Vector2){20, 400}, (Vector2){580, 400}, 6.0f, LIGHTGRAY);

            for(int row=0; row<3; row++) {
                for(int col=0; col<3; col++) {
                    int cx = col*200+100; int cy = row*200+100;
                    if(board[row][col]=='X') {
                        DrawLineEx((Vector2){cx-60, cy-60}, (Vector2){cx+60, cy+60}, 16.0f, myRed);
                        DrawLineEx((Vector2){cx+60, cy-60}, (Vector2){cx-60, cy+60}, 16.0f, myRed);
                    } else if(board[row][col]=='O') {
                        DrawRing((Vector2){cx, cy}, 45.0f, 61.0f, 0, 360, 0, myBlue);
                    }
                }
            }

            if (gameOver && winner != ' ') DrawLineEx(winStart, winEnd, 22.0f, myGold);

            // --- NOTIFICATIONS (BOLD & SYSTEMATIC) ---
            if (gameOver) {
                DrawRectangle(0, 0, 600, 600, Fade(WHITE, 0.92f));
                
                // Result Text
                Color msgColor = DARKGRAY;
                if(winner == 'X') msgColor = myRed;
                else if(winner == 'O') msgColor = myBlue;
                
                DrawTextCenteredBold(statusMsg, 230, 60, msgColor);

                // Instructions (Aligned Perfectly)
                if (framesCounter > 60) {
                    DrawTextCenteredBold("Click to Play Again", 330, 30, DARKGRAY);
                    DrawTextCentered("Press 'M' for Menu", 380, 25, GRAY);
                } else {
                    DrawTextCentered("Wait...", 330, 20, LIGHTGRAY);
                }
            }

            EndDrawing();
        }
    }
    CloseWindow();
    return 0;
}