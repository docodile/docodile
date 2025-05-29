set -e

echo -ne "Installing gendoc...\n"
sleep 1
sudo cp ./gendoc /usr/local/bin
sudo cp -r . /etc/gendoc
sudo chmod +x /usr/local/bin/gendoc

echo -ne "\e[0;32mgendoc installed!\e[0m     \n"