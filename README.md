# Simple-Memory-Shell
 * Author: Harvey Tully 
 * Note: Most of Setup() was provided
 * Language: C
 * IDE used: OnlineGDB
 
Features
 * Basic Shell Simulator
 * Nonvolatile History
 * CTRL+C interrupt to view history
 
Functions
 * User types LINUX commands such as gcc, ls, kill, clear
 * File system stores these commands (10 most recent)
 * User can hit CTRL+C to print the history
 * User types "R" to rerun last command
 * User can also type "R X" to rerun last command starting w/ X (reruns last if none found
 * CTRL+D to exit shell
 * Reload the shell to see the histroy is still there
 
Setup
 * Compiled using UBUNTU
 * Make Document named shell.c
 * Copy/Paste solution into document and save
 * Open UBUNTU terminal
 * type: gcc -o shell shell.c
 * type: ./shell

To add
 * User Login and user specific history
 * Input validation for shell commands and rerun options
