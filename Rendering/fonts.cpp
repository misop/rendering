// fonts.cpp
// FUNKCIE PRE INICIALIZACIU A VYKRESLOVANIE TEXTOV

#include "fonts.h"

// Standardne pre MS win aplikacie
#include <windows.h>

// GLEW kniznica. pre spravu novsich rozsireni
#include "glew.h"
// GL Utility Toolkit (GLUT), v ramci toho sa includuje aj openGL hlavickovy subor
#include "glut.h"

// kniznica pre TrueType fonty
#include <ft2build.h>
#include <freetype/freetype.h>
#include <freetype/ftglyph.h>
#include <freetype/ftoutln.h>
#include <freetype/fttrigon.h>

#include <math.h>

// id display listov pre znaky
GLuint g_2DtextWGL = 0, g_3DtextWGL = 0;


// vytvorenie 2D znakov do display listov pomocou WGL
void init2DFontWGL(const char* font_name)
{
	g_2DtextWGL = glGenLists(96);
	HFONT font = CreateFont(-24,							// Height Of Font
						0,								// Width Of Font
						0,								// Angle Of Escapement
						0,								// Orientation Angle
						FW_BOLD,						// Font Weight
						FALSE,							// Italic
						FALSE,							// Underline
						FALSE,							// Strikeout
						ANSI_CHARSET,					// Character Set Identifier
						OUT_TT_PRECIS,					// Output Precision
						CLIP_DEFAULT_PRECIS,			// Clipping Precision
						ANTIALIASED_QUALITY,			// Output Quality
						FF_DONTCARE|DEFAULT_PITCH,		// Family And Pitch
						font_name);					// Font Name
    HDC hDC = wglGetCurrentDC();
	HFONT oldfont = (HFONT)SelectObject(hDC, font);
	BOOL result = wglUseFontBitmaps(hDC, 32, 96, g_2DtextWGL);
    result = wglUseFontBitmaps(hDC, 32, 96, g_2DtextWGL);
	SelectObject(hDC, oldfont);
	DeleteObject(font);
}

// vytvorenie 3D znakov do display listov pomocou WGL
void init3DFontWGL(const char* font_name)
{
	g_3DtextWGL = glGenLists(256);
	HFONT font = CreateFont(	-24,							// Height Of Font
						0,								// Width Of Font
						0,								// Angle Of Escapement
						0,								// Orientation Angle
						FW_BOLD,						// Font Weight
						FALSE,							// Italic
						FALSE,							// Underline
						FALSE,							// Strikeout
						ANSI_CHARSET,					// Character Set Identifier
						OUT_TT_PRECIS,					// Output Precision
						CLIP_DEFAULT_PRECIS,			// Clipping Precision
						ANTIALIASED_QUALITY,			// Output Quality
						FF_DONTCARE|DEFAULT_PITCH,		// Family And Pitch
						font_name);					// Font Name
    HDC hDC = wglGetCurrentDC();
	HFONT oldfont = (HFONT)SelectObject(hDC, font);
    GLYPHMETRICSFLOAT agmf[256];
	BOOL result = wglUseFontOutlines(hDC, 32, 96, g_3DtextWGL, 0.0f, 0.1f, WGL_FONT_POLYGONS, agmf);
	SelectObject(hDC, oldfont);
	DeleteObject(font);
}

// struktura a polia pre ulozenie jednotlivych 2D znakov
struct GlyphMetrics
{
    int left;
    int top;
    int advance;
    int bitmap_width;
    int bitmap_height;
};
GLuint g_idTextures2DFontFT[96];
GlyphMetrics g_glyphs2DFontFT[96];
bool g_2DFontFTInitialized = false;

// nacitanie znakov z .ttf suboru pre renderovanie 2D znakov pomocou FreeType kniznice
void init2DFontFT(const char* font_file)
{
    FT_Library library;
	if (FT_Init_FreeType(&library)) 
		return;

	FT_Face face;
	if (FT_New_Face(library, font_file, 0, &face))
    {
        printf("Error loading font from file %s.", font_file);
        FT_Done_FreeType(library);
		return;
    }

    int fontHeight = 16;
	FT_Set_Char_Size(face, 64 * fontHeight, 64 * fontHeight, 96, 96);

    glGenTextures(96, g_idTextures2DFontFT);
    for (int i = 0; i < 96; i++)
    {
        // inicializuje bitmapu pre aktualny glyf
        if (FT_Load_Glyph(face, FT_Get_Char_Index(face, i + 32), FT_LOAD_DEFAULT))
		    continue;
        FT_Glyph glyph;
        if(FT_Get_Glyph(face->glyph, &glyph))
		    continue;
        FT_Glyph_To_Bitmap(&glyph, ft_render_mode_normal, 0, 1);
        FT_BitmapGlyph bitmap_glyph = (FT_BitmapGlyph)glyph;
        FT_Bitmap& bitmap = bitmap_glyph->bitmap;

        // uloz potrebne data pre glyf
        GlyphMetrics glyphMetrics;
        glyphMetrics.top = bitmap_glyph->top;
        glyphMetrics.left = bitmap_glyph->left;
        glyphMetrics.advance = face->glyph->advance.x >> 6;
        glyphMetrics.bitmap_width = bitmap.width;
        glyphMetrics.bitmap_height = bitmap.rows;
        g_glyphs2DFontFT[i] = glyphMetrics;

        // ak nemame bitmapu pre glyf, preskoc glyf
        if (bitmap.width == 0 || bitmap.rows == 0)
            continue;

        // vypocitaj rozmery textury, mocniny 2
        int iNX = int(log((float)bitmap.width)/log(2.0)) + 1;
        int texture_width = int(pow(2.0, iNX));
        int iNY = int(log((float)bitmap.rows)/log(2.0)) + 1;
        int texture_height = int(pow(2.0, iNY));

        // rozsir bitmapu do textury
        GLubyte* texture_data = new GLubyte[ 2 * texture_width * texture_height];
        for(int j = 0; j < texture_height; j++) 
        {
		    for(int k = 0; k < texture_width; k++)
            {
                int bitmap_x = int((bitmap.width - 1) * k / float(texture_width - 1));
                int bitmap_y = int((bitmap.rows - 1) * j / float(texture_height - 1));
			    texture_data[2*(k + j*texture_width)] = texture_data[2*(k + j*texture_width)+1] = 
                    bitmap.buffer[bitmap_x + bitmap.width*bitmap_y];
            }
		}

        // nacitaj texturu do opengl
        glBindTexture(GL_TEXTURE_2D, g_idTextures2DFontFT[i]);
	    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture_width, texture_height, 0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, texture_data);
        delete [] texture_data;
    }

    FT_Done_Face(face);
	FT_Done_FreeType(library);
    g_2DFontFTInitialized = true;
}

// renderovanie 2D textu na danu oknovu poziciu pomocou FreeType kniznice
void render2DTextFT(float x, float y, const char *string)
{
    if (!g_2DFontFTInitialized)
        init2DFontFT("fonts/planetbe.ttf");

    float current_x = x;
    float current_y = y;

    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);

    glEnable(GL_TEXTURE_2D);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, viewport[2], 0, viewport[3]);
    glEnable(GL_BLEND);

    for (const char* c = string; *c != '\0'; c++) 
    {
        if (g_glyphs2DFontFT[*c - 32].bitmap_width > 0 && g_glyphs2DFontFT[*c - 32].bitmap_height > 0)
        {
            glBindTexture(GL_TEXTURE_2D, g_idTextures2DFontFT[*c - 32]);

            int bitmap_width = g_glyphs2DFontFT[*c - 32].bitmap_width;
            int bitmap_height = g_glyphs2DFontFT[*c - 32].bitmap_height;
            float offset_x = g_glyphs2DFontFT[*c - 32].left;
            float offset_y = g_glyphs2DFontFT[*c - 32].top - bitmap_height;

            glBegin(GL_QUADS);
	        glTexCoord2d(0, 0); glVertex2f(current_x + offset_x, current_y + offset_y + bitmap_height);
	        glTexCoord2d(0, 1); glVertex2f(current_x + offset_x, current_y + offset_y);
	        glTexCoord2d(1, 1); glVertex2f(current_x + offset_x + bitmap_width, current_y + offset_y);
	        glTexCoord2d(1, 0); glVertex2f(current_x + offset_x + bitmap_width, current_y + offset_y + bitmap_height);
	        glEnd();
        }

        current_x += g_glyphs2DFontFT[*c - 32].advance;
    }

    glDisable(GL_BLEND);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}

// renderovanie 2D textu na danu oknovu poziciu pomocou WGL kniznice
void render2DTextWGL(float x, float y, const char *string)
{
    if (g_2DtextWGL == 0)
        init2DFontWGL("Arial");

    glWindowPos2f(x, y);

    glPushAttrib(GL_LIST_BIT);
    glListBase(g_2DtextWGL - 32);
    glCallLists(strlen(string), GL_UNSIGNED_BYTE, string);
    glPopAttrib();
}

// renderovanie 3D textu na danu svetovu poziciu s danou skalou pomocou WGL kniznice
void render3DTextWGL(float x, float y, float z, float scale, const char *string)
{
    if (g_3DtextWGL == 0)
        init3DFontWGL("Arial");

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glTranslatef(x, y, z);
    glRotatef(-90, 1.f, 0.0f, 0.0f);
    glScalef(-scale, -scale, scale);
    glPushAttrib(GL_LIST_BIT);
    glListBase(g_3DtextWGL - 32);
    glCallLists(strlen(string), GL_UNSIGNED_BYTE, string);
    glPopAttrib();
    glPopMatrix();
}

// renderovanie pripraveneho 2D textu na danu oknovu poziciu pomocou WGL kniznice
void render2DTextGLUT(float x, float y, void *font, const char *string)
{
   glWindowPos2f(x, y);

   for (const char* c = string; *c != '\0'; c++) 
   {
      glutBitmapCharacter(font, *c);
   }
}

// renderovanie 3D textu na danu svetovu poziciu s danou skalou pomocou GLUT kniznice
void render3DTextGLUT(float x, float y, float z, float scale, void *font, const char *string)
{
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glTranslatef(x, y, z);
    glRotatef(-90, 1.f, 0.0f, 0.0f);
    glScalef(-scale, -scale, scale);
    for (const char* c = string; *c != '\0'; c++) 
    {
        glutStrokeCharacter(font, *c);
    }
    glPopMatrix();
}