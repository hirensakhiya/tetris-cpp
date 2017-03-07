#include <iostream>
#include <ncurses.h>
#include "Game.h"

using namespace std;

Game::Game()
{
    score = 0;
    cases = new Cell**[ROWS];
    for(int i = 0; i < ROWS; i++)
    {
        cases[i] = new Cell*[COLUMNS];
        for(int j = 0; j < COLUMNS; j++)
        {
            cases[i][j] = new Cell(i, j);
        }
    }

    Game::generate(); // Prepare the first tetromino
}


void Game::play()
{
    int ch;
    setlocale(LC_ALL, "");

    initscr(); // Initialize screen

    field = newwin(40, 24, 0, 0);
    info = newwin(20, 40, 3, 28);

    cbreak(); // Disable input buffering
    noecho(); // Disable automatic encoding
    nodelay(stdscr, TRUE); // Set input as non blocking
    keypad(stdscr, TRUE); // Capture special keystrokes

    bool playing = true;
    bool turn = true;

    while (playing)
    {
        turn = true;
        Game::next_tetromino(); // Charge next tetromino and generate the next one

        curs_set(0); // Remove cursor since it's a game

        Game::show(); // Display playfield

        while(turn)
        {
            Game::show();

            int ch = getch();
            switch (ch) {
                case 'H': /* user pressed key 'H' or 'h' */
                case 'h':
                    Game::help();
                    break;
                case KEY_LEFT:
                    current -> rotate("left"); // +90°
                    break;
                case KEY_RIGHT:
                    current -> rotate("right"); // -90°
                    break;
                case KEY_DOWN:
                    break;
            }
        }
    }

    endwin(); // Reset terminal
}

void Game::show()
{
    // Reset terminal screen
    wclear( field );
    wclear( info );

    // FIELD
    wprintw( field, "      SUPTETRIS\n\n" );
    for(int i = 0; i < 22; i++)
    {
        wprintw( field, "\u2588");
    }

    wprintw( field, "\n" );

    for(int i = 0; i < ROWS; i++)
    {
        wprintw( field, "\u2588");

        for(int j = 0; j < COLUMNS; j++)
        {
            if (cases[i][j] -> is_empty() )
            {
                wprintw( field,"  ");
            }
            else
            {
                wprintw( field,"\u2588\u2588");
            }
        }

        wprintw( field,"\u2588\n");
    }

    for(int i = 0; i < 22; i++)
    {
        wprintw( field, "\u2588");
    }

    // INFO
    std::string s = "Score: " + std::to_string(score) + "\n";
    wprintw( info, s.c_str() );

    s = "Level: " + std::to_string(level) + "\n\n";
    wprintw( info, s.c_str() );

    s = "Next:\n\n";
    wprintw( info, s.c_str() );

    wrefresh(field); // Refresh field
    wrefresh(info); // Refresh info
}

void Game::help() {
    bool help = true;

    int ch;
    while(help)
    {
        wclear( field );
        wclear( info );

        // Display help
        wprintw( field, "      HELP\n\n");
        wprintw( field, "tetromino L\n");
        wprintw( field, "  \u2588\u2588\n");
        wprintw( field, "  \u2588\u2588\n");
        wprintw( field, "  \u2588\u2588\u2588\u2588\n\n");
        wprintw( field, "tetromino J\n");
        wprintw( field, "    \u2588\u2588\n");
        wprintw( field, "    \u2588\u2588\n");
        wprintw( field, "  \u2588\u2588\u2588\u2588\n\n");
        wprintw( field, "tetromino O\n");
        wprintw( field, "  \u2588\u2588\u2588\u2588\n");
        wprintw( field, "  \u2588\u2588\u2588\u2588\n\n");
        wprintw( field, "tetromino S\n");
        wprintw( field, "  \u2588\u2588\n");
        wprintw( field, "  \u2588\u2588\u2588\u2588\n");
        wprintw( field, "    \u2588\u2588\n\n");
        wprintw( field, "tetromino Z\n");
        wprintw( field, "    \u2588\u2588\n");
        wprintw( field, "  \u2588\u2588\u2588\u2588\n");
        wprintw( field, "  \u2588\u2588\n\n");
        wprintw( field, "tetromino I\n");
        wprintw( field, "  \u2588\u2588\n");
        wprintw( field, "  \u2588\u2588\n");
        wprintw( field, "  \u2588\u2588\n");
        wprintw( field, "  \u2588\u2588\n\n");
        wprintw( field, "tetromino T\n");
        wprintw( field, "  \u2588\u2588\u2588\u2588\u2588\u2588\n");
        wprintw( field, "    \u2588\u2588\n");

        wprintw( info, "Tetris is a game where you create lines of block using tetrominos.\n");
        wprintw( info, "Use the left arrow and right arrow keys\nto rotate the falling tetromino.\n\n");
        wprintw( info, "The score is incremented by 10 for each completed lines\n");
        wprintw( info, "Each completed lines disapear.\n\n");
        wprintw( info, "If the placed tetrominos touch the top\nof the playfield it's the end.\n\n");
        wprintw( info, "Push any key to continue playing");

        wrefresh( field );
        wrefresh( info );

        ch = getch();
        if (ch != ERR)
        {
            help = false;
        }
    }
}

bool Game::line_filled(const int& row)
{
    for(int j = 0; j < COLUMNS; j++)
    {
        if (cases[row][j] -> is_empty())
        {
            return false;
        }
    }

    return true;
}

void Game::empty_line(const int& row)
{
    for(int j = 0; j < COLUMNS; j++)
    {
        cases[row][j] -> empty();
    }

    Game::drop(row);
    score += 10;
}

void Game::empty_lines()
{
    for(int i = 0; i < ROWS; i++)
    {
        if (Game::line_filled(i))
        {
            Game::empty_line(i);
        }
    }
}

void Game::drop(const int& row)
{
    for(int i = 0; i < row - 1; i++) // -1 to prevent index out of range
    {
        for(int j = 0; j < COLUMNS; j++)
        {
            cases[row - i][j] -> vacant = cases[row - i - 1][j] -> vacant;
        }
    }

    for(int j = 0; j < COLUMNS; j++) // Reset last line which will always be empty after a drop
    {
        cases[0][j] -> empty();
    }
}

void Game::generate()
{
    next = Tetromino::random();
}

void Game::next_tetromino() {
    current = next;
    Game::generate();
}
