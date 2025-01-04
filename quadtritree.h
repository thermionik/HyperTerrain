#ifndef QUADTRITREE_H
#define QUADTRITREE_H

#include "types.h"
#include "utility.h"
#include "refcntptr.h"

//#define GEOMORPH 

class Camera;
class Volume;
struct QuadStackNode;
class QuadStack;


class QuadTriTree{
        enum {MINPRIORITY=0, MAXPRIORITY=9999999};
    public:
        QuadTriTree();
        ~QuadTriTree();
  //      QuadTriTree(QuadTriTree*);
        QuadTriTree* NewTri(QuadTriTree*, int);
        void DeleteTri(QuadTriTree*);
		void Destroy();
        void Prioritize(const Point3d&);
//        void Split(QuadTriTree*);
        void Split();
        void Split(Volume*);
//        void Merge(QuadTriTree*);
        void Merge();        
        Vector3 Normal();
        bool IsVisible(const Camera*);
        bool IsVisible();
        bool HasChildren();
        bool IsMergeAble();
        Point3d CenterPoint();
        int Orientation();
        void Collapse();
  private:
        Vertex* newVertex();
        bool HasNeighbors();
        
    public:
        QuadTriTree *parent;
        QuadTriTree *n1, *n2, *n3; //Neighbors
        QuadTriTree *a, *b, *c, *d; //Children
        
        //RefCntPtr<Vertex> v1, v2, v3; //Vertices
		Vertex *v1, *v2, *v3; //Vertices
//        int clipcode;
        float priority;
        unsigned char* variance;  
        int varindex;
        
        QuadStackNode* queueloc;
        QuadStack* queue;
//        static QuadTriTree* TriPool[];
        static int triCount;
        static int vertCount;
};



#endif