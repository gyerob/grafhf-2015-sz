//=============================================================================================
// Szamitogepes grafika hazi feladat keret. Ervenyes 2014-tol.
// A //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// sorokon beluli reszben celszeru garazdalkodni, mert a tobbit ugyis toroljuk.
// A beadott program csak ebben a fajlban lehet, a fajl 1 byte-os ASCII karaktereket tartalmazhat.
// Tilos:
// - mast "beincludolni", illetve mas konyvtarat hasznalni
// - faljmuveleteket vegezni (printf is fajlmuvelet!)
// - new operatort hivni az onInitialization függvényt kivéve, a lefoglalt adat korrekt felszabadítása nélkül
// - felesleges programsorokat a beadott programban hagyni
// - tovabbi kommenteket a beadott programba irni a forrasmegjelolest kommentjeit kiveve
// ---------------------------------------------------------------------------------------------
// A feladatot ANSI C++ nyelvu forditoprogrammal ellenorizzuk, a Visual Studio-hoz kepesti elteresekrol
// es a leggyakoribb hibakrol (pl. ideiglenes objektumot nem lehet referencia tipusnak ertekul adni)
// a hazibeado portal ad egy osszefoglalot.
// ---------------------------------------------------------------------------------------------
// A feladatmegoldasokban csak olyan gl/glu/glut fuggvenyek hasznalhatok, amelyek
// 1. Az oran a feladatkiadasig elhangzottak ES (logikai AND muvelet)
// 2. Az alabbi listaban szerepelnek:
// Rendering pass: glBegin, glVertex[2|3]f, glColor3f, glNormal3f, glTexCoord2f, glEnd, glDrawPixels
// Transzformaciok: glViewport, glMatrixMode, glLoadIdentity, glMultMatrixf, gluOrtho2D,
// glTranslatef, glRotatef, glScalef, gluLookAt, gluPerspective, glPushMatrix, glPopMatrix,
// Illuminacio: glMaterialfv, glMaterialfv, glMaterialf, glLightfv
// Texturazas: glGenTextures, glBindTexture, glTexParameteri, glTexImage2D, glTexEnvi,
// Pipeline vezerles: glShadeModel, glEnable/Disable a kovetkezokre:
// GL_LIGHTING, GL_NORMALIZE, GL_DEPTH_TEST, GL_CULL_FACE, GL_TEXTURE_2D, GL_BLEND, GL_LIGHT[0..7]
//
// NYILATKOZAT
// ---------------------------------------------------------------------------------------------
// Nev    : Gyenei Robert
// Neptun : CUDQRA
// ---------------------------------------------------------------------------------------------
// ezennel kijelentem, hogy a feladatot magam keszitettem, es ha barmilyen segitseget igenybe vettem vagy
// mas szellemi termeket felhasznaltam, akkor a forrast es az atvett reszt kommentekben egyertelmuen jeloltem.
// A forrasmegjeloles kotelme vonatkozik az eloadas foliakat es a targy oktatoi, illetve a
// grafhazi doktor tanacsait kiveve barmilyen csatornan (szoban, irasban, Interneten, stb.) erkezo minden egyeb
// informaciora (keplet, program, algoritmus, stb.). Kijelentem, hogy a forrasmegjelolessel atvett reszeket is ertem,
// azok helyessegere matematikai bizonyitast tudok adni. Tisztaban vagyok azzal, hogy az atvett reszek nem szamitanak
// a sajat kontribucioba, igy a feladat elfogadasarol a tobbi resz mennyisege es minosege alapjan szuletik dontes.
// Tudomasul veszem, hogy a forrasmegjeloles kotelmenek megsertese eseten a hazifeladatra adhato pontokat
// negativ elojellel szamoljak el es ezzel parhuzamosan eljaras is indul velem szemben.
//=============================================================================================

#define _USE_MATH_DEFINES
#include <math.h>
#include <stdlib.h>

#if defined(__APPLE__)
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#else
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#include <windows.h>
#endif
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#endif

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Innentol modosithatod...
//#include <iostream>
//using namespace std;
//--------------------------------------------------------
// 3D Vektor
//--------------------------------------------------------
struct Vector {
	float x, y, z;

	Vector() {
		x = y = z = 0;
	}
	Vector(float x0, float y0, float z0 = 0) {
		x = x0;
		y = y0;
		z = z0;
	}
	Vector operator*(float a) {
		return Vector(x * a, y * a, z * a);
	}
	Vector operator+(const Vector& v) {
		return Vector(x + v.x, y + v.y, z + v.z);
	}
	Vector operator-(const Vector& v) {
		return Vector(x - v.x, y - v.y, z - v.z);
	}
	float operator*(const Vector& v) { 	// dot product
		return (x * v.x + y * v.y + z * v.z);
	}
	Vector operator%(const Vector& v) { 	// cross product
		return Vector(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
	}
	float Length() {
		return sqrt(x * x + y * y + z * z);
	}
};

//--------------------------------------------------------
// Spektrum illetve szin
//--------------------------------------------------------
struct Color {
	float r, g, b;

	Color() {
		r = g = b = 0;
	}
	Color(float r0, float g0, float b0) {
		r = r0;
		g = g0;
		b = b0;
	}
	Color operator*(float a) {
		return Color(r * a, g * a, b * a);
	}
	Color operator*(const Color& c) {
		return Color(r * c.r, g * c.g, b * c.b);
	}
	Color operator+(const Color& c) {
		return Color(r + c.r, g + c.g, b + c.b);
	}
};

const int screenWidth = 600;	// alkalmazás ablak felbontása
const int screenHeight = 600;

//Color image[screenWidth * screenHeight];	// egy alkalmazás ablaknyi kép
Color image[screenWidth][screenHeight];	// egy alkalmazás ablaknyi kép

struct Parabola {
	/*
	 * egyenes egyenlet
	 * pont(x0,y0)+irányvektor(v1,v2)
	 * v2x-v1y = v2x0-v1y0
	 *
	 * normálvektorral(A,B)
	 *  Ax+By = Ax0+By0
	 *
	 * pont távolsága egyenestõl
	 * |Ax+By+C|/sqrt(A^2+B^2)
	 */
	Vector directixpoints[2];
	Vector directix;
	Vector normal;
	Vector focus;
	Vector intersectpoint;
	Vector intersectdirection;
	Vector tangentpoint;

	Vector temp, temp2;

	bool intersected;
	int numpoints;
	float c;
	float ldist, fdist;
	float worldx, worldy;

	Parabola() {
		numpoints = 0;
		c = 0.0f;
		intersected = false;
		fdist = 0.0f;
		ldist = 0.0f;

		worldx = 0.0f;
		worldy = 0.0f;
	}

	void addCP(float x, float y) {
		if (numpoints < 2) {
			directixpoints[numpoints].x = x;
			directixpoints[numpoints].y = y;
			numpoints++;
		} else if (numpoints == 2) {
			focus.x = x;
			focus.y = y;
			directix = directixpoints[1] - directixpoints[0];
//			directix = directix * 1000;
			normal.x = directix.y;
			normal.y = -directix.x;
			normal = normal * (1 / normal.Length());

			c = normal.x * directixpoints[0].x + normal.y * directixpoints[0].y;
			//cout << "normál: " << normal.x << " " << normal.y << " c= " << c					<< "\n" << flush;
			//c=directix.y*directixpoints[0].x-directix.x*directixpoints[0].y;
			numpoints++;
		}
	}

	bool isintersecting(Vector v) {
		fdist = (v - focus).Length();
		ldist = (fabs(normal.x * v.x + normal.y * v.y - c))
				/ (sqrt(normal.x * normal.x + normal.y * normal.y));
		Vector n2 = normal;

		if (fabs((fabs(fdist) - fabs(ldist))) < 0.01f) {// && !intersected) {
			//http://www.inf.unideb.hu/oktatas/mobidiak/Papp_Ildiko/Konstruktiv_geometria/gorbe.pdf 8. oldal
			intersected = true;
			intersectpoint = v;
			temp = (v - focus);
			float intdist = temp.Length();

			bool valto = false;
			for (float Y = 0; Y < screenHeight; Y++) {
				worldy =
						((((screenHeight - (float) Y) / screenHeight) * 2) - 1);

				if (fabs(focus.x * normal.x + worldy * normal.y - c) <= 0.02f
						&& !valto) {
					if ((directixpoints[0].x < directixpoints[1].x)
							&& focus.y < worldy) {
						n2 = normal * -1;
						//cout << worldy << "\n" << flush;
						valto = true;
					} else if ((directixpoints[0].x > directixpoints[1].x)
							&& focus.y > worldy) {
						n2 = normal * -1;
						//cout << worldy << "\n" << flush;
						valto = true;
					}
				}

			}

			if (!valto) {
				worldy = (c - normal.x * focus.x) / normal.y;
				//cout << worldy << "\n" << flush;
				if ((directixpoints[0].x < directixpoints[1].x)
						&& focus.y < worldy) {
					n2 = normal * -1;
				} else if ((directixpoints[0].x > directixpoints[1].x)
						&& focus.y > worldy) {
					n2 = normal * -1;
				}
			}

			temp2 = v + (n2 * intdist);
			tangentpoint = temp2 + ((focus - temp2) * 0.5f);
			intersectdirection = (tangentpoint - v) * (10/(tangentpoint - v).Length());
			return true;
		} else {
			return false;
		}
	}

	bool isinside(Vector v) {
		fdist = (v - focus).Length();
		ldist = (fabs(normal.x * v.x + normal.y * v.y - c))
				/ (sqrt(normal.x * normal.x + normal.y * normal.y));
		//float ldist = (fabs(directix.y*v.x+directix.x*v.y+c))/(sqrt(directix.y*directix.y+directix.x*directix.x));
		//if (fabs(fabs(fdist) - (ldist)) < 0.001) {
		if (fabs(fdist) <= (ldist)) {
			return true;
		} else
			return false;
	}

	void draw() {
		/*if (numpoints > 1) {
		 glColor3f(0, 0, 0);
		 glBegin(GL_POINTS);
		 //glBegin(GL_LINE_STRIP);
		 for (float Y = 0; Y < screenHeight; Y++) {
		 for (float X = 0; X < screenWidth; X++) {
		 //for (float Y = 0; Y < screenHeight; Y += 0.5f) {
		 //	for (float X = 0; X < screenWidth; X += 0.5f) {
		 worldx = -((((screenWidth - (float) X) / screenWidth) * 2)
		 - 1);
		 worldy = ((((screenHeight - (float) Y) / screenHeight) * 2)
		 - 1);

		 if (fabs(worldx * normal.x + worldy * normal.y - c)
		 <= 0.002f) {
		 glVertex2f(worldx, worldy);
		 }
		 }
		 }
		 glEnd();
		 }*/
		if (numpoints > 2) {
			//cout << "parabolarajz\n" << flush;
			//glPointSize(1);
			glBegin(GL_POINTS);
			//glBegin(GL_LINE_STRIP);
			for (float Y = 0; Y < screenHeight; Y++) {
				for (float X = 0; X < screenWidth; X++) {
					//for (float Y = 0; Y < screenHeight; Y += 0.5f) {
					//	for (float X = 0; X < screenWidth; X += 0.5f) {
					worldx = -((((screenWidth - (float) X) / screenWidth) * 2)
							- 1);
					worldy = ((((screenHeight - (float) Y) / screenHeight) * 2)
							- 1);
					Vector v(worldx, worldy, 0);
					if (isinside(v)) {
						glColor3f(1, 1, 0);
						glVertex2f(worldx, worldy);
						//image[screenHeight-1-Y][X] = Color(1, 1, 0);
					}
				}
			}
			glEnd();
			/*
			 bool first = false;
			 Vector f;

			 glColor3f(1, 1, 0);
			 glBegin(GL_TRIANGLE_FAN);
			 glVertex2f(focus.x, focus.y);
			 for (float X = -300; X < screenHeight + 300; X++) {
			 for (float Y = -300; Y < screenWidth + 300; Y++) {
			 worldx = -((((screenWidth - (float) X) / screenWidth) * 2)
			 - 1);
			 worldy = ((((screenHeight - (float) Y) / screenHeight) * 2)
			 - 1);
			 Vector v(worldx, worldy, 0);
			 if (isinside(v)) {
			 if (!first) {
			 f = v;
			 first = true;
			 }
			 glVertex2f(worldx, worldy);
			 }
			 }
			 }
			 glVertex2f(f.x, f.y);
			 glEnd();
			 */
			//glPointSize(4);
			/*glBegin(GL_POINTS);
			 glColor3f(0, 1, 0);
			 glVertex2f(tangentpoint.x, tangentpoint.y);
			 glColor3f(0, 0, 0);
			 glVertex2f(temp.x, temp.y);
			 glColor3f(1, 1, 0);
			 glVertex2f(temp2.x, temp2.y);
			 glEnd();*/
		}

		if (intersected) {
			glColor3f(0, 0.5f, 0);
			//glLineWidth(3);
			//glBegin(GL_LINES);
			//glVertex2f(intersectdirection.x, intersectdirection.y);
			//glVertex2f(tangentpoint.x, tangentpoint.y);
			//glVertex2f(intersectpoint.x, intersectpoint.y);

			/*
			 glVertex2f(intersectpoint.x + intersectdirection.x,
			 intersectpoint.y + intersectdirection.y);
			 glVertex2f(intersectpoint.x, intersectpoint.y);
			 glVertex2f(intersectpoint.x - intersectdirection.x,
			 intersectpoint.y - intersectdirection.y);
			 */
			//glEnd();
			glBegin(GL_LINES);
			glVertex2f(intersectpoint.x - intersectdirection.x,
					intersectpoint.y - intersectdirection.y);
			glVertex2f(intersectpoint.x + intersectdirection.x,
					intersectpoint.y + intersectdirection.y);
			glEnd();
		}

		/*
		 if (numpoints > 1) {
		 glColor3f(1, 1, 1);
		 glBegin(GL_LINES);
		 glVertex2f(directixpoints[0].x, directixpoints[0].y);
		 glVertex2f(directixpoints[1].x, directixpoints[1].y);
		 glEnd();
		 }
		 */
	}
};

Parabola parabola;

struct Spline {
	Vector cp[100];
	int numcp;
	float ido[100];
	bool intersected;
	Vector intersectpoint;
	float intersecttime;
	int intersecti;

	Spline() {
		numcp = 0;
		intersecti = 0;
		intersected = false;
		intersecttime = 0.0f;
	}

	void addCP(float x, float y) {
		if (numcp < 99) {
			cp[numcp].x = x;
			cp[numcp].y = y;
			ido[numcp] = glutGet(GLUT_ELAPSED_TIME);
			//cout << time[numcp] << "\n" << flush;
			numcp++;
			cp[numcp].x = cp[0].x;
			cp[numcp].y = cp[0].y;
			ido[numcp] = ido[numcp - 1] + 500;
		}
	}

	Vector speed(int i) {
		if (i == 0 || i == (numcp)) {
			return Vector(0, 0, 0);
		} else {
			Vector a(cp[i + 1] - cp[i]);
			Vector b(cp[i] - cp[i - 1]);
			float t1 = ido[i + 1] - ido[i];
			float t2 = ido[i] - ido[i - 1];
			return ((a * (1 / t1)) + (b * (1 / t2))) * 0.5f;
		}
	}

	Vector Hermite(int i, float t) {
		Vector a0, a1, a2, a3;
		a0 = cp[i];
		a1 = speed(i);
		a2 =
				(((cp[i + 1] - cp[i]) * 3)
						* (1 / powf((ido[i + 1] - ido[i]), 2)))
						- ((speed(i + 1) + (speed(i) * 2))
								* (1 / (ido[i + 1] - ido[i])));
		a3 = (((cp[i] - cp[i + 1]) * 2) * (1 / powf((ido[i + 1] - ido[i]), 3)))
				+ ((speed(i + 1) + speed(i))
						* (1 / (powf(ido[i + 1] - ido[i], 2))));

		return (a3 * powf(t - ido[i], 3) + a2 * powf(t - ido[i], 2)
				+ a1 * (t - ido[i]) + a0);
	}

	Vector Hermitederive(int i, float t) {
		Vector a0, a1, a2, a3;
		a0 = cp[i];
		a1 = speed(i);
		a2 =
				(((cp[i + 1] - cp[i]) * 3)
						* (1 / powf((ido[i + 1] - ido[i]), 2)))
						- ((speed(i + 1) + (speed(i) * 2))
								* (1 / (ido[i + 1] - ido[i])));
		a3 = (((cp[i] - cp[i + 1]) * 2) * (1 / powf((ido[i + 1] - ido[i]), 3)))
				+ ((speed(i + 1) + speed(i))
						* (1 / (powf(ido[i + 1] - ido[i], 2))));

		return (a3 * powf(t - ido[i], 2)) * 3 + (a2 * (t - ido[i])) * 2 + a1;
	}

	void draw() {
		if (numcp > 0) {
			//kör sugár 5m -> 0.01f
			for (int i = 0; i < numcp; i++) {
				glColor3f(1, 0, 0);
				glBegin(GL_TRIANGLE_FAN);
				glVertex2f(cp[i].x, cp[i].y);
				for (int j = 0; j < 360; j++) {
					//cos/sin (j*pi/180)*sugár+eltolás
					glVertex2f(cos(j * 3.14159 / 180.0) * 0.01 + cp[i].x,
							sin(j * 3.14159 / 180.0) * 0.01 + cp[i].y);
				}
				glEnd();

				glColor3f(1, 1, 1);
				glBegin(GL_LINE_STRIP);
				for (int j = 0; j < 360; j++) {
					//cos/sin (j*pi/180)*sugár+eltolás
					glVertex2f(cos(j * 3.14159 / 180.0) * 0.01 + cp[i].x,
							sin(j * 3.14159 / 180.0) * 0.01 + cp[i].y);
				}
				glVertex2f(cos(0) * 0.01 + cp[i].x, sin(0) * 0.01 + cp[i].y);
				glEnd();
			}
		}
		/*
		 if (numcp > 1) {
		 glColor3f(0, 0, 1);
		 glBegin(GL_LINE_STRIP);
		 for (int i = 0; i < numcp; i++) {
		 glVertex2f(cp[i].x, cp[i].y);
		 }
		 glVertex2f(cp[0].x, cp[0].y);
		 glEnd();
		 }
		 */
		if (numcp > 1) {
			glColor3f(1, 1, 1);
			glBegin(GL_LINE_STRIP);
			Vector v;
			for (int i = 0; i < numcp + 1; i++) {
				//cout<<i<<"\n"<<flush;
				for (float t = ido[i]; t < ido[i + 1]; t += 1.0f) {
					v = Hermite(i, t);
					if (parabola.isintersecting(v)) {	// && !intersected) {
						intersectpoint = v;
						intersecttime = t;
						intersecti = i;
						intersected = true;
					}
					glVertex2f(v.x, v.y);
				}
			}
			glEnd();
			if (intersected) {
				glColor3f(0, 0.7f, 0);

				//glBegin(GL_POINTS);
				//glVertex2f(intersectpoint.x, intersectpoint.y);
				//glEnd();

				glBegin(GL_LINES);
				//for (float t = (ido[intersecti] - 100);	t < ido[intersecti] + 100; t++) {
				v = Hermitederive(intersecti, intersecttime);
				v = v * (10/v.Length());
				glVertex2f(intersectpoint.x + v.x, intersectpoint.y + v.y);
				glVertex2f(intersectpoint.x - v.x, intersectpoint.y - v.y);
				//}
				glEnd();
			}
		}
	}
};

//float camx = 0.0f;
//float camy = 0.0f;
float camx = -0.5f;
float camy = -0.5f;
float dx = 0.000004f;
float dy = 0.000006f;
float tempd = 0.0f;

long time = 0;
long starttime = 0;
long oldtime = 0;
long timediff = 0;
long seconds = 0;

bool spacepressed = false;
Spline crs;

// Inicializacio, a program futasanak kezdeten, az OpenGL kontextus letrehozasa utan hivodik meg (ld. main() fv.)
void onInitialization() {
	glViewport(0, 0, screenWidth, screenHeight);
}

// Rajzolas, ha az alkalmazas ablak ervenytelenne valik, akkor ez a fuggveny hivodik meg
void onDisplay() {
	glClearColor(0.0f, 1.0f, 1.0f, 1.0f);	// torlesi szin beallitasa
//glClearColor(0.0f, 0.0f, 0.0f, 1.0f);	// torlesi szin beallitasa
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // kepernyo torles

// Peldakent atmasoljuk a kepet a rasztertarba
//glDrawPixels(screenWidth, screenHeight, GL_RGB, GL_FLOAT, image);

	parabola.draw();
	crs.draw();
	/*
	 glPointSize(6);
	 glColor3f(0, 0, 0);
	 glBegin(GL_POINTS);
	 glVertex2f(-0.9f, 0.0f);
	 glVertex2f(0.9f, 0.0f);
	 glVertex2f(0.0f, 0.9f);
	 glVertex2f(0.0f, -0.9f);
	 glEnd();
	 */

	glColor3f(1, 1, 1);
	glBegin(GL_LINES);
	glVertex2f(-1.0f, 0.0f);
	glVertex2f(1.0f, 0.0f);
	glEnd();
	glBegin(GL_LINES);
	glVertex2f(0.0f, 1.0f);
	glVertex2f(0.0f, -1.0f);
	glEnd();

	/*
	 glPointSize(4);
	 glBegin(GL_POINTS);
	 for (float i = -1.0f; i < 1.1f; i += 0.1f) {
	 for (float j = -1.0f; j < 1.1f; j += 0.1f) {
	 glVertex2f(i, j);
	 }
	 }
	 glEnd();*/

	glutSwapBuffers();     			// Buffercsere: rajzolas vege
}

// Billentyuzet esemenyeket lekezelo fuggveny (lenyomas)
void onKeyboard(unsigned char key, int x, int y) {
	/*if (key == 'i') {
	 // cout << "d" << flush;
	 //camy += 0.1f;
	 //cout << camx << " " << camy << "\n" << flush;
	 glLoadIdentity();
	 gluOrtho2D(-0.5, 0.5, -0.5, 0.5);
	 glutPostRedisplay(); 		// d beture rajzold ujra a kepet
	 }*/

	if (key == ' ') {
		if (!spacepressed) {
			spacepressed = true;
			oldtime = glutGet(GLUT_ELAPSED_TIME);
			//glScalef(2, 2, 2);
			//cout << "start: " << oldtime << "\n" << flush;
		} else {
			//oldtime = glutGet(GLUT_ELAPSED_TIME);
			//cout << "end: " << glutGet(GLUT_ELAPSED_TIME) << "\n" << flush;
		}
		glutPostRedisplay();
	}
}

// Billentyuzet esemenyeket lekezelo fuggveny (felengedes)
void onKeyboardUp(unsigned char key, int x, int y) {

}

// Eger esemenyeket lekezelo fuggveny
void onMouse(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) { // A GLUT_LEFT_BUTTON / GLUT_RIGHT_BUTTON illetve GLUT_DOWN / GLUT_UP
		float X = -((((screenWidth - (float) x) / screenWidth) * 2) - 1);
		float Y = ((((screenHeight - (float) y) / screenHeight) * 2) - 1);

		if (!spacepressed) {
			crs.addCP(X, Y);
			parabola.addCP(X, Y);
		}

//cout << "x: " << X << " y: " << Y << "\n" << flush;
		glutPostRedisplay(); 		// Ilyenkor rajzold ujra a kepet
	}
}

// Eger mozgast lekezelo fuggveny
void onMouseMotion(int x, int y) {

}

// `Idle' esemenykezelo, jelzi, hogy az ido telik, az Idle esemenyek frekvenciajara csak a 0 a garantalt minimalis ertek
void onIdle() {
//3 méter = 0.006
//2 méter = 0.004
	time = glutGet(GLUT_ELAPSED_TIME);	// program inditasa ota eltelt ido

	if (spacepressed) {
//if (false) {

		for (float ts = (float) oldtime; ts < (float) time; ts += 1) {
			camx += dx;
			camy += dy;

			if (camx >= 0.000000f) {
				if (dy > 0.000000f) {
					tempd = dx;
					dx = -dy;
					dy = tempd;
				} else {
					tempd = dx;
					dx = dy;
					dy = -tempd;
				}
				//cout << "camx>0.5\n" << flush;
				//cout << camx << " " << camy << "\n" << flush;
				camx += dx;
				camy += dy;
				//cout << camx << " " << camy << "\n" << flush;
			} else if (camx <= -1.000000f) {
				if (dy > 0.000000f) {
					tempd = dx;
					dx = dy;
					dy = -tempd;
				} else {
					tempd = dx;
					dx = -dy;
					dy = tempd;
				}
				camx += dx;
				camy += dy;
				//cout << "camx<-0.5\n" << flush;
			}

			if (camy >= 0.000000f) {
				if (dx > 0.000000f) {
					tempd = dx;
					dx = dy;
					dy = -tempd;
				} else {
					tempd = dx;
					dx = -dy;
					dy = tempd;
				}
				camx += dx;
				camy += dy;
				//cout << "camy>0.5\n" << flush;
			} else if (camy <= -1.000000f) {
				if (dx > 0.000000f) {
					tempd = dx;
					dx = -dy;
					dy = tempd;
				} else {
					tempd = dx;
					dx = dy;
					dy = -tempd;
				}
				camx += dx;
				camy += dy;

				//cout << "camy<-0.5\n" << flush;
			}

			/*
			 if (camx >= 1.000000f) {
			 if (dy > 0.000000f) {
			 tempd = dx;
			 dx = -dy;
			 dy = tempd;
			 } else {
			 tempd = dx;
			 dx = dy;
			 dy = -tempd;
			 }
			 //cout << "camx>0.5\n" << flush;
			 //cout << camx << " " << camy << "\n" << flush;
			 camx += dx;
			 camy += dy;
			 //cout << camx << " " << camy << "\n" << flush;
			 } else if (camx <= -1.000000f) {
			 if (dy > 0.000000f) {
			 tempd = dx;
			 dx = dy;
			 dy = -tempd;
			 } else {
			 tempd = dx;
			 dx = -dy;
			 dy = tempd;
			 }
			 camx += dx;
			 camy += dy;
			 //cout << "camx<-0.5\n" << flush;
			 }

			 if (camy >= 1.000000f) {
			 if (dx > 0.000000f) {
			 tempd = dx;
			 dx = dy;
			 dy = -tempd;
			 } else {
			 tempd = dx;
			 dx = -dy;
			 dy = tempd;
			 }
			 camx += dx;
			 camy += dy;
			 //cout << "camy>0.5\n" << flush;
			 } else if (camy <= -1.000000f) {
			 if (dx > 0.000000f) {
			 tempd = dx;
			 dx = -dy;
			 dy = tempd;
			 } else {
			 tempd = dx;
			 dx = dy;
			 dy = -tempd;
			 }
			 camx += dx;
			 camy += dy;

			 //cout << "camy<-0.5\n" << flush;
			 }*//*
			 glLoadIdentity();
			 gluLookAt(camx, camy, 1.0f, camx, camy, 0.0f, 0.0f, 1.0f, 0.0f);
			 glScalef(2, 2, 2);*/
		}
		glLoadIdentity();
		//gluLookAt(camx, camy, 1.0f, camx, camy, 0.0f, 0.0f, 1.0f, 0.0f);
		//glScalef(2, 2, 2);

		gluOrtho2D(camx, camx + 1.0f, camy, camy + 1.0f);

		glutPostRedisplay();

		oldtime = time;
	}

	//glutPostRedisplay();
}

// ...Idaig modosithatod
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// A C++ program belepesi pontja, a main fuggvenyt mar nem szabad bantani
int main(int argc, char **argv) {
	glutInit(&argc, argv); 				// GLUT inicializalasa
	glutInitWindowSize(600, 600); // Alkalmazas ablak kezdeti merete 600x600 pixel
	glutInitWindowPosition(100, 100); // Az elozo alkalmazas ablakhoz kepest hol tunik fel
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH); // 8 bites R,G,B,A + dupla buffer + melyseg buffer

	glutCreateWindow("Grafika hazi feladat"); // Alkalmazas ablak megszuletik es megjelenik a kepernyon

	glMatrixMode(GL_MODELVIEW);	// A MODELVIEW transzformaciot egysegmatrixra inicializaljuk
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);// A PROJECTION transzformaciot egysegmatrixra inicializaljuk
	glLoadIdentity();

	onInitialization();	// Az altalad irt inicializalast lefuttatjuk

	glutDisplayFunc(onDisplay);		// Esemenykezelok regisztralasa
	glutMouseFunc(onMouse);
	glutIdleFunc(onIdle);
	glutKeyboardFunc(onKeyboard);
	glutKeyboardUpFunc(onKeyboardUp);
	glutMotionFunc(onMouseMotion);

	glutMainLoop();					// Esemenykezelo hurok

	return 0;
}

