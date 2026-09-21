// COMP 2450 — Warden of the Foundations (Midterm 1)
// battle/Battle.cpp — YOU build the body.
//
// =====================================================================
// REQUIRED MECHANICS
// =====================================================================
// Re-read the public midterm page if anything below is unclear.
//
// MENU (at minimum, four options):
//   1. Attack          — damage the warden; warden retaliates that turn
//   2. Use item        — pick from inventory; effect; turn ends
//   3. Inspect Warden  — print warden's state; FREE action — turn does
//                        NOT end
//   4. Flee            — leave; gate stays closed
//
// END:
//   Victory  when wardenHP <= 0
//   Defeat   when playerHP <= 0
//   Fled     when the player chose Flee
//
// =====================================================================
// FLOOR-TIED REQUIREMENTS — ALL FOUR MUST APPEAR IN YOUR CODE
// =====================================================================
//
//   F0 (ADT)         — the available actions on a turn must live in a
//                      container of your choice. ABOVE the declaration,
//                      in a comment, name the ADT and defend it in one
//                      sentence (the same kind of defence Trial I Q1
//                      practiced — but this is a different collection
//                      than Q1's items menu; the right ADT may differ).
//
//   F1 (search)      — the Use-item branch MUST call findByName<Item>
//                      against hero.inventory to look up the item the
//                      player typed.
//
//   F2 (sort)        — when the items menu is displayed, sort the
//                      inventory at display time with a comparator
//                      (std::sort or your Floor 2 sortInventory). Pick
//                      a criterion (value — the healing-power stand-in
//                      — weight, or name) and document it in a comment.
//
//   F3 (templates +  — invalid menu input must `throw BattleException`
//      exceptions)     (ready skeleton in Battle.h) — or BagException
//                      where a genuinely bad index is the fault —
//                      caught INSIDE the battle loop so the player gets
//                      another prompt — not a crash, not an exit.
//
// =====================================================================
// WHAT THE GRADER WILL DO
// =====================================================================
//   1. cmake --build the project. If it does not compile, the Warden
//      has won by default.
//   2. Type `battle warden`, play through to BOTH a victory and a
//      defeat (or attempt to — items + RNG permitting).
//   3. Type a deliberately invalid menu choice (e.g., "9" for a
//      4-option menu). The game must NOT crash; it must re-prompt.
//   4. Open this file and find each of the four Floor ties. They must
//      be REAL — i.e., the menu actually runs through your container,
//      Use-item actually goes through findByName, the items menu is
//      actually sorted, the throw actually fires on bad input.

#include "Battle.h"

#include <iostream>
#include <string>
#include <cctype>

#include "../hero/Bag.h"
#include "../hero/BagException.h"
#include "../hero/Item.h"
#include "../bestiary/Search.h"
#include "../hero/Sort.h"

namespace dungeon {


namespace {

// =====================================================================
// Tunable battle parameters. Edit to taste; document any tuning in
// encounter-notes.md so the grader knows what to expect.
// =====================================================================
constexpr int kPlayerStartHP   = 30;  //constexpr means these values are compile-time constants
constexpr int kWardenStartHP   = 50; //k prefix = means constant
constexpr int kPlayerAttackDmg = 6;   // damage per Attack action
constexpr int kWardenAttackDmg = 4;   // warden's retaliation damage

//Could have also down enum class where there is a set of named choices; keep our names of turn choices scoped

struct MenuOptions {
    int number;
    std::string action;
};

}  // anonymous namespace

void ValidateHP(int& HP) {
    if (HP < 0) {
        HP = 0;
    }
    return;
}

void WardenAttack(int& playerHP, int& wardenHP) {
    if (playerHP > 0 && wardenHP > 0) {
        playerHP = playerHP - kWardenAttackDmg;
        ValidateHP(playerHP);
        std::cout << "The Warden retaliates for " << kWardenAttackDmg << ". Your HP -> " << playerHP << "\n";
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

void AttackAction(int& playerHP, int& wardenHP) {
    wardenHP = wardenHP - kPlayerAttackDmg;
    ValidateHP(wardenHP);
    std::cout << "You strike for " << kPlayerAttackDmg <<
        ".  Warden HP -> " << wardenHP << "\n";
    WardenAttack(playerHP, wardenHP);
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
                    return;
                }
                else if (item->name == "Rusty sword") {
                    wardenHP = wardenHP - item->value;
                    playerHP -= 1;
                    ValidateHP(wardenHP);
                    ValidateHP(playerHP);
                    std::cout << "You attack with a Rusty Sword but also cut yourself. Warden HP -> " <<
                        wardenHP << ".\n" << "Your HP: " << playerHP << "\n";
                    return;
                }
                else {
                    std::cout << item->name << " fails to provide aid.\n";
                    return;
                }


            
        }
    }

    
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

    while (playerHP > 0 && wardenHP > 0) {
        try {
            PrintMenu(menu, playerHP, wardenHP);
            MenuOptions selectedOption;
            selectedOption = ReadTurnInput(menu);
            switch (selectedOption.number) {

                case 1: //Attack
                    AttackAction(playerHP, wardenHP);
                    break;

                case 2: //Use Item
                    UseItemAction(hero, playerHP, wardenHP);
                    WardenAttack(playerHP, wardenHP);
                    break;

                case 3:  //Inspect Warden
                    std::cout << "Warden of the Foundations. HP:  " << wardenHP << " / " <<
                        kWardenStartHP << ".\n Attack Strength: " << kWardenAttackDmg <<
                        "\nNo visible weakness. (free action)\n";
                    break;

                case 4: //Flee
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
        return BattleOutcome::Defeat;
    }
    else if (wardenHP <= 0) {
        return BattleOutcome::Victory;
    }
    return BattleOutcome::Fled;
    

    // TODO — write the boss battle. Suggested outline (yours to refactor):
    //
    //   int playerHP = kPlayerStartHP;
    //   int wardenHP = kWardenStartHP;
    //
    //   while (playerHP > 0 && wardenHP > 0) {
    //       print state (HPs, last action — your choice).
    //
    //       try {
    //           show menu (using your F0 container of actions).
    //           read input.
    //           if invalid → throw BattleException(...) [F3 — throw].
    //           dispatch on the action:
    //               Attack:    wardenHP -= kPlayerAttackDmg;
    //                          if wardenHP > 0, playerHP -= kWardenAttackDmg.
    //               Use item:  std::sort(hero.inventory.begin(),
    //                                    hero.inventory.end(),
    //                                    yourComparator)             [F2].
    //                          show sorted menu, read item name.
    //                          const Item* it = findByName<Item>(
    //                              hero.inventory, name);             [F1]
    //                          if (!it) throw BattleException(...);   [F3]
    //                          apply effect (heal? buff next attack? …).
    //                          end turn.
    //               Inspect:   print warden state. FREE — do NOT end turn.
    //               Flee:      return BattleOutcome::Fled.
    //       }
    //       catch (const std::exception& e) {                        [F3 — catch]
    //           std::cout << "  " << e.what() << "  Try again.\n";
    //           continue;   // re-prompt; turn does NOT advance
    //       }
    //   }
    //
    //   return wardenHP <= 0 ? BattleOutcome::Victory
    //                        : BattleOutcome::Defeat;
    //
    // Decompose into helpers however you want. The contract main.cpp
    // depends on is just runWardenBattle(Hero&).
    //
    // Replace the placeholder body below.


}

}  // namespace dungeon
