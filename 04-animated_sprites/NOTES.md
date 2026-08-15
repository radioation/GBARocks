# Notas
Not much to see here. OAM.attr2 is used to set the frame. Since I'm using 16 color 
palettes, I'm protecting the higher bits

```c
    if (rockIndex > 544) rockIndex = 516;
           for( int i=1; i <= 8; ++i ){
                   OAM_MEM[i].attr2 &= 0xfc00;
                   OAM_MEM[i].attr2 |=  OBJ_CHAR(rockIndex);  << NEW FRAME

```
