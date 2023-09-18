# TODO list

- [ ] **Parse User Input**: Implement a mechanism to read and parse user input
commands. This involves handling command lines, parsing arguments, and separating
commands and parameters.
- [ ] **Command Execution**: Implement the ability to execute basic commands
entered by the user. This involves forking a new process, loading the desired
executable, and executing it with the appropriate arguments.
- [ ] **Built-in Commands**: Add support for built-in commands such as `cd`,
`pwd`, `echo`, or `exit`. These commands should be executed within the shell
process itself, without spawning a new process.
- [ ] **Input/Output Redirection**: Extend your shell to handle input and output
redirection using the `<` and `>` symbols. Allow users to redirect input from a
file and redirect output to a file.
- [ ] **Background Process Execution**: Enable the execution of commands in the
background by appending an `&` at the end of the command. Implement the ability
to run a command in the background while still allowing the user to enter new
commands.
- [ ] **Pipeline and Command Chaining**: Implement the ability to connect multiple
commands using the `|` symbol. This allows the output of one command to be used
as input for the next command.
- [ ] **Signal Handling**: Handle common signals like `Ctrl+C` and `Ctrl+Z` to
interrupt or suspend processes, respectively. You can provide options for users
to customize the behavior or handle signals for specific commands.
- [ ] **Command History**: Add a command history feature that allows users to
navigate through previously entered commands using the up and down arrow keys.
Implement functionality to store and recall command history across different
sessions.
- [ ] **Tab Completion**: Provide tab completion support, where users can press
the `tab` key to autocomplete commands, paths, and file names based on entered
partial inputs.
- [ ] **Creation of Script Language**: Allow users to make scripts for
reusability and the capacity of handling more complex tasks. This involves
creating an interpreted language that supports things like: variables, constants,
control flow statements and functions.
- [ ] **Customization and Configuration**: Allow users to customize the shell's
behavior by configuring environment variables, defining aliases, or setting up
startup scripts.
- [ ] **Better Error Handling**: Implement informative error messages for
incorrect commands or syntax errors. Provide meaningful feedback to users to
help them understand and correct their inputs.
- [ ] **Create README file**.
