// Standardne pre MS win aplikacie
#include <windows.h>

// GLEW kniznica. pre spravu novsich rozsireni
#include "glew.h"
// GL Utility Toolkit (GLUT), v ramci toho sa includuje aj openGL hlavickovy subor
#include "glut.h"

#include <stdio.h>
#include <math.h>

#include "shaders.h"
#include "textures.h"
#include "geometry.h"
#include "fonts.h"
#include "meshes.h"

// Globalne premenne
GLboolean g_is_anim = true;
GLboolean g_is_texture = false;
float g_sun_rotation = 0.0f;

// Globalne premenne pre prezeranie sceny
GLdouble g_vzd, g_old_vzd, g_fi, g_xi, g_old_fi, g_old_xi;
GLint g_left_mouse_down_x, g_left_mouse_down_y;
GLint g_right_mouse_down_y;
GLboolean g_right_down = false, g_left_down = false;
GLuint g_window_height, g_window_width;

// id textur
GLuint g_tex_sun, g_tex_earth, g_tex_lava, g_tex_phobos, g_tex_skybox;

// id shaderov
GLuint g_shaders_skybox, g_shaders_perpixel_phong, g_shaders_envmap;

// modely
Scene* g_duck_model = NULL;
Scene* g_car_model = NULL;

void initShaders()
{
	// nacitanie, kompilacia a zlinkovanie shaderov
	g_shaders_skybox = loadProgram("shaders/skybox.vert", "shaders/skybox.frag");
	SetShaderUniform1i(g_shaders_skybox, "cube_map", 0);

	g_shaders_perpixel_phong = loadProgram("shaders/perpixel_phong.vert", "shaders/perpixel_phong.frag");
	SetShaderUniform1i(g_shaders_perpixel_phong, "color_texture", 0);

	g_shaders_envmap = loadProgram("shaders/perpixel_envmap.vert", "shaders/perpixel_envmap.frag");
	SetShaderUniform1i(g_shaders_envmap, "color_texture", 0);
	SetShaderUniform1i(g_shaders_envmap, "cubemap_texture", 1);

	glActiveTexture(GL_TEXTURE0);
}

// inicializacia OpenGL textur
void initTextures()
{
	// ************* prva textura, zem *************
	g_tex_earth = LoadTexture("textures/earth.jpg");

	// ************* druha textura, slnko *************
	g_tex_sun = LoadTexture("textures/sun.jpg");

	// ************* tretia textura, jupiter *************
	g_tex_lava = LoadTexture("textures/lava.png");

	// ************* stvrta textura, phobos *************
	g_tex_phobos = LoadTexture("textures/phobos.jpg");

	// ************* stvrta textura, phobos *************
	g_tex_skybox = LoadCubeTexture("skybox/snow_positive_x.jpg", "skybox/snow_negative_x.jpg",
		"skybox/snow_positive_y.jpg", "skybox/snow_negative_y.jpg",
		"skybox/snow_positive_z.jpg", "skybox/snow_negative_z.jpg");
}

// Open GL Inicializacia
bool initGL(void)
{
	// vypis info o o systeme OpenGL
	printf("OpenGL version: %s\n", glGetString(GL_VERSION));
	printf("OpenGL vendor: %s\n", glGetString(GL_VENDOR));
	printf("OpenGL renderer: %s\n", glGetString(GL_RENDERER));

	// nastav cierne pozadie
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	// nastav z-buffer testovanie
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	// nastav stencil buffer
	glClearStencil(0);

	// nastav zmiesavaciu funkciu
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// nastav vhodne mapovanie textur
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	// initializuj kameru
	g_fi = 0.7f; g_xi = 0.7f; g_vzd = 10.0f;

	// initializuje svetlo
	GLfloat light_ambient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
	GLfloat light_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat light_specular[] = { 0.6f, 0.6f, 0.6f, 1.0f };
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 1.0);
	glEnable(GL_LIGHT0);

	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	return true;
}

// Procedura obsluhujuca vykreslovanie
void render(void)   
{
	// Vymaz (vypln) obrazovku a z-buffer definovanymi hodnotami
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);	

	// Nastav modelview maticu (transformaciu) na jednotkovu maticu
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();									

	// Nastav view matice pre pohlad na svetovy suradnicovy system
	GLfloat camera_pos[4];
	camera_pos[0] = g_vzd * cos(g_fi) * cos(g_xi);
	camera_pos[1] = g_vzd * sin(g_fi) * cos(g_xi);
	camera_pos[2] = g_vzd * sin(g_xi);
	camera_pos[3] = 1;
	gluLookAt(camera_pos[0], camera_pos[1], camera_pos[2], 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f );
	float view_matrix[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, view_matrix);

	// vykresli suradnicove osi svetoveho systemu roznymi farbami, bez shaderov
	glLineWidth(1);
	glBegin( GL_LINES );
	glColor3f(1.0f, 0.0f, 0.0f );glVertex3f( 0.0f, 0.0f,0.0f );glVertex3f( 100.0f, 0.0f, 0.0f );
	glColor3f(0.0f, 1.0f, 0.0f );glVertex3f( 0.0f, 0.0f,0.0f );glVertex3f( 0.0f, 100.0f, 0.0f );
	glColor3f(0.0f, 0.0f, 1.0f );glVertex3f( 0.0f, 0.0f,0.0f );glVertex3f( 0.0f, 0.0f, 100.0f );
	glEnd();

	// OBJEKTY POUZIVAJUCE SKYBOX SHADER
	// skybox je dany vo svetovych suradniciach, akurat jeho stred je stale v bode, kdeje kamera
	glUseProgram(g_shaders_skybox);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, g_tex_skybox);
	glPushMatrix();
	glScalef(500, 500, 500);
	renderCubeVBO();
	glPopMatrix();
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	glUseProgram(0);

	// TERAZ IDU OBJEKTY POUZIVAJUCE PER-PIXEL PHONG SHADER
	glUseProgram(g_shaders_perpixel_phong);
	if (g_is_texture)
		SetShaderUniform1i(g_shaders_perpixel_phong, "texturing_enabled", 1);
	else
		SetShaderUniform1i(g_shaders_perpixel_phong, "texturing_enabled", 0);

	// vykresli slnko
	SetShaderUniform1i(g_shaders_perpixel_phong, "lighting_enabled", 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, g_tex_sun);
	glPushMatrix();
	glRotatef(g_sun_rotation, 0.0f, 0.0f, 1.0f);
	glTranslatef(6.0f, 0.0f, 0.0f);
	// nastav poziciu svetla do stredu slnka
	GLfloat sun_position[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, sun_position);
	glScalef(0.3f, 0.3f, 0.3f);
	glColor3f(1.0f, 1.0f, 1.0f );
	renderSphereVBO();
	glPopMatrix();

	// vykresli phobos
	SetShaderUniform1i(g_shaders_perpixel_phong, "lighting_enabled", 1);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, g_tex_phobos);
	glPushMatrix();
	glTranslatef(0.0f, 0.0f, 4.0f);
	glScalef(1.0f, 1.0f, 1.0f);
	renderSphereVBO();
	glPopMatrix();

	// vykresli kacicu
	glPushMatrix();
	glRotatef(90, 1.0f, 0.0f, 0.0f);
	glScalef(0.01f, 0.01f, 0.01f);
	if (g_duck_model)
		RenderScene(g_duck_model, g_shaders_perpixel_phong);
	glPopMatrix();

	// vykresli auto
	glPushMatrix();
	glTranslatef(0.0f, 3.0f, 0.0f);
	glRotatef(g_sun_rotation / 3, 0.0f, 0.0f, 1.0f);
	glRotatef(90, 1.0f, 0.0f, 0.0f);
	glScalef(0.2f, 0.2f, 0.2f);
	if (g_car_model)
		RenderScene(g_car_model, g_shaders_perpixel_phong);
	glPopMatrix();

	// // OBJEKTY POUZIVAJUCE ENVIRONMENT MAPPING SHADER
	glUseProgram(g_shaders_envmap);
	if (g_is_texture)
		SetShaderUniform1i(g_shaders_envmap, "texturing_enabled", 1);
	else
		SetShaderUniform1i(g_shaders_envmap, "texturing_enabled", 0);

	// vykresli 2x torus s environment mapou
	glPushMatrix();
	glLoadIdentity();
	SetShaderUniformMatrix4f(g_shaders_envmap, "view_matrix", view_matrix);
	SetShaderUniform1i(g_shaders_envmap, "color_texture", 0);
	SetShaderUniform1i(g_shaders_envmap, "cubemap_texture", 1);
	SetShaderUniform4f(g_shaders_envmap, "camera_world", camera_pos);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, g_tex_lava);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_CUBE_MAP, g_tex_skybox);
	glTranslatef(3, 0, 0);
	renderTorusVBO();
	glTranslatef(-6, 0, 0);
	renderTorusVBO();
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	glActiveTexture(GL_TEXTURE0);
	glPopMatrix();
	glUseProgram(0);

	// vykresli 3D text, bez shaderov
	glEnable(GL_LIGHTING);
	glColor3f(0.0f, 1.0f, 1.0f);
	render3DTextWGL(7, 0, 2, 1, "Project GRC - Jadro & Kniznice ...");
	glDisable(GL_LIGHTING);

	// vykresli 2D text, bez shaderov
	glColor3f(1.0f, 1.0f, 0.0f);
	render2DTextFT(10, g_window_height - 20, "Projekt GRC - Base Core & Libraries ...");
	render2DTextFT(10, g_window_height - 45, "F1 - zapni/vypni animaciu");
	render2DTextFT(10, g_window_height - 70, "F2 - zapni/vypni textury");

	// Zamen buffre a zobraz vykreslene objekty
	glutSwapBuffers();
}

// Procedura obsluhujuca zmenu rozmerov okna
void reshape(int w, int h)
{
	g_window_width = w;
	g_window_height = h;

	// nastvenie casti okna pre vykreslenie, tentoraz kreslime do celeho okna
	glViewport(0, 0, w, h);
	// Budeme menit projekcnu maticu (transformaciu)
	glMatrixMode(GL_PROJECTION);                        
	glLoadIdentity();                                  

	if (h == 0) 
		h = 1;
	// Chceme perspektivnu projekciu
	float ratio = (float)w/(float)h;
	gluPerspective(60, ratio, 0.1, 1000.0);
	//glOrtho(-10.0f*ratio, 10.0f*ratio, -10.0f, 10.0f, 1.0, 5000.0); 
}

// Procedura obsluhujuca stlacenie klavesy
void keyboard(unsigned char key, int x, int y)
{
	switch (key) 
	{
		// Ak Esc bolo stlacene
	case 27:
		DeleteAllVBO();
		if (g_duck_model)
			delete g_duck_model;
		if (g_car_model)
			delete g_car_model;
		// Ukoncenie programu
		exit(0);
		break;
	default:        
		break;
	}
}

// Procedura obsluhujuca stlacenie specialnej klavesy
void special_keys(int a_keys, int x, int y)
{
	switch (a_keys) 
	{
		// Ak F1 bolo stlacene
	case GLUT_KEY_F1:
		g_is_anim = !g_is_anim;
		glutPostRedisplay();
		break;
		// Ak F2 bolo stlacene
	case GLUT_KEY_F2:
		g_is_texture = !g_is_texture;
		glutPostRedisplay();
		break;
		// Ak F3 bolo stlacene
	case GLUT_KEY_F3:

		break;
	default:
		break;
	}
}

// Procedura obsluhujuca stlacenie tlacitka mysi
void mouse_klik(int button, int state, int x, int y )
{
	switch (button) 
	{
	case GLUT_LEFT_BUTTON:
		if (state == GLUT_DOWN)
		{
			g_left_down = true;
			g_left_mouse_down_x = x;
			g_left_mouse_down_y = y;
			g_old_fi = g_fi;
			g_old_xi = g_xi;
			break;
		}
		else if (state == GLUT_UP)
		{
			g_left_down = false;
		}
		break;
	case GLUT_RIGHT_BUTTON:
		if (state == GLUT_DOWN)
		{
			g_right_down = true;
			g_right_mouse_down_y = y;
			g_old_vzd = g_vzd;
			break;
		}
		else if (state == GLUT_UP)
		{
			g_right_down = false;
		}
		break;
	default:
		break;
	}

	glutPostRedisplay();
}

// Procedura obsluhujuca pohyb mysi
void mouse_move(int x, int y)
{
	if (g_left_down == GL_TRUE)
	{
		g_fi = g_old_fi + (g_left_mouse_down_x - x)/200.0f;
		g_xi = g_old_xi + (y - g_left_mouse_down_y)/200.0f;
		glutPostRedisplay();
	}

	if (g_right_down == GL_TRUE)
	{
		g_vzd = g_old_vzd + (g_right_mouse_down_y - y)/10.0f;
		glutPostRedisplay();
	} 

}

void timer(int id)
{
	if (g_is_anim)
	{
		g_sun_rotation += 3.5f;
	}

	glutPostRedisplay();
	glutTimerFunc(50, timer, 0);
}

// Zakladna funkcia spustena na zaciatku
int main(int argc, char** argv)
{
	// GLUT Inicializacia
	glutInit(&argc, argv);                              
	// Display Mod (Rgb a Double Buffering)
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH | GLUT_STENCIL);
	//glutInitDisplayString("stencil>=8 rgb>=8 double depth>=24");
	// Nastavime pociatocnu poziciu a velkost okna
	glutInitWindowPosition(50, 50);
	glutInitWindowSize(800, 600);                   
	// Vyrobime okno s danym menom
	glutCreateWindow("--== GRC projekt ==--" );  

	// Zinicializujeme GLEW
	glewInit();

	// Nastavime zakladne vlastnosti OpenGL
	initGL();
	initSphereVBO(32, 32);
	initTorusVBO(0.6f, 2.0f, 32, 32);
	initTextures();
	initShaders();
	init2DFontFT("fonts/optien.ttf");

	// nacitaj modely
	g_duck_model = LoadScene("./models/Duck/duck.dae");
	g_car_model = LoadScene("./models/F40/F40.dae");

	// Zaregistrujeme callback funkcie
	glutDisplayFunc(render);                            
	glutReshapeFunc(reshape);                        
	glutKeyboardFunc(keyboard);                      
	glutSpecialFunc(special_keys);
	glutMouseFunc(mouse_klik);
	glutMotionFunc(mouse_move);
	glutTimerFunc(0, timer, 0);

	// Spustenie hlavneho okruhu zachytavania sprav
	glutMainLoop();

	return 0;
}
