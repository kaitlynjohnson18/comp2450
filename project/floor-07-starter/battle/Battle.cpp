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
void PrintMenu(Bag<MenuOptions> menu, int playerHP, int wardenHP) {
    std::cout << "\n -- Your turn --    your HP: " << playerHP << "    Warden HP: " << wardenHP << "\n";
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
                hero.eventLog.push_front("You drink a potion and heal by " + std::to_string(item->value));
                return;
            }
            else if (item->name == "Rusty sword") {
                wardenHP = wardenHP - item->value;
                playerHP -= 1;
                ValidateHP(wardenHP);
                ValidateHP(playerHP);
                std::cout << "You attack with a Rusty Sword but also cut yourself. Warden HP -> " <<
                    wardenHP << ".\n" << "Your HP: " << playerHP << "\n";
                hero.eventLog.push_front("You use a Rusty Sword");
                return;
            }
            else {
                std::cout << item->name << " fails to provide aid.\n";
                return;
            }



        }
    }
}

}  // anonymous namespace

BattleOutcome runWaveSurvival(Hero& hero, int waves) {
    return BattleOutcome::Fled;
}

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







}  // namespace dungeon
