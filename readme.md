# Amchik/tasks

... — simple To-Do manager.

![https://i.imgur.com/fXXvUoV.png](https://i.imgur.com/fXXvUoV.png)

## Usage

At commit a8ae43ee2028d7f89d48b6661400187929cc7ac5 you can parse taskrc and
print it as table.

* `table` - Print table of file
* `diagnostic` - Analyze only errors
* `use <file>` - Change current file
* `edit <type> <primary key or @NUM for task type> <key> <value>` - Edit record
* `delete <type> <primary eky or @NUM for task type>` - Remove record

By defaults uses `Tasksfile` and `~/.taskrc`

Running without arguments equals passing only `table`.

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

#### Editing via `edit`

Syntax: `executable edit <type> <prim> <key> <value>`

For type `task` `prim` can be formatted as `@N`, where `N` -
number of task in table.

**NOTE:** if edit file from `a` bytes to `b` bytes, where `a > b` in end
of file will be writes with `a - b - 1` zero bytes.

##### Base

| Key | Description | Notes |
|-----|-------------|-------|
|`primary`| Primary value of query | |
|`post`| Post value of query | Cannot be used in `label`. In `task` it will be redirected to `status` |

##### `task`

| Key | Description | Notes |
|-----|-------------|-------|
|`label`| Label of task | Label must been exists. Cannot be change to none (not implented). |
|`priority`| Priority of task | _May be_ if not a number priority will sets to zero |
|`status`| Completion status | `completed` - completed, another - not completed. |

For edit description use `primary`.

##### `label`

| Key | Description | Notes |
|-----|-------------|-------|
|`color`| Color of label | Must be formatted as `#HHHHHH` |

For edit name use `primary`.

## Building

For build main executeable type `make`.

For run unit tests use `make check`.

Build with CFLAG `-DCLICLIENT_NOWARN` for disable
any warning in cliclient. Also you can disable only
file warning (Using file Tasksfile. Run...) via 
`-DCLICLIENT_NOFILEWARN`.

