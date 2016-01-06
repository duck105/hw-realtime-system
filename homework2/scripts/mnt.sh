sudo mount rootfs-dirty.ext4 mnt
sudo cp test_weighted_rr/test_weighted_rr mnt/root/
sudo cp test_weighted_rr/test_sjf mnt/root/
sudo cp test_weighted_rr/test_rms mnt/root/
sudo cp test_weighted_rr/run.sh mnt/root/
sudo cp test_weighted_rr/run_sjf.sh mnt/root/
sudo cp test_weighted_rr/run_rms.sh mnt/root/
sudo umount mnt

