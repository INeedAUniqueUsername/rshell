# Assignment 2 - Basic Shell (rshell)

**By Alex Chen and Kyle Tran**

## Description
rshell is a basic shell with the purpose of executing programs followed by arguments. The program displays a prompt with the currently logged in user and awaits for input. Input is in the form of the program you want to execute followed by arguments you wish to pass to the program. You can also pass in Connectors `&&`, `||`, and `;` in order to chain commands together. `&&` indicates that the next command executes only if the previous command succeeded and `||` requires that the previous command failed while `;` functions the same as a line separator. With this there is the possibility of chaining multiple commands with multiple arguments in a single line.

## Known Bugs
- Program crashes if a statement begins with a connector
- String arguments are not properly handled as quotes are treated as regular characters.
- `cd` does not work
