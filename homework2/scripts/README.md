## README ##


### Ubuntu Environment ###

```
$ sudo apt-get install qemu-system
$ sudo apt-get install libncurses5-dev
```

### Buildroot ###

[site](https://buildroot.uclibc.org/download.html)

```
$ wget https://buildroot.uclibc.org/downloads/buildroot-2015.11.1.tar.bz2
$ bzip2 -d FileName.bz2
$ tar xvf FileName.tar
```

#### Compile Config ####

```
$ make menuconfig
```

這次作業設定，由於需要改寫 kernel 版本為 2.6.32.* 左右，因此相關的 gcc 編譯器等設定如下。同時，根據機器架構設定，助教預設在 visual box 上面，通常為 i386，而我們用實驗室 server，編 x86 64。

```
Target options  --->
	Target Architecture (x86_64)  --->
		x86_64

Toolchain  --->
	C library (glibc)  --->
		glibc
	Kernel Headers (Manually specified Linux version)  --->
		custom 2.6.x
		2.6.32.6

System configuration  --->
	[*] Enable root login with password
	[*] remount root filesystem read-write during boot 

Filesystem images  ---> 
	[*] ext2/3/4 root filesystem
	[*] tar the root filesystem
```

#### Compile ####

`make -j8` 用 8 個執行緒編譯，如果機器有很多個 core，那麼可以用 `make -j20` 在非常短的時間內完成。

接著在 `buildroot`

```
$ make -j8
$ cp output/image/rootfs.ext* WORKSPACE_PATH
```

### Linux kernel config  ###

```
$ make defconfig
$ make menuconfig
```

```
[_] Enable loadable module support

File systems  ---> 
	[*] Second extended fs support
		[*] ext2

Device Drivers  --->
	Generic Driver Options  ---> 
		[*] Create a kernel maintained /dev tmpfs (EXPERIMENTAL) 
```

create `arch/x86/boot/bzImage`

```
$ make -j20
```

### Test Flow ###

#### move test execute file ####

`mnt.sh`

```
sudo mount rootfs.ext2 mnt
sudo cp test_weighted_rr/test_weighted_rr mnt/root/
sudo umount mnt
```

`run.sh`
```
qemu-system-x86_64 -M pc -kernel arch/x86/boot/bzImage -hda rootfs-dirty.ext4 -netdev user,id=network0 -device e1000,netdev=network0 -nographic -append "root=/dev/sda console=ttyS0"
```

### How to enlarge the rootfs.ext4 ###

http://stackoverflow.com/questions/3495854/extend-the-size-of-already-existing-ext2-image

```
$ dd if=/dev/zero of=foo.img count=0 bs=1M seek=2000   # assuming target size is 2000M
# e2fsck -f foo.img
# resize2fs foo.img 
```

### Screen Workspace ###

[commond tutorial](http://ssorc.tw/19)

執行

```
$ screen
```

* ctrl+a c  
開啟新的視窗，並同時切換到這個新的視窗
* ctrl+a tab  
切換視窗
* ctrl+a |  
切割垂直視窗，並產生新的視窗。
* ctrl+a k  
關閉此視窗


### Speical Thanks ###

* 蕭光宏‧蕭大帥
* Lab 332 Parallel & Distributed Laboratory

## Virtual Box ##

### 第一次運行 ###

```
$ cd /usr/src
$ sudo wget http://newslab.csie.ntu.edu.tw/course/rts2015/projects/linux-2.6.32.60.tar.gz
$ sudo tar -zxvf linux-2.6.32.60.tar.gz
$ cd linux-2.6.32.60
$ sudo make mrproper
$ sudo make menuconfig
```

```
$ scp morris1028@140.112.30.245:~/RTS/linux-2.6.32.60/arch/x86/include/asm/unistd_64.h arch/x86/include/asm/
$ scp morris1028@140.112.30.245:~/RTS/linux-2.6.32.60/kernel/sched_weighted_rr.c kernel/
```

```
$ sudo make bzImage
$ sudo make modules
$ sudo make modules_install
$ sudo make install
```

```
$ sudo vim /etc/default/grub
#Add "#" to comment the following 2 lines
#GRUB_HIDDEN_TIMEOUT=10
#GRUB_HIDDEN_TIMEOUT_QUIET=true
$ sudoupdate-grub2
$ sudoshutdown -r now
```

### Test flow ###

```
$ cd /usr/src/linux-2.6.32.60
$ sudo make bzImage
$ sudo make modules
$ sudo make modules_install
$ sudo make install
```

### Debug by `printk` ###

在另一個 terminal 開啟以下代碼，並且等待執行測試 scheduler 得執行檔在 kernel 運行時所打印出的 `printk` 訊息。

```
$ sudo more /proc/kmsg
```

### 已知問題 ###

測試代碼會在 `sched_setscheduler()` 沒有回應，但在 qemu 版本測試下沒有問題。