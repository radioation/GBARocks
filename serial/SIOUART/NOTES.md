# Notas on UART 
* from [here](http://problemkaputt.de/gbatek-sio-uart-mode.htm).
  This particular page feels a bit sparse compared to the Normal 
  and Multiplayer modes, but still useable.

* As before, `RCNT` is just 0

* `SIOCNT` is a bit more complex. You have serveral choices to make
  for Baud Rate, flow control, parity, FIFO, Data length (bit)

  Based on some googling I'm going with:
  * Data Bits: 8
  * Parity: None 0
  * Stop Bits: not configuratble?
  * baud rate: 115200

  I'll experiment with FIFO and flow control at some other date.


* stujj's [gba-serial-adventures](https://github.com/stuij/gba-serial-adventures) 
  lists pins to connect to a FDTI TTL-232R-3V3  cable.


The ROM will just send out `GBA ROCKS\r\n` byte-by-byte through the 
UART. After that it'll just echo back any bytes it reads from the UART.

Works with PuTTY on windows.
1. Select "Connection type:" : `Serial`
2. Set "Serial Line" to your COM port (in my case COM3)
3. Set "Speed" to `115200`





