## Install ##

在這次作業提供的 kernel 資料夾下，覆蓋 `.config` 即可，接著

```
mkdir mnt
```

作為由 buildrootfs-2015.11.1 建造出來的 rootfs 的 mount 資料夾 (搬移測試執行檔用途)，請參考 scripts 部份。

> 特別注意：若 qemu 使用中，相對的 rootfs 不應該再次被 mount，它們對同一份檔案讀寫會造成下次 mount 被 lock 住，唯一的解決方案是重新建造 rootfs (請查閱 buildrootfs-2015.11.1 的操作)，再把 mount 資料全數移除。養成習慣在 qemu 下達 `# poweroff` 後，在進行 mount 指令。