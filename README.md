# Watch-Glasses-OpenGL
It is a version of classic watch ball game made with OpenGL.

Project Instalation
*The project was written using Dev-Cpp Integrated development environment.

1- Download the glut library from the link below.
https://github.com/ismaillkasap/glut-3.7.6

2- Copy the glut64.dll and glut32.dll files to the C:\Windows\System32 folder

3- Copy the glut64.lib and glut32.lib files to the C:\Program Files(x86)\Dev-Cpp\MinGW64\x86_64-w64-mingw32\lib folder.

4- To work with Dev-C ++, add the glut.h file to the C:\Program Files(x86)\Dev-Cpp\MinGW64\x86_64-w64-mingw32\include\GL folder.

5- After opening the project with Dev-Cpp, open the Project Settings screen with Ctrl+H. In the Linker section of the Parameters tab, add the following if none exists. (with hyphen)
-lopengl32
-lglu32
-lglut64

Have fun!
