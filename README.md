  # Z-Lisp
  
  Z-Lisp is a Lisp-like language, born by following [Build Your Own Lisp](https://www.buildyourownlisp.com/), and then updated to include more features.
  
  ## Table of Contents
  - [Basic Syntax](#basic-syntax)
  - [Data Types](#data-types)
  - [Built-in Functions](#built-in-functions)
  - [Examples](#examples)
  - [Getting Started](#getting-started)
  
  ## Basic Syntax
  The langue syntax rules are very simple:
  * **Expressions**: The fundamental building blocks are Expressions. An Expression is a list of Components enclosed in parentheses: `(comp1 comp2 comp3)`. The first Component must be a Function.
    Expressions are evaluated by running the Function with the following Components as its arguments.
  * **Lists**: Lists are exactly like Expressions, but they use curly-braces: `{comp1 comp2 comp3}`. The difference between Expressions and Lists is that Expressions are evaluated, while Lists are not.
  * **Components**: A Component can be one of the following:
    * Number: Represent numerical values (e.g., 123, -4.5).
    * Strings: Sequences of characters enclosed in double quotes (e.g., "hello").
    * Symbols: Names/keywords used to identify variables or functions (e.g., print, myVar).
    * Expressions.
    * Lists.
  * **Comments**: Anything following ; is a comment, and will be ignored by the interpreter.
  
  ## Data Types
  The following primitive types exist in the language and can be stored in variables or passed as arguments:
  * Numbers.
  * Strings.
  * Lists.
  * Functions.
  
  ## Built-in Functions
  In Z-Lisp everything is either data (Number, String, List) or a Function, 
  and all built-in mathematical operators (e.g., +, -) and keywords (e.g., if, def) are simply pre-defined variables which contains Functions.
  
| Function | Description | Arguments |
|---|---|---|
| `list` | Creates a list. |  Any number of values. |
| `head` | Returns the first element of a list. | A list. |
| `tail` | Returns a list containing all elements except the first. | A list. |
| `join` | Concatenates two lists. | Two lists. | 
| `len` | Returns the length of a list. | A list. |
| `+` | Adds numbers together (Cumulative). |  Any number of numbers. |
| `-` | Subtracts numbers (Cumulative). |  Any number of numbers. |
| `*` | Multiplies numbers (Cumulative). |  Any number of numbers. |
| `/` | Divides numbers (Cumulative). | Any number of numbers. |
| `%` | Returns the remainder of division (Cumulative). | Any number of numbers. |
| `^` | Raises a number to a power  (Cumulative). | Any number of numbers. |
| `min` | Returns the minimum of a set of numbers. | Any number of numbers. |
| `max` | Returns the maximum of a set of numbers. | Any number of numbers. |
| `>` | Greater than comparison (Cumulative). | Any number of numbers. |
| `<` | Less than comparison (Cumulative). | Any number of numbers. |
| `==` | Equality comparison. | Two values. |
| `!=` | Inequality comparison. | Two values. |
| `def` | Defines variables globally. | A List of Symbols followed by values. |
| `=` | Define variable locally. | A List of Symbols followed by values. |
| `env` | Returns the current environment. | None ({}). |
| `if` | Conditional statement. |  A Number (Condition), a List ("then" expression), and a second List ("else" expression). |
| `fun` | Defines an anonymous function. | A list of Symbols (parameters) and a second List (body expression). |
| `eval` | Evaluates a List as an Expression. | A List. |
| `load` | Loads and evaluates code from a file. | A string (file path). |
| `print` | Prints a value to the console. | Any value. |
| `error` | Prints an error message to the console. | A string (error message). |
| `exit` | Exits the program. |  None ({}). |

## Examples
**1. Arithmetic Operations**
```zlisp
  (+ 1 2 3)  ; Evaluates to 6.0
  (* 5 (- 10 4)) ; Evaluates to 30.0
```

2.0 Lists
```zlisp
(list 1 2 3) ; Creates a list {1 2 3}
(head {1 2 3}) ; Returns {1}
(tail {1 2 3}) ; Returns {2 3}
```

3.0 Variables and Functions
```zlisp
  (def {foo bar} 10 20) ; Define foo with 10, and bar with 20
  (fun {x y} {+ x y}) ; Define anonymous function that adds two numbers
  (def {add-two} (fun {x y} {+ x y})) ; Define function add-two
  (add-two foo bar) ; Call add-two with foo and bar, returns 30.0  
```

4.0 Conditionals
```zlisp
(if (> 5 2)
    {print "5 is greater than 2"}
    {print "5 is not greater than 2"})
```
