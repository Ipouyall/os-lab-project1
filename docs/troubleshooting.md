# troubleshooting with gdb
## start debug

first, you need to install gdb. on ubuntu, you can do this with:
```shell
sudo apt-get install gdb
```
then, run:
```shell
make qemu-gdb
```
or
```shell
make qemu-nox-gdb
```
if you want to run in current terminal.
![debug xv6](screenshots/troubleshooting/run-in-debug.png)

now, you can connect to qemu with gdb, in another terminal, run:
```shell
gdb
```
![gdb](screenshots/troubleshooting/gdb.png)

for connecting debugger, you may need to run:
```shell
target remote tcp::26000
```
***
## breakpoints
### to list breakpoints
```shell
maintenance info breakpoints 
```
or
```shell
info breakpoints
```
![list breakpoints](screenshots/troubleshooting/list-bps.png)
### to set breakpoint
add by function name:
```shell
break [function name]
```
add by line number:
```shell
break [file name]:[line number]
```
add by address:
```shell
break *0x[address]
```
![gdb triggered](screenshots/troubleshooting/triggerd-bp.png)
![qemu triggered](screenshots/troubleshooting/triggerd-bp-qemu.png)
### to delete breakpoint
if you want to delete breakpoint by number(you can see it in `info breakpoints`):
```shell
delete [breakpoint number]
```
if you want to delete breakpoint by function name:
```shell
delete [function name]
```
if you want to delete breakpoint by line number:
```shell
delete [file name]:[line number]
```
if you want to delete breakpoint by address:
```shell
delete *0x[address]
```
***
## run
### to run
```shell
continue
```
### to run until breakpoint
```shell
until
```
### to run until function return
```shell
finish
```
***
## step
### to step
```shell
step
```
### to step over
```shell
next
```
***
## to see the source code of current breakpoint:
```shell
list
```
![list](screenshots/troubleshooting/list.png)
***
## print value of a variable
### to print value of variable
```shell
print [variable name]
```
### to print value of variable in hex
```shell
print/x [variable name]
```
### to print value of variable in binary
```shell
print/b [variable name]
```
### to print value of variable in decimal
```shell
print/d [variable name]
```
### to print value of variable in octal
```shell
print/o [variable name]
```
### to print value of variable in string
```shell
print/s [variable name]
```
***
## watch
### to watch variable
```shell
watch [variable name]
```
### to watch variable in hex
```shell
watch/x [variable name]
```
### to watch variable in binary
```shell
watch/b [variable name]
```
### to watch variable in decimal
```shell
watch/d [variable name]
```
### to watch variable in octal
```shell
watch/o [variable name]
```
### to watch variable in string
```shell
watch/s [variable name]
```
***
## backtrace
### to print backtrace
```shell
backtrace
```
or
```shell
bt
```
![backtrace](screenshots/troubleshooting/backtrace.png)
***
## register(s) information
### to print registers
```shell
info registers
```
![registers](screenshots/troubleshooting/info-regs.png)
### to print register
```shell
info register [register name]
```
![register](screenshots/troubleshooting/info-reg.png)
## variable(s)
### to print variables
```shell
info variables
```
![variables](screenshots/troubleshooting/info-vars.png)
![vars](screenshots/troubleshooting/info-vars2.png)
### to print local variables
```shell
info locals
```
![locals](screenshots/troubleshooting/local-vars.png)
## disassemble
### to disassemble
```shell
disassemble
```
or
```shell
disas
```
### to disassemble function
```shell
disassemble [function name]
```
or
```shell
disas [function name]
```
***
## gdb TUI
to open, press ```Ctrl + X + R```.

for example, if we add breakpoint for ```console.c:196```:
![gdb-tui](screenshots/troubleshooting/tui-intro.png)

### for source code
```shell
layout src
```
![src](screenshots/troubleshooting/tui-srcL.png)
### for assembly
```shell
layout asm
```
![asm](screenshots/troubleshooting/tui-asmL.png)
***
## move between frames
![frame stack](screenshots/troubleshooting/tui-checkFrame.png)
### to move to next frame
```shell
up
```
![up](screenshots/troubleshooting/tui-up.png)
### to move to previous frame
```shell
down
```
![down](screenshots/troubleshooting/tui-down.png)
***
