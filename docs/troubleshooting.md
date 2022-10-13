# troubleshooting with gdb

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

then, run:
```shell
target remote localhost:26000
```
