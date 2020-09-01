clear
echo compiling easylogging ...
g++ ../dependancies/easyloggingpp/src/easylogging++.cc --shared -fPIC -o easylogging.so
echo finished
echo compiling pipeline ...
g++ -I ../dependancies --std=c++14 pipeline_v3.cpp -lpthread ./easylogging.so -g0 -O3 -o pipeline
echo finished
echo running pipeline ...
./pipeline
echo finished
