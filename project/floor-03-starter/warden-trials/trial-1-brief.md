# Trial I — *The Foundations Brief*

*Warden of the Foundations · Midterm 1 · 15 %*

Four short answers, one from each floor above. **50–100 words per answer.** No code on this trial (except the lambdas in Q3); just clear thinking.

AI is welcome to *check* your answers; it is not welcome to *write your sentences*. These answers are short enough that an LLM voice is recognisable.

---

## 1. Floor 0 — ADT

> Your battle's "Use item" menu shows the *currently usable* items in your hero's inventory on this turn. Name the right ADT for that menu. Defend the choice against its closest neighbour (e.g., why `bag` instead of `set`, or `list` instead of `bag`).

(your answer — 50–100 words)

1. The best ADT would be a list. The situation requires showing the currently usable items. This insinuates that items 
need to be removed or added to adapt to changing availability. Therefore, ADTs such as stack, queue, or deque would not 
apply because they can only be removed from a specific end. Duplicate items could also be present, ruling out a set. 
Having a menu insinuates that items should be presented in a specific order, ruling out the unordered bag. However, a bag 
might work if the bag is implemented using a data structure that keeps track of order like through a vector.


---

## 2. Floor 1 — search & Big-O

> Your inventory is kept sorted by healing power (in this codebase an item's `value` measures its potency, so `value` plays the healing-power role). The player types `use Healing potion`. Linear or binary search to find it by name? Justify, and give the Big-O for each.

(your answer — 50–100 words)

2. Binary search would be the better option. Since the inventory is already sorted by healing power, this eliminates 
any concerns about an unsorted inventory. Linear search has a best case of O(1) and a worst case of O(N). Binary search 
has a best case of O(1) and a worst case of O(logN). Based on these values, binary search does not increase nearly as fast 
and is more efficient than linear search, especially with large amounts of data since it doesn’t need to check every single 
potential item like linear search does. Therefore, it is the better choice for searching ordered data.


---

## 3. Floor 2 — sort & comparators

> Your "Use item" menu must be displayable sorted *either* by healing power (meaning `value`) *or* by weight. Show a one-line comparator (lambda) for each. One sentence on what language feature makes one `std::sort` call serve both orders.

*Note: `Item` has no healing field — an item's `value` measures its potency, so `value` plays the healing-power role here and in your battle.*

```cpp
// by healing power — i.e. by value
auto byValue  = /* your lambda */;

// by weight
auto byWeight = /* your lambda */;
```

(one-sentence answer — what language feature?)

3. 
Value:
auto byValue = [](const Item& a, const Item& b) { return a.value < b.value };

Weight
auto byWeight sort = [](const Item& a, const Item& b) { return a.weight < b.weight };

Templates probably allow std::sort to serve both types of comparisons because it adapts the code to fit whatever type of variable is used.


---

## 4. Floor 3 — templates & exceptions

> Why does `Bag<T>` live in `Bag.h` instead of `Bag.cpp`? And: when the player types `9` for a 4-option menu, where in your code should the validation **throw**, and where should it **catch**?

(your answer — 50–100 words)

4. The template needs to be accessible for every type of potential data.  A .cpp file compiles whatever variable 
types are within the code and cannot create the code for the variable calling the template. The header allows the 
template to create the correct code for the input T before .cpp executes it. The throw statement is after the potential 
error’s code, such as after the user types in potentially invalid input. The catch statement is after the try block. 
For an invalid menu entry, it would be at the end of the menu’s while loop, allowing the user to reinput menu data after 
catching the error.

