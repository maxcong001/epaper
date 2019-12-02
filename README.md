# epaper 

## epaper is a REST sever for e-paper on raspberry pi
epaper will get the host IP and listen on port 6502.
note: you can change the host ip in the /etc/hosts
To control it in web page. use the index.html.    
The web project is under: https://github.com/maxcong001/epaper-web     
you can get the build image by : docker pull cv0031178/epaper    
you can download bcm2835 from:         
http://www.airspayce.com/mikem/bcm2835/bcm2835-1.60.tar.gz    

note: please set /etc/hosts accordingly
for example:    
yourip raspberry    

if you use build image for test, you can run 
docker run -ti  --privileged --net host -p 6502:6502 b3561d3ee68f bash    


note: you must run the container in the privileged mode and open port 6502 

