
//{{BLOCK(title)

//======================================================================
//
//	title, 256x256@4, 
//	+ palette 16 entries, not compressed
//	+ 109 tiles (t|f|p reduced) not compressed
//	+ regular map (in SBBs), not compressed, 32x32 
//	Total size: 32 + 3488 + 2048 = 5568
//
//	Time-stamp: 2026-08-21, 18:38:37
//	Exported by Cearn's GBA Image Transmogrifier, v0.9.2
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT_TITLE_H
#define GRIT_TITLE_H

#define titleTilesLen 3488
extern const unsigned int titleTiles[872];

#define titleMapLen 2048
extern const unsigned short titleMap[1024];

#define titlePalLen 32
extern const unsigned short titlePal[16];

#endif // GRIT_TITLE_H

//}}BLOCK(title)
