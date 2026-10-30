
#ifndef MYGRAPHICS_H
#define MYGRAPHICS_H

#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>
#import <OpenGLES/ES2/gl.h>
#import <OpenGLES/ES2/glext.h>


typedef struct vertex2d{
	GLfloat point[2];  
} Vertex2d, Vector2d;

typedef struct vertex2d_color{
	GLfloat point[2]; 
	GLfloat color[4];
} Vertex2dC;

typedef struct vertex2d_texcoord{
	GLfloat point[2]; 
	GLfloat texcoord[4];
} Vertex2dT;


typedef struct vertex3d_{
	GLfloat point[3];
} Vertex3d, Vector3d;

typedef struct vertex3d_normal{
	GLfloat point[3];
	GLfloat norm[3]; 
} Vertex3dN;

typedef struct vertex3d_normal_color{
	GLfloat point[3];
	GLfloat norm[3];
	GLfloat color[4];
} Vertex3dNC;

typedef struct vertex3d_normal_colorbytes{
	GLfloat point[3];
	GLfloat norm[3];
	GLbyte color[4];
} Vertex3dNCb;

typedef struct vertex3d_normal_color_idbyte{
	GLfloat point[3];
	GLfloat norm[3];
	GLfloat color[4];
	GLubyte id;
} Vertex3dNCIb;

typedef struct vertex3d_color{
	GLfloat point[3];
	GLfloat color[4]; 
} Vertex3dC;

typedef struct vertex3d_texcoord{
	GLfloat point[3];
	GLfloat texcoord[2]; 
} Vertex3dT;

typedef struct vertex3d_normal_texcoord{
	GLfloat point[3];
	GLfloat norm[3];
	GLfloat texcoord[2]; 
} Vertex3dNT;

typedef struct vertex3d_normal_color_texcoord{
	GLfloat point[3];
	GLfloat norm[3];
	GLfloat color[4];
	GLfloat texcoord[2]; 
} Vertex3dNCT;


 



float dot3f( float *a, float *b );
void cross3f( float *a, float* b, float *retvect );

void mat4fXmat4f( float *a, float *b, float *retmat );
																							

void transposeMat4f( float* a, float *retmat );


#endif
