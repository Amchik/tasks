Not completed yet.

# Amchik/tasks

... — simple To-Do manager.

## Usage

At commit 6a74ce you can parse taskrc and
print it as table.

* `taskrc <file>` - Full analyze of file
* `table <file>` - Print table of file
* `diagnostic <file>` - Analyze only errors

### TaskRC

Syntax: `<type> <primary> [with <<param> <value>>[, <param> <value>]... [and <param> <value>]] [post]`.

For type `task` you can change `label` and `priority`. Label must be exists, `priority`
must be integer. Set post to `"completed"` for complete task.

For type `label` you can change only color in format `#HHHHHH`, where `H`
— hex digit. Note: color `#000000` will not been print.

#### Examples

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

