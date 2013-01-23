#include "Viewer.h"

#define PI 3.1415926535
double Xdir=0,Ydir=0.5,Zdir=2.5;
double step=0.5;
#define XY 1
#define YZ 2
#define XZ 3

void Viewer::make_vector_proper()
{
	newpoint n;
	for(unsigned int i=0;i<tracks2.size();i++)
	{
		n.c.r=GLfloat(colorlist[i].r)/255.0;
		n.c.g=GLfloat(colorlist[i].g)/255.0;
		n.c.b=GLfloat(colorlist[i].b)/255.0;
		//glBegin(GL_POINTS);
		for(unsigned int j=0;j<tracks2[i].size();j++)
		{
			if(tracks2[i][j].visible)
			{
				
				n.x=((GLfloat)tracks2[i][j].p.x/(GLfloat)imgcols);
				n.y=1.0-((GLfloat)tracks2[i][j].p.y/(GLfloat)imgrows);
				n.z=1.0-(tracks2[i][j].z/255.0);
				n.num=i;
				disppoints.push_back(n);
			}
		}
		//glEnd();
	}
}
Viewer::Viewer()
{

	cameradirection = 0.0;
	updownrotate = 0.0;
	rotX=rotZ=0.0;
	rotY=-60.0;
	Wx=window_width;Wy=window_height;
	frame=0;
}

Viewer::~Viewer()
{

}

void Viewer::Init()
{
	// clear the background to the color values specified in the parentheses
	glClearColor (0.0, 0.0, 0.0, 0.0);

	// this is a 3D scene. so, clear the depth
	glClearDepth(1.0);

	// I need depth. enable depth
	glEnable(GL_DEPTH_TEST);

	glShadeModel (GL_SMOOTH);

}

void Viewer::Init2()
{
	glClearColor(0.0, 0.0, 0.0, 0.0);    //set background color to Black
	glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0,1.0, 0.0,1.0);

}

void Viewer::show_tracks(vector<vector<showcircle> > t,int r,int c)
{
	tracks=t;
	imgrows=r;
	imgcols=c;
	int maxsize=0;
	for(unsigned int i=0;i<tracks.size();i++)
		maxsize=(maxsize<tracks[i].size())?tracks[i].size():maxsize;
	for(unsigned int i=0;i<maxsize;i++)
	{
		vector<showcircle> temp;
		for(unsigned int j=0;j<tracks.size();j++)
		{
			if(tracks[j].size() > i)
				temp.push_back(tracks[j][i]);
		}
		tracks2.push_back(temp);
	}
	int rr=0,gg=0,bb=0;
	for(unsigned int i=0;i<tracks2.size();i++)
	{
		rr=rr%256;
		gg=gg%256;
		bb=bb%256;
		if(rr%256==0 && gg%256==0 && bb%256==0)
		{
			rr=rand()%256;gg=rand()%256;bb=rand()%256;
		}
		Color c;
		c.r=rr;c.g=gg;c.b=bb;
		colorlist.push_back(c);
		rr+=10;
		gg+=20;
		bb+=30;
	}
	make_vector_proper();
	/*for(int i=0;i<tracks2.size();i++)
	{
		int j=1;
		trackshow.push_back(j);
	}*/
}

void Viewer::show_contours(vector<vector<vector<cv::Point> > > cc)
{
	contours=cc;
}

void Viewer::display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Specifies which matrix stack is the target for subsequent matrix operations
	// In this example, the projection matrix is set to perspective projection matrix stack
	glMatrixMode(GL_PROJECTION);
	// all matrix values from previous frames set to identity
	glLoadIdentity();
	
	// perspective projection loaded with new values for Wx and Wy updated
	gluPerspective(45, (GLfloat) Wx/(GLfloat) Wy, 1.0, 1000.0);
	// use glOrtho for a 3D orthogonal projection
	//glOrtho(-100, 100, -100, 100, -100, 100);

	// Applies subsequent matrix operations to the modelview matrix stack.
	glMatrixMode(GL_MODELVIEW);
	// Clears all the previously loaded values in the modelview matrix
	glLoadIdentity();
	
	glLineWidth(4.0);
	glTranslatef(-Xdir,-Ydir,-Zdir);
	glRotatef(rotY,0.0,1.0,0.0);
	glRotatef(rotZ,0.0,0.0,1.0);
	glPushMatrix();
	glColor3f(1.0,1.0,1.0);
	//Camera(FALSE,0,0);
	//glutSolidCube(1.0);	
	DrawAxes();
	DrawTracks(frame);
	glPopMatrix();

	/* glFlush(); This isn't necessary for double buffers */
	glutSwapBuffers();
}

void Viewer::DrawAxes()
{
	char X[2]="X";
	glColor3f(1.0,0.0,0.0);
	glRasterPos3f(0.5,0.0,0.0);
	glutBitmapCharacter(GLUT_BITMAP_8_BY_13,'X');
	char Y[2]="Y";
	glRasterPos3f(0.0,0.5,0.0);
	glutBitmapCharacter(GLUT_BITMAP_8_BY_13,'Y');
	char Z[2]="Z";
	glRasterPos3f(0.0,0.0,0.5);
	glutBitmapCharacter(GLUT_BITMAP_8_BY_13,'Z');
	glBegin(GL_LINES);
	//X-axis
	glColor3f(1.0,0.0,0.0);
	glVertex3f(0.0,0.0,0.0);
	glVertex3f(1.0,0.0,0.0);
	
	//Y-axis
	glColor3f(0.0,1.0,0.0);
	glVertex3f(0.0,0.0,0.0);
	glVertex3f(0.0,1.0,0.0);
	
	//Z-axis
	glColor3f(0.0,0.0,1.0);
	glVertex3f(0.0,0.0,0.0);
	glVertex3f(0.0,0.0,1.0);
	
	glEnd();
}
void Viewer::DrawTracks()
{	
	glPointSize(2.0);
	double maxx=0,maxy=0,maxz=0;
	for(unsigned int i=0;i<tracks2.size();i++)
	{
		glColor3f(GLfloat(colorlist[i].r)/255.0,GLfloat(colorlist[i].g)/255.0,GLfloat(colorlist[i].b)/255.0);
		glBegin(GL_POINTS);
		for(unsigned int j=0;j<tracks2[i].size();j++)
		{
			if(tracks2[i][j].visible)
			{
				//glPushMatrix();
				double x,y,z;
				x=((GLfloat)tracks2[i][j].p.x/(GLfloat)imgcols);
				y=((GLfloat)tracks2[i][j].p.y/(GLfloat)imgrows);
				z=tracks2[i][j].z/255.0;
				glVertex3f(x,y,z);
			}
		}
		glEnd();
	}
	
}

void Viewer::DrawTracks(int end)
{	
	glPointSize(2.0);
	//cout<<end<<endl;	
	for(unsigned int i=0; i<=end && i<tracks.size();i++)
	{
		//int i=end;
		int count=0;
		for(unsigned int j=0;j<tracks[i].size();j++)
		{
			glColor3f(GLfloat(colorlist[j].r)/255.0,GLfloat(colorlist[j].g)/255.0,GLfloat(colorlist[j].b)/255.0);
			if(tracks[i][j].visible)
			{
				//glPushMatrix();
				double x,y,z;
				x=((GLfloat)tracks[i][j].p.x/(GLfloat)imgcols);
				y=1.0-((GLfloat)tracks[i][j].p.y/(GLfloat)imgrows);
				z=tracks[i][j].z/255.0;
				glBegin(GL_POINTS);
					glVertex3f(x,y,z);
				glEnd();
				if(i==end)
				{
					glPushMatrix();
					glTranslatef(x,y,z);
					glutSolidSphere(0.01,10,10);
					glPopMatrix();
					////cout<<i<<" "<<j<<endl;
					//if(tracks[i][j].age < AGE_THRESH -1 || j > contours[i].size()-1)
					//{
					//	count++;
					//	continue;
					//}
					vector<cv::Point> c=tracks[i][j].contour;
					glBegin(GL_POLYGON);
					for(unsigned int ii=0;ii<c.size();ii++)
					{
						double xx,yy,zz;
						xx=((GLfloat)c[ii].x/(GLfloat)imgcols);
						yy=1.0-((GLfloat)c[ii].y/(GLfloat)imgrows);
						zz=z;
						
							glVertex3f(xx,yy,zz);
						
					}
					glEnd();
				}
				
			}
			
		}
		
	}
	
}

void Viewer::Camera(int pickmode,int x,int y)
{
   static double theta = 0;
   //GLint viewport[4];

   ///* Camera setup */
   //glMatrixMode(GL_PROJECTION);
   //glLoadIdentity();
   //if (pickmode == TRUE) {
   //   glGetIntegerv(GL_VIEWPORT,viewport); /* Get the viewport bounds */
   //   gluPickMatrix(x,viewport[3]-y,3.0,3.0,viewport);
   //}
   //gluPerspective(70.0,          /* Field of view */
   //                1.0,          /* aspect ratio  */
   //                10.0,1000.0);  /* near and far  */
   theta=fmod(theta,360.0);
   updownrotate=fmod(updownrotate,360.0);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
   gluLookAt(5*cos(theta*PI/180)*sin(updownrotate*PI/180),
             5*cos(updownrotate*PI/180),
             5*sin(theta*PI/180)*sin(updownrotate*PI/180), 
             0.0,0.0,0.0,                                   /* Focus    */
             0.0,1.0,0.0);                                  /* Up       */
   theta = (cameradirection);
}

void Viewer::KeyboardNormal(unsigned char key,int x, int y)
{
   switch (key) 
   {
   case 27: /* ESC */
   case 'Q':
   case 'q': 
	   exit(0); break;
   case 'x':
	   Xdir+=0.1;
	   break;
   case 'X':
	   Xdir-=0.1;
	   break;
   case 'y':
	   Ydir+=0.1;
	   break;
   case 'Y':
	   Ydir-=0.1;
	   break;
   case 'z':
	   Zdir+=0.1;
	   break;
   case 'Z':
	   Zdir-=0.1;
	   break;
   }
}
void Viewer::SpecialKeyboard(int key,int x, int y)
{
   switch (key) 
   {
   case GLUT_KEY_LEFT:  
	   cameradirection -= 1;
	   rotY-=step;
	   break;
   case GLUT_KEY_RIGHT: 
	   cameradirection += 1;  
	   rotY+=step;
	   break;
   case GLUT_KEY_UP:    
	   updownrotate -= 2;  
	   rotZ-=step;
	   break;
   case GLUT_KEY_DOWN:  
	   updownrotate += 2;
	   rotZ+=step;
	   break;
   }
}
int Viewer::find_num(GLdouble x,GLdouble y,int type)
{
	double dmax=10000;
	int dreturn=-1;
	for(int i=0;i<disppoints.size();i++)
	{
		double d;
		if(type==XY)
			d=sqrt(double((disppoints[i].x - x)*(disppoints[i].x - x)) + double((disppoints[i].y - y)*(disppoints[i].y - y)));
		if(type==YZ)
			d=sqrt(double((disppoints[i].y - x)*(disppoints[i].y - x)) + double((disppoints[i].z - y)*(disppoints[i].z - y)));
		if(type==XZ)
			d=sqrt(double((disppoints[i].x - x)*(disppoints[i].x - x)) + double((disppoints[i].z - y)*(disppoints[i].z - y)));
		//if(d<0.00)
			//return i;
		if(d < dmax)
		{
			dmax=d;
			dreturn = disppoints[i].num;
		}
	}
	if(dmax < 0.01)
		return dreturn;
	else
	return -1;
}
void Viewer::MouseClick(GLint button,GLint state,GLint x,GLint y)
{
	y=window_height - y;
	int type=0;
	double xx;
	double yy;
	if(y < window_height/2 && x > window_width/2)
	{
		return;
	}
	if(y < window_height/2 && x < window_width/2)
	{
		type=XY;
		xx=2*GLdouble(x)/window_width;
		yy=2*GLdouble(y)/window_height;
	}
	if(y > window_height/2 && x < window_width/2)
	{
		type=YZ;
		xx=2*GLdouble(x)/window_width;
		yy=2*(GLdouble(y) - window_height/2)/window_height;
	}
	if(y > window_height/2 && x > window_width/2)
	{
		type=XZ;
		xx=2*(GLdouble(x) - window_width/2)/window_width;
		yy=2*(GLdouble(y) - window_height/2)/window_height;
	}
	if(button==GLUT_LEFT_BUTTON && state==GLUT_DOWN)
	{
		int num=find_num(xx,yy,type);
		if(glutGetModifiers()!=GLUT_ACTIVE_CTRL)
			trackshow.clear();
		if(num<0)
			return;
		if(check_if_set(num))
		{
			for(unsigned int i=0;i<trackshow.size();i++)
			{
				if(trackshow[i]==num)
				{
					trackshow.erase(trackshow.begin()+i);
					break;
				}
			}
		}
		else
			trackshow.push_back(num);
	}
}
void Viewer::reshape(int w, int h)
{
	window_width=w;
	window_height=h;
	//glViewport(0,0,(GLsizei) w, (GLsizei) h);
	Wx = w;
	Wy = h;
}

bool Viewer::check_if_set(int n)
{
	for(unsigned int i=0;i<trackshow.size();i++)
	{
		if(trackshow[i]==n)
			return true;
	}
	return false;
}
void Viewer::display2()
{
	glClear(GL_COLOR_BUFFER_BIT);
	glViewport(0,0,window_width/2,window_height/2);
	glColor3f(1.0,1.0,1.0);
	glBegin(GL_LINES);
		glVertex2f(0,0);
		glVertex2f(1,0);
		glVertex2f(1,0);
		glVertex2f(1,1);
		glVertex2f(1,1);
		glVertex2f(0,1);
		glVertex2f(0,1);
		glVertex2f(0,0);
	glEnd();
	glRasterPos2f(0.01,0.01);
	glutBitmapCharacter(GLUT_BITMAP_8_BY_13,'X');
	glutBitmapCharacter(GLUT_BITMAP_8_BY_13,'Y');
	int difnum=-1;
	for(unsigned int i=0;i<disppoints.size();i++)
	{
		if(check_if_set(disppoints[i].num))
		{
			glColor3f(1.0,1.0,1.0);
			glPointSize(6.0);
			glBegin(GL_POINTS);
				glVertex2f(disppoints[i].x,disppoints[i].y);
			glEnd();		
		}
		glColor3f(disppoints[i].c.r,disppoints[i].c.g,disppoints[i].c.b);
		if(difnum!=disppoints[i].num)
		{
			glPointSize(6.0);
			difnum=disppoints[i].num;
		}
		else
			glPointSize(2.0);
		glBegin(GL_POINTS);
			glVertex2f(disppoints[i].x,disppoints[i].y);
		glEnd();
	}

	glViewport(0,window_height/2,window_width/2,window_height/2);
	glColor3f(1.0,1.0,1.0);
	glBegin(GL_LINES);
		glVertex2f(0,0);
		glVertex2f(1,0);
		glVertex2f(1,0);
		glVertex2f(1,1);
		glVertex2f(1,1);
		glVertex2f(0,1);
		glVertex2f(0,1);
		glVertex2f(0,0);
	glEnd();
	glRasterPos2f(0.01,0.01);
	glutBitmapCharacter(GLUT_BITMAP_8_BY_13,'Y');
	glutBitmapCharacter(GLUT_BITMAP_8_BY_13,'Z');
	for(unsigned int i=0;i<disppoints.size();i++)
	{
		if(check_if_set(disppoints[i].num))
		{
			glColor3f(1.0,1.0,1.0);
			glPointSize(6.0);
			glBegin(GL_POINTS);
				glVertex2f(disppoints[i].y,disppoints[i].z);
			glEnd();		
		}
		glColor3f(disppoints[i].c.r,disppoints[i].c.g,disppoints[i].c.b);
		glPointSize(2.0);
		glBegin(GL_POINTS);
		glVertex2f(disppoints[i].y,disppoints[i].z);
		glEnd();
	}

	glViewport(window_width/2,window_height/2,window_width/2,window_height/2);
	glColor3f(1.0,1.0,1.0);
	glBegin(GL_LINES);
		glVertex2f(0,0);
		glVertex2f(1,0);
		glVertex2f(1,0);
		glVertex2f(1,1);
		glVertex2f(1,1);
		glVertex2f(0,1);
		glVertex2f(0,1);
		glVertex2f(0,0);
	glEnd();
	glRasterPos2f(0.01,0.01);
	glutBitmapCharacter(GLUT_BITMAP_8_BY_13,'X');
	glutBitmapCharacter(GLUT_BITMAP_8_BY_13,'Z');
	for(unsigned int i=0;i<disppoints.size();i++)
	{
		if(check_if_set(disppoints[i].num))
		{
			glColor3f(1.0,1.0,1.0);
			glPointSize(6.0);
			glBegin(GL_POINTS);
				glVertex2f(disppoints[i].x,disppoints[i].z);
			glEnd();		
		}
		glColor3f(disppoints[i].c.r,disppoints[i].c.g,disppoints[i].c.b);
		glPointSize(2.0);
		glBegin(GL_POINTS);
		glVertex2f(disppoints[i].x,disppoints[i].z);
		glEnd();
	}

	glutSwapBuffers();
}
int movestep=1;
void Viewer::timerfunc()
{
	frame++;
	frame=frame%tracks.size();
	rotY=rotY+movestep;
	if(rotY>59)
		movestep=-1;
	if(rotY < -59)
		movestep=1;
	if(frame==0)
	{
		throw ("EXIT");
	}
	uchar *data=(uchar*)malloc(window_width*window_height*3);
	glReadPixels(0,0,window_width,window_height,GL_RGB,GL_UNSIGNED_BYTE,data);
	cv::Mat glCapture(window_height,window_width,CV_8UC3);
	glCapture.data=data;
	cv::flip(glCapture,glCapture,0);
	glVideo.push_back(glCapture);
	//cv::imshow("window",glCapture);
	//cv::waitKey(10);
}