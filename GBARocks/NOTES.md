```bash 
 grit ship.png -gt -gB4 -Mw2 -Mh2 -pn16 -ftc
 grit ufo.png -gt -gB4 -Mw2 -Mh2 -pn16 -ftc
 grit boom.png -gt -gB4 -Mw4 -Mh4 -pn16 -ftc
 grit shot.png -gt -gB4 -Mw1 -Mh1 -pn16 -ftc
 grit space.png  -gB4 -mR4 -mLs -pn16 -ftc
 grit gb_rock.png -gt -gB4 -Mw4 -Mh4 -pn16 -ftc
 grit gb_mid_rock.png -gt -gB4 -Mw2 -Mh2 -pn16 -ftc
 grit gb_small_rock.png -gt -gB4 -Mw1 -Mh1 -pn16 -ftc
 grit title.png  -gB4 -mR4 -mLs -pn16 -ftc -mp 1
```

*IMP* : the MAP itself uses the upper 4 bits to select the palette 
    for a tile in 16-color mode. So for two 4-bit backgrounds using 
    diffeernt palets use `-mp` followed by the palette number 
   to specify at compile time.
```bash
 grit title.png  -gB4 -mR4 -mLs -pn16 -ftc -mp 1
```



