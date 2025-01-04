#ifndef TYPES_H
#define TYPES_H

//#include "utility.h"

/*#ifndef MAXFLOAT
#define MAXFLOAT  99999999
#endif
*/


typedef struct vertex{
    vertex() : x(0.0), y(0.0), z(0.0) {}
    vertex(const vertex& nvert) : x(nvert.x), y(nvert.y), z(nvert.z) {}
    vertex(float xc, float yc, float zc){ x = xc; y = yc; z = zc; }
    float x,y,z;
} Vertex;




struct TPos {
    float x, y, z;
//    int count;
//    int clip;
};
/*
struct RichPoint3d {
    float x, y, z;
    char texcoord[2];
    Vector3 normal;
};
*/
struct Point3d {
    float x, y, z;
};



enum QType {None, SAbove, SBelow, MAbove, MBelow};
#define SPLIT SBelow
#define MERGE MAbove

#ifndef SQR
#define SQR(a) ( (a) * (a) )
#endif

#ifndef MAX
#define MAX(a,b) ((a < b) ? (b) : (a))
#endif

#ifndef NULL
#define NULL 0
#endif

#define clActive 0x1000
#define clLeft 0x1
#define clRight 0x2
#define clTop 0x4
#define clBottom 0x8
#define clNear 0x10
#define clFar 0x20
#define clOut 0x40
#define clIn 0x80

#define NEAR_CLIP .1
#define FAR_CLIP 2500



#endif