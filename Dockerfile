from arm32v7/debian AS builder

RUN apt update&&apt install -y libcpprest-dev build-essential libboost-log-dev cmake git wget
RUN mkdir /epaper&&cd /epaper&&wget http://www.airspayce.com/mikem/bcm2835/bcm2835-1.60.tar.gz&&tar -zxvf bcm2835-1.60.tar.gz&&cd bcm2835-1.60&&./configure&&make&&make install&&cd ..&&rm -rf *
RUN cd /epaper&&git clone https://github.com/maxcong001/epaper.git&&cd epaper&&mkdir build&&cd build&&cmake ..&&make


#FROM arm32v7/alpine
FROM arm32v7/debian
COPY --from=builder /epaper/epaper/include/epaper/HZK* /
COPY --from=builder /epaper/epaper/build/bin/epaper_example /epaper_example
COPY --from=builder /lib/arm-linux-gnueabihf/libpthread.so.0 /lib/arm-linux-gnueabihf/libpthread.so.0
COPY --from=builder /usr/lib/arm-linux-gnueabihf/libboost_log.so.1.67.0 /usr/lib/arm-linux-gnueabihf/libboost_log.so.1.67.0
COPY --from=builder /usr/lib/arm-linux-gnueabihf/libboost_log_setup.so.1.67.0 /usr/lib/arm-linux-gnueabihf/libboost_log_setup.so.1.67.0
COPY --from=builder /usr/lib/arm-linux-gnueabihf/libboost_system.so.1.67.0 /usr/lib/arm-linux-gnueabihf/libboost_system.so.1.67.0
COPY --from=builder /usr/lib/arm-linux-gnueabihf/libboost_thread.so.1.67.0 /usr/lib/arm-linux-gnueabihf/libboost_thread.so.1.67.0
COPY --from=builder /usr/lib/arm-linux-gnueabihf/libcpprest.so.2.10 /usr/lib/arm-linux-gnueabihf/libcpprest.so.2.10
COPY --from=builder /usr/lib/arm-linux-gnueabihf/libcrypto.so.1.1 /usr/lib/arm-linux-gnueabihf/libcrypto.so.1.1
COPY --from=builder /usr/lib/arm-linux-gnueabihf/libssl.so.1.1 /usr/lib/arm-linux-gnueabihf/libssl.so.1.1
COPY --from=builder /usr/lib/arm-linux-gnueabihf/libstdc++.so.6 /usr/lib/arm-linux-gnueabihf/libstdc++.so.6
COPY --from=builder /lib/arm-linux-gnueabihf/libm.so.6 /lib/arm-linux-gnueabihf/libm.so.6
COPY --from=builder /lib/arm-linux-gnueabihf/libgcc_s.so.1 /lib/arm-linux-gnueabihf/libgcc_s.so.1
COPY --from=builder /lib/arm-linux-gnueabihf/libc.so.6 /lib/arm-linux-gnueabihf/libc.so.6 
COPY --from=builder /usr/lib/arm-linux-gnueabihf/libboost_filesystem.so.1.67.0 /usr/lib/arm-linux-gnueabihf/libboost_filesystem.so.1.67.0
COPY --from=builder /usr/lib/arm-linux-gnueabihf/libboost_regex.so.1.67.0 /usr/lib/arm-linux-gnueabihf/libboost_regex.so.1.67.0
COPY --from=builder /usr/lib/arm-linux-gnueabihf/libboost_chrono.so.1.67.0 /usr/lib/arm-linux-gnueabihf/libboost_chrono.so.1.67.0
COPY --from=builder /usr/lib/arm-linux-gnueabihf/libboost_atomic.so.1.67.0 /usr/lib/arm-linux-gnueabihf/libboost_atomic.so.1.67.0
COPY --from=builder /usr/lib/arm-linux-gnueabihf/libboost_date_time.so.1.67.0 /usr/lib/arm-linux-gnueabihf/libboost_date_time.so.1.67.0
COPY --from=builder /lib/arm-linux-gnueabihf/librt.so.1 /lib/arm-linux-gnueabihf/librt.so.1
COPY --from=builder /usr/lib/arm-linux-gnueabihf/libicudata.so.63 /usr/lib/arm-linux-gnueabihf/libicudata.so.63
COPY --from=builder /usr/lib/arm-linux-gnueabihf/libicui18n.so.63 /usr/lib/arm-linux-gnueabihf/libicui18n.so.63 
COPY --from=builder /usr/lib/arm-linux-gnueabihf/libicuuc.so.63 /usr/lib/arm-linux-gnueabihf/libicuuc.so.63
COPY --from=builder /usr/lib/arm-linux-gnueabihf/libbrotlidec.so.1 /usr/lib/arm-linux-gnueabihf/libbrotlidec.so.1
COPY --from=builder /usr/lib/arm-linux-gnueabihf/libbrotlienc.so.1 /usr/lib/arm-linux-gnueabihf/libbrotlienc.so.1
COPY --from=builder /usr/lib/arm-linux-gnueabihf/libboost_random.so.1.67.0 /usr/lib/arm-linux-gnueabihf/libboost_random.so.1.67.0
COPY --from=builder /lib/arm-linux-gnueabihf/libz.so.1 /lib/arm-linux-gnueabihf/libz.so.1
COPY --from=builder /lib/arm-linux-gnueabihf/libdl.so.2 /lib/arm-linux-gnueabihf/libdl.so.2
COPY --from=builder /usr/lib/arm-linux-gnueabihf/libbrotlicommon.so.1 /usr/lib/arm-linux-gnueabihf/libbrotlicommon.so.1

ENTRYPOINT ["/epaper_example"]