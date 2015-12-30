## README ##


Files:

- run.sh
- bzImage: x86-64 kernel with LKM disabled
- rootfs.ext4
- rootfs-enlarged.ext4
- br_config: buildroot-2015.08.1 config used to generate the rootfs
- README

### linux kernel build config ###

路徑 `linux-2.6.32.60/arch/x86/Kconfig` 更動，原則上修改以下幾點 `make menuconfig` 開啟後，把主要要測試核心程式碼編譯設定即可，可在十分鐘內完成。修改 kernel code 後，下達 `make -j8` 接著 `sh run.sh` 開啟 qemu 測試，若要掛載一些執行檔案進去測試，請先編譯，再使用 `sh mnt.sh` 將執行檔案丟入 `rootfs-ext4`。



```
() Enable loadable module support -->
```

```
File systems
	- [*] The Extended 4 (ext4) filesystem
```

```
Device Drivers --->
	Generic Driver Options -->
		- [*] Create a kernel maintained /dev tmpfs (EXPERIMENTAL)
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