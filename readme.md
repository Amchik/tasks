Not completed yet.

# Amchik/tasks

... — simple To-Do manager.

![https://i.imgur.com/fXXvUoV.png](https://i.imgur.com/fXXvUoV.png)

## Usage

At commit a8ae43ee2028d7f89d48b6661400187929cc7ac5 you can parse taskrc and
print it as table.

* `table` - Print table of file
* `diagnostic` - Analyze only errors
* `use <file>` - Change current file

By defaults uses `Tasksfile` and `~/.taskrc`

### TaskRC

Syntax: `<type> <primary> [with <<param> <value>>[, <param> <value>]... [and <param> <value>]] [post]`.

For type `task` you can change `label` and `priority`. Label must be exists, `priority`
must be integer. Set post to `"completed"` for complete task.

For type `label` you can change only color in format `#HHHHHH`, where `H`
— hex digit. Note: color `#000000` will not been print.

#### Examples

```
# Comment!
task "Fix this \"todo manager\"" with label github and priority 0  
task "Make a soup"               with label food   ,   priority 10 completed
task "Go to a shop"              completed

label github 
label food   with color #e74c3c
```

## Building

For build main executeable type `make`.

For run unit tests use `make check`.

