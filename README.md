# GUT - gmb's universal turing
 
Very simple Turing-like programming language written in C.

## Basic concept of the system:
```
	--+---+---+---+---+---+---+---+---+--- tape with some data (input)
	  | 1 | 1 | b | b | ? | 0 | 0 | 1 |
	--+---+---+---+---+---+---+---+---+---
               /\                 
	      |  |                ~~~~~ 
       l <-   |__|   -> r         | 0 |
	      head                ~~~~~
	       /\                 state(q)
	       |
	       +------------------> rules  
```
There is an infinite long tape(we use a finite log tape) with squares on it, each square contains a symbol. A head moves along the tape and it can read and/or write exactly one square on each step. It can move left or right, but only one square per step.
This system has a state, which can be changed. There is a set of rules, that describes the behaviour of the system, based on the current symbol and the state.
Every program (ruleset) MUST contain a rule for halt.
The system has to be initialized at startup (q = 0, head_pos = 0 [first square of the tape]).


## rules (program):

syntax: symbol(1) old_state(<= 16) new_state(<= 16) write(1) move(1) halt(1)

* symbol: a character that is on the tape, any character except '#' and ' '(space)
* old state: the state of the system : 
	* __ - whatever
	* qx - equals x ?
	* nx - NOT equals x ?
* new state: state after step : __ - no changes
				ex : state = x
				pp : plus 1 (++)
				mm : minus 1 (--)
				px : plus x
				mx : minus x
- write: overwrite the current symbol : _ - no changes
					any other character 
- move: moves the head: r - right
			l - left
			_ - no moves 
- halt: end of the program? : 0 - no
			      1 - yes

## example program:

In this example, the machine uses unary numeral system to add 2 numbers.
```
<t
111+11+bbbbbbbbbbbbb
t>
<r
1 __ p1 _ r 0
+ __ __ _ r 0
b q0 __ _ _ 1
b n0 mm 1 r 0
r>
```

## CHANGELOG
------------------
2015-01-09: v.1.2
------------------
  - "-u" switch added (usermode)
  - "-s" -> "-d" (debug)
  - "-S" switch added (Assembly)
------------------
2014-08-29: v.1.1
------------------
 - oldstate: 'e' -> 'q'
 - newstate: 'e' option added
 - state: state used to be a ONE digit number, now it can be a 10 digit number(at compare or modification)
 - values of 'status' and 'head' before and after a step (at displaying)

------------------
2014-08-22: v.1.0
------------------
 - first release

 gmb, 2014
