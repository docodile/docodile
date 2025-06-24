set -e

echo -ne "Installing Docodile...\r"

sudo cp ./docodile /usr/local/bin
sudo cp -r . /etc/docodile
sudo chmod +x /usr/local/bin/docodile

echo -ne "\e[0;32mDocodile installed!\e[0m        \n"