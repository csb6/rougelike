#ifndef DISPLAY_TERMBOX_H
#define DISPLAY_TERMBOX_H
#include <string>
#include "termbox.h"
#include "actor.h"
#include <array>

//Display Constants
constexpr int MinDisplayWidth = 30;
constexpr int MinDisplayHeight = 16;
//GUI dimensions subtracted from tb_width()/height() to get play area screen dimensions
constexpr int GUIHeight = 10;
constexpr int GUIWidth = 10;
constexpr int InputTimeout = 10; //in milliseconds
constexpr char WallTile = '#';
constexpr char PlayerTile = '@';
constexpr char ItemTile = 'i';
constexpr int MaxLogSize = 4; //in number of messages

//Map Constants
//  Map should take up at least min screen space so min-size screen is always full
constexpr int MapWidth = 30;
constexpr int MapHeight = 30;

struct Cell {
    char icon = '.';
    ItemCategory item_cat = ItemCategory::None;
    ActorId actor_id = -1;
    ItemId item_id = -1;
};

class Display {
//Purpose: Puts/manages content onscreen using termbox library
private:
    //m_screenWidth/Height are dimensions of onscreen area to contain tiles
    int m_cursorX, m_cursorY;
    int m_screenWidth, m_screenHeight, m_cornerX, m_cornerY;
    //The latest termbox input event
    tb_event m_event;
    //Variables for printing text without absolute positioning
    int m_textCol, m_textX, m_textY, m_textMaxWidth;

    std::string m_log[MaxLogSize];
    //Row that next log message should display on; number of log messages stored
    int m_logRow;
    //boardWidth/Height are dimensions of current window bounded on sides by GUI
    inline int boardWidth() { return tb_width()-GUIWidth; }
    inline int boardHeight() { return tb_height()-GUIHeight; }
    void clearChar(int col, int row);
    void putChar(int col, int row, char letter,
		 const uint16_t fg = TB_WHITE, const uint16_t bg = TB_BLACK);
    int getCameraCoord(int playerCoord, bool isX);
    void drawGUI();
    inline int convertCoord(int coord, bool isX);
public:
    Display();
    ~Display();
    bool getInput();
    //x and y = coords in terms of game map, not display
    //col and row = coords in terms of display, not game map
    bool isEmpty(int x, int y);
    bool hasCursor() { return m_cursorX != -1 && m_cursorY != -1; }
    void moveCursor(int x, int y);
    void translateCursor(int dx, int dy);
    void hideCursor();
    void printText(int col, int row, const std::string text,
		   const uint16_t fg = TB_WHITE, const uint16_t bg = TB_BLACK);
    int input(std::string msg, int col = 0, int row = 0);
    void log(const std::string &text);
    void printTextCol(int gridCol, const std::string text,
		      const uint16_t fg = TB_WHITE, const uint16_t bg = TB_BLACK);
    //Note: draw functions alter screen buffer; must call present() to push to display
    void draw(const std::array<std::array<Cell, MapWidth>, MapHeight> &map,
              int playerX, int playerY);
    //Setters/Getters
    void clear() { tb_clear(); }
    void present() { tb_present(); }
    bool largeEnough();
    int getEventType() { return m_event.type; };
    int getEventKey() { return m_event.key; }
    char getEventChar() { return m_event.ch; }
    //To get board x/y, have to add back cornerX/Y (see convertCoord)
    int getCursorX() { return m_cursorX + m_cornerX; }
    int getCursorY() { return m_cursorY + m_cornerY; }
};
#endif
