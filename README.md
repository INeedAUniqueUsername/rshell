# Assignment 2 - Basic Shell (rshell)

**By Alex Chen and Kyle Tran**

## Description
rshell is a basic shell with the purpose of executing programs followed by arguments. The program displays a prompt with the currently logged in user and awaits for input. Input is in the form of the program you want to execute followed by arguments you wish to pass to the program. You can also pass in operators &&, ||, and ; which tell the program whether or not to execute the next program. With this there is the possibility of chaining multiple programs in one input each with possibly multiple arguments.

## Known Bugs
- String arguments are not properly handled as quotes are treated as regular characters.
- Comments are not properly removed from lines, but this does not affect the apparent execution of the program.
