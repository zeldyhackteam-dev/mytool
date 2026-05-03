kodlar: pkg update -y && pkg upgrade -y
pkg install git python -y
pip install requests colorama

cd ~
git clone https://github.com/zeldyhackteam-dev/mytool
cd mytool
python tool.py
