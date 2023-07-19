//
//  main.c
//  normal-cubemap
//
//  Created by Igor on 18/07/2023.
//

#include <stdio.h>
#include <stdlib.h>
#include <GLUT/GLUT.h>

float gAngle = 0.f;
float gLightPosition[] = { -5.f, 5.f, -5.f, 0.f };

void init(void);
void display(void);
void reshape(int w, int h);
void idle(void);
void keyDown(unsigned char key, int x, int y);

int main(int argc, char * argv[]) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  glutInitWindowSize(800, 600);
  glutCreateWindow("Normalization Cube Map");

  init();

  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutIdleFunc(idle);
  glutKeyboardFunc(keyDown);
  glutMainLoop();

  return 0;
}

void init(void) {
  glClearColor(0.f, 0.f, 0.f, 0.f);
  glShadeModel(GL_SMOOTH);
  glEnable(GL_DEPTH_TEST);
  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

  float diffuse[] = { 0.2f, 0.5f, 0.7f, 1.f };
  float specular[] = { 1.f, 1.f, 1.f, 1.f };
  float shininess[] = { 50.f };


  glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
  glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
  glMaterialfv(GL_FRONT, GL_SHININESS, shininess);
  glLightfv(GL_LIGHT0, GL_POSITION, gLightPosition);

  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
}

void display(void) {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  glPushMatrix();
  glDisable(GL_LIGHTING);

  glTranslatef(gLightPosition[0], gLightPosition[1], gLightPosition[2]);

  glutSolidSphere(0.1, 4, 4);

  glEnable(GL_LIGHTING);
  glPopMatrix();

  glPushMatrix();

  glRotatef(gAngle, 0.1f, 0.2f, 0.3f);

  glutSolidTorus(0.5, 1.0, 32, 32);

  glPopMatrix();

  glutSwapBuffers();
}

void reshape(int w, int h) {
  glViewport(0, 0, (GLsizei)w, (GLsizei)h);

  double width = (double)w;
  double height = (double)h;

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(60.0, width / height, 0.1, 1000.0);
  gluLookAt(-2.5, 2.5, -2.5,
            0.0, 0.0, 0.0,
            0.0, 1.0, 0.0);
}

void idle(void) {
  gAngle += 1.f;
  glutPostRedisplay();
}

void keyDown(unsigned char key, int x, int y) {
  if (key == 27) {
    exit(0);
  }
}
