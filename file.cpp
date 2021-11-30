#include <iostream>
#include <stdio.h>
#include <vector>
#include <stdlib.h> // for wiping the screen
#include <unistd.h> // used for the delay after the title
#include <map>
#include "text.h" // for changing how the text appears

const int fieldSize = 10; // the number of units square the field is
const std::string goblin = "g"; // character representing goblins
const std::string ogre = "o"; // character representing ogres
const std::string empty = "."; // character representing empty space
const std::string player = "p"; // character representing the player
const std::string teleportIcon = "@"; // character used to animate teleportation
const std::string gameInfo = "Type w a s d to move.\nType 'exit' to stop the program." 
    "\nType \'t\' to teleport. The next time you move, you will skip over a space." 
    "\nYou can only teleport twice!" 
    "\nType i to open this information.\n\nA warning will appear when you are next to an ogre." 
    "\nIf you land on an ogre, you lose." 
    "\nWhen all spaces next to the ogre are explored or contain other ogres, the ogre will appear as an \'" + orange + ogre + toDefault + "\'." 
    "\nYou are displayed as a \'" + yellow + player + toDefault + "\'." 
    "\nThe \'"+green + goblin + toDefault + "\'s are goblins." 
    "\nFind all the goblins to win.\n\nenter to continue.\n";
std::vector<int> ogresY;
std::vector<int> ogresX;
 
int totalOgres = 0;
int totalGoblins = 0;


std::vector<std::vector<std::string>> generateField() { // generates field with a 20% chance of goblins and a 10% chance of ogres
    std::vector<std::vector<std::string>> field;
    for (int c = 0; c < fieldSize; c++) {
        std::vector<std::string> column;
        for (int r = 0; r < fieldSize; r++) {
            int randomNumber = rand()%10;
            if (randomNumber == 7 || randomNumber == 8) { // add goblin
                totalGoblins++;
                column.push_back(goblin);
            } else if (randomNumber == 9) { // add ogre
                totalOgres++;
                ogresY.push_back(c);
                ogresX.push_back(r);
                column.push_back(ogre);
            } else {
                column.push_back(empty);
            }
        }
        field.push_back(column);
    }
    return field;
}


std::vector<std::vector<std::string>> generateBlankField() { // generates a field with only spaces
    std::vector<std::vector<std::string>> field;
    for (int c = 0; c < fieldSize; c++) {
        std::vector<std::string> column;
        for (int r = 0; r < fieldSize; r++) {
            column.push_back(" ");
        }
        field.push_back(column);
    }
    return field;
}


void updateField(std::vector<std::vector<std::string>> field, std::pair<int, int> position, bool isTeleporting, std::pair<int, int> exclude = {-1,-1}) { // displays field on screen
    std::cout << " " << underlined << "                     \n" << toDefault;
    for (int c = 0; c < fieldSize; c++) {
        std::vector<std::string> column = field[c];
        std::cout << "| ";
        for (int r = 0; r < fieldSize; r++) {
            if (c == exclude.second && r == exclude.first) {
                std::cout << " ";
            } else if (c == position.second && r == position.first) {
                if (isTeleporting) {
                    std::cout << teal << teleportIcon;
                } else {
                    std::cout << yellow << player;
                }
            } else {
                if (column[r] == goblin) {
                    std::cout << green << goblin;
                } else if (column[r] == ogre) {
                    std::cout << orange << ogre;
                } else {
                    std::cout << column[r];
                }
            }
            std::cout << " " << toDefault;
        }
        std::cout << "|\n";
    }
    std::cout << " " << strickenThrough << "                     \n" << toDefault;
}


void clearScreen() {
    std::cout << std::flush;
    std::system("clear");
}

void drawDisplay(std::vector<std::vector<std::string>> field, std::pair<int, int> position, bool isTeleporting, std::pair<int, int> formerPosition = {0,0}) {
    if (isTeleporting) {
        clearScreen();
        updateField(field, formerPosition, true, position);
        usleep(500000);
        clearScreen();
        updateField(field, position, true);
        usleep(500000);
    }
    clearScreen();
    updateField(field, position, false);
}


bool searchForOgres(std::vector<std::vector<std::string>> field, std::pair<int, int> position) { // checks for ogres in adjacent spaces, returns warning if so
    bool foundOgres = false;
    if (position.first > 0 && field[position.second][position.first-1] == ogre) {
        foundOgres = true;
    } else if (position.first < fieldSize-1 && field[position.second][position.first+1] == ogre) {
        foundOgres = true;
    } else if (position.second > 0 && field[position.second-1][position.first] == ogre) {
        foundOgres = true;
    } else if (position.second < fieldSize-1 && field[position.second+1][position.first] == ogre) {
        foundOgres = true;
    }
    return foundOgres;
}


void displayInformation() { // displays the how to play information
    clearScreen();
    std::cout << gameInfo;
    std::cin.ignore(); // when something is entered, continue
    clearScreen();
}


int main () {
    clearScreen();
    std::cout << bold << green << "G" << gray << "oblin Mining\n" << toDefault << std::flush; // display title for 2 sec
    usleep(2000000);

    displayInformation(); // show info so player knows what to do
    
    srand(time(NULL)); // seed randomizer

    bool isRunning = true; // while game is running
    
    while (isRunning) {
        std::vector<std::vector<std::string>> field = generateField(); // the entire field
        std::vector<std::vector<std::string>> knownField = generateBlankField(); // the field displayed

        bool isGameOver = false;

        std::pair<int,int> position = {rand()%10, rand()%10}; // random start point

        while (field[position.second][position.first] == ogre) { // make sure player does not start on an ogre
            position.first = rand()%10;
            position.second = rand()%10;
        }

        int goblinsFound = 0;
        int teleportsRemaining = 2;

        knownField[position.second][position.first] = field[position.second][position.first]; // put the player's start location on the known field
        updateField(knownField, position, false); // draw starting field
        
        if (searchForOgres(field, position)) { // so the player knows if they start near an ogre
            std::cout << "There is an ogre nearby! ";
        }

        bool isTeleporting = false;

        while (!isGameOver) {

            std::pair<int, int> difference = {0,0};
            bool justTeleported = false;
            std::string endMessage = "";

            std::string newInput;

            std::cin >> newInput;

            if (newInput == "exit") { // quit
                endMessage = "You have terminated the program.";
                isRunning = false;
                isGameOver = true;
            } else if (newInput == "a") { // move left
                if (isTeleporting == false && position.first > 0) {
                    difference.first = -1;
                } else if (isTeleporting && position.first > 1) {
                    difference.first = -2;
                    isTeleporting = false;
                    justTeleported = true;
                }
            } else if (newInput == "d") { // move right
                if (isTeleporting == false && position.first < fieldSize-1) {
                    difference.first = 1;
                } else if (isTeleporting && position.first < fieldSize-2) {
                    difference.first = 2;
                    isTeleporting = false;
                    justTeleported = true;
                }
            } else if (newInput == "s") { // move down
                if (isTeleporting == false && position.second < fieldSize-1) {
                    difference.second = 1;
                } else if (isTeleporting && position.second < fieldSize-2) {
                    difference.second = 2;
                    isTeleporting = false;
                    justTeleported = true;
                }
            } else if (newInput == "w") { // move up
                if (isTeleporting == false && position.second > 0) {
                    difference.second = -1;
                } else if (isTeleporting && position.second > 1) {
                    difference.second = -2;
                    isTeleporting = false;
                    justTeleported = true;
                }
            } else if (newInput == "i") { // display information
                displayInformation();
            } else if (newInput == "t") { // initiate teleport
                if (teleportsRemaining > 0) {
                teleportsRemaining--;
                isTeleporting = true;
                } else {
                    endMessage = "You have already used both your teleports! ";
                }
            }

            position.second += difference.second;
            position.first += difference.first;

            std::string spaceContains = field[position.second][position.first];

            if (spaceContains == goblin) {
                goblinsFound++;
                field[position.second][position.first] = empty;
                if (goblinsFound == totalGoblins) {
                    endMessage = "You win!";
                    isGameOver = true;
                }
            } else if (spaceContains == ogre || knownField[position.second][position.first] == ogre) {
                endMessage = "You have hit an ogre.";
                isGameOver = true;
            }

            if (knownField[position.second][position.first] == " ") { // add the space moved onto to the map
                knownField[position.second][position.first] = spaceContains;
            }

            for (int o = 0; o < totalOgres; o++) { // displays ogres on the screen when all four sides are surrounded.
                int ogreX = ogresX[o];
                int ogreY = ogresY[o];
                int sidesFound = 0;
                int boardEdgesTouching = 0;
                if (ogreX < fieldSize-1 && (knownField[ogreY][ogreX+1] != " " || field[ogreY][ogreX+1] == ogre)) {
                    sidesFound++;
                } if (ogreX > 0 && (knownField[ogreY][ogreX-1] != " " || field[ogreY][ogreX-1] == ogre)) {
                    sidesFound++;
                } if (ogreY < fieldSize-1 && (knownField[ogreY+1][ogreX] != " " || field[ogreY+1][ogreX] == ogre)) {
                    sidesFound++;
                } if (ogreY > 0 && (knownField[ogreY-1][ogreX] != " " || field[ogreY-1][ogreX] == ogre)) {
                    sidesFound++;
                }
                if (ogreY == 0) {
                    boardEdgesTouching++;
                } if (ogreY == fieldSize-1) {
                    boardEdgesTouching++;
                } if (ogreX == 0) {
                    boardEdgesTouching++;
                } if (ogreX == fieldSize-1) {
                    boardEdgesTouching++;
                }
                if (sidesFound+boardEdgesTouching == 4) {
                    knownField[ogreY][ogreX] = ogre; // so it displays on field
                    field[ogreY][ogreX] = empty; // so it won't give warnings to the player
                }
            }

            if (searchForOgres(field, position) && endMessage == "") { // check for nearby ogres
                endMessage = "There is an ogre nearby! ";
            }

            // update the display

            std::pair<int,int> formerPosition = {position.first-difference.first, position.second-difference.second};
            drawDisplay(knownField, position, justTeleported, formerPosition);
            

            std::cout << endMessage;
        }

        if (goblinsFound == totalGoblins) { // print success or fail message
            std::cout << "\nYou found all " << totalGoblins << " goblins!\n";
        } else {
            std::cout << "\nYou found " << goblinsFound << " out of " << totalGoblins << " goblins.\n";
        }
        if (isRunning) { // if still running, check if want to play again
            std::cout << "\nDo you want to play again? y/n ";
            std::string playAgain;
            std::cin >> playAgain;
            if (playAgain != "y") {
                isRunning = false;
            }
        }
        clearScreen();
        ogresX = {};
        ogresY = {};
        totalGoblins = 0;
        totalOgres = 0;
    }


    return 0;
}
