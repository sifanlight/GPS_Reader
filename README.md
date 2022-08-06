## GPS Reader
This code reads and encodes the GPS data and send it to the server.</br>
To build the program simply run:
```
$> cmake .
$> make
```
And run the ```GPSReader``` program.</br>
To change the address for the server go to the ```include/datatypes.h``` and change:
```
#define PORT_NUM 11222
#define SERVER_IP_ADDR "127.0.0.1"
```
