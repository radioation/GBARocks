#!/usr/bin/env python

import os,    argparse
import numpy as np
import math
from PIL import Image
import shutil
from pathlib import Path


def main(args):
        

    imageFilename = args.input_filename
    outputFilename = args.output_filename


    with Image.open( imageFilename ) as im:
        #inputImg = im.convert('RGB')
        imageWidthPixels, imageHeightPixels = im.size
        px = im.load()
        print( im.mode )
        print( im.getcolors() )

        # check width and height are multiples of 8
        if imageWidthPixels % 8 != 0 or    imageHeightPixels % 8 != 0 :
            print("Image width and height must be multiples of 8.")
            return

        tileCols = int(imageWidthPixels / 8 )
        tileRows = int(imageHeightPixels / 8 )

        print(f'tiles x: {tileCols} y: {tileRows} ')

        pal = im.getpalette()
        print(f'palette length: {len(pal)/3}') # 24 bpp out of ProMotionNG
        print("----")
        for c in range(0, int(len(pal)/3)):
            val = (pal[c*3]<<16) + (pal[c*3+1]<< 8) + (pal[c*3+2] )
            print(f'SPRITE_PALETTE[{c}] = RGB8( {pal[c*3]}, {pal[c*3+1]}, {pal[c*3+2]} );')


        print("\n----")

        for tile_y in range( 0, tileRows ):
            for tile_x in range( 0, tileCols ):
                for y in range(0, 8): 
                    print('0x', end = '' )
                    for x in range(3,-1,-1): 
                        print( f'{px[ x + tile_x * 8, y + tile_y * 8]:x}', end = "" )
                    print(', 0x', end = '' )
                    for x in range(7,3,-1): 
                        print( f'{px[ x + tile_x * 8, y + tile_y * 8]:x}', end = "" )
                       
                    print(", ", end= "")
                print("")

# the program.
if __name__ == '__main__':
    parser = argparse.ArgumentParser( 
            description = "inspect indexed png images from ProMotion NG to C",
            epilog = "As an alternative to the commandline, params can be placed in a file, one per line, and specified on the commandline like '%(prog)s @params.conf'.",
            fromfile_prefix_chars = '@' )

    # parameter list
    parser.add_argument( "-i",
            "--input_filename",
            default = 'image.png',
            help = "input image filename",
            metavar = "ARG")


    parser.add_argument( "-o",
            "--output_filename",
            default = 'out.h',
            help = "Output filename",
            metavar = "ARG")


    args = parser.parse_args()


    main(args)
