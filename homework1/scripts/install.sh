sudo apt-get install fakeroot build-essential kernel-package libncurses5 libncurses5-dev
cd /usr/src
sudo wget https://cdn.kernel.org/pub/linux/kernel/v2.6/longterm/v2.6.32/linux-2.6.32.68.tar.xz
sudo tar xvf linux-2.6.32.68.tar.xz
cd linux-2.6.32.68
sudo make mrproper
sudo make menuconfig
sudo make bzImage -j8
sudo make modules
sudo make modules_install
sudo make install
sudo vim /etc/default/grub
sudo update-grub2
sudo shutdown -r now
