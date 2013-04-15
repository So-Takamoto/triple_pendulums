#include <vector>
#include <deque>
#include <string>
#include <sstream>
#include <iomanip>
#include "freeglut.h"
#pragma comment( lib, "freeglut.lib" )

#include "pendulums.h"

const int Windowsize = 600;
const int PointNum = 1000;
pendulums::pendClass pend;
std::deque<std::vector<std::pair<double,double> > > locus;
bool showLocus = true;

void locusAdd(){
	locus.push_front(pend.vertices());
	if(locus.size() > PointNum){
		locus.pop_back();
	}
}

void timertick(int value)
{
	const int repeat = 10;
	const int sleepms = 16;
	for(int i = 0; i < repeat; i++){
		pend.move(sleepms / 1000.0 / static_cast<double>(repeat));
		locusAdd();
	}
    glutPostRedisplay();
    glutTimerFunc(sleepms, timertick, 0);
}


void keydown(unsigned char key, int x, int y){
	switch(key){
	case '1':
	case '2':
	case '3':
		pend.setIntegrate(key - '0');
		break;
	case 'i':
		pend.initialize();
		locus.clear();
		break;
	case 'l':
		showLocus = !showLocus;
		break;
	case 'q':
		exit(0);
	}
}

void display_string(float x, float y, std::string str){
	glDisable(GL_TEXTURE_2D);
	glColor4f(0.7f,0.8f,0.8f,1.0f);
	glRasterPos3f(x, y, -1.0f);
	const char* p = str.c_str();
	while (*p != '\0') glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *p++);
 	glEnable(GL_TEXTURE_2D);
}

void display_params(){
	std::stringstream ss;

	display_string(-0.9f, 0.9f, "<Keys> 1/2/3:Methods, i:init, l:locus q:quit");

	ss.setf(std::ios::showpoint);
	ss.str("");
	ss  << "Current Method: " << pend.getIntegrateName();
	display_string(-0.9f, 0.82f, ss.str());
	ss.str("");
	ss << "energy: "
		<< std::setprecision(5) << std::setw(6) << pend.energy() << "(T:"
		<< std::setprecision(3) << std::setw(6) << pend.T() << ", U:"
		<< std::setprecision(3) << std::setw(6) << pend.U() << ")";
	display_string(-0.9f, 0.74f, ss.str());
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	float colors[3][3] = {{0.275f, 0.510f, 0.706f},{0.196f, 0.804f, 0.196f},{0.933f, 0.902f, 0.522f}};

	if(showLocus){
		glPointSize(1.0f);
		glBegin(GL_POINTS);
		int pCount = 0;
		for(auto it = locus.begin(); it != locus.end(); ++it){
			for(int i = 0; i < 3; i++){
				float colorMul = 1.0f - static_cast<float>(static_cast<double>(pCount)/static_cast<double>(locus.size()));
				glColor3f(colors[i][0]*colorMul, colors[i][1]*colorMul, colors[i][2]*colorMul);
				glVertex2d(it->at(i+1).first, it->at(i+1).second);
			}
			pCount++;
		}
		glEnd();
	}
	glLineWidth(4.0f);
	glPointSize(6.0f);
	glColor3f(1.000f, 0.498f, 0.000f);
	for(int i = 0; i < 3; i++){
		glBegin(GL_LINES);
		glVertex2d(locus.front().at(i).first, locus.front().at(i).second);
		glVertex2d(locus.front().at(i+1).first, locus.front().at(i+1).second);
		glEnd();
	}
	glColor3f(1.000f, 1.000f, 1.000f);
	glBegin(GL_POINTS);
	for(int i = 0; i < 3; i++){
		glVertex2d(locus.front().at(i+1).first, locus.front().at(i+1).second);
	}
	glEnd();
	
	display_params();

	glutSwapBuffers();
}
int main(int argc, char** argv){
	locusAdd();

    glutInitWindowPosition(100, 100);
	glutInitWindowSize(Windowsize, Windowsize);
 
    glutInit(&argc, argv);
 
    glutCreateWindow("Triple Pendulums");
 
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
 
    glutDisplayFunc(display);
    glutTimerFunc(16, timertick, 0);
	glutKeyboardFunc(keydown);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    glutMainLoop();

	return 0;
}
