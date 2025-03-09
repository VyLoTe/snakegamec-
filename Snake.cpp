#include <iostream>
#include <conio.h>
#include <ctime>
#include <vector>
#include <windows.h>
#include <chrono>
#include <thread>
using namespace std;
 
//defind parameter    
const int WIDTH = 80;
const int HEIGHT = 30;
const int SNAKE_SPEED_MS = 70;

//create snake
typedef pair<int, int> Point;
vector<Point> snake;
#define x0 first
#define y0 second

//initialize needy variable 
bool gameOver, isExit = false;
int score = 0, speed, foodX, foodY, x, y;

void initSnake();
void generateFood();
void initGame();

void gotoXY();
void draw();

void stateSnake();
void update();

void Input();
void hideCursor();
bool isEat();
void playGame();

enum Direction {
    UP, 
    DOWN, 
    LEFT, 
    RIGHT,
    STOP = 0
};
Direction dir;

void generateFood() {
    foodX = rand() % (WIDTH-2)+1;
    foodY = rand() % (HEIGHT-2)+1;
}

bool isEat() {
    return (snake[0].x0 == foodX && snake[0].y0 == foodY);
}

void initSnake() {
    snake.clear();          
    snake.push_back({WIDTH/2, HEIGHT/2});
}

void initGame() {
    gameOver = false;
    score = 0;
    dir = STOP;
    initSnake();
    generateFood();
}

void gotoXY(int x, int y) {
    COORD coord;        
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void hideCursor() {
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;
    cursorInfo.bVisible = false;  
    cursorInfo.dwSize = 1;        
    SetConsoleCursorInfo(consoleHandle, &cursorInfo);
}

void draw() {
    gotoXY(0, 0); 

    cout << "Score: " << score << "\n";
    
    for (int i = 0; i < HEIGHT; ++i) {
        for (int j = 0; j < WIDTH; ++j) {
            bool isSnakePart = false;
            for (auto segment : snake) {
                if (segment.x0 == j && segment.y0 == i) {
                    cout << static_cast<char>(1);
                    isSnakePart = true;
                    break;
                }
            }
            if (!isSnakePart) {
                if (i == 0 || i == HEIGHT - 1) {
                    cout << "=";  
                } else if (j == 0 || j == WIDTH - 1) {
                    cout << static_cast<char>(199);  
                } else if (i == foodY && j == foodX) {
                    cout << "@";  
                } else 
                    cout << " ";  
            }
        }
        cout << "\n";  
    }
}

void stateSnake() {
    for (int i = snake.size()-1; i > 0; --i) {
        snake[i] = snake[i-1];
    }
    if (dir == LEFT) snake[0].x0--;
    if (dir == RIGHT) snake[0].x0++;
    if (dir == UP) snake[0].y0--;
    if (dir == DOWN) snake[0].y0++;
}

void update() {
    stateSnake();
    
    if (foodX == snake[0].x0 && foodY == snake[0].y0) {
        score += 10;
        generateFood();
        snake.push_back(snake.back());
    }
    
    if (snake[0].x0 <= 0 || snake[0].x0 >= WIDTH-1 || snake[0].y0 <= 0 || snake[0].y0 >= HEIGHT-1) {
        gameOver = true;
    }
    
    for (int i = 1; i < snake.size(); ++i) {
        if (snake[0] == snake[i+1]) {
            gameOver = true;
            break;
        }
    }
}

void Input() {
    static char prevKey = '\0';  
    char key;
    bool isDulplicate = true;

    if (_kbhit()) {
        key = _getch();
        
        switch (key) {
            case 'a': 
                if (prevKey == 'd') {
                    isDulplicate = false;
                    break; 
                }
                isDulplicate = true;
                dir = LEFT; 
                break;
            case 'd': 
                if (prevKey == 'a') {
                    isDulplicate = false;
                    break; 
                }
                isDulplicate = true;
                dir = RIGHT; 
                break;
            case 'w': 
                if (prevKey == 's') {
                    isDulplicate = false;
                    break; 
                }
                isDulplicate = true;
                dir = UP; 
                break;
            case 's': 
                if (prevKey == 'w') {
                    isDulplicate = false;
                    break; 
                }
                isDulplicate = true;
                dir = DOWN; 
                break;
            case 'x': 
                gameOver = true; 
                break;
        }
        if (isDulplicate) prevKey = key; 
    }
}

void playGame() {
    initGame();
    hideCursor();
    
    auto lastUpdate = chrono::high_resolution_clock::now();
    auto lastFoodCheck = chrono::high_resolution_clock::now();

    while (!gameOver) {
        hideCursor();
        Input();

        auto now = chrono::high_resolution_clock::now();

        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastUpdate).count();
        if (duration >= SNAKE_SPEED_MS) {
            lastUpdate = now;
            update();

            if (isEat()) {
                generateFood();
                lastFoodCheck = now;
            }
        }

        auto foodDuration = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastFoodCheck).count();
        if (foodDuration >= 5000) {
            if (!isEat()) { 
                generateFood();
                lastFoodCheck = now;
            }
        }
        draw();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    cout << "Game Over! Final Score: " << score << endl;

    cout << "Do you want replay ? (Y/N): ";
    char choice; cin >> choice;
    if (choice == 'y') {                
        playGame();
    }
    if (choice == 'n') isExit = true;
}


int main() {
    cout << "1.Start" << "\n" << "2.Exit" << "\n";
    while (true)
    {
        int option; cin >> option;

        switch (option) 
        {
        case 1:
            playGame();
            if (isExit) {
                cout << "You have exited!";
                return 0;
            }
            isExit = false;
            break;
        case 2:
            cout << "You have exited!";
            return 0;
        default:
            cout << "Khong co option " << option << "\n";
            break;
        }
    }

    return 0;
}