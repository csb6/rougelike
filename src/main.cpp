/*TODO:
[ ] Profile to see what is causing memory leaks when resizing window
[ ] Add way to save/load
[ ] Add way to drop items onto map (remove from inventory)
[X] Add basic test suite for key functionality (see old RPG code)
[ ] Add better, safer, more comprehensive way to draw GUI
[ ] Add better, faster way to get ref to Item from an (x, y) coordinate
[X] Add isPlayer flag to Actor class to clearly differentiate player from
    monsters without having to do dynamic dispatch/inheritance stuff
[X] Add way to equip items/armor
[ ] Add armor/weapon items that improve defense/attack
[X] Add way to specify items in .ini files
[X] Add variable damage
[ ] Tune combat/limit teleportation
[ ] Make some more maps to play
[ ] Add support for 'stair' tiles that allow you to move between lvels
[ ] Adjust skills; maybe have teleport skill, use it to determine range? Maybe
    use cunning skill?
[ ] Improve Monster AI to avoid barriers; maybe use A* again?
[ ] Add procedural map generator code
[X] Delete skills that aren't useful/usable
[ ] Reference melee skill and strength skill for attacking; factor in armor
    and agility for defense
[ ] Add 'negotiate' option with monsters that generates funny dialogue
    (e.g. low-negotiate skill player: "Hey, Commie bastard, want to surrender
    so I can kill you somewhat quicker?")
[ ] Find way to gracefully exit; use it in loadMap()'s error branch
[ ] Add keybindings file loaded on start
[ ] Test map file loading code on Windows (possibly using Wine?)
*/
#include "include/gameboard.h"
#include "include/input.h"
#include <iostream>
#include <tuple>
#include <cstdio> //for FILENAME_MAX
#ifdef _WIN32
#include <libloaderapi.h>
#elif __APPLE__
#include <mach-o/dyld.h>
#endif

/* Get absolute path to the directory that the executable is in*/
std::string getLocalDir()
{
#ifdef _WIN32
    LPSTR path[FILENAME_MAX];
    DWORD size = sizeof(path);
    if(GetModuleFileNameA(NULL, path, size) == 0) {
        std::cerr << "Error: Map file name could not be retrieved\n";
        exit(1);
    }
    std::string dirPath(path);
#elif __APPLE__
    char path[FILENAME_MAX];
    uint32_t size = sizeof(path);
    //Get path to executable
    if(_NSGetExecutablePath(path, &size) != 0) {
        std::cerr << "Error: Map file name too long\n";
        exit(1);
    }
    char realPath[FILENAME_MAX];
    //Remove ., .. from path
    realpath(path, realPath);
    std::string dirPath(realPath);
#endif
    //Strip off executable name from end ("/rpg2")
    return dirPath.substr(0, dirPath.size()-4);
}

/*/* Prompts user for integer value using given message
static int inputSkill(int index, const std::string &message)
{
    std::cout << index+1 << ". "<< message;
    int skillValue;
    std::cin >> skillValue;
    return skillValue;
}

/* Prompts for integer values for each of given messages, saving the responses.
   Each prompt will loop until the user enters a point value less than their
   remaining points, which are tracked over all prompts
static void inputAllSkills(const std::string messages[], int responses[], int messagesLen, int maxPoints)
{
    int usedPoints = 0;
    int newPoints = 0;
    std::cout << "You have " << maxPoints << " points to use for the next "
        << messagesLen << " categories.\n";
    for(int i = 0; i < messagesLen; ++i) {
        do {
            newPoints = inputSkill(i, messages[i]);
            if(usedPoints + newPoints > maxPoints) {
                std::cout << "You don't have enough points to do that.\n";
            }
            else {
                responses[i] = newPoints;
                if(usedPoints + newPoints == maxPoints) {
                    std::cout << "No more points available!\n\n";
                    return;
                }
                usedPoints += newPoints;
                std::cout << "\tPoints used: " << usedPoints << "\tPoints left: "
                    << maxPoints - usedPoints << "\n";
                break;
            }
        } while(true);
    }
}

/* Creates an actor with the give traits
static void assignSkills(Actor &actor, const int skills[])
{
    actor.m_strength = skills[0];
    actor.m_cunning = skills[1];
    actor.m_agility = skills[2];
    actor.m_education = skills[3];
    actor.m_sidearmSkill = skills[4];
    actor.m_longarmSkill = skills[5];
    actor.m_meleeSkill = skills[6];
    actor.m_barterSkill = skills[7];
    actor.m_negotiateSkill = skills[8];
}

/* Runs player character creation, giving option for quickstart or to redo
   creation if not satisified with stats
static void skillSelection(Actor &actor)
{
    //Player character creation
    int skills[SkillAmount];
    std::string prompts[SkillAmount] = { "Enter strength: ", "Enter cunning: ",
        "Enter agility: ","Enter education: ",
        "Enter sidearmSkill: ", "Enter longarmSkill: ",
        "Enter meleeSkill: ", "Enter barterSkill: ",
        "Enter negotiateSkill: "};
    while(true) {
        std::fill(std::begin(skills), std::end(skills), 0);
        std::cout << "Welcome to the game! Enter 'q' to quickstart or enter any other letter for character creation:\n\n";
        char response;
        std::cin >> response;
        if(response == 'q' || response == 'Q') {
            break;
        }

        //Custom character creation
        std::cout << "Enter your name: ";
        std::string name;
        std::cin >> name;
        inputAllSkills(prompts, skills, SkillAmount, MaxInitPoints);

        //Confirm the user's choices
        std::cout << "Enter your name: " << name << "\n";
        for(int i=0; i<SkillAmount; ++i) {
            std::cout << prompts[i] << skills[i] << "\n";
        }
        std::cout << "\nIs the above how you want your character to look? [Y/n] ";
        char choice;
        std::cin >> choice;
        if(choice == 'y' || choice == 'Y') {
            //Create player character based on choices
            actor.setName(name);
            assignSkills(actor, skills);
            break;
        }
    }
}*/

int main()
{
    //Actor player(0, 0, "Player", PlayerTile, true);
    //skillSelection(player);

    bool running = true;
    Display screen;
    GameBoard board(screen, getLocalDir() + "trapped-map.csv",
                    std::make_tuple('@', "Player", Strength{5}, Weight{10}));
    Input device(running, screen, board);

    //Start main game loop
    while(running && device.process()) {
        //board.updateActors();
        board.present();
    }

    return 0;
}
