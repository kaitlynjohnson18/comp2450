# Trial I — *The Keepers' Brief*

*Warden of the Middle Gates · Midterm 2 · 15 %*

Four short answers, one for each floor span above. **50–100 words per answer.** No code on this trial (except the one-liner in Q2); just clear thinking.

AI is welcome to *check* your answers; it is not welcome to *write your sentences*. These answers are short enough that an LLM voice is recognisable.

---

## 1. Floors 4 / 4½ — linked lists

> Your encounter's **event log** (every "Aric strikes Imp for 6", every "Imp retaliates for 3") is appended to `hero.eventLog`, which is a `Chain<std::string>`. Defend that against a `Bag<std::string>` (vector-backed) in terms of one operation the battle actually performs — naming the Big-O of that operation in `Chain` and in `Bag`. *Hint: think about what `Bag` has to do the moment its capacity is exceeded.*

(your answer — 50–100 words)

When appending to the hero.eventLog, the event is added using the chain’s push_front function. This function allocates a 
new Node and adjusts a few pointers, making this function O(1). However, the Bag’s equivalent function would push_back this 
value onto a vector. If the capacity of the Bag was exceeded, then a new vector would have to be created and every value 
copied over, making this function O(N). A linked list’s Nodes are much more efficient than adjusting a vector’s size, making 
a Chain a better choice than Bag for a situation where events are added continuously.


---

## 2. Floor 5 — iterators

> When the player picks the **Inspect log** menu option, your encounter calls `printLog(hero.eventLog.begin(), hero.eventLog.end(), 5, ...)` — the same function template your Floor 5 iterators brought to life. (Because `eventLog` is built with `push_front` everywhere, the *front* of the chain is the most recent entry; the forward iterator walks newest-to-oldest naturally.) Show **one line** of C++ that does the equivalent walk by hand using an explicit `Chain<std::string>::iterator` loop. Then one sentence: why is this iterator *bidirectional* (and what becomes impossible the moment you ask for random-access on it)?

```cpp
for (Chain<std::string>::iterator it = hero.eventLog.begin(); it != hero.eventLog.end(); it++)   std::cout << *it << “\n”;
```

(one-sentence answer — bidirectional vs random-access)

This iterator is bidirectional because the – operation is available which allows the iterator to go backwards as well, 
but an efficient Big O becomes impossible for random-access because the nodes have to be traversed one-by-one instead of 
automatically going to the right one.


---

## 3. Floor 6 — stacks

> Your `undo` command pops the most recent action off a `Stack<UndoAction>` and restores its snapshot. Defend the choice of stack against a queue or a deque in one sentence — referencing the *contract* of LIFO (not the implementation). Then one extra sentence: name one piece of battle state that your `undo` *intentionally does not* restore, and explain why the player won't mind.

(your answer — 50–100 words)

A stack will return the last item put in, and you would want the most recent action to be undone, making the stack 
a better choice than a queue or deque since it’s not reasonable to undo the very first action you did. The undo 
intentionally does not restore the warden’s previous hp status, and the player won’t mind because this keeps the warden 
at a potentially lower hp than it should be.


---

## 4. Floor 7 — queues

> Each wave's enemies sit in a `Queue<Enemy>` in arrival order. Explain — in one sentence — what *visibly* changes for the player if you swap `Queue<Enemy>` for `Stack<Enemy>`: which enemy gets attacked first, and in what order the rest are picked off.

(your answer — one or two sentences)

Since Stack is LIFO instead of FIFO, the player would attack and confront the most recent enemy added to the order instead of 
the first enemy added. Then the rest of the enemies would be attacked starting with the next newest while the first 
one put into the stack is attacked last.

