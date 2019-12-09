# epaper 

## epaper is a REST sever for e-paper on raspberry pi
epaper will get the host IP and listen on port 6502.

To control it in web page. use the index.html.    
The web project is under: https://github.com/maxcong001/epaper-web     
or you can use the image:
cv0031178/epaper_web

docker run -ti --net host cv0031178/epaper_web
source /etc/apache2/envvars
/usr/sbin/apache2


you can get the build image by : docker pull cv0031178/epaper    
you can download bcm2835 from:         
http://www.airspayce.com/mikem/bcm2835/bcm2835-1.60.tar.gz    



if you use build image for test, you can run 
docker run -ti --privileged --net host cv0031178/epaper:release

note: 
1. you must run the container in the privileged mode and open port 6502 or use host network
2. you need to plugin the epaper, the binary will check if there is epaper.
3. please set /etc/hosts accordingly. for example:    yourip raspberry 
