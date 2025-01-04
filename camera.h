#ifndef CAMERA_H
#define CAMERA_H

#include "types.h"

#ifdef WIN32
#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>
#else
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#endif

struct Clipper{
    float nearp;
    float farp;
    float top;
    float bottom;
    float left;
    float right;
};

// Rotation Indexes
#define ROTATE_PITCH (0)
#define ROTATE_YAW   (1)
#define ROTATE_ROLL	 (2)

class Camera{
    public:
        Camera();
        Camera(Point3d);
        void SetLocation(Point3d);
        Point3d GetLocation() const;
        void Move(float,float,float);
        void GetOrientation();
        void Display();
        bool isInsideFrustrum(Point3d);
    public:
        Point3d position;
        int fov;
        Clipper clip;
//        GLfloat gCameraPosition[]	= { 0.f, 0.f, -555.f };
        GLfloat gCameraRotation[3];
        //Rot3d rotation;
};

#endif