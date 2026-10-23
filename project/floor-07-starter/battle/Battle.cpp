// COMP 2450 — Floor 4 starter (post-Warden reference state)
// battle/Battle.cpp — REFERENCE implementation of runWardenBattle.
//
// This is the post-midterm baseline. If you wrote your own Warden boss
// battle for Trial III, drop YOUR battle/Battle.cpp over this file and
// keep working on Floor 4. The reference below is intentionally compact;
// your own version is almost certainly more polished. The point of
// shipping it here is that Floor 4 builds and runs even for students
// whose midterm submission was incomplete.
//
// Floor 4 thread:
//   The battle loop now also writes one event into hero.eventLog every
//   turn — "warden HP X / Y", "you struck for D damage", "the warden
//   struck for D", and the final outcome. By Friday, when push_front
//   actually does something, `log 10` after a battle will replay the
//   fight backward.
//
// Floor 0–3 ties (still required for the Warden's grade):
//   F0 — actions live in a Bag<MenuAction>.
//   F1 — Use-item branch calls findByName<Item> on hero.inventory.
//   F2 — items menu std::sort's the inventory by value-descending first.
//   F3 — bad input throws BagException, caught inside the loop.

#include "Battle.h"

#include <algorithm>
#include <iostream>
#include <string>

#include "../bestiary/Search.h"
#include "../hero/Bag.h"
#include "../hero/BagException.h"
#include "../hero/Item.h"
#include "../hero/Queue.h"
#include "../hero/Sort.h"
#include "../hero/Stack.h"
#include "../hero/UndoAction.h"

namespace dungeon {

namespace {

constexpr int kPlayerStartHP   = 30;
constexpr int kWardenStartHP   = 50;
constexpr int kPlayerAttackDmg = 6;
constexpr int kWardenAttackDmg = 4;

// F0 — abstract data type for the available turn actions. Bag<MenuAction>
// is the Floor-3 container template; one stamping for an enum-payload
// list is all the action menu needs. Defended in one sentence: the menu
// is a small, ordered, indexed list with no insertions mid-list, which
// is exactly what Bag<T> (vector-backed) is good for.

struct MenuOptions {
    int number;
    std::string action;
};

struct Enemy {
    std::string name;
    int hp;
    int attack;
};

void ValidateHP(int& HP) {
    if (HP < 0) {
        HP = 0;
    }
    return;
}

void WardenAttack(Hero& hero, int& playerHP, int& wardenHP) {
    if (playerHP > 0 && wardenHP > 0) {
        playerHP = playerHP - kWardenAttackDmg;
        ValidateHP(playerHP);
        std::cout << "The Warden retaliates for " << kWardenAttackDmg << ". Your HP -> " << playerHP << "\n";
        hero.eventLog.push_front(
            "the Warden struck for " + std::to_string(kWardenAttackDmg));
    }
}

void WardenAttackWave(Hero& hero, int& playerHP, const Queue<Enemy>& line) {
    if (playerHP > 0 && line.front().hp > 0) {
        playerHP = playerHP - line.front().attack;
        ValidateHP(playerHP);
        std::cout << line.front().name << " retaliates for " << line.front().attack <<
            ".   Your HP -> " << playerHP << ".\n";
        hero.eventLog.push_front(line.front().name + " struck for " + std::to_string(line.front().attack)); //Floor 4/4.5
    }
}
void PrintMenu(Bag<MenuOptions> menu, int playerHP, int wardenHP) {
    std::cout << "\n -- Your turn --    your HP: " << playerHP << "    Warden HP: " << wardenHP << "\n";
    for (MenuOptions& option : menu) {
        std::cout << "   " << option.number << ". " << option.action << "\n";
    }
    std::cout << ">";
}

void PrintMenuWave(Bag<MenuOptions> menu, int playerHP, int currWave, int numWave, const Enemy& current, const Queue<Enemy>& line) {
    std::cout << "\n-- Wave " << currWave << "/" << numWave << " --   "
        << "your HP " << playerHP << "       enemies remaining: " << line.size() << "\n";
    std::cout << "  front of line: " << current.name << " (HP " <<
        current.hp << ", ATK " << current.attack << ")\n";
    for (MenuOptions& option : menu) {
        std::cout << "   " << option.number << ". " << option.action << "\n";
    }
    std::cout << ">";
    
}

MenuOptions ReadTurnInput(const Bag<MenuOptions>& menu) {
    std::string in;
    int input;
    std::getline(std::cin, in);
    // if (!std::getline(std::cin, line)) {return MenuAction::Flee; }
    try {
        input = std::stoi(in);
    }
    catch (...) {//catching any exception type thrown from the stoi() function in order to throw a BattleException
        throw BattleException(" Alas, this is not a menu number. (enter 1 to " + std::to_string(menu.size()) + ")");
        //Floor 3: throwing the BattleException when something other than a number is input
    }

    for (size_t i = 0; i < menu.size(); i++) {
        if (menu.at(i).number == input) {
            return menu.at(i);
        }
    }
    throw BagException(static_cast<std::size_t>(input), menu.size());
    //Floor 3: throwing a BagException when it is a bad index
}



void AttackAction(Hero& hero, int& playerHP, int& wardenHP) {
    wardenHP = wardenHP - kPlayerAttackDmg;
    ValidateHP(wardenHP);
    std::cout << "You strike for " << kPlayerAttackDmg <<
        ".  Warden HP -> " << wardenHP << "\n";
    hero.eventLog.push_front(
        "you struck for " + std::to_string(kPlayerAttackDmg));
    WardenAttack(hero, playerHP, wardenHP);
}

void DequeueLine(Queue<Enemy>& line, Hero& hero) {
    if (line.front().hp == 0) {
        std::cout << line.front().name << " falls. Next in line steps forward.\n";
        hero.eventLog.push_front(line.front().name + " fell"); //Floor 4/4.5
        line.dequeue();
        return;
    }
}

void AttackActionWave(Hero& hero, Queue<Enemy>& line) {
    line.front().hp = line.front().hp - kPlayerAttackDmg;
    ValidateHP(line.front().hp);
    std::cout << "You strike " << line.front().name << " for " << kPlayerAttackDmg <<
        ".   HP -> " << line.front().hp << "\n";
    hero.eventLog.push_front(
        "You struck " + line.front().name + " for " + std::to_string(kPlayerAttackDmg)); //Floor 4/4.5

}

void UseItemAction(Hero& hero, int& playerHP, int& wardenHP) {
    if (hero.inventory.empty()) {
        std::cout << "Your satchel is empty.\n";
    }
    else {
        std::string input;
        const Item* item;
        sortInventory(hero, "value desc");
        //Floor 2 (sort): sorted by descending value so that the most valuable items are at the top and therefore most visible

        std::cout << "Choose an item by name: \n" << ">";
        printInventory(hero);
        if (!std::getline(std::cin, input) || input.empty()) {
            std::cout << "You hesitate.\n";
            return;
        }

        item = findByName<Item>(hero.inventory, input); //Floor 1 (search) tie-in
        // function template specialization
        if (!item) { //if it is a nullptr and you didn't find anything
            throw BattleException(" No such item exists.");
            //Floor 3: throwing a BattleException for an invalid item input
        }
        else {
            if (item->name == "Healing potion") {
                playerHP = playerHP + item->value;
                if (playerHP > kPlayerStartHP) {
                    playerHP = kPlayerStartHP;
                }
                std::cout << "You guzzle a delicious draught of Healing potion. HP -> " <<
                    playerHP << ". \n";
                hero.eventLog.push_front("You drink a potion and heal by " + std::to_string(item->value)); //Floor 4/4.5
                return;
            }
            else if (item->name == "Rusty sword") {
                wardenHP = wardenHP - item->value;
                playerHP -= 1;
                ValidateHP(wardenHP);
                ValidateHP(playerHP);
                std::cout << "You attack with a Rusty Sword but also cut yourself. Warden HP -> " <<
                    wardenHP << ".\n" << "Your HP: " << playerHP << "\n";
                hero.eventLog.push_front("You use a Rusty Sword"); //Floor 4/4.5
                return;
            }
            else {
                std::cout << item->name << " fails to provide aid.\n";
                return;
            }



        }
    }
}

Queue<Enemy> BuildWave(int wave) {
    Queue<Enemy> line;
    for (int i = 0; i <= wave; i++) {
        if (i % 4 == 0) {
            line.enqueue({ "Zombie " + std::to_string(i + 1), 10, 2 });
        }
        else if (i % 4 == 1) {
            line.enqueue({ "Skeleton " + std::to_string(i + 1), 16, 4 });
        }
        else if (i % 4 == 2) {
            line.enqueue({ "Creeper " + std::to_string(i + 1), 8, 5 });
        }
        else if (i % 4 == 3) {
            line.enqueue({ "Spider " + std::to_string(i + 1), 11, 3 });
        }
    }

    return line;
}

void recordUndo(Stack<UndoAction>& battleUndo, std::string description, const Hero& hero, int playerHP) {
    UndoAction a; //Floor 6
    a.description = description;
    a.inventorySnapshot = hero.inventory;
    a.playerHP = playerHP;
    battleUndo.push(a);
}

}  // anonymous namespace


BattleOutcome runWardenBattle(Hero& hero) {

    int playerHP = kPlayerStartHP;
    int wardenHP = kWardenStartHP;


    //The Menu uses a Bag because you need to be able to access any element at any time, and it doesn't 
    //particularly matter what order actions are presented in as long as you can access them 
    //and the order doesn't change randomly everytime you print them.
    Bag<MenuOptions> menu;
    menu.push_back({ 1, "Attack" });
    menu.push_back({ 2, "Use Item" });
    menu.push_back({ 3, "Inspect Warden" });
    menu.push_back({ 4, "Flee" });

    hero.eventLog.push_front("battle warden — engaged");

    while (playerHP > 0 && wardenHP > 0) {
        try {
            PrintMenu(menu, playerHP, wardenHP);
            MenuOptions selectedOption;
            selectedOption = ReadTurnInput(menu);
            switch (selectedOption.number) {

            case 1: //Attack
                AttackAction(hero, playerHP, wardenHP);
                break;

            case 2: //Use Item
                UseItemAction(hero, playerHP, wardenHP);
                WardenAttack(hero, playerHP, wardenHP);
                break;

            case 3:  //Inspect Warden
                std::cout << "Warden of the Foundations. HP:  " << wardenHP << " / " <<
                    kWardenStartHP << ".\n Attack Strength: " << kWardenAttackDmg <<
                    "\nNo visible weakness. (free action)\n";
                break;

            case 4: //Flee
                hero.eventLog.push_front("battle warden - fled");
                return BattleOutcome::Fled;
            }

        }
        catch (const std::exception& e) {
            std::cout << e.what() << " - try again\n";
            //Floor 3: catching both the BagException and the Battle Exception to print unique error and to tell user to try again
            //The code will then loop back through for another turn without warden retaliation
        }


    }

    if (playerHP <= 0) {
        hero.eventLog.push_front("battle warden - outcome: Defeat");
        return BattleOutcome::Defeat;
    }
    else if (wardenHP <= 0) {
        hero.eventLog.push_front("battle warden - outcome: Victory");
        return BattleOutcome::Victory;
    }
    return BattleOutcome::Fled;

}

BattleOutcome runWaveSurvival(Hero& hero, int waves) {

    int playerHP = kPlayerStartHP;
    if (waves < 1) waves = 1;

    Bag<MenuOptions> menu;
    menu.push_back({ 1, "Attack" });
    menu.push_back({ 2, "Use Item" });
    menu.push_back({ 3, "Inspect Log" });
    menu.push_back({ 4, "Undo" });
    menu.push_back({ 5, "Flee" });

    //important for the UndoAction to be a stack because you want to undo the last action put in, which aligns with Stack's LIFO
    Stack<UndoAction> battleUndo; //Floor 6
    bool undoUsed = false;

    hero.eventLog.push_front("battle waves " + std::to_string(waves) + " - yard opens"); //Floor 4/4.5

    for (int w = 1; w <= waves; w++) {
        Queue<Enemy> line = BuildWave(w); //Floor 7
        hero.eventLog.push_front("wave " + std::to_string(w) + " enters"); //Floor 4/4.5

        while (!line.empty() && playerHP > 0) {
            try {
                Enemy& front = line.front();
                MenuOptions selectedOption;
                PrintMenuWave(menu, playerHP, w, waves, line.front(), line);
                selectedOption = ReadTurnInput(menu);
                switch (selectedOption.number) {
                case 1: //Attack
                    recordUndo(battleUndo, "attack " + line.front().name, hero, playerHP);
                    undoUsed = false;
                    AttackActionWave(hero, line);
                    WardenAttackWave(hero, playerHP, line);
                    DequeueLine(line, hero);
                    break;

                case 2: //Use Item
                    recordUndo(battleUndo, "use item", hero, playerHP);
                    undoUsed = false;
                    UseItemAction(hero, playerHP, line.front().hp);
                    WardenAttackWave(hero, playerHP, line);
                    DequeueLine(line, hero); //Floor 7
                    break;

                case 3: //Inspect Log
                    std::cout << " -- recent log (newest first; free action) -- \n";
                    printLog(hero.eventLog.begin(), hero.eventLog.end(), 5, "newest first", hero.eventLog.size()); //Floor 5

                    break;

                case 4: //Undo
                    if (undoUsed) {
                        std::cout << "you may only undo once per turn. \n";
                    }
                    else if (battleUndo.empty()) {
                        std::cout << "nothing to undo yet.\n";
                    }
                    else {
                        const UndoAction& a = battleUndo.top(); //Floor 6
                        hero.inventory = a.inventorySnapshot;
                        playerHP = a.playerHP;
                        std::cout << "Undid: " << a.description << ". Your HP --> " <<
                            playerHP << ". Enemy state is unchanged. \n";
                        hero.eventLog.push_front("(undo) reverted: " + a.description); //Floor 4/4.5
                        battleUndo.pop();
                        undoUsed = true;
                        //Does not restore the enemy's HP
                    }
                    break;

                case 5: //Flee
                    hero.eventLog.push_front("waves - fled"); //Floor 4/4.5
                    return BattleOutcome::Fled;
                }


            }
            catch (const std::exception& e) {
                std::cout << e.what() << " - try again\n";
            }
        }
        if (playerHP <= 0) {
            hero.eventLog.push_front("battle waves - outcome: Defeat"); //Floor 4/4.5
            return BattleOutcome::Defeat;
        }
        hero.eventLog.push_front("wave " + std::to_string(w) + " cleared"); //Floor 4/4.5
        std::cout << "Wave " << w << " cleared.\n";
        
    }
    hero.eventLog.push_front(
        "battle waves -- outcome: Victory"); //Floor 4/4.5
    return BattleOutcome::Victory;

}







}  // namespace dungeon
