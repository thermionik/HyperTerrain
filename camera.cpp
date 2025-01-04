#include "camera.h"


#ifndef WIN32
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#else
#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#endif

Camera::Camera(){ 
    position.x = 59; 
    position.y = 10;
    position.z = 59; 

    gCameraRotation[0] = 0.f;
    gCameraRotation[1] = 90.f;
    gCameraRotation[2] = 0.f;
};

Camera::Camera(Point3d startPos) : position(startPos) {};

Point3d Camera::GetLocation() const
{
    return position; 
};
void Camera::SetLocation(Point3d pos)
{
    position = pos; 
};
void Camera::GetOrientation(){};

void Camera::Move(float x, float y, float z)
{
    position.x += x;
    position.y = y;
    position.z += z;
};

void Camera::Display()
{
	// Clear the GL buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	//glPushMatrix();
    
//    gluLookAt(0.0, 0.0, -10.0, 0, 0.0, 0, 0.0, 1.0, 0.0);
    // Rotate to the Camera Rotation angle
    glRotatef(gCameraRotation[ROTATE_PITCH], 1.f, 0.f, 0.f);
    glRotatef(gCameraRotation[ROTATE_YAW],   0.f, 1.f, 0.f);

    // Move to the Camera Position
    glTranslatef(-position.x, -position.y , -position.z);
};