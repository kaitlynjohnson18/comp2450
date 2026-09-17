# Trial II — *The Critique*

*Warden of the Foundations · Midterm 1 · 15 %*

This trial **requires** you to use an LLM. You will paste the prompt and the reply, then critique what you got. Reading and judging AI output is part of the job now; we are practicing it on purpose.

---

## The prompt

Open Claude (or your LLM of choice) and paste **this exact prompt**:

> *Write a C++ function template that returns the largest element of a `std::vector<T>`. Handle the case where the vector is empty. Briefly explain your design choices.*

Tell us which tool you used and the date:

- **Tool:** (e.g., Claude / ChatGPT / Copilot Chat / …)
- **Date of the conversation:** YYYY-MM-DD

Tool: Gemini
Date of the conversation: 2026-06-17

---

## The LLM's reply

Paste the **full reply**, code and prose. Use a fenced code block for the code. Do not edit the reply — we want to see what you actually got.

```cpp
// (paste the LLM's code here)
```

(paste any prose explanation here, verbatim)


Prompt: Write a C++ function template that returns the largest element of a std::vector<T>. Handle the case where the vector is empty. Briefly explain your design choices.

Reply:
Here is a C++ template function to find the maximum element in a std::vector<T>, along with proper error handling and design choices.

```cpp
C++
#include <vector>
#include <algorithm>
#include <stdexcept>
#include <utility>

template <typename T>
const T& find_max(const std::vector<T>& vec) {
    if (vec.empty()) {
        throw std::invalid_argument("find_max: vector cannot be empty");
    }

    auto max_it = std::max_element(vec.cbegin(), vec.cend());
    return *max_it;
}
```

Design Choices
Range-Based Algorithm (std::max_element): Uses the C++ Standard Library algorithm to avoid re-implementing manual loop comparisons, reducing code complexity and potential off-by-one errors.

Return by Const Reference (const T&): Returning a reference avoids unnecessary copying of large or heavy custom types T.

Const Reference Parameter (const std::vector<T>&): Accepts the input vector by const reference to avoid copies and allow the function to work on both const and non-const vectors.

Const Iterators (cbegin() / cend()): Explicitly specifies read-only access to elements inside the vector.

Exception for Empty Vectors: Throws std::invalid_argument from <stdexcept> when given an empty vector. Because a reference cannot be bound to a null or non-existent object without producing undefined behavior, throwing an exception is the idiomatic standard in C++ when no fallback/default value exists.

---

## My critique  *( ≥ 150 words )*

Your critique must read like an engineer reviewing a colleague's pull request — specific, technical, and naming exactly what the LLM did. A vague critique ("looks fine to me, but it could be better") is *not* full credit even if the LLM's code was great.

Address each of these:

- **Compile.** Did you actually try to compile and run the code? On what types? What happened?
- **Empty case.** What does the empty-vector case actually do in this code? Is the LLM's choice reasonable? Is it different from what *you* would have chosen?
- **Requirements on `T`.** What does the code require of `T`? What happens if you instantiate it with a type that doesn't support `<` (e.g., a custom struct)?
- **Const-correctness.** Pass by value or by reference? Return by value or by reference? Would you change either?
- **`std::max_element`?** Did the LLM cheat by using the algorithm header, or did it write the loop? Which would *you* have shipped, and why?
- **One concrete improvement.** Name one specific change you would make before merging this into a real project. Not "make it better" — a concrete code change.

(your critique — at least 150 words)

The code did compile, and it did work correctly once a properly formatted int main() was created in Visual Studios. I 
attempted it using a vector of ints, a vector of strings, and an empty vector. The empty-vector case uses the built-in 
vector function .empty(). If this returns true, then the vector contains no elements and the code throws 
an invalid argument exception. This is reasonable. The .empty() usage is a good and accurate way to check if the vector is empty. 
Additionally, this check occurs at the beginning of the text, ensuring that the empty vector is caught before any other code 
executes. The choice to throw an invalid argument exception also seems reasonable. If the vector is empty, then there isn’t a 
largest element, meaning that the argument of the vector is invalid for this situation. This code requires T to have a value 
that can be compared to each other since it needs to find a maximum value. If the type doesn’t support <, then the items cannot 
be compared to each other and the max element cannot be found. The code would not work, causing the program to not compile. This 
code should work for any number variables such as int or double as well as strings. However, it would not work on variables such 
as custom classes where the class doesn’t point to a specific value that can be compared. This code executes by reference. It returns a 
const pointer, uses a const vector pointer, and checks the elements using a .cbegin() and .cend(). These functions ensure that the 
elements are not changed. I would agree with this choice. This function should only be identifying the maximum element in the vector. 
It should not be changing this maximum value or any other element in the vector. Additionally, passing a vector by reference allows 
less memory to be used during the function, especially if the vector is very large. Therefore, const passing by reference is the 
correct choice in this situation. The LLM did use std::max_element instead of a traditional loop. I would have shipped the 
max_element usage. Typically, the built-in functions are optimized and tested. Therefore, max_element will probably be more 
efficient and be able to handle most cases of data. Writing a loop would be less efficient code and would require checks to 
ensure that all situations work within the code. One improvement to this code would be to make sure that a catch statement is 
incorporated in int main(). Currently, this function throws an exception, but the code fails to mention catching the exception in 
the main code. Without catching the exception, the program could crash. Ensuring that a catch statement exists to handle this 
exception would allow the program to continue functioning even with an invalid vector input.

