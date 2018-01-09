/*****************************************************************************************/
/**********************************Proje Adý - Watch Glasses******************************/
//**************************************Proje Öðrencileri:********************************/
/********************************1. Nida Süleyman - 1141602604****************************/
/********************************2. Ýsmail Kasap - 1141602036*****************************/
/*****************************************************************************************/
//****************************************User Guide:*************************************/
/************************F tuþu: Oyuna baþlar ve devam etmesini saðlar.*******************/
/************************K tuþu: Kamera açýsýný deðiþtirmektedir.*************************/
/************************R tuþu: Oyun kaybedildiðinde, tekrar baþlatýlmasýný saðlar*******/
/************************1 tuþu: Yanýt olarak en baþtaki bardaðýn seçilmesini saðlar.*****/
/************************2 tuþu: Yanýt olarak ortadaki bardaðýn seçilmesini saðlar.*******/
/************************3 tuþu: Yanýt olarak en sondaki bardaðýn seçilmesini saðlar.*****/
//******************************************Notes:****************************************/
/************************1. En iyi skor oyun kapatýldýðýnda dahil tutulmaktadýr.**********/
/************************2. Oyun içi yönlendirmeler yapýlmýþtýr.**************************/
/************************3. Kamera açýsýna baðlý olarak, yönlendirme ve skor bilgilerinin*/
/************************** textlerin konumu deðiþmektedir.*******************************/
/************************4. FPS deðeri, windows title'ýnda görülebilir.*******************/
/************************5. Belli bir hýza kadar her turda bardaklarýn deðiþme hýzý art-**/
/************************** maktadýr, sonrasýnda çok fazla hýzlanmamasý için sabitlenir.**/	
/*****************************************************************************************/

#include <windows.h>
#include <math.h>
#include <stdlib.h>
#include <gl\glut.h>
#include "glm.c"
#include <unistd.h>

int frame,time,timebase=0;
char s[30];
bool TopCam = false;
bool openState = true;
bool isSwaping = false;
bool isContinue = true;
bool isAnw = false;
bool choAva = false;
int BallPos = 2;
int VisBallPos = 2;
int ChoosenPos = 2;
int Current_Point = 0;
int Best_Point = 0;
int difficulty_level = 15000;
GLuint texName[5];

FILE *SaveFile;	
GLubyte *data;
GLubyte header[54];
GLuint dataPos;			
GLuint width, height;
GLuint imageSize;	
GLboolean isBegin = false;

// Iþýklandýrma deðiþkenleri
float light0_position[] = { 0.5, 0.5, 0.5, 0.0 };
float light0_color[] = { 1.0, 1.0, 1.0, 1.0 };
float light1_position[] = { 0.0, 5.0, 10.0, 1.0 };
float light1_color[] = { 1.0, 0.3, 0.3, 1.0 };
float spot_direction[] = { 0.0, 0.0, 1.0 };

//Masa
float tblx=0.0f,tbly=0.0f,tblz=10.0f;         // Masa'nýn konum koordinatlarý
float tblAci=0.0f;

//Top
float ball_position1[] = { 10.0, 10.5, 10.0};
float ball_position2[] = { 0.0, 10.5, 10.0 };
float ball_position3[] = { -10.5, 10.5, 10.0 };
float ball_position_space[] = { 0.0, 0.0, 0.0 };
// Top'un konum koordinatlarý	

// Kamera deðiþkenleri
float kx=0.0f,ky=15.0f,kz=-45.0f;       // Kameranýn konum koordinatlarý
float kxa=0.0f,kya=0.1f,kza=1.0f;       // Kameranýn baktýðý yön koordinatlarý
float dx=0.0f,dy=1.0f,dz=0.0f;          // Kameranýn üst vektörü koordinatlarý
float kAci=0.0f;
float kameraAci=45.0f;

GLint table_display_list;
GLint glass_display_list;
GLint cube_display_list;
GLint ball_display_list;

struct Glass{
	float locx;
	float locy;
	float locz;
	int CurrentPossition;
	bool isOpened = false;
};

void createGlass(struct Glass glass){
  glPushMatrix();
    glTranslatef(glass.locx, glass.locy, glass.locz);
	glRotatef(180, 0.0f, 0.0f, 1.0f);				
	glCallList(glass_display_list);
	glPopMatrix();
	glFlush();
}

void createBall(struct Glass glass){
  glPushMatrix();
    glTranslatef(glass.locx, glass.locy, glass.locz);
	glRotatef(180, 0.0f, 0.0f, 1.0f);				
	glCallList(ball_display_list);
	glPopMatrix();
	glFlush();
}
	
struct Glass glass1;
struct Glass glass2;
struct Glass glass3;

char rota[] = {'^', '^', '^', '<', '^', '^', '^', '>', '^', '^', '^', '<',
               '^', '^', '<', '^', '^', '^', '^', '^', '^', '^', '<', '^',
               '^', '^', '^', '^', '^', '<'}; 
unsigned char rotaPos, curPos=0;      // Rota içinde nerede olduðumuzu saklar 

GLuint drawTable() {
    GLMmodel* tblModel = glmReadOBJ("objs/table.obj");
	glmUnitize(tblModel);
    glmScale(tblModel, 20);
	glmFacetNormals(tblModel);
	glmVertexNormals(tblModel, 90.0);
	glmDraw(tblModel, GLM_SMOOTH | GLM_MATERIAL);
}

GLuint drawBall() {
    GLMmodel* ballModel = glmReadOBJ("objs/soccer ball.obj");
	glmUnitize(ballModel);
    glmScale(ballModel, 0.7);
	glmFacetNormals(ballModel);
	glmVertexNormals(ballModel, 90.0);
	glmDraw(ballModel, GLM_SMOOTH | GLM_MATERIAL);
}

GLuint drawGlass() {
    GLMmodel* cupModel = glmReadOBJ("objs/cup.obj");
	glmUnitize(cupModel);
    glmScale(cupModel, 1.5);
	glmFacetNormals(cupModel);
	glmVertexNormals(cupModel, 90.0);
	glmDraw(cupModel, GLM_SMOOTH | GLM_MATERIAL);
}

void drawCube(){        // içinde gezindiðimiz tünel parçasý
//  Tavan
	glColor3f(0.8f, 0.4f, 0.3f);
	glBegin(GL_QUADS);
		glTexCoord2f(0.0, 0.0); 
		glVertex3f(-100.0f, 100.0f, -100.0f);
		glTexCoord2f(0.0, 1.0); 
		glVertex3f(-100.0f, 100.0f,  100.0f);
		glTexCoord2f(1.0, 1.0); 
		glVertex3f( 100.0f, 100.0f,  100.0f);
		glTexCoord2f(1.0, 0.0); 
		glVertex3f( 100.0f, 100.0f, -100.0f);
	glEnd();
    glEnable(GL_TEXTURE_2D); 
//  Taban
    glColor3f(0.3f, 0.6f, 0.9f);
    	glBindTexture(GL_TEXTURE_2D, texName[1]);
	glBegin(GL_QUADS);
		glTexCoord2f(0.0, 0.0); 
        glVertex3f(-100.0f, 0.0f, -100.0f);
		glTexCoord2f(0.0, 10.0); 
        glVertex3f(-100.0f, 0.0f,  100.0f);
		glTexCoord2f(10.0, 10.0); 
        glVertex3f( 100.0f, 0.0f,  100.0f);
		glTexCoord2f(10.0, 0.0); 
        glVertex3f( 100.0f, 0.0f, -100.0f);
	glEnd();

   
	glBindTexture(GL_TEXTURE_2D, texName[0]);
//  Sað Duvar

    if (rota[rotaPos] != '>') {
        glColor3f(0.3f, 0.6f, 0.9f);
    	glBegin(GL_QUADS);
    		glTexCoord2f(0.0, 1.0); 
    		glVertex3f(-100.0f, 0.0f, -100.0f);
    		glTexCoord2f(2.0, 1.0); 
    		glVertex3f(-100.0f, 0.0f,  100.0f);
    		glTexCoord2f(2.0, 0.0); 
    		glVertex3f(-100.0f, 100.0f, 100.0f);
    		glTexCoord2f(0.0, 0.0); 
    		glVertex3f(-100.0f, 100.0f, -100.0f);
    	glEnd();
    }

//  Sol Duvar
    if (rota[rotaPos] != '<') {
        glColor3f(0.3f, 0.6f, 0.9f);
    	glBegin(GL_QUADS);
    		glTexCoord2f(0.0, 1.0); 
    		glVertex3f(100.0f, 0.0f, -100.0f);
    		glTexCoord2f(2.0, 1.0); 
    		glVertex3f(100.0f, 0.0f,  100.0f);
    		glTexCoord2f(2.0, 0.0); 
    		glVertex3f(100.0f, 100.0f, 100.0f);
    		glTexCoord2f(0.0, 0.0); 
    		glVertex3f(100.0f, 100.0f, -100.0f);
    	glEnd();
    }

//  Ön Duvar
    if (rota[rotaPos] != '^') {
        glColor3f(1.0f, 1.0f, 1.0f);
        glBegin(GL_QUADS);
        	glTexCoord2f(0.0, 1.0); 
        	glVertex3f(-100.0f, 0.0f, 100.0f);
        	glTexCoord2f(2.0, 1.0); 
        	glVertex3f(100.0f, 0.0f,  100.0f);
        	glTexCoord2f(2.0, 0.0); 
        	glVertex3f(100.0f, 100.0f, 100.0f);
        	glTexCoord2f(0.0, 0.0); 
        	glVertex3f(-100.0f, 100.0f, 100.0f);
        glEnd();
    }  
	glDisable(GL_TEXTURE_2D);  
}

GLuint loadBMP_custom(const char * imagepath){
	// Open the file
	FILE * file = fopen(imagepath, "rb");
	if (!file){
		printf("Image could not be opened\n"); 
		return 0;
	}
	if ( fread(header, 1, 54, file)!=54 ){ // If not 54 bytes read : problem
		printf("Not a correct BMP file\n");
		return 0;
	}
	if ( header[0]!='B' || header[1]!='M' ){
		printf("Not a correct BMP file\n");
		return 0;
	}
	// Read ints from the byte array
	dataPos    = *(int*)&(header[0x0A]);
	imageSize  = *(int*)&(header[0x22]);
	width      = *(int*)&(header[0x12]);
	height     = *(int*)&(header[0x16]);
	// Some BMP files are misformatted, guess missing information
	if (imageSize==0)    imageSize=width*height*3; // 3 : one byte for each Red, Green and Blue component
	if (dataPos==0)      dataPos=54; // The BMP header is done that way

	// Create a buffer
	data = new unsigned char [imageSize];
	// Read the actual data from the file into the buffer
	fread(data,1,imageSize,file);
	//Everything is in memory now, the file can be closed
	fclose(file);
}

void TextureCover(){    
	glGenTextures(2, texName); // texName dizisi 3 farklý doku için kullanýlacak (ikisi BMP)

	loadBMP_custom("texture/brick.bmp");
	glBindTexture(GL_TEXTURE_2D, texName[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, data);
	
	loadBMP_custom("texture/base.bmp");
	glBindTexture(GL_TEXTURE_2D, texName[1]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, data);	
	
	loadBMP_custom("texture/Ilisionistic.bmp");	
	glBindTexture(GL_TEXTURE_2D, texName[2]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	 		
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, data);	
}

// Görüntüleme Listeleri (Display Lists) oluþturur
GLuint createDL(char object) {
	GLuint dList;

	dList = glGenLists(1);

	glNewList(dList,GL_COMPILE);
		if(object == 't') drawTable();
		if(object == 'b') drawBall();			
		if(object == 'g') drawGlass();	
		if (object == 'c') drawCube();
	glEndList();

	return(dList);
}

void drawStrokeText(char*string,int x,int y,int z, char pos)
{
	  char *c;
	  glPushMatrix();
	  glTranslatef(x+146, y-17, z);
	  glRotatef(180, 0.0f, 0.0f, 1.0f);
	
	  if(pos == 'c'){
	  	glScalef(0.04f,-0.04f,z);	
	  }else if(pos == 'k'){
	  	glScalef(0.02f,-0.02f,z);	
	  }else if(pos == 't'){
	  	glScalef(0.01f,-0.01f,z);	
	  }
	  
	  for (c=string; *c != '\0'; c++)
	  {
    		glutStrokeCharacter(GLUT_STROKE_ROMAN , *c);
	  }
	  glPopMatrix();
}


void initScene() {    
    glEnable(GL_DEPTH_TEST);
    
    table_display_list = createDL('t');
    ball_display_list = createDL('b');
    glass_display_list = createDL('g');
    cube_display_list = createDL('c');    

//  LIGHTS
    glLightfv(GL_LIGHT0, GL_POSITION, light0_position);     // Genel ýþýk
    glLightfv(GL_LIGHT0, GL_SPECULAR, light0_color);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_color);
  
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
}

float stdCenterTextX = -100.0f;
float stdCenterTextY = 50.0f;
float stdCenterTextZ = 80.0f; 



void renderScene(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	char current_point[100];
	char best_point[100];
	sprintf(current_point, "Current Score: %d", Current_Point);
	sprintf(best_point, "Best Score: %d", Best_Point);
	if(TopCam != true){
		if(isBegin != true){
			drawStrokeText("press the 'f' key to start the game.",stdCenterTextX,50.0f, stdCenterTextZ, 'c');	
		}else if(isSwaping != true && isBegin != false){	
			drawStrokeText("choose one of '1', '2' or '3'.",stdCenterTextX-12.f, stdCenterTextY, stdCenterTextZ, 'c');	
		}else if(isAnw == true && isBegin != false && isContinue != false){
			drawStrokeText("correct, press 'f' to continue.", stdCenterTextX-10.0f, stdCenterTextY, stdCenterTextZ, 'c');	
		}else if(isAnw == true && isContinue == false){		
			drawStrokeText("game over",-133.0f,50.0f,80.f, 'c');	
			drawStrokeText("press 'r' to restart game.",stdCenterTextX-15.0f, stdCenterTextY-10.0f, stdCenterTextZ, 'c');	
		}		
		glPushMatrix();
			drawStrokeText(current_point , -205.0f, 75.0f, 80.f, 'k');		
			drawStrokeText(best_point,-205.0f,80.0f,80.f, 'k');	
		glPopMatrix();		
	}else{
		if(isBegin != true){
			glPushMatrix();
		    	glRotatef(90, 1.0f, 0.0f, 0.0f);	
				drawStrokeText("press the 'f' key to start the game.",-134.0,32.5f, -10.0, 't');	
						
			glPopMatrix();
		}else if(isSwaping != true && isBegin != false){
			glPushMatrix();	
				glRotatef(90, 1.0f, 0.0f, 0.0f);	
				drawStrokeText("choose one of '1', '2' or '3'.", -134.0-2.0f, 32.5f, -10.0, 't');
			glPopMatrix();	
		}else if(isAnw == true && isBegin != false && isContinue != false){
			glPushMatrix();	
				glRotatef(90, 1.0f, 0.0f, 0.0f);
				drawStrokeText("correct, press 'f' to continue.", -134.0-3.0f, 32.5f, -10.0, 't');	
			glPopMatrix();
		}else if(isAnw == true && isContinue == false){	
			glPushMatrix();	
				glRotatef(90, 1.0f, 0.0f, 0.0f);	
				drawStrokeText("game over",-134.0-9.0f,32.5f+1.0f,-10.0, 't');	
				drawStrokeText("press 'r' to restart game.",-134.0-4.0f, 32.5-1.0f, -10.0, 't');
			glPopMatrix();	
		}
		glPushMatrix();	
			glRotatef(90, 1.0f, 0.0f, 0.0f);				
			drawStrokeText(current_point , -134.0+5.0f, 32.5f-13.0, -10.0, 't');	
			drawStrokeText(best_point, -134.0-19.0f,32.5f-13.0,-10.0, 't');	
		glPopMatrix();
	}
	
//Spot Iþýklar
    glLightfv(GL_LIGHT1, GL_POSITION, light1_position);    
    glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, spot_direction);
    glLightfv(GL_LIGHT1, GL_SPECULAR, light1_color);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, light1_color);  
    glLightf(GL_LIGHT1, GL_SPOT_CUTOFF,180.0);              // far ýþýðýnýn yayýlma açýsý
    glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, 1.0);      // mesafe arttýkça far ýþýðýnýn gücünün azalmasý
    glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, 1.0);             // merkezde daha aydýnlýk, kenarlarda daha sönük ýþýk için
    
    glEnable(GL_TEXTURE_2D); 
//  Table çizilir
    glEnable(GL_TEXTURE_2D); 
	    glPushMatrix();    	  
	    	glBindTexture(GL_TEXTURE_2D, texName[2]);	 			        
		    glTranslatef(tblx, tbly, tblz);
			glRotatef(90, 0.0f, 1.0f, 0.0f);
			glCallList(table_display_list);
		glPopMatrix();		
	glDisable(GL_TEXTURE_2D);

//  Top çizilir	
    glPushMatrix(); 
	if(BallPos == 0){
		glTranslatef(ball_position_space[0], ball_position_space[1], ball_position_space[2]);		
	}
	if(BallPos == 1){
		glTranslatef(ball_position1[0], ball_position1[1], ball_position1[2]);		
	}  
	if(BallPos == 2){
		glTranslatef(ball_position2[0], ball_position2[1], ball_position2[2]);		
	}  
	if(BallPos == 3){
		glTranslatef(ball_position3[0], ball_position3[1], ball_position3[2]);		
	}         	
	glCallList(ball_display_list);
	glPopMatrix();			

	createGlass(glass1);
	createGlass(glass2);
	createGlass(glass3);
	
//  Sahne       
    drawCube();    
    glEnable(GL_TEXTURE_2D); 
	glBindTexture(GL_TEXTURE_2D, texName[0]);
	glPushMatrix();    
	    glTranslatef(tblx, tbly, tblz);
		glRotatef(0, 0.0f,01.0f, 0.0f);
		    glColor3f(1.0f, 1.0f, 1.0f);
	        glBegin(GL_QUADS);
	        	glTexCoord2f(0.0, 1.0); 
	        	glVertex3f(-100.0f, 0.0f, 88.0f);
	        	glTexCoord2f(2.0, 1.0); 
	        	glVertex3f(100.0f, 0.0f,  88.0f);
	        	glTexCoord2f(2.0, 0.0); 
	        	glVertex3f(100.0f, 100.0f, 88.0f);
	        	glTexCoord2f(0.0, 0.0); 
	        	glVertex3f(-100.0f, 100.0f, 88.0f);
	        glEnd();
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);	
	glutPostRedisplay();
	glutSwapBuffers();
	
	frame++;
	time=glutGet(GLUT_ELAPSED_TIME);
	if (time - timebase > 1000) {
		sprintf(s,"Watch Glasses FPS:%4.2f",frame*1000.0/(time-timebase));
		glutSetWindowTitle(s);
		timebase = time;
		frame = 0;
	}
}

void changeSize(int w, int h) {
if(h == 0) h = 1;
	float ratio = 1.0f * w / h;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
    glViewport(0, 0, w, h);
	gluPerspective(45,ratio,1,2000);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(kx, ky, kz, kx + kxa, ky + kya, kz + kza, dx, dy, dz);
}

void swapGlasses(struct Glass &lowGl, struct Glass &highGl){
	int diff = abs(highGl.CurrentPossition - lowGl.CurrentPossition);		
			
	for(int i=0;i<50*diff;i++){
		if(i<25*diff){
			lowGl.locx -= 0.2f;
			lowGl.locz += 0.1f;	
			highGl.locx += 0.2f;
			highGl.locz -= 0.1f;					
			usleep(difficulty_level);
			renderScene();			
		}else{
			lowGl.locx -= 0.2f;
			lowGl.locz -= 0.1f;
			highGl.locx += 0.2f;
			highGl.locz += 0.1f;	
			usleep(difficulty_level);
			renderScene();
		}			
	}
	if(difficulty_level >= 0){
		difficulty_level -= 500;	
	}	
	
	if(VisBallPos == lowGl.CurrentPossition){
		VisBallPos = highGl.CurrentPossition;
	}
	else if(VisBallPos == highGl.CurrentPossition){
		VisBallPos = lowGl.CurrentPossition;
	}
	
	int TempPossition = lowGl.CurrentPossition;
	lowGl.CurrentPossition = highGl.CurrentPossition;	
	highGl.CurrentPossition = TempPossition;	
}

int randomNumber(){
	return rand() % 3;
}

int randomNumber(int selected_number){
	int TempRand = rand() % 3;
	if(TempRand == selected_number){
		randomNumber(selected_number);
	}
	else{
		return TempRand;
	}
}

struct Glass& getGlass(int index){
	if(index == 0){
		return glass1;
	}
	else if(index == 1){
		return glass2;
	}else{
		return glass3;
	}
}

struct Glass& getPosGlass(int index){
	if(glass1.CurrentPossition == index){
		return glass1;
	}else if(glass2.CurrentPossition == index){
		return glass2;
	}else if(glass3.CurrentPossition == index){
		return glass3;
	}			
}

void openGlass(struct Glass &glass){
		for(int i=0;i<10;i++){
			glass.locy += 0.2f;			
			glass.locz -= 0.2f;	
			usleep(10000);
			renderScene();			
	}
	openState = true;	
}

void closeGlass(struct Glass &glass){
		for(int i=0;i<10;i++){
			glass.locy -= 0.2f;			
			glass.locz += 0.2f;	
			usleep(10000);
			renderScene();			
	}	
	openState = false;
}

bool checkAnw(int ball_pos, int asw_pos){
		isAnw = true;
		choAva = false;
		if(BallPos == ChoosenPos){
			isContinue = true;
			Current_Point += 100;
			
			if(Current_Point > Best_Point){
				Best_Point = Current_Point;
				SaveFile = fopen("save.txt", "w");
				fprintf(SaveFile, "%d", Best_Point);
				fclose(SaveFile);
			}
		}else{
			isContinue = false;
		}
}

int rnd1, rnd2;
void processNormalKeys(unsigned char key, int x, int y) { 	
	if(choAva != true && isContinue != false){   
		if (key == 'f') { 	
			isAnw = false;	
			isSwaping = true;	
			closeGlass(getPosGlass(ChoosenPos));						
			if(isBegin != true){
				isBegin = true;		
			}		
			BallPos = 0;
			int rndIterationNumber = (rand() % 5) + 5;
			for(int i=0; i<=rndIterationNumber; i++){
				rnd1 = randomNumber();
	    		rnd2 = randomNumber(rnd1);
			  
				struct Glass &a = getGlass(rnd1);
				struct Glass &b = getGlass(rnd2);	
				if(a.CurrentPossition < b.CurrentPossition){			
					swapGlasses(a,b);
				}else{
					swapGlasses(b,a);			
				}	
			} 
			isSwaping = false;
			BallPos = VisBallPos;
			choAva = true;
	    } 
	}	  
    if(key == 'k'){
		if(TopCam == false){
			glRotatef(270, 1.0f, 0.0f, 0.0f);	
			glTranslatef(0.0f, 0.0f, 3.0f);	
			TopCam = true;
		}else{
			glTranslatef(0.0f, 0.0f, -3.0f);
			glRotatef(-270, 1.0f, 0.0f, 0.0f);	    		
			TopCam = false;
		}    	
	}  
	else if(key == 'r'){
		closeGlass(getPosGlass(ChoosenPos));
		Current_Point = 0;
		BallPos = 2;
		ChoosenPos = 2;
		VisBallPos = 2;
		isBegin = false;
		isContinue = true;
		choAva = false;		
		difficulty_level = 15000;
		struct Glass &a = getPosGlass(2);
		openGlass(a);
	} 
	if(choAva == true){
		if(key == '1'){
		struct Glass &a = getPosGlass(1);
		
		if(openState != false){
			closeGlass(a);
		}else{
			openGlass(a);
		}	
		ChoosenPos =1;
		isSwaping = true;	
		checkAnw(BallPos, ChoosenPos);	
		} 
			else if(key == '2'){
			struct Glass &a = getPosGlass(2);
			
			if(openState != false){
				closeGlass(a);
			}else{
				openGlass(a);
			}	
			ChoosenPos =2;
			isSwaping = true;
			checkAnw(BallPos, ChoosenPos);			
		} 
			else if(key == '3'){
			struct Glass &a = getPosGlass(3);
			
			if(openState != false){
				closeGlass(a);
			}else{
				openGlass(a);
			}	
			ChoosenPos =3;	
			isSwaping = true;	
			checkAnw(BallPos, ChoosenPos);
		} 	
	}
}

int main(int argc, char **argv){
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(0,0);
	glutInitWindowSize(1200,700);
	glutCreateWindow("Watch Glasses");
	
	
	SaveFile = fopen("save.txt", "r+");

	if (SaveFile == NULL)
	{
		printf("\ncan't open save file\n");
		SaveFile = fopen("save.txt", "w+");
	}else{		
		fscanf(SaveFile,"%d",&Best_Point);
	}
	fclose(SaveFile);
	
	glass1.locx = 10.0f; glass1.locy = 11.0f; glass1.locz = 10.0f; glass1.CurrentPossition = 1; glass1.isOpened = false;
	glass2.locx = 0.0f; glass2.locy = 11.0f; glass2.locz = 10.0f; glass2.CurrentPossition = 2; glass2.isOpened = false;
	glass3.locx = -10.0f; glass3.locy = 11.0f; glass3.locz = 10.0f; glass3.CurrentPossition = 3; glass3.isOpened = false;

	initScene();

	if(isBegin != true){
		openGlass(glass2);
	}

	//glutIdleFunc(renderScene);
	glutDisplayFunc(renderScene);	
	glutIgnoreKeyRepeat(0);		
	glutReshapeFunc(changeSize);
	glutKeyboardFunc(processNormalKeys);
	TextureCover();
	glutMainLoop();

	return(0);
}
