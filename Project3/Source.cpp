#include <stdlib.h>
#include <stdio.h>
#include <GL/glut.h>
#include <time.h>
#include <math.h>

static double ex = 0.0, ey = 0.0, ez = 0.0; /* 視点の位置 */
static double r = 0.0;            /* 視点の向き */
static double oy = 0.0;
static double pdown1 = 0.0, pdown2 = 0.0;
static double px1 = 0.0, py1 = 0.0, pz1 = 0.0;
static double px2 = 0.0, py2 = 0.0, pz2 = 0.0;
static double pxtmp = 0.0, pytmp = 0.0, pztmp = 0.0;
static double x = 0.0, y = 0.0, z = 0.0;

static GLfloat red[] = { 0.8, 0.2, 0.2, 1.0 };
static GLfloat green[] = { 0.2, 0.8, 0.2, 1.0 };
static GLfloat blue[] = { 0.2, 0.2, 0.8, 1.0 };
static GLfloat yellow[] = { 0.8, 0.8, 0.2, 1.0 };
int fitime =10000;
char fitimea[6];

static GLfloat color[][4] = {{ 0.0, 0.0, 0.0, 0.0 },//dummy
                             { 0.8, 0.2, 0.2, 1.0 },//red
                             { 0.2, 0.8, 0.2, 1.0 },//green
                             { 0.2, 0.2, 0.8, 1.0 },//blue
                             { 0.8, 0.8, 0.2, 1.0 }//yellow
};

/* 物体の色 */
static GLfloat ground[][4] = {
 { 0.6, 0.6, 0.6, 1.0 },
 { 0.3, 0.3, 0.3, 1.0 }
};

void falldisplay();
void deletedisplay();
void display();
void init();

int field[6][9][6] = { 0 };
int cfield[6][9][6] = { 0 };
int ffield[6][9][6] = { 0 };
int dfield[6][9][6] = { 0 };

int dflag = 1;//downflag 1=down
int hflag = 0;//harfdownflag 1=p1 harf down 2=p2 harf down
int rflag = 0;
int rcheck = 0;
int pcolor1, pcolor2;
int score = 0;
int highscore = 0;
char scorea[6];
char highscorea[6];
int rensa = 1;
int prescore = 0;
int deffect = 0;

void timer(int value) {
 static GLboolean isUp = GL_TRUE;
 GLfloat top = -0.9;
 if (top > 0.9F) isUp = GL_FALSE;
 else if (top <= -0.9F) isUp = GL_TRUE;
 top += (isUp == GL_TRUE ? 0.01 : -0.01);
 glutPostRedisplay();
 if (hflag == 0) {
  glutTimerFunc(10, timer, 0);
  if(fitime>0)fitime--;
 }
 else {
  glutTimerFunc(3, timer, 0);
 }

}

void copyfield(int a[][9][6],int b[][9][6])
{
 int i, j, k;
 for (i = 0; i < 6; ++i)
  for (j = 0; j < 9; ++j)
   for (k = 0; k < 6; ++k)
	b[i][j][k] = a[i][j][k];
}//a_to_b

void pcount_or_vanish(int i, int j, int k,int *n,int f[][9][6]) {
 int c = f[i][j][k];

 f[i][j][k] = 0; (*n)++;
 if (i + 1 < 5 && f[i + 1][j][k] == c)pcount_or_vanish(i + 1, j, k, n,f);
 if (j + 1 < 9 && f[i][j + 1][k] == c)pcount_or_vanish(i, j + 1, k, n,f);
 if (k + 1 < 5 && f[i][j][k + 1] == c)pcount_or_vanish(i, j, k + 1, n,f);
 if (i - 1 >= 1 && f[i - 1][j][k] == c)pcount_or_vanish(i - 1, j, k, n,f);
 if (j - 1 >= 1 && f[i][j - 1][k] == c)pcount_or_vanish(i, j - 1, k, n,f);
 if (k - 1 >= 1 && f[i][j][k - 1] == c)pcount_or_vanish(i, j, k - 1, n,f);
 if (deffect == 1) {
  dfield[i][j][k] = c;
 }
}

int pdelete(void) {
 int i, j, k, n, n2 = 0, d = 0;
 copyfield(field, cfield);
 for (i = 1; i < 5; ++i)
  for (j = 1; j < 9; ++j)
   for (k = 1; k < 5; ++k) {
	n = field[i][j][k];
	if (n > 0) {
	 deffect = 0;
	 n = 0; pcount_or_vanish(i, j, k, &n, field);
	 if (n > 3) {
	  deffect = 1;
	  pcount_or_vanish(i, j, k, &n2, cfield); d += n;
	 }
	}
   }
 copyfield(cfield, field);
 return d;
}

int pfall() {
 int i, j, k, yy, n=0;
 for (i = 1; i < 5; ++i)
  for (k = 1; k < 5; ++k) {

   for (j = 1; j < 8; ++j) {
	if (field[i][j][k] == 0) {//jが空白
	 for (yy = j + 1; yy < 9 && field[i][yy][k] == 0; ++yy);
	 if (yy == 9)break;//上全て空白なら次
	 n++;//落下処理あり
	 for (yy = j; yy < 8; ++yy) {
	  ffield[i][yy][k] = field[i][yy + 1][k];
	  field[i][yy + 1][k] = 0;
	 }
	 break;
	}
   }
  }
 return n;
}

void scene(void)
{


 int i, j, k;
 int fallcount,deletecount;
 glRotated(r, 0.0, 1.0, 0.0);
 glTranslated(0.0, -3.0, 0.0);

 if (fitime > 0) {
  /*ぷよ1*/
  glPushMatrix();
  glMaterialfv(GL_FRONT, GL_DIFFUSE, color[pcolor1]);
  glTranslated(-0.5 + px1, 7.0 - pdown1 + py1, -0.5 + pz1);
  glutSolidSphere(0.5, 10, 10);
  glPopMatrix();

  /*ぷよ2*/
  glPushMatrix();
  glMaterialfv(GL_FRONT, GL_DIFFUSE, color[pcolor2]);
  glTranslated(-0.5 + px2, 7.0 - pdown2 + py2, +0.5 + pz2);
  glutSolidSphere(0.5, 10, 10);
  glPopMatrix();
 }

 //既ぷよ
 for (i = 1; i < 5; ++i) {
  for (j = 1; j < 9; ++j) {
   for (k = 1; k < 5; ++k) {
	if (field[i][j][k]>0) {
	 glPushMatrix();
	 glMaterialfv(GL_FRONT, GL_DIFFUSE, color[field[i][j][k]]);
	 glTranslated(-2.5 + i, j-1, -2.5 + k);
	 glutSolidSphere(0.5, 10, 10);
	 glPopMatrix();
	}
   }
  }
 }

 //落下可能確認
 if ((int)(pdown1 * 100) % 100 == 0 && (int)(pdown2 * 100) % 100 == 0) {
  if (field[2 + (int)px1][7 - (int)pdown1+ (int)py1][2 + (int)pz1] == 0 && field[2 + (int)px2][7 - (int)pdown2+(int)py2][3 + (int)pz2] == 0) {
   dflag = 1;
  }
  else {
   dflag = 0;
   if (field[2 + (int)px1][7 - (int)pdown1+(int)py1][2 + (int)pz1] == 0 &&
	7 - (int)pdown1 + (int)py1 != 8 - (int)pdown2 + (int)py2) {
	hflag = 1;//1下空
   }
   else if (field[2 + (int)px2][7 - (int)pdown2 + (int)py2][3 + (int)pz2] == 0 &&
	7 - (int)pdown2 + (int)py2 != 8 - (int)pdown1 + (int)py1) {
	hflag = 2;//2下空
   }
   else hflag = 0;
  }
 }
 //回転
 if (rflag == 1&&(y==0||y==180||z==0||z==180)) {
  x += 3;
  px2 = pxtmp + sin(x*3.141592654 / 180.0);
  pz2 = pztmp - 1 + cos(x*3.141592654 / 180.0);
  if ((int)x % 90==0) {
   rflag = 0;
   if (x / 90 == 1) {
	px2 = pxtmp + 1;
	pz2 = pztmp - 1;
	y = 0;
   }
   else if (x / 90 == 2) {
	px2 = pxtmp ;
	pz2 = pztmp - 2;
	z = 180;
   }
   else if (x / 90 == 3) {
	px2 = pxtmp - 1;
	pz2 = pztmp - 1;
	y = 180;
   }
   else if (x / 90 == 4) {
	px2 = pxtmp ;
	pz2 = pztmp ;
	z = 0;
   }
   if (x == 360)x = 0;
  }
 }
 else if (rflag == 2&&(x==90||x==270||z==90||z==270)) {
  y += 3;
  px2 = pxtmp + cos(y*3.141592654 / 180.0);// +sin(x*3.141592654 / 180.0);
  py2 = pytmp + sin(y*3.141592654 / 180.0);
  pz2 = pztmp - 1;// +cos(x*3.141592654 / 180.0);
  if ((int)y % 90 == 0) {
   rflag = 0;
   if (y / 90 == 1) {
	px2 = pxtmp;
	py2 = pytmp + 1;
	pz2 = pztmp - 1;
	z = 90;
   }
   else if (y / 90 == 2) {
	px2 = pxtmp - 1;
	py2 = pytmp;
	pz2 = pztmp - 1;
	x = 270;
   }
   else if (y / 90 == 3) {
	px2 = pxtmp;
	py2 = pytmp - 1;
	pz2 = pztmp - 1;
	z = 270;
   }
   else if (y / 90 == 4) {
	px2 = pxtmp+1;
	py2 = pytmp;
	pz2 = pztmp-1;
	x = 90;
   }
   if (y == 360)y = 0;
  }
 }
 else if (rflag == 3 && (x == 0 || x == 180||y==90||y==270)) {
  z += 3;
  px2 = pxtmp;// +cos(y*3.141592654 / 180.0);// +sin(x*3.141592654 / 180.0);
  py2 = pytmp + sin(z*3.141592654 / 180.0);
  pz2 = pztmp -1 + cos(z*3.141592654 / 180.0);// +cos(x*3.141592654 / 180.0);
  if ((int)z % 90 == 0) {
   rflag = 0;
   if (z / 90 == 1) {
	//px2 = pxtmp;
	py2 = pytmp + 1;
	pz2 = pztmp - 1;
	y = 90;
   }
   else if (z / 90 == 2) {
	//px2 = pxtmp - 1;
	py2 = pytmp;
	pz2 = pztmp - 2;
	x = 180;
   }
   else if (z / 90 == 3) {
	//px2 = pxtmp;
	py2 = pytmp - 1;
	pz2 = pztmp - 1;
	y = 270;
   }
   else if (z / 90 == 4) {
	//px2 = pxtmp + 1;
	py2 = pytmp;
	pz2 = pztmp;
	x = 0;
   }
   if (z == 360)z = 0;
  }
 }
 //落下可能
 else {
  if (field[2][8][2] > 0 || field[2][8][3] > 0)fitime = 0;
  if (fitime > 0) {
   if (dflag == 1) {
	if (fitime > 0) {
	 pdown1 += 0.01;
	 pdown2 += 0.01;
	}
   }
   else if (dflag == 0) {//接地
	if (hflag == 1) {
	 pdown1 += 0.01;
	}
	else if (hflag == 2) {
	 pdown2 += 0.01;
	}
	else if (hflag == 0) {//両接地
	 dflag = 1;
	 field[2 + (int)px1][8 - (int)pdown1 + (int)py1][2 + (int)pz1] = pcolor1;
	 field[2 + (int)px2][8 - (int)pdown2 + (int)py2][3 + (int)pz2] = pcolor2;
	 pdown1 = pdown2 = 0;
	 px1 = 0; py1 = 0; pz1 = 0; px2 = 0; py2 = 0; pz2 = 0;
	 pcolor1 = rand() % 4 + 1;
	 pcolor2 = rand() % 4 + 1;
	 x = y = z = 0;
	 //glutDisplayFunc(falldisplay);
	 while (prescore = pdelete()) {
	  score += prescore * 10 * rensa++;
	  for (deletecount = 0; deletecount < 200; ++deletecount) {
	   deletedisplay();
	  }
	  for (i = 0; i < 6; ++i)
	   for (j = 0; j < 9; ++j)
		for (k = 0; k < 6; ++k) {
		 dfield[i][j][k] = 0;
		}
	  printf("%d\n", score);
	  while (pfall()) {
	   for (fallcount = 0; fallcount < 100; ++fallcount) {
		//timer(1);
		falldisplay();
	   }
	   for (i = 1; i < 5; ++i)
		for (j = 1; j < 9; ++j)
		 for (k = 1; k < 5; ++k) {
		  field[i][j][k] += ffield[i][j][k];
		  ffield[i][j][k] = 0;
		 }
	  }//fall_end
	 }//delete_end
	 rensa = 1;
	 //glutDisplayFunc(display);
	}
   }
   rflag = 0;
  }
 }
 
 /* 赤い箱 */
 glPushMatrix();
 glTranslated(0.0, 0.0, -3.0);
 glMaterialfv(GL_FRONT, GL_DIFFUSE, red);
 glutSolidCube(1.0);
 glPopMatrix();

 /* 緑の箱 */
 glPushMatrix();
 glTranslated(0.0, 0.0, 3.0);
 glMaterialfv(GL_FRONT, GL_DIFFUSE, green);
 glutSolidCube(1.0);
 glPopMatrix();

 /* 青い箱 */
 glPushMatrix();
 glTranslated(-3.0, 0.0, 0.0);
 glMaterialfv(GL_FRONT, GL_DIFFUSE, blue);
 glutSolidCube(1.0);
 glPopMatrix();

 /* 黄色い箱 */
 glPushMatrix();
 glTranslated(3.0, 0.0, 0.0);
 glMaterialfv(GL_FRONT, GL_DIFFUSE, yellow);
 glutSolidCube(1.0);
 glPopMatrix();

 /* 地面 */
 glBegin(GL_QUADS);
 glNormal3d(0.0, 1.0, 0.0);
 for (j = -2; j < 2; ++j) {
  for (i = -2; i < 2; ++i) {
   glMaterialfv(GL_FRONT, GL_DIFFUSE, ground[(i + j) & 1]);
   glVertex3d((GLdouble)i, -0.5, (GLdouble)j);
   glVertex3d((GLdouble)i, -0.5, (GLdouble)(j + 1));
   glVertex3d((GLdouble)(i + 1), -0.5, (GLdouble)(j + 1));
   glVertex3d((GLdouble)(i + 1), -0.5, (GLdouble)j);
  }
 }
 glEnd();

 glRotated(90, 0.0, 0.0, -1.0);
 glTranslated(-1.5, -1.5, 0.0);
 glBegin(GL_QUADS);
 glNormal3d(0.0, 1.0, 0.0);
 for (j = -2; j < 2; ++j) {
  for (i = -6; i < 2; ++i) {
   glMaterialfv(GL_FRONT, GL_DIFFUSE, ground[(i + j) & 1]);
   glVertex3d((GLdouble)i, -0.5, (GLdouble)j);
   glVertex3d((GLdouble)i, -0.5, (GLdouble)(j + 1));
   glVertex3d((GLdouble)(i + 1), -0.5, (GLdouble)(j + 1));
   glVertex3d((GLdouble)(i + 1), -0.5, (GLdouble)j);
  }
 }
 glEnd();

 glRotated(90, 1.0, 0.0, 0.0);
 glTranslated(0.0, -1.5, -1.5);
 glBegin(GL_QUADS);
 glNormal3d(0.0, 1.0, 0.0);
 for (j = -2; j < 2; ++j) {
  for (i = -6; i < 2; ++i) {
   glMaterialfv(GL_FRONT, GL_DIFFUSE, ground[(i + j) & 1]);
   glVertex3d((GLdouble)i, -0.5, (GLdouble)j);
   glVertex3d((GLdouble)i, -0.5, (GLdouble)(j + 1));
   glVertex3d((GLdouble)(i + 1), -0.5, (GLdouble)(j + 1));
   glVertex3d((GLdouble)(i + 1), -0.5, (GLdouble)j);
  }
 }
 glEnd();

 glRotated(90, 1.0, 0.0, 0.0);
 glTranslated(0.0, -1.5, -1.5);
 glBegin(GL_QUADS);
 glNormal3d(0.0, 1.0, 0.0);
 for (j = -2; j < 2; ++j) {
  for (i = -6; i < 2; ++i) {
   glMaterialfv(GL_FRONT, GL_DIFFUSE, ground[(i + j) & 1]);
   glVertex3d((GLdouble)i, -0.5, (GLdouble)j);
   glVertex3d((GLdouble)i, -0.5, (GLdouble)(j + 1));
   glVertex3d((GLdouble)(i + 1), -0.5, (GLdouble)(j + 1));
   glVertex3d((GLdouble)(i + 1), -0.5, (GLdouble)j);
  }
 }
 glEnd();

 glRotated(90, 1.0, 0.0, 0.0);
 glTranslated(0.0, -1.5, -1.5);
 glBegin(GL_QUADS);
 glNormal3d(0.0, 1.0, 0.0);
 for (j = -2; j < 2; ++j) {
  for (i = -6; i < 2; ++i) {
   glMaterialfv(GL_FRONT, GL_DIFFUSE, ground[(i + j) & 1]);
   glVertex3d((GLdouble)i, -0.5, (GLdouble)j);
   glVertex3d((GLdouble)i, -0.5, (GLdouble)(j + 1));
   glVertex3d((GLdouble)(i + 1), -0.5, (GLdouble)(j + 1));
   glVertex3d((GLdouble)(i + 1), -0.5, (GLdouble)j);
  }
 }
 glEnd();

}

void glutprintstring(double x, double y, char* str, int length) {
 double shift = -1.0f;
 int i;
 glRasterPos3f(x, y, shift);
 for (i = 0; i < length; i++) {
  glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, str[i]);
 }
}

void display(void)
{
 static GLfloat lightpos[] = { 6.0, 8.0, 10.0, 1.0 }; /* 光源の位置 */

													  /* 画面クリア */
 glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

 /* モデルビュー変換行列の初期化 */
 glLoadIdentity();

 /* 視点の移動 */
 glTranslated(0.0, oy, 0.0);
 gluLookAt(9.0 + ex*0.3, 12.0 + ey*0.4, 15.0 + ez*0.5, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);


 glutprintstring(3.0, 5.0, "SCORE", 5);
 sprintf_s(scorea, "%05d", score);
 glutprintstring(3.5, 4.5, scorea, 6);

 glutprintstring(3.5, -1.0, "HIGHSCORE", 9);
 sprintf_s(highscorea, "%05d", highscore);
 glutprintstring(4.5, -1.5, highscorea, 6);

 glutprintstring(3.5, 3.5, "TIMER", 6);
 sprintf_s(fitimea, "%05d", fitime);
 glutprintstring(3.5, 3.0, fitimea, 6);


 if (fitime <= 0) {
  glutprintstring(3.5, 2.0, "FINISH", 6);
  glutprintstring(3.0, 1.0, "RESTART:press'w'", 16);
  glutprintstring(3.5, 0.5, "QUIT:press'q'", 13);
 }


 /* 光源の位置を設定 */
 glLightfv(GL_LIGHT0, GL_POSITION, lightpos);

 /* シーンの描画 */
 scene();

 glFlush();
}

void falldisplay() {
 
 static GLfloat lightpos[] = { 6.0, 8.0, 10.0, 1.0 }; /* 光源の位置 */
 static double fall = 1.0;
 if (fall < 0.0)fall = 1.0;
 fall -= 0.01;
													  /* 画面クリア */
 glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

 /* モデルビュー変換行列の初期化 */
 glLoadIdentity();

 /* 視点の移動 */
 glTranslated(0.0, oy, 0.0);
 gluLookAt(9.0 + ex*0.3, 12.0 + ey*0.4, 15.0 + ez*0.5, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

 /* 光源の位置を設定 */
 glLightfv(GL_LIGHT0, GL_POSITION, lightpos);

 glutprintstring(3.0, 5.0, "SCORE", 5);
 sprintf_s(scorea, "%05d", score);
 glutprintstring(3.5, 4.5, scorea, 6);

 glutprintstring(3.5, -1.0, "HIGHSCORE", 9);
 sprintf_s(highscorea, "%05d", highscore);
 glutprintstring(4.5, -1.5, highscorea, 6);

 glutprintstring(3.5, 3.5, "TIMER", 6);
 sprintf_s(fitimea, "%05d", fitime);
 glutprintstring(3.5, 3.0, fitimea, 6);


 if (fitime <= 0) {
  glutprintstring(3.5, 2.0, "FINISH", 6);
  glutprintstring(3.0, 1.0, "RESTART:press'w'", 16);
  glutprintstring(3.5, 0.5, "QUIT:press'q'", 13);
 }

 /* シーンの描画 */
 int i, j, k;
 glRotated(r, 0.0, 1.0, 0.0);
 glTranslated(0.0, -3.0, 0.0);

 for (i = 1; i < 5; ++i) {
  for (j = 1; j < 9; ++j) {
   for (k = 1; k < 5; ++k) {
	if (field[i][j][k]>0) {
	 glPushMatrix();
	 glMaterialfv(GL_FRONT, GL_DIFFUSE, color[field[i][j][k]]);
	 glTranslated(-2.5 + i, j - 1, -2.5 + k);
	 glutSolidSphere(0.5, 10, 10);
	 glPopMatrix();
	}
   }
  }
 }

 for (i = 1; i < 5; ++i) {
  for (j = 1; j < 9; ++j) {
   for (k = 1; k < 5; ++k) {
	if (ffield[i][j][k]>0) {
	 glPushMatrix();
	 glMaterialfv(GL_FRONT, GL_DIFFUSE, color[ffield[i][j][k]]);
	 glTranslated(-2.5 + i, j - 1 + fall, -2.5 + k);
	 glutSolidSphere(0.5, 10, 10);
	 glPopMatrix();
	}
   }
  }
 }

 /* 赤い箱 */
 glPushMatrix();
 glTranslated(0.0, 0.0, -3.0);
 glMaterialfv(GL_FRONT, GL_DIFFUSE, red);
 glutSolidCube(1.0);
 glPopMatrix();

 /* 緑の箱 */
 glPushMatrix();
 glTranslated(0.0, 0.0, 3.0);
 glMaterialfv(GL_FRONT, GL_DIFFUSE, green);
 glutSolidCube(1.0);
 glPopMatrix();

 /* 青い箱 */
 glPushMatrix();
 glTranslated(-3.0, 0.0, 0.0);
 glMaterialfv(GL_FRONT, GL_DIFFUSE, blue);
 glutSolidCube(1.0);
 glPopMatrix();

 /* 黄色い箱 */
 glPushMatrix();
 glTranslated(3.0, 0.0, 0.0);
 glMaterialfv(GL_FRONT, GL_DIFFUSE, yellow);
 glutSolidCube(1.0);
 glPopMatrix();

 /* 地面 */
 glBegin(GL_QUADS);
 glNormal3d(0.0, 1.0, 0.0);
 for (j = -2; j < 2; ++j) {
  for (i = -2; i < 2; ++i) {
   glMaterialfv(GL_FRONT, GL_DIFFUSE, ground[(i + j) & 1]);
   glVertex3d((GLdouble)i, -0.5, (GLdouble)j);
   glVertex3d((GLdouble)i, -0.5, (GLdouble)(j + 1));
   glVertex3d((GLdouble)(i + 1), -0.5, (GLdouble)(j + 1));
   glVertex3d((GLdouble)(i + 1), -0.5, (GLdouble)j);
  }
 }
 glEnd();

 glRotated(90, 0.0, 0.0, -1.0);
 glTranslated(-1.5, -1.5, 0.0);
 glBegin(GL_QUADS);
 glNormal3d(0.0, 1.0, 0.0);
 for (j = -2; j < 2; ++j) {
  for (i = -6; i < 2; ++i) {
   glMaterialfv(GL_FRONT, GL_DIFFUSE, ground[(i + j) & 1]);
   glVertex3d((GLdouble)i, -0.5, (GLdouble)j);
   glVertex3d((GLdouble)i, -0.5, (GLdouble)(j + 1));
   glVertex3d((GLdouble)(i + 1), -0.5, (GLdouble)(j + 1));
   glVertex3d((GLdouble)(i + 1), -0.5, (GLdouble)j);
  }
 }
 glEnd();

 glRotated(90, 1.0, 0.0, 0.0);
 glTranslated(0.0, -1.5, -1.5);
 glBegin(GL_QUADS);
 glNormal3d(0.0, 1.0, 0.0);
 for (j = -2; j < 2; ++j) {
  for (i = -6; i < 2; ++i) {
   glMaterialfv(GL_FRONT, GL_DIFFUSE, ground[(i + j) & 1]);
   glVertex3d((GLdouble)i, -0.5, (GLdouble)j);
   glVertex3d((GLdouble)i, -0.5, (GLdouble)(j + 1));
   glVertex3d((GLdouble)(i + 1), -0.5, (GLdouble)(j + 1));
   glVertex3d((GLdouble)(i + 1), -0.5, (GLdouble)j);
  }
 }
 glEnd();

 glRotated(90, 1.0, 0.0, 0.0);
 glTranslated(0.0, -1.5, -1.5);
 glBegin(GL_QUADS);
 glNormal3d(0.0, 1.0, 0.0);
 for (j = -2; j < 2; ++j) {
  for (i = -6; i < 2; ++i) {
   glMaterialfv(GL_FRONT, GL_DIFFUSE, ground[(i + j) & 1]);
   glVertex3d((GLdouble)i, -0.5, (GLdouble)j);
   glVertex3d((GLdouble)i, -0.5, (GLdouble)(j + 1));
   glVertex3d((GLdouble)(i + 1), -0.5, (GLdouble)(j + 1));
   glVertex3d((GLdouble)(i + 1), -0.5, (GLdouble)j);
  }
 }
 glEnd();

 glRotated(90, 1.0, 0.0, 0.0);
 glTranslated(0.0, -1.5, -1.5);
 glBegin(GL_QUADS);
 glNormal3d(0.0, 1.0, 0.0);
 for (j = -2; j < 2; ++j) {
  for (i = -6; i < 2; ++i) {
   glMaterialfv(GL_FRONT, GL_DIFFUSE, ground[(i + j) & 1]);
   glVertex3d((GLdouble)i, -0.5, (GLdouble)j);
   glVertex3d((GLdouble)i, -0.5, (GLdouble)(j + 1));
   glVertex3d((GLdouble)(i + 1), -0.5, (GLdouble)(j + 1));
   glVertex3d((GLdouble)(i + 1), -0.5, (GLdouble)j);
  }
 }
 glEnd();


 glFlush();

}

void deletedisplay() {

 static GLfloat lightpos[] = { 6.0, 8.0, 10.0, 1.0 }; /* 光源の位置 */
 static int del = 200;
 if (del < 0)del = 200;
 del--;
 /* 画面クリア */
 glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

 /* モデルビュー変換行列の初期化 */
 glLoadIdentity();

 /* 視点の移動 */
 glTranslated(0.0, oy, 0.0);
 gluLookAt(9.0 + ex*0.3, 12.0 + ey*0.4, 15.0 + ez*0.5, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

 /* 光源の位置を設定 */
 glLightfv(GL_LIGHT0, GL_POSITION, lightpos);

 glutprintstring(3.0, 5.0, "SCORE", 5);
 sprintf_s(scorea, "%05d", score);
 glutprintstring(3.5, 4.5, scorea, 6);

 glutprintstring(3.5, -1.0, "HIGHSCORE", 9);
 sprintf_s(highscorea, "%05d", highscore);
 glutprintstring(4.5, -1.5, highscorea, 6);

 glutprintstring(3.5, 3.5, "TIMER", 6);
 sprintf_s(fitimea, "%05d", fitime);
 glutprintstring(3.5, 3.0, fitimea, 6);


 if (fitime <= 0) {
  glutprintstring(3.5, 2.0, "FINISH", 6);
  glutprintstring(3.0, 1.0, "RESTART:press'w'", 16);
  glutprintstring(3.5, 0.5, "QUIT:press'q'", 13);
 }

 /* シーンの描画 */
 int i, j, k;
 glRotated(r, 0.0, 1.0, 0.0);
 glTranslated(0.0, -3.0, 0.0);

 for (i = 1; i < 5; ++i) {
  for (j = 1; j < 9; ++j) {
   for (k = 1; k < 5; ++k) {
	if (field[i][j][k]>0) {
	 glPushMatrix();
	 glMaterialfv(GL_FRONT, GL_DIFFUSE, color[field[i][j][k]]);
	 glTranslated(-2.5 + i, j - 1, -2.5 + k);
	 glutSolidSphere(0.5, 10, 10);
	 glPopMatrix();
	}
   }
  }
 }

 for (i = 1; i < 5; ++i) {
  for (j = 1; j < 9; ++j) {
   for (k = 1; k < 5; ++k) {
	if (dfield[i][j][k]>0) {
	 glPushMatrix();
	 glMaterialfv(GL_FRONT, GL_DIFFUSE, color[dfield[i][j][k]]);
	 glTranslated(-2.5 + i, j - 1 , -2.5 + k);
	 glutWireSphere(0.5, del, 10);
	 glPopMatrix();
	}
   }
  }
 }

 /* 赤い箱 */
 glPushMatrix();
 glTranslated(0.0, 0.0, -3.0);
 glMaterialfv(GL_FRONT, GL_DIFFUSE, red);
 glutSolidCube(1.0);
 glPopMatrix();

 /* 緑の箱 */
 glPushMatrix();
 glTranslated(0.0, 0.0, 3.0);
 glMaterialfv(GL_FRONT, GL_DIFFUSE, green);
 glutSolidCube(1.0);
 glPopMatrix();

 /* 青い箱 */
 glPushMatrix();
 glTranslated(-3.0, 0.0, 0.0);
 glMaterialfv(GL_FRONT, GL_DIFFUSE, blue);
 glutSolidCube(1.0);
 glPopMatrix();

 /* 黄色い箱 */
 glPushMatrix();
 glTranslated(3.0, 0.0, 0.0);
 glMaterialfv(GL_FRONT, GL_DIFFUSE, yellow);
 glutSolidCube(1.0);
 glPopMatrix();

 /* 地面 */
 glBegin(GL_QUADS);
 glNormal3d(0.0, 1.0, 0.0);
 for (j = -2; j < 2; ++j) {
  for (i = -2; i < 2; ++i) {
   glMaterialfv(GL_FRONT, GL_DIFFUSE, ground[(i + j) & 1]);
   glVertex3d((GLdouble)i, -0.5, (GLdouble)j);
   glVertex3d((GLdouble)i, -0.5, (GLdouble)(j + 1));
   glVertex3d((GLdouble)(i + 1), -0.5, (GLdouble)(j + 1));
   glVertex3d((GLdouble)(i + 1), -0.5, (GLdouble)j);
  }
 }
 glEnd();

 glRotated(90, 0.0, 0.0, -1.0);
 glTranslated(-1.5, -1.5, 0.0);
 glBegin(GL_QUADS);
 glNormal3d(0.0, 1.0, 0.0);
 for (j = -2; j < 2; ++j) {
  for (i = -6; i < 2; ++i) {
   glMaterialfv(GL_FRONT, GL_DIFFUSE, ground[(i + j) & 1]);
   glVertex3d((GLdouble)i, -0.5, (GLdouble)j);
   glVertex3d((GLdouble)i, -0.5, (GLdouble)(j + 1));
   glVertex3d((GLdouble)(i + 1), -0.5, (GLdouble)(j + 1));
   glVertex3d((GLdouble)(i + 1), -0.5, (GLdouble)j);
  }
 }
 glEnd();

 glRotated(90, 1.0, 0.0, 0.0);
 glTranslated(0.0, -1.5, -1.5);
 glBegin(GL_QUADS);
 glNormal3d(0.0, 1.0, 0.0);
 for (j = -2; j < 2; ++j) {
  for (i = -6; i < 2; ++i) {
   glMaterialfv(GL_FRONT, GL_DIFFUSE, ground[(i + j) & 1]);
   glVertex3d((GLdouble)i, -0.5, (GLdouble)j);
   glVertex3d((GLdouble)i, -0.5, (GLdouble)(j + 1));
   glVertex3d((GLdouble)(i + 1), -0.5, (GLdouble)(j + 1));
   glVertex3d((GLdouble)(i + 1), -0.5, (GLdouble)j);
  }
 }
 glEnd();

 glRotated(90, 1.0, 0.0, 0.0);
 glTranslated(0.0, -1.5, -1.5);
 glBegin(GL_QUADS);
 glNormal3d(0.0, 1.0, 0.0);
 for (j = -2; j < 2; ++j) {
  for (i = -6; i < 2; ++i) {
   glMaterialfv(GL_FRONT, GL_DIFFUSE, ground[(i + j) & 1]);
   glVertex3d((GLdouble)i, -0.5, (GLdouble)j);
   glVertex3d((GLdouble)i, -0.5, (GLdouble)(j + 1));
   glVertex3d((GLdouble)(i + 1), -0.5, (GLdouble)(j + 1));
   glVertex3d((GLdouble)(i + 1), -0.5, (GLdouble)j);
  }
 }
 glEnd();

 glRotated(90, 1.0, 0.0, 0.0);
 glTranslated(0.0, -1.5, -1.5);
 glBegin(GL_QUADS);
 glNormal3d(0.0, 1.0, 0.0);
 for (j = -2; j < 2; ++j) {
  for (i = -6; i < 2; ++i) {
   glMaterialfv(GL_FRONT, GL_DIFFUSE, ground[(i + j) & 1]);
   glVertex3d((GLdouble)i, -0.5, (GLdouble)j);
   glVertex3d((GLdouble)i, -0.5, (GLdouble)(j + 1));
   glVertex3d((GLdouble)(i + 1), -0.5, (GLdouble)(j + 1));
   glVertex3d((GLdouble)(i + 1), -0.5, (GLdouble)j);
  }
 }
 glEnd();


 glFlush();

}

void resize(int w, int h)
{
 /* ウィンドウ全体をビューポートにする */
 glViewport(0, 0, w, h);

 /* 透視変換行列の指定 */
 glMatrixMode(GL_PROJECTION);

 /* 透視変換行列の初期化 */
 glLoadIdentity();
 gluPerspective(30.0, (double)w / (double)h, 0.1, 100.0);

 /* モデルビュー変換行列の指定 */
 glMatrixMode(GL_MODELVIEW);
}

void keyboard(unsigned char key, int xk, int yk)
{
 /* ESC か q をタイプしたら終了 */
 if (key == '\033' || key == 'q') {
  exit(0);
 }

 if (hflag == 0 && rflag == 0&&fitime>0) {
  switch (key) {
  case '8':
   if (field[2 + (int)px2][7 - (int)pdown2 + (int)py2][2 + (int)pz2] == 0 &&
	field[2 + (int)px1][7 - (int)pdown1 + (int)py1][1 + (int)pz1] == 0) {
   //if (pz1 != -1 && pz2 != -2) {
	pz1 -= 1; pz2 -= 1; pxtmp = px1; pytmp = py1; pztmp = pz1;
   }break;
  case '4':
   if (field[1 + (int)px2][7 - (int)pdown2 + (int)py2][3 + (int)pz2] == 0 &&
	field[1 + (int)px1][7 - (int)pdown1 + (int)py1][2 + (int)pz1] == 0) {
   //if (px1 != -1 && px2 != -1) {
	px1 -= 1; px2 -= 1; pxtmp = px1; pytmp = py1; pztmp = pz1;
   }break;
  case '2':
   if(field[2 + (int)px2][7 - (int)pdown2 + (int)py2][4 + (int)pz2]==0&&
	field[2 + (int)px1][7 - (int)pdown1 + (int)py1][3 + (int)pz1]==0){
   //if (pz1 != 2 && pz2 != 1) {
	pz1 += 1; pz2 += 1; pxtmp = px1; pytmp = py1; pztmp = pz1;
   } break;
  case '6':
   if (field[3 + (int)px2][7 - (int)pdown2 + (int)py2][3 + (int)pz2] == 0 &&
	field[3 + (int)px1][7 - (int)pdown1 + (int)py1][2 + (int)pz1] == 0) {
   //if (px1 != 2 && px2 != 2) {
	px1 += 1; px2 += 1; pxtmp = px1; pytmp = py1; pztmp = pz1;
   }break;
  case '5':
   if (field[2 + (int)px2][6 - (int)pdown2 + (int)py2][3 + (int)pz2] == 0 &&
	field[2 + (int)px1][6 - (int)pdown1 + (int)py1][2 + (int)pz1] == 0) {
	py1 -= 1; py2 -= 1; pxtmp = px1; pytmp = py1; pztmp = pz1;
   }break;
  case '1':
   if (
	(x == 0 &&
	 field[3 + (int)px2][7 - (int)pdown2 + (int)py2][3 + (int)pz2] == 0 &&
	 field[3 + (int)px1][7 - (int)pdown1 + (int)py1][2 + (int)pz1] == 0) ||
	(x == 90 &&
	 field[2 + (int)px2][7 - (int)pdown2 + (int)py2][2 + (int)pz2] == 0 &&
	 field[2 + (int)px1][7 - (int)pdown1 + (int)py1][1 + (int)pz1] == 0) ||
	(x == 180 &&
	 field[1 + (int)px2][7 - (int)pdown2 + (int)py2][3 + (int)pz2] == 0 &&
	 field[1 + (int)px1][7 - (int)pdown1 + (int)py1][2 + (int)pz1] == 0) ||
	(x == 270 &&
	 field[2 + (int)px2][7 - (int)pdown2 + (int)py2][4 + (int)pz2] == 0 &&
	 field[2 + (int)px1][7 - (int)pdown1 + (int)py1][3 + (int)pz1] == 0)) {
	rflag = 1;
	pxtmp = px1; pztmp = pz1; pytmp = py1;
   }break;
  case '7':
   if (
	(y == 0 &&
	 field[2 + (int)px2][8 - (int)pdown2 + (int)py2][3 + (int)pz2] == 0 &&
	 field[2 + (int)px1][8 - (int)pdown1 + (int)py1][2 + (int)pz1] == 0) ||
	(y == 90 &&
	 field[1 + (int)px2][7 - (int)pdown2 + (int)py2][3 + (int)pz2] == 0 &&
	 field[1 + (int)px1][7 - (int)pdown1 + (int)py1][2 + (int)pz1] == 0) ||
	(y == 180 &&
	 field[2 + (int)px2][6 - (int)pdown2 + (int)py2][3 + (int)pz2] == 0 &&
	 field[2 + (int)px1][6 - (int)pdown1 + (int)py1][2 + (int)pz1] == 0) ||
	(y == 270 &&
	 field[2 + (int)px2][7 - (int)pdown2 + (int)py2][4 + (int)pz2] == 0 &&
	 field[2 + (int)px1][7 - (int)pdown1 + (int)py1][3 + (int)pz1] == 0)) {
	rflag = 2;
	pxtmp = px1; pztmp = pz1; pytmp = py1;
   }break;
  case '9':
   //if (field[2 + (int)px2][7 - (int)pdown2 + (int)py2][3 + (int)pz2] == 0 &&
	//field[2 + (int)px1][7 - (int)pdown1][2 + (int)pz1] == 0) {
   if (
	(z == 0 &&
	 field[2 + (int)px2][8 - (int)pdown2 + (int)py2][3 + (int)pz2] == 0 &&
	 field[2 + (int)px1][8 - (int)pdown1 + (int)py1][2 + (int)pz1] == 0) ||
	(z == 90 &&
	 field[2 + (int)px2][7 - (int)pdown2 + (int)py2][2 + (int)pz2] == 0 &&
	 field[2 + (int)px1][7 - (int)pdown1 + (int)py1][1 + (int)pz1] == 0) ||
	(z == 180 &&
	 field[2 + (int)px2][6 - (int)pdown2 + (int)py2][3 + (int)pz2] == 0 &&
	 field[2 + (int)px1][6 - (int)pdown1 + (int)py1][2 + (int)pz1] == 0) ||
	(z == 270 &&
	 field[2 + (int)px2][7 - (int)pdown2 + (int)py2][4 + (int)pz2] == 0 &&
	 field[2 + (int)px1][7 - (int)pdown1 + (int)py1][3+ (int)pz1] == 0)) {
	rflag = 3;
	pxtmp = px1; pztmp = pz1; pytmp = py1;
   }break;
   //default:printf("%c\n", (char)key);
  }
  glutPostRedisplay();
 }

 switch (key) {
 case 'a':ex--; ey--; ez--; break;
 case 'z':ex++; ey++; ez++; break;
 case 's':r += 10; break;
 case 'x':r -= 10; break;
 case 'd':oy += 0.1; break;
 case 'c':oy -= 0.1; break;
 case 'w':   
  pdown1 = pdown2 = 0;
  px1 = 0; py1 = 0; pz1 = 0; px2 = 0; py2 = 0; pz2 = 0;
  dflag = 0; hflag = 0;
  if (score > highscore) {
   highscore = score;
  }
  score = 0;
  init(); fitime = 10000;
 glutPostRedisplay();
 }
}

void mouse(int button, int state, int x, int y)
{
 if (state == GLUT_DOWN) {
  switch (button) {
  case GLUT_LEFT_BUTTON:
   r += 15;
   break;
  case GLUT_MIDDLE_BUTTON:
   break;
  case GLUT_RIGHT_BUTTON:
   r -= 15;
   break;
  default:
   break;
  }
  glutPostRedisplay();
 }
}

void init(void)
{
 int i, j, k;
 /* 初期設定 */
 glClearColor(1.0, 1.0, 1.0, 0.0);
 glEnable(GL_DEPTH_TEST);
 glEnable(GL_CULL_FACE);
 glEnable(GL_LIGHTING);
 glEnable(GL_LIGHT0);

 for (i = 0; i < 6; ++i) {
  for (j = 0; j < 9; ++j) {
   for (k = 0; k < 6; ++k) {
	 field[i][j][k] = 0;
   }
  }
 }

 for (i = 0; i < 6; ++i) {
  for (j = 0; j < 6; ++j) {
   field[i][0][j] = -1;
  }
 }
 for (i = 0; i < 6; ++i) {
  for (j = 0; j < 8; ++j) {
   for (k = 0; k < 6; ++k) {
	if (i==0||i==5||k==0||k==5) {
	 field[i][j][k] = -1;
	}
   }
  }
 }

 srand((unsigned int)time(NULL));
 pcolor1 = rand() % 4+1;
 pcolor2 = rand() % 4+1;

}

int main(int argc, char *argv[])
{
 glutInitWindowSize(750, 600);
 glutInit(&argc, argv);
 glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH);
 glutCreateWindow(argv[0]);
 glutDisplayFunc(display);
 glutReshapeFunc(resize);
 glutKeyboardFunc(keyboard);
 glutMouseFunc(mouse);
 init();
 glutTimerFunc(100, timer, 0);
 glutMainLoop();
 return 0;
}