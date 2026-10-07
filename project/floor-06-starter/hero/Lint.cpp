// COMP 2450 — Floor 6 starter
// hero/Lint.cpp — body of isBalanced.  THE WORK IS HERE (Wednesday).

#include "Lint.h"

#include "Stack.h"

namespace dungeon {

// TODO Floor 6 (Wednesday).
//
// The classic stack-balancing algorithm. Walk `s` left-to-right:
//
//   * If c is '(', '[', or '{':   push it on the stack.
//   * If c is ')', ']', or '}':
//       - if the stack is empty, the string is unbalanced — return false
//         (we hit a closer with nothing waiting to be closed).
//       - otherwise peek at the top:
//           if top is the matching opener, pop and continue;
//           if it is the WRONG opener, return false
//             (a `)` cannot close a `{`).
//   * Any other character is ignored — text, spaces, digits, letters.
//
// At the end of the loop, the string is balanced iff the stack is empty.
// (If anything is left on the stack, those openers were never closed.)
//
// Reference body (do NOT paste — type it from memory in class):
//
//     Stack<char> s;
//     for (char c : input) {
//         switch (c) {
//             case '(': case '[': case '{':
//                 s.push(c); break;
//             case ')':
//                 if (s.empty() || s.top() != '(') return false;
//                 s.pop(); break;
//             case ']':
//                 if (s.empty() || s.top() != '[') return false;
//                 s.pop(); break;
//             case '}':
//                 if (s.empty() || s.top() != '{') return false;
//                 s.pop(); break;
//             default:
//                 break;
//         }
//     }
//     return s.empty();

bool isBalanced(const std::string& line) {
    Stack<char> s;
    for (char c : line) { //each character within the string looped through
        switch (c) {
        case '(': 
        case '{': 
        case '[':  //if opening, then just push on the stack
            s.push(c);
            break;
        case ')':    // if end parentheses, then check to make sure it's not empty and that the top is the open parentheses
            if (s.empty() || s.top() != '(') return false;  //not in order if empty or not open parentheses
            s.pop();  // remove from stack if it is corresponding parentheses
            break;
        case '}':
            if (s.empty() || s.top() != '{') return false;
            s.pop();
            break;
        case ']':
            if (s.empty() || s.top() != '[') return false;
            s.pop();
            break;
        default:
            break;  //if none of those, then just ignores and loops to next character
        }

    }
    return s.empty();  //if the stack is empty and therefore all of the items had corresponding items and matched appropriately, then true


}

}  // namespace dungeon
