#include "SDT.h"
#include "CSDT.h"
#include "VSDT.h"
#include "SVSDT.h"
#include "QSDT.h"
#include "SQSDT.h"
#include <time.h>
#include "Option.h"
#include <GLUT/glut.h>
#include <GL/glui.h>
#include "trackball.c"


//quaternion
static float lastquat[4];
static float curquat[4];
//mouse
static int left_mouse;
static int middle_mouse;
static int mousex, mousey;

//mesh
static GLuint meshid;
static GLuint seedid;

int viewport[4];
//FPS
int split = 0; //
double fps = 60;
int frame = 0;

// toggle
int _resolution_w = 2048;//
int _resolution_h = 2048;//

//main
int col[3] = {255,0,0};
GLUI* glui;
SDT* _sdt;
vector<Vector3d> _bpos;
vector<double> _distance;
unsigned char _teximage[12582912]; //1024*1024*3
int _num_sample=40;//サンプル数
int _sdt_mode = 3;
int _show_boundary;
int _toggle_texture;
int   _srand_num= 30;
float _pitch = 0.1;
int _iso_line = 1;
GLuint _texname;
int state = 0;


void init_seed();
void update_grid()
{
	if(_iso_line)
	{
		for(int i = 0 ; i < (int)_distance.size() ; i++)
		{
			int id  = 4 * _distance[i] / _pitch  ;
			if(_distance[i] <1.0e-16 )
			{

				_teximage[3 * i + 0] = (unsigned char)0x00;
				_teximage[3 * i + 1] = (unsigned char)0x80;
				_teximage[3 * i + 2] = (unsigned char)0;
			}

			else if(id % 2 )
			{

				_teximage[3 * i + 0] = (unsigned char)col[0];
				_teximage[3 * i + 1] = (unsigned char)col[1];
				_teximage[3 * i + 2] = (unsigned char)col[2];
			}
			else
			{
				_teximage[3 * i + 0] = 0xff;
				_teximage[3 * i + 1] = 0xff;
				_teximage[3 * i + 2] = 0xff;

			}
		}
	}
	else
	{
		for(int i = 0 ; i < (int)_distance.size() ; i++)
		{
			double d = 6*_distance[i] ;

			if( d < 1)
			{
				_teximage[3 * i +0] = 0;
				_teximage[3 * i +1] = d * 255;
				_teximage[3 * i +2] = 0xff;
			}
			else if(d<2)
			{
				d -= 1;
				_teximage[3 * i +0] = 0;
				_teximage[3 * i +1] = 0xff;
				_teximage[3 * i +2] = 0xff - d * 255;
			}
			else if(d<3)
			{
				d -= 2;
				_teximage[3 * i +0] = d*255;
				_teximage[3 * i +1] = 0xff;
				_teximage[3 * i +2] = 0;
			}
			else if( d<4)
			{
				d -= 3;
				_teximage[3 * i +0] = 0xff;
				_teximage[3 * i +1] = 0xff - d * 255;
				_teximage[3 * i +2] = 0;
			}
			else
			{
				_teximage[3 * i +0] = 0xff;
				_teximage[3 * i +1] = 0;
				_teximage[3 * i +2] = 0;
			}
		}
	}
	glTexImage2D(GL_TEXTURE_2D, 0, 3, _resolution_w, _resolution_h, 0, GL_RGB, GL_UNSIGNED_BYTE, _teximage);
}

void init_grid()
{
	for(int h = 0 ; h < _resolution_h ; h++)
	{
		for(int w = 0 ; w < _resolution_w ; w++)
		{
			if((h >> 6 & 0x01) == (w >>6 &0x01)) 
			{
				_teximage[3 * (h *_resolution_w + w)+0] = 0xff;
				_teximage[3 * (h *_resolution_w + w)+1] = 0x00;
				_teximage[3 * (h *_resolution_w + w)+2] = 0x00;
			}
			else
			{
				_teximage[3 * (h *_resolution_w + w)+0] = 0xff;
				_teximage[3 * (h *_resolution_w + w)+1] = 0xff;
				_teximage[3 * (h *_resolution_w + w)+2] = 0xff;
			}
		}
	}
}

unsigned __stdcall start_sdt(void* arg)
{
	if(state == 0) 
	{
		cerr<<"You need setup"<<endl;
		return -1;
	}
	int t0 = clock();
	if(_sdt->run())
	{
		cerr<<"computation succeeded"<<endl;	
	}	
	_sdt->get_dist(_distance);
	int t1 = clock();
	cerr<<"time : "<<(float)(t1-t0)/CLOCKS_PER_SEC<<"sec."<<endl;
	delete _sdt;
	state = 0;
	update_grid();
	return 1;
}
void init_random()
{
	if(state != 0)
	{
		delete _sdt;
	}
	state = 1;
	switch(_sdt_mode)
	{
	case 0:
		_sdt = new SDT();
		break;
	case 1:
		_sdt = new CSDT();
		break;
	case 2:
		_sdt = new QSDT();
		break;
	case 3:
		_sdt = new VSDT();
		break;
	case 4:
		_sdt = new SVSDT();
		break;
	case 5:
		_sdt = new SQSDT();
		break;
	}
	srand(_srand_num);
	_sdt->init(_resolution_h,_resolution_w,_num_sample);
	_sdt->get_boundary(_bpos);
	init_seed();
	start_sdt(NULL);

}
/*
void init_file()
{

if(_sdt != NULL) delete _sdt;
switch(_sdt_mode)
{
case 0:
_sdt = new QSDT();
break;
case 1:
_sdt = new VSDT();
break;
case 2:
_sdt = new SVSDT();
break;
case 3:
_sdt = new SQSDT();
break;
case 4:
_sdt = new SDT();
break;
case 5:
_sdt = new CSDT();
break;
}
_sdt->init(string(_filename));

if(_teximage != NULL) 
{
delete[] _teximage;
}


//_teximage = new unsigned char [  3];

}
*/
void init_seed()
{
	if(glIsList(seedid))glDeleteLists(seedid,1);
	seedid = glGenLists(1);
	glNewList(seedid, GL_COMPILE);

	GLfloat mat_ambient[]    = { 0, 0, 0,1.0 };
	GLfloat mat_diffuse[]    = { 0.4, 0.4, 0.8, 1.0 };
	GLfloat mat_specular[]   = { 0.5, 0.5, 0.5, 1.0 };
	GLfloat mat_shininess[]  = { 100.0 };
	glMaterialfv(GL_FRONT, GL_AMBIENT,  mat_ambient); 
	glMaterialfv(GL_FRONT, GL_DIFFUSE,  mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS,mat_shininess);
	glBegin(GL_MODELVIEW);
	for(int i = 0 ; i < (int)_bpos.size() ; i++)
	{
		glTranslated(_bpos[i].x, _bpos[i].y, _bpos[i].z);
		glutSolidSphere(0.02, 10,10);
		glTranslated(-_bpos[i].x, -_bpos[i].y, -_bpos[i].z);
	}
	glEndList();
}
void init_sphere()
{
	if(glIsList(meshid))glDeleteLists(meshid,1);
	meshid = glGenLists(1);
	glNewList(meshid, GL_COMPILE);

	GLfloat mat_ambient[]    = { 0.0, 0.0, 0.0, 1.0 };
	GLfloat mat_diffuse[]    = { 0.8, 0.8, 0.8, 1.0 };
	GLfloat mat_specular[]   = { 0.4, 0.4, 0.4, 1.0 };
	GLfloat mat_shininess[]  = { 100.0 };
	glMaterialfv(GL_FRONT, GL_AMBIENT,  mat_ambient); 
	glMaterialfv(GL_FRONT, GL_DIFFUSE,  mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS,mat_shininess);
	//glEnable(GL_TEXTURE_2D);


	vector<float> vx,vy,vz, tu,tv;
	int width = 361 ;
	int height = 181 ;
	double rad = 3.1415926 / 180;
	for(int h = 0 ; h < height ; h++)
	{
		for(int w = 0 ; w < width ; w++)
		{
			int a = w % 360;
			int b = h - 90;
			vx.push_back( cos( a * rad ) *  cos( b * rad));
			vy.push_back( sin( a * rad ) *  cos( b * rad));
			vz.push_back( sin( b * rad));
			tu.push_back((float) w / 360.0);
			tv.push_back(1-(float) h / 180.0);
		}
	}
	for(int h = 0 ; h < height - 1; h++)
	{
		for(int w = 0 ; w < width - 1; w++)
		{
			int base = w + width * h;
			glBegin(GL_POLYGON);

			glNormal3f(vx[base],vy[base],vz[base]);
			glTexCoord2f(tu[base],tv[base]);
			glVertex3f(vx[base],vy[base],vz[base]);

			base += 1;
			glNormal3f(vx[base],vy[base],vz[base]);
			glTexCoord2f(tu[base],tv[base]);
			glVertex3f(vx[base],vy[base],vz[base]);

			base += width;
			glNormal3f(vx[base],vy[base],vz[base]);
			glTexCoord2f(tu[base],tv[base]);
			glVertex3f(vx[base],vy[base],vz[base]);

			base -= 1;
			glNormal3f(vx[base],vy[base],vz[base]);
			glTexCoord2f(tu[base],tv[base]);
			glVertex3f(vx[base],vy[base],vz[base]);

			glEnd();
		}
	}
	glEndList();
}
/*
void draw_polygon()
{

}*/
void show_help()
{
}
void update_perspective()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();             
	gluPerspective(45, (double)viewport[2] / viewport[3], 0.1 , 3);// translate - rad, translate + rad);		
}

void display()
{
	float m[4][4];
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0, 0, -3);//視点移動
	build_rotmatrix(m, curquat);
	glMultMatrixf(&(m[0][0])); //回転

	if(_toggle_texture)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, 3, _resolution_w, _resolution_h, 0, GL_RGB, GL_UNSIGNED_BYTE, _teximage);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, _texname);		
	}
	else
	{
		glDisable(GL_TEXTURE_2D);
	}
	glCallList(meshid);
	// draw seed

	glDisable(GL_TEXTURE_2D);
	if(_show_boundary) glCallList(seedid);


	int prev_time = split;
	split = clock();
	fps = (double) (CLOCKS_PER_SEC)/(split - prev_time ) ;
	char fps_char[80];
	sprintf(fps_char, "sdt - %4d fps",(int)fps); 
	::glutSetWindowTitle(fps_char);

	glutSwapBuffers(); 
}

static void
reshape(int width, int height)
{
	glViewport(0, 0, width, height);
	glGetIntegerv(GL_VIEWPORT,viewport);       
	update_perspective();
	glutPostRedisplay();
}

void reset(void)
{
	left_mouse = 0;
	middle_mouse = 0;
	trackball(curquat, 0.0, 0.0, 0.0, 0.0);
	update_perspective();
}

void
keyboard(unsigned char c, int x, int y)
{
}

void
motion(int x, int y)
{
	int W = viewport[2];
	int H = viewport[3];

	if (left_mouse) 
	{
		if (mousex != x || mousey != y) 
		{
			trackball(lastquat,
				(double)(2.0*mousex - W) / W,
				(double)(H - 2.0*mousey) / H,
				(double)(2.0*x - W) / W,
				(double)(H - 2.0*y) / H);
			add_quats(lastquat, curquat, curquat);
		}
	} 
	else if (middle_mouse)
	{
		update_perspective();
	}
	mousex = x;
	mousey = y;
	glutPostRedisplay();
}

void
mouse(int b, int s, int x, int y)
{
	mousex = x;
	mousey = y;
	//ボタンを押している
	if (s == GLUT_DOWN) 
	{
		switch (b) 
		{
		case GLUT_LEFT_BUTTON:
			left_mouse = GL_TRUE;
			break;
		case GLUT_MIDDLE_BUTTON:
			middle_mouse = GL_TRUE;
			break;
		}
	}
	else //ボタンを離した
	{
		if (left_mouse)
		{
			left_mouse = GL_FALSE;
		} 
		else if (middle_mouse)
		{
			middle_mouse = GL_FALSE;
		}
	}
	motion(x, y);
}

void

animate(void)
{
	glutPostRedisplay();

	glui->sync_live(); //同期をとる
	
}

void
init(void)
{
	static float light0_ambient[] =  {0.1, 0.1, 0.1, 1.0};
	static float light0_diffuse[] =  {1.0, 1.0, 1.0, 0.0};
	static float light0_position[] = {0,0,10, 0};
	static float light0_specular[] = {0.0, 0.0, 0.0, 0.0};

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);  
	glClearColor(0,0,0,1);
	glEnable(GL_POLYGON_SMOOTH);


	init_grid();
	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &_texname);
	glBindTexture(GL_TEXTURE_2D , _texname);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, _resolution_w, _resolution_h, 0, GL_RGB, GL_UNSIGNED_BYTE, _teximage);

	glLightfv(GL_LIGHT0, GL_AMBIENT, light0_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light0_specular);
	glLightfv(GL_LIGHT0, GL_POSITION, light0_position);

	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);
	glShadeModel(GL_SMOOTH);
	glGetIntegerv(GL_VIEWPORT, viewport);    
	reset();
	init_sphere();
}

void timer(int arg)
{	
//	glutTimerFunc(50 , timer , 0);
}



void
menu(int choice)
{
}


int
main(int argc, char **argv)
{
	int W = 480;
	int H = 480;
	glutInit(&argc, argv);
	glutInitWindowSize(W,H);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_RGB | GLUT_DOUBLE );

	int main_window = glutCreateWindow("sdt");
	glutReshapeFunc(reshape);
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutMotionFunc(motion);
	glutMouseFunc(mouse);
	glutTimerFunc(100 , timer , 0);


	//メニュー作成

	init();

	glui = GLUI_Master.create_glui("sdt GUI");

	GLUI_Panel* sdt_type_panel = glui->add_panel("SDT type");
	GLUI_RadioGroup *sdt_type_group = glui->add_radiogroup_to_panel(sdt_type_panel,&_sdt_mode);//もっと引数

	glui->add_radiobutton_to_group(sdt_type_group, "brute force");
	glui->add_radiobutton_to_group(sdt_type_group, "region growing");
	glui->add_radiobutton_to_group(sdt_type_group, "wavefront(quaternion)");
	glui->add_radiobutton_to_group(sdt_type_group, "wavefront(vector)");
	glui->add_radiobutton_to_group(sdt_type_group, "sweeping(vector)");	
	glui->add_radiobutton_to_group(sdt_type_group, "sweeping(quaternion)");
	sdt_type_group->set_selected(2);
	//init
	GLUI_Panel* init_panel = glui->add_panel("Init");
	//random sampling
	glui->add_button_to_panel(init_panel,"Run",0,(GLUI_Update_CB)init_random );
	GLUI_Spinner* sample_num = glui->add_spinner_to_panel(init_panel,"#samples", GLUI_SPINNER_INT, &_num_sample);
	sample_num->set_int_limits(1, 100, GLUI_LIMIT_WRAP);
	GLUI_Spinner* srand_num = glui->add_spinner_to_panel(init_panel,"seed", GLUI_SPINNER_INT, &_srand_num);
	glui->add_checkbox_to_panel(init_panel,"isolines", &_iso_line); 
	//sample_num->set_int_limits(1, 100, GLUI_LIMIT_WRAP);
	GLUI_Spinner* sample_pitch = glui->add_spinner_to_panel(init_panel,"pitch", GLUI_SPINNER_FLOAT, &_pitch);
	sample_pitch->set_float_limits(0.01, 1, GLUI_LIMIT_WRAP);

	glui->add_checkbox("show boundary", &_show_boundary); 
	glui->add_checkbox("show distance fields", &_toggle_texture); 

	//rotation
	GLUI_Panel* rot_panel = glui->add_panel("Rotation");
	GLUI_Spinner* rotx = glui->add_spinner_to_panel(rot_panel,"x", GLUI_SPINNER_FLOAT, &curquat[0]);
	rotx->set_float_limits(-1, 1, GLUI_LIMIT_WRAP);
	GLUI_Spinner* roty = glui->add_spinner_to_panel(rot_panel,"y", GLUI_SPINNER_FLOAT, &curquat[1]);
	roty->set_float_limits(-1, 1, GLUI_LIMIT_WRAP);
	GLUI_Spinner* rotz = glui->add_spinner_to_panel(rot_panel,"z", GLUI_SPINNER_FLOAT, &curquat[2]);
	rotz->set_float_limits(-1, 1, GLUI_LIMIT_WRAP);
	GLUI_Spinner* rotw = glui->add_spinner_to_panel(rot_panel,"w", GLUI_SPINNER_FLOAT, &curquat[3]);
	rotw->set_float_limits(-1, 1, GLUI_LIMIT_WRAP);

	GLUI_Panel* color_panel = glui->add_panel("Color");
	//random sampling
	GLUI_Spinner* colr = glui->add_spinner_to_panel(color_panel,"R", GLUI_SPINNER_INT, &col[0]);
	colr->set_int_limits(0,255, GLUI_LIMIT_WRAP);
	GLUI_Spinner* colg = glui->add_spinner_to_panel(color_panel,"G", GLUI_SPINNER_INT, &col[1]);
	colr->set_int_limits(0,255, GLUI_LIMIT_WRAP);
	GLUI_Spinner* colb = glui->add_spinner_to_panel(color_panel,"B", GLUI_SPINNER_INT, &col[2]);
	colr->set_int_limits(0,255, GLUI_LIMIT_WRAP);


	glui->add_button("Exit", 0, (GLUI_Update_CB)exit);
	glui->set_main_gfx_window(main_window);
	GLUI_Master.set_glutIdleFunc(animate);

	glutMainLoop();
	return 0;          
}
