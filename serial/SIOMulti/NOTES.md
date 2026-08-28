# Notas on Multimode

from [here](https://problemkaputt.de/gbatek-sio-multi-player-mode.htm)

* Use a GBA link cable.

* `SIODATA_SEND` appears to be `SIOMLT_SEND` ( or `REG_SIOMLT_SEND` in `gba_sio.h` )

* cable order matters.  w/ just 2 GBA's I had to use p1 and p2 
  for it to work. Which is a result of 

    "- Transfer ends if next child does not output data after certain time."

  fF you connect p1 and p3 or p4, p2 will time out and transfer ends.




