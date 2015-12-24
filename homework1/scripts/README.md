## README ##

按照 `install.sh` 指令依序執行，安裝 linux kernel 到開機畫面進行選取。

當執行下述指令時，

```
sudo vim /etc/default/grub
```

注解掉以下兩行

```
#GRUB_HIDDEN_TIMEOUT=10
#GRUB_HIDDEN_TIMEOUT_QUIET=true
```

此設定用來決定 GRUB 2 選單會有多長時間不被顯示。當螢幕是空白時，使用者可以按下任何鍵以顯示選單。