# Amchik/tasks

~~-- simple todo manager.~~

Completed on ~7.17%.

## Features (now)

Parsing config queryies (taskrc):

Syntax: `<type> <primary> [with <<param> <value>>[, <param> <value>]... [and <param> <value>]] [post]`.

Examples:

```
task "Fix this \"todo manager\"" with label github and priority 0  
task "Make a soup"               with label food   ,   priority 10 completed
task "Go to a shop"              completed

label github 
label food   with color #e74c3c
```

## Building

For build main executeable type `make`,
or for build unit tests type `make test` (`make check` - run unit tests).

Note: for build unit tests after main executable remove file `main.o`.

