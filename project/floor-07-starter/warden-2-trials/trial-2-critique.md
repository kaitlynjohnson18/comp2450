# Trial II — *The Critique*

*Warden of the Middle Gates · Midterm 2 · 15 %*

This trial **requires** you to use an LLM. You will paste the prompt and the reply, then critique what you got. Reading and judging AI output is part of the job now; we are practicing it on purpose.

---

## The prompt

Open Claude (or your LLM of choice) and paste **this exact prompt**:

> *Write a C++ class template `Deque<T>` that wraps a doubly-linked list and supports `push_front`, `push_back`, `pop_front`, `pop_back`, `front`, `back`, `size`, and `empty`. Each method should be O(1). Briefly explain your design choices.*

Tell us which tool you used and the date:

- **Tool:** Google Gemini
- **Date of the conversation:** 2026-10-19

---

## The LLM's reply

Paste the **full reply**, code and prose. Use a fenced code block for the code. Do not edit the reply — we want to see what you actually got.

Prompt:
Write a C++ class template Deque<T> that wraps a doubly-linked list and supports push_front, push_back, pop_front, 
pop_back, front, back, size, and empty. Each method should be O(1). Briefly explain your design choices.

Reply:
Here is an O(1) Deque<T> implementation wrapping a custom doubly-linked list, complete with dynamic memory management (Rule of Five) 
and safety checks.

```cpp
#include <cstddef>
#include <stdexcept>
#include <utility>

template <typename T>
class Deque {
private:
    struct Node {
        T value;
        Node* prev;
        Node* next;

        // Perfect forwarding constructor to support emplace-style insertion
        template <typename... Args>
        Node(Node* p, Node* n, Args&&... args)
            : value(std::forward<Args>(args)...), prev(p), next(n) {}
    };

    Node* head{nullptr};
    Node* tail{nullptr};
    std::size_t count{0};

    void clear() noexcept {
        Node* current = head;
        while (current) {
            Node* next_node = current->next;
            delete current;
            current = next_node;
        }
        head = tail = nullptr;
        count = 0;
    }

public:
    // -------------------------------------------------------------------------
    // Constructors & Destructor (Rule of Five)
    // -------------------------------------------------------------------------
    Deque() = default;

    ~Deque() {
        clear();
    }

    // Copy Constructor - O(N)
    Deque(const Deque& other) {
        for (Node* curr = other.head; curr != nullptr; curr = curr->next) {
            push_back(curr->value);
        }
    }

    // Copy Assignment Operator
    Deque& operator=(const Deque& other) {
        if (this != &other) {
            Deque temp(other);
            swap(temp);
        }
        return *this;
    }

    // Move Constructor - O(1)
    Deque(Deque&& other) noexcept 
        : head(other.head), tail(other.tail), count(other.count) {
        other.head = nullptr;
        other.tail = nullptr;
        other.count = 0;
    }

    // Move Assignment Operator - O(1)
    Deque& operator=(Deque&& other) noexcept {
        if (this != &other) {
            clear();
            head = other.head;
            tail = other.tail;
            count = other.count;

            other.head = nullptr;
            other.tail = nullptr;
            other.count = 0;
        }
        return *this;
    }

    void swap(Deque& other) noexcept {
        std::swap(head, other.head);
        std::swap(tail, other.tail);
        std::swap(count, other.count);
    }

    // -------------------------------------------------------------------------
    // Capacity
    // -------------------------------------------------------------------------
    [[nodiscard]] bool empty() const noexcept {
        return count == 0;
    }

    [[nodiscard]] std::size_t size() const noexcept {
        return count;
    }

    // -------------------------------------------------------------------------
    // Element Access
    // -------------------------------------------------------------------------
    T& front() {
        if (empty()) {
            throw std::out_of_range("Deque::front(): Deque is empty");
        }
        return head->value;
    }

    const T& front() const {
        if (empty()) {
            throw std::out_of_range("Deque::front(): Deque is empty");
        }
        return head->value;
    }

    T& back() {
        if (empty()) {
            throw std::out_of_range("Deque::back(): Deque is empty");
        }
        return tail->value;
    }

    const T& back() const {
        if (empty()) {
            throw std::out_of_range("Deque::back(): Deque is empty");
        }
        return tail->value;
    }

    // -------------------------------------------------------------------------
    // Modifiers
    // -------------------------------------------------------------------------
    void push_front(const T& value) {
        emplace_front(value);
    }

    void push_front(T&& value) {
        emplace_front(std::move(value));
    }

    void push_back(const T& value) {
        emplace_back(value);
    }

    void push_back(T&& value) {
        emplace_back(std::move(value));
    }

    template <typename... Args>
    T& emplace_front(Args&&... args) {
        Node* new_node = new Node(nullptr, head, std::forward<Args>(args)...);
        if (empty()) {
            head = tail = new_node;
        } else {
            head->prev = new_node;
            head = new_node;
        }
        ++count;
        return head->value;
    }

    template <typename... Args>
    T& emplace_back(Args&&... args) {
        Node* new_node = new Node(tail, nullptr, std::forward<Args>(args)...);
        if (empty()) {
            head = tail = new_node;
        } else {
            tail->next = new_node;
            tail = new_node;
        }
        ++count;
        return tail->value;
    }

    void pop_front() {
        if (empty()) {
            throw std::out_of_range("Deque::pop_front(): Deque is empty");
        }
        Node* old_head = head;
        head = head->next;
        if (head) {
            head->prev = nullptr;
        } else {
            tail = nullptr;
        }
        delete old_head;
        --count;
    }

    void pop_back() {
        if (empty()) {
            throw std::out_of_range("Deque::pop_back(): Deque is empty");
        }
        Node* old_tail = tail;
        tail = tail->prev;
        if (tail) {
            tail->next = nullptr;
        } else {
            head = nullptr;
        }
        delete old_tail;
        --count;
    }
};
```
Design Choices
1. Explicit head, tail, and count Pointers:

   Maintaining direct head and tail pointers guarantees $O(1)$ access, insertion, and 
   deletion at both ends without needing to traverse the list.
   
   Storing count explicitly enables an $O(1)$ .size() call, rather than iterating through 
   the list $O(N)$ times.
   
2. Perfect Forwarding & Perfect Value Construction:

   Overloaded both lvalue (const T&) and rvalue (T&&) versions for 
   push_front and push_back.
   
   Implemented emplace_front and emplace_back using variadic templates (Args&&...) 
   to construct values in-place inside the allocated nodes, avoiding unnecessary copies or moves.
   
3. Exception Safety & Bounds Checking:

   front(), back(), pop_front(), and pop_back() throw std::out_of_range exceptions 
   when called on an empty queue rather than triggering undefined behavior.
   
   Const and non-const overloads are provided for front() and back() to maintain 
   const correctness.
   
4. Resource Management (Rule of Five):

   Provides explicit copy constructors, move constructors, copy/move assignment operators, 
   and a destructor to handle raw pointer memory safely without memory leaks or double-frees. 
   Copy assignment relies on the strong exception-safe copy-and-swap idiom.

---

## My critique  *( ≥ 150 words )*

Your critique must read like an engineer reviewing a colleague's pull request — specific, technical, and naming exactly what the LLM did. A vague critique ("looks fine to me, but it could be better") is *not* full credit even if the LLM's code was great.

Address each of these:

- **Compile.** Did you actually try to compile and run the code? On what types? What happened?
- **Is it really O(1)?** The prompt asks for all eight operations to be O(1). Audit the implementation. Did the LLM rely on `std::list` (whose `back()` and `pop_back()` are O(1) thanks to a tail pointer), or did it hand-roll storage that walks from `head` for `back()` (silently O(n))? The prose claim is not the same as the code.
- **Empty case.** What does `pop_front()` or `pop_back()` do on an empty deque? Is the LLM's choice reasonable? Remember the ADT contract makes *no promise* about popping when empty — that call is undefined behaviour at the contract level. Audit what your own Floor 7 code *actually* does (your `Chain<T>::pop_front` guards the empty case, so your `Queue<T>::dequeue` happens to no-op) — then say which of the two you should judge the LLM's deque against: the contract, or your implementation's accident.
- **Composition.** Did the LLM compose over your `Chain<T>` (the right move in your codebase), over `std::list` (also fine), or reimplement doubly-linked storage from scratch (duplication — mark down)?
- **Const-correctness.** Did the LLM give you both `T& front()` and `const T& front() const`? Both `T& back()` and `const T& back() const`? Or only the mutable overloads?
- **One concrete improvement.** Name one specific change you would make before merging this into a real project. Not "make it better" — a concrete code change.

(your critique — at least 150 words)

The code did compile, and did work properly. I tested the push_front, push_back, pop_front, pop_back, front, back, 
size, and empty functions. They all worked with full and empty lists. However, functions like pop_front, pop_back, 
front, and back threw exceptions for empty lists but they didn’t catch the exceptions, which caused the program to stop. 
However, for full lists, they all worked, and they worked for different types of variables including int, double, and 
strings. The front, back, size, and empty functions are O(1) because the code explicitly stores head and tail pointers 
as well as a count variable. Therefore, the code doesn’t need to traverse the link every time to get these values, making 
these functions O(1). For the push_back, push_front, pop_back, and pop_front functions, the code does use a doubly-linked 
list to either allocate a new node and adjust pointers or delete a node and adjust pointers. This is an O(1) function. It 
didn’t have to traverse through pointers for any of these functions. The pop_front and pop_back functions throw an out of 
range exception for an empty deque. This is a reasonable way to stop the actual code from executing and causing unexpected 
behavior when trying to access a node with nullptr. However, the code does need a catch statement because the thrown exception 
causes the program to crash. In our code for pop_front and pop_back, the functions return automatically when the head or tail 
is nullptr, causing none of the code to actually execute. This achieves the same purpose as the LLM’s version by not 
executing the actual code for the functions, but the LLM’s version does let the user know that this specific function could 
not be executed since the deque was empty. Even though the contract for a deque doesn’t specify the need to guard against the 
empty case, addressing the empty case for these functions is good coding practice and guards against unexpected output. The LLM’s code 
reimplements the linked list from scratch without using a std::list or another already implemented list like Chain. This allows the 
user to better understand what the code is doing, but it causes the code to be cluttered. Additionally, the user doesn’t necessarily 
need to understand the implementation of the deque ADT to use it. Therefore, I would have shipped a version with a std::list or 
an already implemented list like Chain to make the functions specific for the deque more clear. However, the list that it 
did build was well-built. It made sure to implement constructors, destructors, copy functions, and any of the other functions 
needed within the Rule of Five. It also uses an emplace_front and emplace_back to make the push_front and push_back functions 
more efficient. Nonetheless, with all of these functions to make the list, it makes the deque class very cluttered. The LLM 
did give both the const and non-const versions of front and back. It also defends adding both of these versions in its prose 
below the code. Before shipping this code, I would make sure to add a catch statement for the out of range exceptions thrown. 
Right now, the exceptions just cause the program to crash and stop, which is not favorable. Additionally, I would separate 
out the doubly-linked list implementation from the deque implementation to make the code more clear.

