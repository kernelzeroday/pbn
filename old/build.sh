cd libs/libsodium-1.0.18/
bash autogen.sh
./configure
make 
sudo make install
cd ../c-toxcore
bash autogen.sh
./configure
make
sudo make install
cd ../..
make

