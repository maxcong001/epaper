from debian AS builder

RUN apt update&&apt install libcpprest-dev build-essential libboost-log-dev cmake git wget\
&&wget http://www.airspayce.com/mikem/bcm2835/bcm2835-1.60.tar.gz&&tar -zxvf bcm2835-1.60.tar.gz&&cd bcm2835-1.60&&./configure&&make&&make install&&cd ..&&rm -rf\ 
&&git clone https://github.com/maxcong001/epaper.git&&cd epaper&&mkdir build&&cd build&&cmake ..&&make

