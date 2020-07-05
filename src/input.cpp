#include "include/input.h"

Input::Input(bool &running, Display &screen, GameBoard &board)
    : m_running(running), m_screen(screen), m_board(board)
{}

/* Checks with screen to see if any user input, then changes game state
   based on the event type (e.g. move player, resize screen) */
bool Input::process()
{
    if(!m_screen.getInput()) {
	return false;
    }

    switch(m_screen.getEventType())
    {
    //Respond to user key presses
    case TB_EVENT_KEY:
	//First, look at key combos
	switch(m_screen.getEventKey())
	{
	case TB_KEY_CTRL_X:
	case TB_KEY_CTRL_C:
	    //Stop program immediately
	    m_running = false;
	    break;
	case TB_KEY_ESC:
	    //Redraws whole screen (useful for exiting inventory subscreen, etc.)
	    m_board.redraw();
	    break;
	    //Basic player movement
	case TB_KEY_ARROW_RIGHT:
	    m_board.translatePlayer(1, 0);
	    break;
	case TB_KEY_ARROW_LEFT:
	    m_board.translatePlayer(-1, 0);
	    break;
	case TB_KEY_ARROW_UP:
	    m_board.translatePlayer(0, -1);
	    break;
	case TB_KEY_ARROW_DOWN:
	    m_board.translatePlayer(0, 1);
	    break;
	default:
	    //If not a key combo, look at individual keys
	    switch(m_screen.getEventChar())
	    {
	    case 'i':
		m_board.showInventory();
		break;
	    case '@':
		//m_board.showStats();
		break;
	    case 'e':
		//m_board.showPlayerEquipped();
		break;
	    case 'E':
		//m_board.equipPlayerItem();
		break;
	    case 'D':
		//m_board.deequipPlayerItem();
		break;
		//Controls for showing/moving cursor
	    case 'r':
		//m_board.bindCursorMode(m_board.player(), &GameBoard::rangeAttack);
		break;
	    case 't':
                m_board.bindCursorMode(&GameBoard::moveActor);
		break;
	    }
	}
	break;
	//Adjust screen if window is resized
    case TB_EVENT_RESIZE:
	m_screen.clear();
	if(!m_screen.largeEnough()) {
	    m_screen.printText(1, 1, "Error: screen not large enough");
	}
	else {
	    //Redraw as much of map as possible
	    m_board.redraw();
	}
	break;
    }

    return true;
}
