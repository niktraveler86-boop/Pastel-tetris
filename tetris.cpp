#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <random>
#include <string>
#include <iomanip>


using namespace std;

// Editable settings for programmer mode
struct Settings {
    vector<string> blockColors = {
        "\033[0m",          // Reset
        "\033[38;5;159m",   // I - Cyan
        "\033[38;5;218m",   // J - Pink
        "\033[38;5;229m",   // L - Light Yellow
        "\033[38;5;117m",   // O - Light Blue
        "\033[38;5;224m",   // S - Soft Orange
        "\033[38;5;120m",   // Z - Mint
        "\033[38;5;183m",   // T - Lavender
    };
    // control keys (single character strings; drop is space by default)
    string keyLeft  = "a";
    string keyRight = "d";
    string keyDown  = "s";
    string keyRotate= "w";
    string keyDrop  = " ";
    string keyQuit  = "q";
} settings;

const int WIDTH = 10;
const int HEIGHT = 20;

const vector<vector<vector<int>>> tetrominoShapes = {
    {{1,1,1,1}},           // I
    {{1,1,1},{0,0,1}},     // J
    {{1,1,1},{1,0,0}},     // L
    {{1,1},{1,1}},         // O
    {{0,1,1},{1,1,0}},     // S
    {{1,1,0},{0,1,1}},     // Z
    {{1,1,1},{0,1,0}}      // T
};

struct Tetromino {
    int type, x, y;
    vector<vector<int>> shape;
};

vector<vector<int>> board(HEIGHT, vector<int>(WIDTH, 0));
Tetromino current, nextPiece;
bool gameOver = false;
int score = 0, linesCleared = 0, level = 1, highScore = 0;
string playerName = "Player";

// --- New: Creator / Credits screen shown BEFORE loading ---
void kelompok() {
    cout << "\033[2J\033[1;1H"; // Clear terminal
    cout << "=========================================\n";
    cout << "               Kelompok 6                \n";
    cout << "=========================================\n\n";
    cout << "Nama : \n";
    cout << "1. Evander Enrique\n";
    cout << "2. Gracia Roveli Berlian Purba\n";
    cout << "3. Laurencia Caroline Alexander\n";
    cout << "4. Naftali Keyko\n";
    cout << "5. Nikita Aurelia Tampubolon\n\n";
    cout << "Press Enter to continue...";
    string tmp;
    getline(cin, tmp);
}
// -------------------------------------------------------

// Loading screen — large ASCII "TETRIS" (block letters) + progress bar
void loadingScreen(int seconds = 3, int bar_width = 40) {
    cout << "\033[2J\033[1;1H";

    // ASCII block letters for "TETRIS" (6 rows)
    vector<string> t = {
        "#####",
        "  #  ",
        "  #  ",
        "  #  ",
        "  #  ",
        "  #  "
    };
    vector<string> e = {
        "#####",
        "#    ",
        "###  ",
        "#    ",
        "#    ",
        "#####"
    };
    vector<string> r = {
        "#### ",
        "#   #",
        "#   #",
        "#### ",
        "# #  ",
        "#  ##"
    };
    vector<string> i = {
        " ### ",
        "  #  ",
        "  #  ",
        "  #  ",
        "  #  ",
        " ### "
    };
    vector<string> s = {
        " ####",
        "#    ",
        " ### ",
        "    #",
        "#   #",
        " ### "
    };

    // Combine to form lines for "TETRIS" with spacing between letters
    cout << "\n\n";
    for (size_t row = 0; row < 6; ++row) {
        // indent for nicer centering
        cout << "           ";
        // T
        cout << t[row] << "  ";
        // E
        cout << e[row] << "  ";
        // T
        cout << t[row] << "  ";
        // R
        cout << r[row] << "  ";
        // I
        cout << i[row] << "  ";
        // S
        cout << s[row] << "\n";
    }
    cout << "\n           -- Loading TETRIS by Group 6 --\n\n";

    const int ticks = bar_width;
    using namespace std::chrono;
    auto step_duration = milliseconds((seconds * 1000) / ticks);

    cout << " Loading: [";
    for (int i = 0; i < bar_width; ++i) cout << ' ';
    cout << "] 0%" << flush;

    for (int tck = 1; tck <= ticks; ++tck) {
        this_thread::sleep_for(step_duration);
        int filled = tck;
        int percent = (tck * 100) / ticks;

        cout << '\r' << " Loading: [";
        for (int i = 0; i < filled; ++i) cout << '=';
        for (int i = filled; i < bar_width; ++i) cout << ' ';
        cout << "] " << setw(3) << percent << "%" << flush;
    }

    cout << "\n\n";
}

// Programmer/editor screen: edit name, colors, controls
void programmerEditor() {
    while (true) {
        cout << "\033[2J\033[1;1H";
        cout << "=== PROGRAMMER EDITOR ===\n";
        cout << "1) Edit player name (current: \"" << playerName << "\")\n";
        cout << "2) Edit pastel block colors (ANSI strings)\n";
        cout << "3) Edit control keys (single character strings)\n";
        cout << "4) Show current settings\n";
        cout << "5) Done (return to game)\n";
        cout << "Choose an option (1-5): ";
        string choice;
        getline(cin, choice);
        if (choice == "1") {
            cout << "Enter new player name (max 12 chars). Empty = keep current: ";
            string in; getline(cin, in);
            if (!in.empty()) {
                if (in.size() > 12) in = in.substr(0,12);
                playerName = in;
            }
        } else if (choice == "2") {
            cout << "\nEditing block colors. These are raw strings inserted before the glyph.\n";
            cout << "If you don't want to change an entry, just press Enter.\n";
            for (size_t i = 0; i < settings.blockColors.size(); ++i) {
                cout << i << ") current: ";
                // show a sample colored dot if it's an ANSI code
                cout << settings.blockColors[i] << "●" << "\033[0m";
                cout << "  raw: \"" << settings.blockColors[i] << "\"\n";
                cout << "  new value (or Enter to keep): ";
                string nv; getline(cin, nv);
                if (!nv.empty()) settings.blockColors[i] = nv;
            }
            cout << "Colors updated. Press Enter to continue...";
            string tmp; getline(cin, tmp);
        } else if (choice == "3") {
            cout << "\nCurrent controls:\n";
            cout << " Left:  \"" << settings.keyLeft << "\"\n";
            cout << " Right: \"" << settings.keyRight << "\"\n";
            cout << " Down:  \"" << settings.keyDown << "\"\n";
            cout << " Rotate:\""<< settings.keyRotate << "\"\n";
            cout << " Drop:  \"" << settings.keyDrop << "\" (space allowed)\n";
            cout << " Quit:  \"" << settings.keyQuit << "\"\n";
            cout << "Enter new value for Left (single char). Enter to keep: ";
            string kv; getline(cin, kv);
            if (!kv.empty()) settings.keyLeft = kv.substr(0,1);
            cout << "Right: "; getline(cin, kv); if (!kv.empty()) settings.keyRight = kv.substr(0,1);
            cout << "Down: "; getline(cin, kv); if (!kv.empty()) settings.keyDown = kv.substr(0,1);
            cout << "Rotate: "; getline(cin, kv); if (!kv.empty()) settings.keyRotate = kv.substr(0,1);
            cout << "Drop (type a space for space): "; getline(cin, kv); if (!kv.empty()) settings.keyDrop = kv.substr(0,1);
            cout << "Quit: "; getline(cin, kv); if (!kv.empty()) settings.keyQuit = kv.substr(0,1);
            cout << "Controls updated. Press Enter to continue...";
            string tmp; getline(cin, tmp);
        } else if (choice == "4") {
            cout << "\033[2J\033[1;1H";
            cout << "=== CURRENT SETTINGS ===\n";
            cout << "Player name: " << playerName << "\n";
            cout << "Colors:\n";
            for (size_t i = 0; i < settings.blockColors.size(); ++i) {
                cout << " " << i << ") " << settings.blockColors[i] << "●" << "\033[0m"
                     << "  raw: \"" << settings.blockColors[i] << "\"\n";
            }
            cout << "Controls:\n";
            cout << " Left:  \"" << settings.keyLeft << "\"\n";
            cout << " Right: \"" << settings.keyRight << "\"\n";
            cout << " Down:  \"" << settings.keyDown << "\"\n";
            cout << " Rotate:\""<< settings.keyRotate << "\"\n";
            cout << " Drop:  \"" << settings.keyDrop << "\"\n";
            cout << " Quit:  \"" << settings.keyQuit << "\"\n";
            cout << "\nPress Enter to return...";
            string tmp; getline(cin, tmp);
        } else if (choice == "5") {
            break;
        } else {
            // ignore
        }
    }
}

// Print board (uses settings.blockColors and playerName)
void printCuteBoard() {
    cout << "\033[2J\033[1;1H"; // Clear terminal
    cout << " Pastel Tetris  - Player: " << playerName << "\n";
    cout << "Score: " << score << "  Lines: " << linesCleared << "  Level: " << level << "  High: " << highScore << "\n";
    cout << "Controls: left=" << settings.keyLeft
         << " right=" << settings.keyRight
         << " down=" << settings.keyDown
         << " rotate="<< settings.keyRotate
         << " drop=" << (settings.keyDrop == " " ? "<space>" : settings.keyDrop)
         << " quit=" << settings.keyQuit << "\n";
    cout << "Next Piece:\n";
    // Draw next tetromino
    for (size_t y = 0; y < nextPiece.shape.size(); ++y) {
        for (size_t x = 0; x < nextPiece.shape[0].size(); ++x) {
            if (nextPiece.shape[y][x])
                cout << settings.blockColors[nextPiece.type+1] << "●" << settings.blockColors[0];
            else
                cout << " ";
        }
        cout << endl;
    }
    // Draw main board
    for (int y = 0; y < HEIGHT; ++y) {
        cout << "│";
        for (int x = 0; x < WIDTH; ++x) {
            bool printed = false;
            for (size_t ty = 0; ty < current.shape.size(); ++ty)
            for (size_t tx = 0; tx < current.shape[0].size(); ++tx) {
                if (current.shape[ty][tx] &&
                    current.y + ty == y && current.x + tx == x) {
                    cout << settings.blockColors[current.type + 1] << "●" << settings.blockColors[0];
                    printed = true;
                }
            }
            if (!printed) {
                if (board[y][x]) cout << settings.blockColors[board[y][x]] << "■" << settings.blockColors[0];
                else cout << " ";
            }
        }
        cout << "│\n";
    }
    cout << "└";
    for (int i = 0; i < WIDTH; ++i) cout << "─";
    cout << "┘\n";
    if (gameOver) cout << "\n🌸 GAME OVER! 🌸\nYour Score: " << score << "\nPress Enter to restart or q then Enter to quit...\n";
}

bool isCollision(int nx, int ny, const vector<vector<int>>& nshape) {
    for (size_t ty = 0; ty < nshape.size(); ++ty)
    for (size_t tx = 0; tx < nshape[0].size(); ++tx)
        if (nshape[ty][tx]) {
            int x = nx + tx, y = ny + ty;
            if (x < 0 || x >= WIDTH || y >= HEIGHT) return true;
            if (y >= 0 && board[y][x]) return true;
        }
    return false;
}

vector<vector<int>> rotate(const vector<vector<int>>& shape) {
    size_t h = shape.size(), w = shape[0].size();
    vector<vector<int>> res(w, vector<int>(h));
    for (size_t y = 0; y < h; ++y)
    for (size_t x = 0; x < w; ++x)
        res[x][h - y - 1] = shape[y][x];
    return res;
}

void fixTetromino() {
    for (size_t ty = 0; ty < current.shape.size(); ++ty)
    for (size_t tx = 0; tx < current.shape[0].size(); ++tx)
        if (current.shape[ty][tx]) {
            int x = current.x + tx, y = current.y + ty;
            if (y >= 0 && y < HEIGHT && x >=0 && x < WIDTH)
                board[y][x] = current.type + 1;
        }
}

void clearLines() {
    for (int y = HEIGHT - 1; y >= 0; --y) {
        bool full = true;
        for (int x = 0; x < WIDTH; x++) if (!board[y][x]) full = false;
        if (full) {
            for (int j = y; j > 0; --j)
                board[j] = board[j - 1];
            board[0] = vector<int>(WIDTH, 0);
            linesCleared++;
            score += 100 * level;
            if (linesCleared / 5 + 1 > level) level = linesCleared / 5 + 1;
            ++y;
        }
    }
}

void spawnTetromino() {
    current = nextPiece;
    current.x = WIDTH / 2 - current.shape[0].size() / 2;
    current.y = -1;
    nextPiece.type = rand() % 7;
    nextPiece.shape = tetrominoShapes[nextPiece.type];
    if (isCollision(current.x, current.y + 1, current.shape)) {
        gameOver = true;
        if (score > highScore) highScore = score;
    }
}

void moveTetromino(int dx, int dy) {
    if (!isCollision(current.x + dx, current.y + dy, current.shape)) {
        current.x += dx;
        current.y += dy;
    }
}

void hardDrop() {
    while (!isCollision(current.x, current.y + 1, current.shape))
        current.y++;
}

void rotateTetromino() {
    auto newShape = rotate(current.shape);
    if (!isCollision(current.x, current.y, newShape))
        current.shape = newShape;
}

void cuteGameLoop() {
    score = 0;
    linesCleared = 0;
    level = 1;
    board = vector<vector<int>>(HEIGHT, vector<int>(WIDTH, 0));
    nextPiece.type = rand() % 7;
    nextPiece.shape = tetrominoShapes[nextPiece.type];
    spawnTetromino();

    while (true) {
        printCuteBoard();
        std::this_thread::sleep_for(std::chrono::milliseconds(70 + 400 / (level + 1)));
        string input;
        if (!gameOver) {
            cout << "Enter move: ";
            getline(cin, input);
            for (char ch : input) {
                string s(1, ch);
                if (s == settings.keyLeft) moveTetromino(-1, 0);
                if (s == settings.keyRight) moveTetromino(1, 0);
                if (s == settings.keyDown) {
                    if (!isCollision(current.x, current.y + 1, current.shape))
                        current.y++;
                }
                if (s == settings.keyRotate) rotateTetromino();
                if (s == settings.keyDrop) hardDrop();
                if (s == settings.keyQuit) return;
            }
            // also allow single-key commands when input was empty (for direct key press in some consoles)
            if (input.empty()) {
                // do nothing
            }
            if (!isCollision(current.x, current.y + 1, current.shape))
                current.y++;
            else {
                fixTetromino();
                clearLines();
                spawnTetromino();
            }
        } else {
            getline(cin, input);
            if (!input.empty() && input[0] == settings.keyQuit[0]) return;
            gameOver = false;
            cuteGameLoop();
            return;
        }
    }
}

void getPlayerName() {
    cout << "\033[2J\033[1;1H"; // Clear terminal
    cout << "            -- ENTER YOUR NAME --\n\n";
    cout << "Please enter your name (max 12 chars). Press Enter to confirm:\n> ";
    string input;
    getline(cin, input);
    if (input.size() == 0) {
        playerName = "Player";
    } else {
        size_t start = input.find_first_not_of(" \t\r\n");
        size_t end = input.find_last_not_of(" \t\r\n");
        if (start == string::npos) playerName = "Player";
        else {
            string trimmed = input.substr(start, end - start + 1);
            if (trimmed.size() > 12) trimmed = trimmed.substr(0, 12);
            playerName = trimmed;
        }
    }
    cout << "\nWelcome, " << playerName << "!\n";
    this_thread::sleep_for(chrono::milliseconds(400));
}

#ifdef _WIN32
#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")
#endif

int main() {
    srand(static_cast<unsigned int>(time(0)));

    cout << "\033[?25l"; // Hide cursor

    kelompok();       // NEW: show Kelompok 6 and the list, waits for Enter
    loadingScreen(3);     // show 3-second loading screen
    getPlayerName();      // ask for player's name before starting the game

    // Prompt to enter programmer editor
    cout << "\nOpen programmer editor to edit settings, " << playerName << "?? (yes/no) \n";
    string yn; getline(cin, yn);
    if (!yn.empty() && (yn[0] == 'y' || yn[0] == 'Y')) {
        programmerEditor();
    }

    cuteGameLoop();
    cout << "\033[?25h"; // Show cursor again
    return 0;
}
