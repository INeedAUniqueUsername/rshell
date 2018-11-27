# Assignment 2 - Basic Shell (rshell)

**By Alex Chen and Kyle Tran**

## Description
rshell is a shell program that aims to replicate some features of Bash while using C++. The program displays a prompt with the currently logged in user and awaits for input. Input is in the form of the program you want to execute followed by arguments you wish to pass to the program. You can also pass in Connectors `&&`, `||`, and `;` in order to chain commands together. `&&` indicates that the next command executes only if the previous command succeeded and `||` requires that the previous command failed while `;` functions the same as a line separator. With this there is the possibility of chaining multiple commands with multiple arguments in a single line. You can also test the existence of paths, files, and directories using bracket expressions, and modify operation precedence using parentheses.

## Known Bugs
- Program does not properly handle terminating semicolon
- Upon encountering an unclosed Chain, Test, or String, the program will return an error rather than ask for additional input.
