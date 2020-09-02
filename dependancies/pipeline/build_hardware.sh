clear
echo compiling easylogging ...
g++ ../dependancies/easyloggingpp/src/easylogging++.cc --shared -fPIC -o easylogging.so
echo finished
echo compiling hardware ...
g++ -I ../dependancies --std=c++14 hardware.cpp ./easylogging.so -g3 -O0 -o hardware
echo finished
echo running hardware ...
./hardware
echo finished
