# os-project1

## instructions

compile module
```shell
make
```

load to kernel
```shell
sudo insmod driver.ko 
```
![Diagram](./screenshot/loaded-driver.png)

remove from kernel
```shell
sudo rmmod driver
```

to see kernel logs
```shell
sudo dmesg
```
![Diagram](./screenshot/kernel-logs.png)

to clean directory
```shell
make clean
```