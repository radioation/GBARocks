import numpy as np
import os,  argparse
from noise import pnoise2
import random
from PIL import Image




# the biome levels
def get_biome(value):
    if value < -0.10:
        return '~'  # Water
    elif value < 0.0:
        return '.'  # Sand
    elif value < 0.20:
        return ','  # Grassland
    elif value < 0.4:
        return '^'  # Hills 
    else:
        return 'M'  # Mountain


def print_world(world):
    for row in world:
        print(''.join(row))

def a8_world_row1( tile ):
    match tile:
        case '~': # Water
            return "2,3,"
        case '.': # Sand
            return "27,28,"
        case ',': # Grassland
            return "187,188,"
        case '^': # Hills
            return "141,140,"
        case 'M': # Mountain
            return "10,11,"

def a8_world_row2( tile ):
    match tile:
        case '~': # Water
            return "2,3,"
        case '.': # Sand
            return "29,30,"
        case ',': # Grassland
            return "189,190,"
        case '^': # Hills
            return "141,140,"
        case 'M': # Mountain
            return "12,13,"     



def gen_atari(world, basename):
    map_cols = len(world[0])
    map_rows = len(world)
    num_bytes = map_cols * map_rows * 4;
    with open( basename + "A8.c", 'w') as ofile:
        ofile.write(f"char map[{num_bytes}] ={{\n")
        for row in world:
            for col in row:
                ofile.write( a8_world_row1( col ) )
            ofile.write("\n")
            for col in row:
                ofile.write( a8_world_row2( col ) )
            ofile.write("\n")
        ofile.write("\n}\n")
        ofile.close();

def gen_gba(world, basename):
    map_cols = len(world[0])
    map_rows = len(world)
    num_bytes = map_cols * map_rows * 4;
    with open( basename + "gba.c", 'w') as ofile:
        ofile.write(f"char map[{num_bytes}] ={{\n")
        for row in world:
            for col in row:
                ofile.write( a8_world_row1( col ) )
            ofile.write("\n")
            for col in row:
                ofile.write( a8_world_row2( col ) )
            ofile.write("\n")
        ofile.write("\n}\n")


def gen_megadrive( world, basename ):
    map_cols = len(world[0])
    map_rows = len(world)
    print(f'map cols: {map_cols} rows: { map_rows } ')
    # hardcding for now
    #   water is tile 1 (2nd)
    #   sand  is tile 2
    #   grass is tile 4
    #   hills is tile 6
    #   mountain is tile 7
    tile_imgs = []
    with Image.open( "path_of_tiles_overworld.png" ) as img:
        width, height = img.size
        # get tiles ( water, sand, grass, hills, mountain )
        for tile in range( 0, int(height / 16 )):
            tmp_img = img.crop(( 0, tile*16, 16, tile*16+16 ))
            tile_imgs.append( tmp_img )
 
        # make overworld image
        pal = img.getpalette()
        out_img = Image.new( mode="P", size=(map_cols*16, map_rows*16))
        out_img.putpalette( pal )

        for y, row in enumerate(world):
            for x, col in enumerate(row):
                dest = ( x*16, y*16 )
                match col:
                    case '~': # Water
                        out_img.paste( tile_imgs[1], dest )
                    case '.': # Sand
                        out_img.paste( tile_imgs[2], dest )
                    case ',': # Grassland
                        out_img.paste( tile_imgs[4], dest )
                    case '^': # Hills
                        out_img.paste( tile_imgs[6], dest )
                    case 'M': # Mountain
                        out_img.paste( tile_imgs[7], dest )
        
        out_img.save( basename + "_gen.png" )


def main( args ):
    print(f"World seed: {args.seed}")

    world = np.zeros((args.height, args.width), dtype=str)
    for y in range(args.height):
        for x in range(args.width):
            nx = x / args.scale
            ny = y / args.scale
            elevation = pnoise2(
                nx,
                ny,
                octaves=args.octaves,
                persistence=args.persistence,
                lacunarity=args.lacunarity,
                #repeatx=args.repeat_x,
                #repeaty=args.repeat_y,
                base=args.seed
            )
            world[y][x] = get_biome(elevation)
    print_world( world )
    #gen_atari( world, args.base_filename )
    #gen_megadrive( world, args.base_filename )
    gen_gba( world, args.base_filename )


if __name__ == '__main__':
    parser = argparse.ArgumentParser( 
        description = "Create worlds for A8 and MegaDrive",
        fromfile_prefix_chars = '@' )

    parser.add_argument( "-w",
        "--width",
        default = 40,
        type=int,
        help = "map width in tiles ",
        metavar = "ARG")

    parser.add_argument( "-H",
        "--height",
        default = 20,
        type=int,
        help = "map height in tiles ",
        metavar = "ARG")

    parser.add_argument( "-s",
        "--scale",
        default = 15,
        type=int,
        help = "Zoom level of terrain (lower noisy, higher smoother)",
        metavar = "ARG")

    parser.add_argument( "-o",
        "--octaves",
        default = 24,
        type=int,
        help = "Number of layers/passes. more passes = more detail",
        metavar = "ARG")

    parser.add_argument( "-p",
        "--persistence",
        default = 0.2,
        type=float,
        help = "How much more each successive pass brings. Higer == more roughness",
        metavar = "ARG")

    parser.add_argument( "-l",
        "--lacunarity",
        default = 2.0,
        type=float,
        help = "Level of detail per pass. Gap between frequences in successive octaves",
        metavar = "ARG")

    #parser.add_argument( "-r",
    #    "--repeat_x",
    #    default = 1024,
    #    type=int,
    #    help = "",
    #    metavar = "ARG")

    #parser.add_argument( "-R",
    #    "--repeat_y",
    #    default = 1024,
    #    type=int,
    #    help = "",
    #    metavar = "ARG")

    parser.add_argument( "-S",
        "--seed",
        default = 1250,
        type=int,
        help = "Zoom level of terrain (lower noisy, higher smoother)",
        metavar = "ARG")

    parser.add_argument( "-b",
        "--base_filename",
        default = "outfile_",
        help = "base name for output files (will auto add A8, C64, MD as needed)",
        metavar = "ARG")


    args = parser.parse_args()
    main(args)
