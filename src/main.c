#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <GLUT/GLUT.h>

#define BMP_FILE_HEADER_SIZE 14

float gAngle = 0.f;
float gLightPosition[] = { 5.f, 5.f, 5.f, 0.f };
double gZoom = 5.0;
double gCameraAngle = 270.0;
double gCameraY = 5.0;
double gWidth;
double gHeight;

GLuint gNegXTexture;
GLuint gNegYTexture;
GLuint gNegZTexture;
GLuint gPosXTexture;
GLuint gPosYTexture;
GLuint gPosZTexture;

GLuint cubeMap;

void init(void);
void display(void);
void reshape(int w, int h);
void idle(void);
void keyDown(unsigned char key, int x, int y);
void updateCamera(void);
bool loadBmp(GLenum, const char *);
bool loadTexture2d(const char *, GLuint *);
bool loadCubeMap(GLuint *);

int main(int argc, char * argv[]) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  glutInitWindowSize(800, 600);
  glutCreateWindow("Normalization Cube Map");
  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutIdleFunc(idle);
  glutKeyboardFunc(keyDown);

  init();

  glutMainLoop();

  return 0;
}

void init(void) {
  glClearColor(0.f, 0.f, 0.f, 0.f);
  glColor3f(1.f, 1.f, 1.f);
  glShadeModel(GL_SMOOTH);
  glEnable(GL_DEPTH_TEST);
  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

  float ambient[] = {0.5f, 0.5f, 0.8f, 1.f};
  float diffuse[] = { 1.f, 1.f, 1.f, 1.f };
  float specular[] = { 1.f, 1.f, 1.f, 1.f };
  float shininess[] = { 50.f };


  glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
  glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
  glMaterialfv(GL_FRONT, GL_SHININESS, shininess);
  glLightfv(GL_LIGHT0, GL_POSITION, gLightPosition);

  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);

  if(!loadTexture2d("../assets/left.bmp", &gNegXTexture) ||
     !loadTexture2d("../assets/bottom.bmp", &gNegYTexture) ||
     !loadTexture2d("../assets/front.bmp", &gNegZTexture) ||
     !loadTexture2d("../assets/right.bmp", &gPosXTexture) ||
     !loadTexture2d("../assets/top.bmp", &gPosYTexture) ||
     !loadTexture2d("../assets/back.bmp", &gPosZTexture)) {
    perror("Failed to sky box textures");
    exit(EXIT_FAILURE);
  } else {
    puts("Loaded skybox textures successfully");
  }

  if(!loadCubeMap(&cubeMap)) {
    perror("Failed to load cube map");
    exit(EXIT_FAILURE);
  } else {
    puts("Loaded cube map successfully");
  }
}

void display(void) {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(60.0, gWidth / gHeight, 0.1, 1000.0);

  double camX = cos(gCameraAngle * M_PI / 180.0) * gZoom;
  double camZ = sin(gCameraAngle * M_PI / 180.0) * gZoom;

  gluLookAt(camX, gCameraY, camZ,
            0.0, 0.0, 0.0,
            0.0, 1.0, 0.0);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  /* Draw sky box */
  glPushMatrix();
  glDisable(GL_LIGHTING);

  glScalef(500.f, 500.f, 500.f);

  glColor3f(1.f, 1.f, 1.f);
  glDisable(GL_TEXTURE_CUBE_MAP);
  glEnable(GL_TEXTURE_2D);

  glBindTexture(GL_TEXTURE_2D, gNegZTexture);

  glBegin(GL_QUADS);

  glTexCoord2f(1.f, 0.f);
  glVertex3f(-1.f, -1.f, 1.f);
  glTexCoord2f(0.f, 0.f);
  glVertex3f(1.f, -1.f, 1.f);
  glTexCoord2f(0.f, 1.f);
  glVertex3f(1.f, 1.f, 1.f);
  glTexCoord2f(1.f, 1.f);
  glVertex3f(-1.f, 1.f, 1.f);

  glEnd();

  glBindTexture(GL_TEXTURE_2D, gNegXTexture);

  glBegin(GL_QUADS);

  glTexCoord2f(1.f, 0.f);
  glVertex3f(1.f, -1.f, 1.f);
  glTexCoord2f(0.f, 0.f);
  glVertex3f(1.f, -1.f, -1.f);
  glTexCoord2f(0.f, 1.f);
  glVertex3f(1.f, 1.f, -1.f);
  glTexCoord2f(1.f, 1.f);
  glVertex3f(1.f, 1.f, 1.f);

  glEnd();

  glBindTexture(GL_TEXTURE_2D, gPosZTexture);

  glBegin(GL_QUADS);

  glTexCoord2f(0.f, 0.f);
  glVertex3f(-1.f, -1.f, -1.f);
  glTexCoord2f(1.f, 0.f);
  glVertex3f(1.f, -1.f, -1.f);
  glTexCoord2f(1.f, 1.f);
  glVertex3f(1.f, 1.f, -1.f);
  glTexCoord2f(0.f, 1.f);
  glVertex3f(-1.f, 1.f, -1.f);

  glEnd();

  glBindTexture(GL_TEXTURE_2D, gPosXTexture);

  glBegin(GL_QUADS);

  glTexCoord2f(0.f, 0.f);
  glVertex3f(-1.f, -1.f, 1.f);
  glTexCoord2f(1.f, 0.f);
  glVertex3f(-1.f, -1.f, -1.f);
  glTexCoord2f(1.f, 1.f);
  glVertex3f(-1.f, 1.f, -1.f);
  glTexCoord2f(0.f, 1.f);
  glVertex3f(-1.f, 1.f, 1.f);

  glEnd();

  glBindTexture(GL_TEXTURE_2D, gNegYTexture);

  glBegin(GL_QUADS);

  glTexCoord2f(0.f, 0.f);
  glVertex3f(-1.f, -1.f, 1.f);
  glTexCoord2f(1.f, 0.f);
  glVertex3f(1.f, -1.f, 1.f);
  glTexCoord2f(1.f, 1.f);
  glVertex3f(1.f, -1.f, -1.f);
  glTexCoord2f(0.f, 1.f);
  glVertex3f(-1.f, -1.f, -1.f);

  glEnd();

  glBindTexture(GL_TEXTURE_2D, gPosYTexture);

  glBegin(GL_QUADS);

  glTexCoord2f(0.f, 1.f);
  glVertex3f(-1.f, 1.f, 1.f);
  glTexCoord2f(1.f, 1.f);
  glVertex3f(1.f, 1.f, 1.f);
  glTexCoord2f(1.f, 0.f);
  glVertex3f(1.f, 1.f, -1.f);
  glTexCoord2f(0.f, 0.f);
  glVertex3f(-1.f, 1.f, -1.f);

  glEnd();

  glEnable(GL_LIGHTING);
  glDisable(GL_TEXTURE_2D);
  glEnable(GL_TEXTURE_CUBE_MAP);
  glPopMatrix();
  /* ============ */

  /* Draw light */
  glPushMatrix();
  glDisable(GL_LIGHTING);
  glDisable(GL_TEXTURE_CUBE_MAP);
  glDisable(GL_TEXTURE_2D);

  glTranslatef(gLightPosition[0], gLightPosition[1], gLightPosition[2]);

  glutSolidSphere(0.2, 4, 4);

  glEnable(GL_LIGHTING);
  glEnable(GL_TEXTURE_CUBE_MAP);
  glEnable(GL_TEXTURE_2D);
  glPopMatrix();
  /* ========== */

  /* Draw mesh */
  glEnable(GL_TEXTURE_CUBE_MAP);

  glEnable(GL_TEXTURE_GEN_S);
  glEnable(GL_TEXTURE_GEN_T);
  glEnable(GL_TEXTURE_GEN_R);

  glPushMatrix();
  glRotatef(gAngle, 0.1f, 0.2f, 0.3f);

  glutSolidTeapot(2.0);

  glDisable(GL_TEXTURE_GEN_S);
  glDisable(GL_TEXTURE_GEN_T);
  glDisable(GL_TEXTURE_GEN_R);

  glDisable(GL_TEXTURE_CUBE_MAP);

  glPopMatrix();

  glutSwapBuffers();
}

void reshape(int w, int h) {
  glViewport(0, 0, (GLsizei)w, (GLsizei)h);

  gWidth = (double)w;
  gHeight = h < 1 ? 1.0 : (double)h;
}

void idle(void) {
  gAngle += 1.f;
  glutPostRedisplay();
}

void keyDown(unsigned char key, int x, int y) {
  if (key == 27) {
    exit(0);
  }

  if (key == 'w') {
    gZoom -= 0.5;

    if (gZoom < 1.0) {
      gZoom = 1.0;
    }
  }

  if (key == 's') {
    gZoom += 0.5;

    if (gZoom > 15.0) {
      gZoom = 15.0;
    }
  }

  if (key == 'a') {
    gCameraAngle += 5.0;

    if (gCameraAngle > 359.0) {
      gCameraAngle = 0.0;
    }
  }

  if (key == 'd') {
    gCameraAngle -= 5.0;

    if (gCameraAngle < 0.0) {
      gCameraAngle = 359.0;
    }
  }

  if (key == 't') {
    gCameraY += 0.5;

    if (gCameraY > 15.0) {
      gCameraY = 15.0;
    }
  }

  if (key == 'g') {
    gCameraY -= 0.5;

    if (gCameraY < -15.0) {
      gCameraY = -15.0;
    }
  }

  printf("Zoom: %f, angle: %f\n", gZoom, gCameraAngle);
}

bool loadBmp(GLenum target, const char *imagePath) {
  unsigned char fileHeader[BMP_FILE_HEADER_SIZE];
  unsigned char infoHeader[124];

  FILE *file = fopen(imagePath, "rb");
  if (!file) {
    perror("Image could not be opened");
    return false;
  }

  if (fread(fileHeader, 1, BMP_FILE_HEADER_SIZE, file) != BMP_FILE_HEADER_SIZE) {
    perror("Not enough bytes in BMP header");
    return false;
  }

  if (fileHeader[0] != 'B' || fileHeader[1] != 'M') {
    perror("BMP file is corrupted");
    return false;
  }

  int32_t sizeInBytes = *(int32_t*)&(fileHeader[0x02]);
  printf("Size in bytes: %d\n", sizeInBytes);

  int32_t dataPos = *(int32_t*)&(fileHeader[0x0A]);
  printf("Data offset: %d\n", dataPos);

  uint8_t infoHeaderSize = dataPos - BMP_FILE_HEADER_SIZE;

  if (fread(infoHeader, 1, infoHeaderSize, file) != infoHeaderSize) {
    perror("Not enough bytes to read the Info Header of the BMP");
    return false;
  }

  if(infoHeaderSize != *(int32_t*)&(infoHeader[0])) {
    perror("Corrupted BMP file");
    return false;
  }

  printf("Info header size: %d\n", infoHeaderSize);

  int32_t width = infoHeader[4] | infoHeader[5] << 8 | infoHeader[6] << 16 | infoHeader[7] << 24;
  printf("Width: %d\n", width);

  int32_t height = infoHeader[8] | infoHeader[9] << 8 | infoHeader[10] << 16 | infoHeader[11] << 24;
  printf("Height: %d\n", height);

  int16_t bitCount = *(int16_t*)&(infoHeader[14]);
  printf("Bit count: %d\n", bitCount);

  int32_t compression = *(int32_t*)&(infoHeader[16]);
  printf("Compression: %d\n", compression);

  int32_t imageSize = *(int32_t*)&(infoHeader[20]);
  printf("Image size: %d\n", imageSize);

  if (imageSize != (sizeInBytes - infoHeaderSize - BMP_FILE_HEADER_SIZE)) {
    perror("Image size is wrong, BMP file is corrupted");
    return false;
  }


  unsigned char *data = (unsigned char *)malloc(imageSize);

  fread(data, 1, imageSize, file);
  fclose(file);

  unsigned char *flipped = (unsigned char *)malloc(imageSize);

  for(int row = 0; row < height; row++) {
    for(int pixel = 0; pixel < width * 3; pixel += 3) {
      int index = row * (width * 3) + pixel;
      int indexOriginal = (height - 1 - row) * (width * 3) + pixel;

      flipped[index] = data[indexOriginal];
      flipped[index + 1] = data[indexOriginal + 1];
      flipped[index + 2] = data[indexOriginal + 2];
    }
  }

  glTexImage2D(target, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, flipped);

  free(data);

  return true;
}

bool loadTexture2d(const char *imagePath, GLuint *textureId) {
  glGenTextures(1, textureId);
  glBindTexture(GL_TEXTURE_2D, *textureId);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  if(!loadBmp(GL_TEXTURE_2D, imagePath)) {
    perror("Failed to load BMP file");
    return false;
  }

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  return true;
}

bool loadCubeMap(GLuint *textureId) {
  glGenTextures(1, textureId);
  glBindTexture(GL_TEXTURE_CUBE_MAP, *textureId);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

  if(!loadBmp(GL_TEXTURE_CUBE_MAP_POSITIVE_X, "../assets/left.bmp") ||
     !loadBmp(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, "../assets/right.bmp") ||
     !loadBmp(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, "../assets/bottom.bmp") ||
     !loadBmp(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, "../assets/top.bmp") ||
     !loadBmp(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, "../assets/back.bmp") ||
     !loadBmp(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, "../assets/front.bmp")) {
    perror("Failed loading cube map files");
    return false;
  }

  glEnable(GL_TEXTURE_CUBE_MAP);

  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
  glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
  glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);

  return true;
}
