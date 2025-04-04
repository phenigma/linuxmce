/* $Id: gears.c,v 1.2 1999/10/21 16:39:06 brianp Exp $ */

/*
 * 3-D gear wheels.  This program is in the public domain.
 *
 * Command line options:
 *    -info      print GL implementation information
 *
 *
 * Brian Paul
 */

/* Conversion to GLUT by Mark J. Kilgard */

/*
 * $Log: gears.c,v $
 * Revision 1.2  1999/10/21 16:39:06  brianp
 * added -info command line option
 *
 * Revision 1.1.1.1  1999/08/19 00:55:40  jtg
 * Imported sources
 *
 * Revision 3.2  1999/06/03 17:07:36  brianp
 * an extra quad was being drawn in front and back faces
 *
 * Revision 3.1  1998/11/03 02:49:10  brianp
 * added fps output
 *
 * Revision 3.0  1998/02/14 18:42:29  brianp
 * initial rev
 *
 */


#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <GL/glut.h>
#include <unistd.h>

#include <SDL.h>
#include <SDL_image.h> 

#include "../ViaOverlay.h"

#ifndef M_PI
#define M_PI 3.14159265
#endif

#define NUM_TEXTURES 200
GLuint textures[NUM_TEXTURES];

GLfloat xrot; /* X Rotation ( NEW ) */
GLfloat yrot; /* Y Rotation ( NEW ) */
GLfloat zrot; /* Z Rotation ( NEW ) */


static GLint T0 = 0;
static GLint Frames = 0;
// Definition--------------------------------------------------------------
#define SCREEN_WIDTH	1024
#define SCREEN_HEIGHT	768

// for Alpha Blending
RECTL rDest;
static unsigned char    *lpAlphaSurface;
static VMI_INFO_PARAM VMI_Info;
GLint x=0,y=0,width=0,height=0;
GLint x_previous=0, y_previous=0, width_previous=0, height_previous=0;

//for Overlay Select
int IsOverlayFlag = 0;

/**

  Draw a gear wheel.  You'll probably want to call this function when
  building a display list since we do a lot of trig here.
 
  Input:  inner_radius - radius of hole at center
          outer_radius - radius at center of teeth
          width - width of gear
          teeth - number of teeth
          tooth_depth - depth of tooth

 **/

static void
gear(GLfloat inner_radius, GLfloat outer_radius, GLfloat width,
  GLint teeth, GLfloat tooth_depth)
{
  GLint i;
  GLfloat r0, r1, r2;
  GLfloat angle, da;
  GLfloat u, v, len;

  r0 = inner_radius;
  r1 = outer_radius - tooth_depth / 2.0;
  r2 = outer_radius + tooth_depth / 2.0;

  da = 2.0 * M_PI / teeth / 4.0;

  glShadeModel(GL_FLAT);

  glNormal3f(0.0, 0.0, 1.0);

  /* draw front face */
  glBegin(GL_QUAD_STRIP);
  for (i = 0; i <= teeth; i++) {
    angle = i * 2.0 * M_PI / teeth;
    glVertex3f(r0 * cos(angle), r0 * sin(angle), width * 0.5);
    glVertex3f(r1 * cos(angle), r1 * sin(angle), width * 0.5);
    if (i < teeth) {
      glVertex3f(r0 * cos(angle), r0 * sin(angle), width * 0.5);
      glVertex3f(r1 * cos(angle + 3 * da), r1 * sin(angle + 3 * da), width * 0.5);
    }
  }
  glEnd();

  /* draw front sides of teeth */
  glBegin(GL_QUADS);
  da = 2.0 * M_PI / teeth / 4.0;
  for (i = 0; i < teeth; i++) {
    angle = i * 2.0 * M_PI / teeth;

    glVertex3f(r1 * cos(angle), r1 * sin(angle), width * 0.5);
    glVertex3f(r2 * cos(angle + da), r2 * sin(angle + da), width * 0.5);
    glVertex3f(r2 * cos(angle + 2 * da), r2 * sin(angle + 2 * da), width * 0.5);
    glVertex3f(r1 * cos(angle + 3 * da), r1 * sin(angle + 3 * da), width * 0.5);
  }
  glEnd();

  glNormal3f(0.0, 0.0, -1.0);

  /* draw back face */
  glBegin(GL_QUAD_STRIP);
  for (i = 0; i <= teeth; i++) {
    angle = i * 2.0 * M_PI / teeth;
    glVertex3f(r1 * cos(angle), r1 * sin(angle), -width * 0.5);
    glVertex3f(r0 * cos(angle), r0 * sin(angle), -width * 0.5);
    if (i < teeth) {
      glVertex3f(r1 * cos(angle + 3 * da), r1 * sin(angle + 3 * da), -width * 0.5);
      glVertex3f(r0 * cos(angle), r0 * sin(angle), -width * 0.5);
    }
  }
  glEnd();

  /* draw back sides of teeth */
  glBegin(GL_QUADS);
  da = 2.0 * M_PI / teeth / 4.0;
  for (i = 0; i < teeth; i++) {
    angle = i * 2.0 * M_PI / teeth;

    glVertex3f(r1 * cos(angle + 3 * da), r1 * sin(angle + 3 * da), -width * 0.5);
    glVertex3f(r2 * cos(angle + 2 * da), r2 * sin(angle + 2 * da), -width * 0.5);
    glVertex3f(r2 * cos(angle + da), r2 * sin(angle + da), -width * 0.5);
    glVertex3f(r1 * cos(angle), r1 * sin(angle), -width * 0.5);
  }
  glEnd();

  /* draw outward faces of teeth */
  glBegin(GL_QUAD_STRIP);
  for (i = 0; i < teeth; i++) {
    angle = i * 2.0 * M_PI / teeth;

    glVertex3f(r1 * cos(angle), r1 * sin(angle), width * 0.5);
    glVertex3f(r1 * cos(angle), r1 * sin(angle), -width * 0.5);
    u = r2 * cos(angle + da) - r1 * cos(angle);
    v = r2 * sin(angle + da) - r1 * sin(angle);
    len = sqrt(u * u + v * v);
    u /= len;
    v /= len;
    glNormal3f(v, -u, 0.0);
    glVertex3f(r2 * cos(angle + da), r2 * sin(angle + da), width * 0.5);
    glVertex3f(r2 * cos(angle + da), r2 * sin(angle + da), -width * 0.5);
    glNormal3f(cos(angle), sin(angle), 0.0);
    glVertex3f(r2 * cos(angle + 2 * da), r2 * sin(angle + 2 * da), width * 0.5);
    glVertex3f(r2 * cos(angle + 2 * da), r2 * sin(angle + 2 * da), -width * 0.5);
    u = r1 * cos(angle + 3 * da) - r2 * cos(angle + 2 * da);
    v = r1 * sin(angle + 3 * da) - r2 * sin(angle + 2 * da);
    glNormal3f(v, -u, 0.0);
    glVertex3f(r1 * cos(angle + 3 * da), r1 * sin(angle + 3 * da), width * 0.5);
    glVertex3f(r1 * cos(angle + 3 * da), r1 * sin(angle + 3 * da), -width * 0.5);
    glNormal3f(cos(angle), sin(angle), 0.0);
  }

  glVertex3f(r1 * cos(0), r1 * sin(0), width * 0.5);
  glVertex3f(r1 * cos(0), r1 * sin(0), -width * 0.5);

  glEnd();

  glShadeModel(GL_SMOOTH);

  /* draw inside radius cylinder */
  glBegin(GL_QUAD_STRIP);
  for (i = 0; i <= teeth; i++) {
    angle = i * 2.0 * M_PI / teeth;
    glNormal3f(-cos(angle), -sin(angle), 0.0);
    glVertex3f(r0 * cos(angle), r0 * sin(angle), -width * 0.5);
    glVertex3f(r0 * cos(angle), r0 * sin(angle), width * 0.5);
  }
  glEnd();

}

void release_textures()
{
  for(int i = 0; i < NUM_TEXTURES; ++i)
	  if(textures[i] != 0)
	  {
		  printf("Releasing texture %d/%d ... \n", i, textures[i]);
		  glDeleteTextures(1, &textures[i]);
		  textures[i] = 0;
	  }
}

static GLfloat view_rotx = 20.0, view_roty = 30.0, view_rotz = 0.0;
static GLint gear1, gear2, gear3;
static GLfloat angle = 0.0;

static void
draw(void)
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glPushMatrix();
  glRotatef(view_rotx, 1.0, 0.0, 0.0);
  glRotatef(view_roty, 0.0, 1.0, 0.0);
  glRotatef(view_rotz, 0.0, 0.0, 1.0);

  glPushMatrix();
  glTranslatef(-3.1, 4.2, 0.0);
  glRotatef(-2.0 * angle - 25.0, 0.0, 0.0, 1.0);
  glCallList(gear3);
  glPopMatrix();

  glPopMatrix();

  glutSwapBuffers();

  usleep(50000);

  Frames++;
 
  {
	  GLint t = glutGet(GLUT_ELAPSED_TIME);
	  if (t - T0 >= 5000) {
		  GLfloat seconds = (t - T0) / 1000.0;
		  GLfloat fps = Frames / seconds;
		  printf("%d frames in %g seconds = %g FPS\n", Frames, seconds, fps);
		  T0 = t;
		  Frames = 0;
     }
  }
}


static void
idle(void)
{
  angle += 2.0;
  glutPostRedisplay();
}

/* change view angle, exit upon ESC */
/* ARGSUSED1 */
static void
key(unsigned char k, int x, int y)
{
  switch (k) {
  case 'z':
    view_rotz += 5.0;
    break;
  case 'Z':
    view_rotz -= 5.0;
    break;
  case 27:  /* Escape */
	release_textures();
    exit(0);
    break;
  default:
    return;
  }
  glutPostRedisplay();
}

/* change view angle */
/* ARGSUSED1 */
static void
special(int k, int x, int y)
{
  switch (k) {
  case GLUT_KEY_UP:
    view_rotx += 5.0;
    break;
  case GLUT_KEY_DOWN:
    view_rotx -= 5.0;
    break;
  case GLUT_KEY_LEFT:
    view_roty += 5.0;
    break;
  case GLUT_KEY_RIGHT:
    view_roty -= 5.0;
    break;
  default:
    return;
  }
  glutPostRedisplay();
}

/* new window size or exposure */
static void
reshape(int width, int height)
{
  GLfloat h = (GLfloat) height / (GLfloat) width;

  glViewport(0, 0, (GLint) width, (GLint) height);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glFrustum(-1.0, 1.0, -h, h, 5.0, 60.0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glTranslatef(0.0, 0.0, -40.0);
}

int MinPowerOf2(int Value)
{
	int Result = 1;
	while(Result<Value)
		Result*= 2;
	return Result;	
}

/* function to load in bitmap as a GL texture */
int LoadGLTextures(int i, char * pTextureName)
{
	/* Create storage space for the texture */
	SDL_Surface *pTextureImage; 

	/* Load The Bitmap, Check For Errors, If Bitmap's Not Found Quit */
	if ( ( pTextureImage = IMG_Load(pTextureName)))
	{
		int Width = MinPowerOf2(pTextureImage->w);
		int Height = MinPowerOf2(pTextureImage->h);

		if(Width != pTextureImage->w || Height != pTextureImage->h)
		{
			Uint32 saved_flags = pTextureImage->flags&(SDL_SRCALPHA|SDL_RLEACCELOK);
			Uint8  saved_alpha = pTextureImage->format->alpha;

			if ((saved_flags & SDL_SRCALPHA) == SDL_SRCALPHA) 
				SDL_SetAlpha(pTextureImage, 0, 0);

			SDL_Surface *LocalSurface = SDL_CreateRGBSurface(SDL_SWSURFACE, Width, Height, 
				pTextureImage->format->BitsPerPixel, 
				pTextureImage->format->Rmask, pTextureImage->format->Gmask, pTextureImage->format->Bmask,
				pTextureImage->format->Amask);

			SDL_BlitSurface(pTextureImage, NULL, LocalSurface, NULL);

			if ((saved_flags & SDL_SRCALPHA) == SDL_SRCALPHA) 
				SDL_SetAlpha(LocalSurface, saved_flags, saved_alpha);

			SDL_FreeSurface(pTextureImage); 
			pTextureImage = LocalSurface;
		}

		/* Create The Texture */
		glGenTextures( 1, &textures[i] );

		/* Typical Texture Generation Using Data From The Bitmap */
		glBindTexture( GL_TEXTURE_2D, textures[i] );

		static int vmem = 0;
		vmem += Width * Height * 4;
		printf("Loading texture %d/%d size %dx%d...   Video memory used %dMB \n", i, textures[i], Width, Height, vmem / 1024 / 1024);

		if(pTextureImage->format->BytesPerPixel == 4)
		{
			/* Generate The Texture */
			glTexImage2D( GL_TEXTURE_2D, 0, 4, pTextureImage->w,
				pTextureImage->h, 0, GL_RGBA,
				GL_UNSIGNED_BYTE, pTextureImage->pixels );
		}
		else
		{
			/* Generate The Texture */
			glTexImage2D( GL_TEXTURE_2D, 0, 3, pTextureImage->w,
				pTextureImage->h, 0, GL_RGB,
				GL_UNSIGNED_BYTE, pTextureImage->pixels );
		}

		/* Linear Filtering */
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	}
	else
		return false;

	/* Free up any memory we may have used */
	if ( pTextureImage )
		SDL_FreeSurface(pTextureImage);

	return true;
}

static void
init(int argc, char *argv[])
{
  static GLfloat pos[4] =
  {5.0, 5.0, 10.0, 0.0};
  static GLfloat red[4] =
  {0.8, 0.1, 0.0, 1.0};
  static GLfloat green[4] =
  {0.0, 0.8, 0.2, 1.0};
  static GLfloat blue[4] =
  {0.2, 0.2, 1.0, 1.0};

//////////////

	/* Enable Texture Mapping ( NEW ) */
	glEnable( GL_TEXTURE_2D );

	/* Enable smooth shading */
	glShadeModel( GL_SMOOTH );

	/* Set the background black */
	glClearColor( 0.0f, 0.0f, 0.0f, 0.5f );

	/* Depth buffer setup */
	glClearDepth( 1.0f );

	/* Enables Depth Testing */
	glEnable( GL_DEPTH_TEST );

	/* The Type Of Depth Test To Do */
	glDepthFunc( GL_LEQUAL );

	/* Really Nice Perspective Calculations */
	glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );

//////////////

  glLightfv(GL_LIGHT0, GL_POSITION, pos);
  glEnable(GL_CULL_FACE);
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glEnable(GL_DEPTH_TEST);

  /* make the gears */
  gear1 = glGenLists(1);
  glNewList(gear1, GL_COMPILE);
  glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, red);
  gear(1.0, 4.0, 1.0, 20, 0.7);
  glEndList();

  gear2 = glGenLists(1);
  glNewList(gear2, GL_COMPILE);
  glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, green);
  gear(0.5, 2.0, 2.0, 10, 0.7);
  glEndList();

  gear3 = glGenLists(1);
  glNewList(gear3, GL_COMPILE);
  glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, blue);
  gear(1.3, 2.0, 0.5, 10, 0.7);
  glEndList();

  glEnable(GL_NORMALIZE);

  if (argc > 1 && strcmp(argv[1], "-info")==0) {
     printf("GL_RENDERER   = %s\n", (char *) glGetString(GL_RENDERER));
     printf("GL_VERSION    = %s\n", (char *) glGetString(GL_VERSION));
     printf("GL_VENDOR     = %s\n", (char *) glGetString(GL_VENDOR));
     printf("GL_EXTENSIONS = %s\n", (char *) glGetString(GL_EXTENSIONS));
  }

  char *pTextureName = "logo.png";
  for(int i = 0; i < NUM_TEXTURES; ++i)
  {
	LoadGLTextures(i, pTextureName);

	if(i >= 100 && (!(i % 10)))
	{
		printf("\n\n=================== Loaded so far %d textures\n", i);
		usleep(100 * 1000);
	}
  }
}

void 
visible(int vis)
{
  if (vis == GLUT_VISIBLE)
    glutIdleFunc(idle);
  else
    glutIdleFunc(NULL);
}


int main(int argc, char *argv[])
{
  int dwRet = 0;

  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);

  //william
  dwRet = setenv("VIA_3D_OVERLAY","no",1);
  if(dwRet !=0)
	  printf("set env error!\n");
  IsOverlayFlag = 0;
  
  glutInitWindowPosition(0, 0);
  glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
  glutCreateWindow("Gears");
  init(argc, argv);

  glutDisplayFunc(draw);
  glutReshapeFunc(reshape);
  glutKeyboardFunc(key);
  glutSpecialFunc(special);
  glutVisibilityFunc(visible);

  ViaOverlay::Instance().WindowCreated(SCREEN_WIDTH, SCREEN_HEIGHT);
  ViaOverlay::Instance().WindowResized();
  ViaOverlay::Instance().ResetAlphaMask();
  
  ViaOverlay::Instance().FillRectangleInAlphaMask(0, 0, SCREEN_WIDTH / 3, SCREEN_HEIGHT, (char)0x20);
  ViaOverlay::Instance().FillRectangleInAlphaMask(SCREEN_WIDTH / 3, 0, SCREEN_WIDTH / 3, SCREEN_HEIGHT, (char)0x80);
  ViaOverlay::Instance().FillRectangleInAlphaMask(2 * SCREEN_WIDTH / 3, 0, SCREEN_WIDTH / 3, SCREEN_HEIGHT, (char)0xD0);
  ViaOverlay::Instance().WorldChanged();

  glutMainLoop();

  release_textures();

  return 0;             /* ANSI C requires main to return int. */
} 
