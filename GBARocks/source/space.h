
//{{BLOCK(space)

//======================================================================
//
//	space, 512x512@4, 
//	+ palette 16 entries, not compressed
//	+ 109 tiles (t|f|p reduced) not compressed
//	+ regular map (in SBBs), not compressed, 64x64 
//	Total size: 32 + 3488 + 8192 = 11712
//
//	Time-stamp: 2026-08-16, 11:28:51
//	Exported by Cearn's GBA Image Transmogrifier, v0.9.2
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT_SPACE_H
#define GRIT_SPACE_H

#define spaceTilesLen 3488
extern const unsigned int spaceTiles[872];

#define spaceMapLen 8192
extern const unsigned short spaceMap[4096];

#define spacePalLen 32
extern const unsigned short spacePal[16];

#endif // GRIT_SPACE_H

//}}BLOCK(space)
