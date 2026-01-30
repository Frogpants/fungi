rm -rf build
mkdir build

sudo apt update
sudo apt install libx11-dev libxrandr-dev libxinerama-dev libxcursor-dev libxxf86vm-dev libgl1-mesa-dev

cd build

cmake ..