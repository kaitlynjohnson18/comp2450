# Trial III — *The Encounter* — Notes

*Warden of the Foundations · Midterm 1 · 60 %*

This file is the grader's map of your battle code. Help us find each piece. Be specific — file and line numbers are encouraged.

---

## AI declaration for Trial III

(One or two sentences. *Which* tool, *what* you asked it for, *what* you did with the result. "Did not use" is fine — but the line must be there.)

---

Did not use.

## Two functions I wrote without AI assistance

These are the functions Friday's quiz can ask me to modify on paper. I wrote each of these by hand, with no autocomplete on the body. Pick functions you know cold — three of the four quiz questions pull from these.


1. `<AttackAction>` in `<battle/Battle.cpp>:<140:146>`
2. `<runWardenBattle>` in `<battle/Battle.cpp>:<208:265>`

## Floor 0–3 ties — where to find them in my code

Help the grader find each of the four required ties.

**Floor 0 (ADT).** The available menu actions are stored in:

Container Type: `<Bag>`
Declared at: `battle/Battle.cpp:<215>`
Comment in code: Pasted here
The Menu uses a Bag because you need to be able to access any element at any time, and it doesn't 
particularly matter what order actions are presented in as long as you can access them and the order 
doesn't change randomly everytime you print them. The implementation of Bag in the code achieves all of these goals.


**Floor 1 (search).** `findByName<Item>` is called at:

- `battle/Battle.cpp:<170>` (Use-item branch)
- 

**Floor 2 (sort).** The at-display sort (`std::sort` with a comparator, or your Floor 2 `sortInventory`) is called at:

- `battle/Battle.cpp:<157>` (before displaying the items menu)
- Sort criterion: `<descending value>` (e.g., descending value — the healing-power stand-in — or ascending weight)
- One-sentence reason: (why this criterion?)
It is sorted by descending value so that the most valuable items are at the top and therefore are most visible to the user

**Floor 3 (templates + exceptions).** `BattleException` (or `BagException` where a bad index is the fault) is:

- Thrown at: `battle/Battle.cpp:<127>` on `<BattleException>` for a turn input of letters
- Thrown at: `battle/Battle.cpp:<136>` on `<BagException>` for a bad index
- Thrown at `battle/Battle.cpp:<172>` on `<BattleException>` for an invalid item input
- Caught at: `battle/Battle.cpp:<248>`
---

## Reflection  *( ≤ 200 words )*

Which Floor's tie was hardest to wire in, and why? Was anything obvious in hindsight that wasn't obvious when you started?

(your reflection)

Floor 3’s tie was hardest to wire in. At first, I couldn’t figure out whether to use a BagException or a BattleException for 
the code. Then, I couldn’t figure out how to make each of them produce unique messages without creating significant amounts of 
extra code to catch each type of message. Ultimately, I figured out how to implement the .what() function along with additional 
generic messages to produce the desired error message. I placed a catch statement that dealt with all types of errors at the
end of the main loop, allowing it to catch both the BagException and the BattleException with their own messages created 
when the errors were thrown. In hindsight, using one catch block at the end of the loop seems obvious, but I originally thought 
I would need to implement catch blocks for every single potential error message.


---

## Tuning notes (optional)

If you changed any of the tunables (`kPlayerStartHP`, `kWardenStartHP`, `kPlayerAttackDmg`, `kWardenAttackDmg`), or added new ones (item healing amounts, etc.), list them and their values here so the grader can play through your battle without surprise.

Used default HP and attack values. Healing potion heals the user by the default value of the Healing Potion. 
The Rusty Sword applies damage to the Warden based on the item's value but also does one damage to the player.

(your notes, or "defaults")
