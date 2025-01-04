#ifndef DSCAPE_H
#define DSCAPE_H

#include <list>
using namespace std;

#include "SDL/SDL_opengl.h"
#include "types.h"
#include "quadtritree.h"
#include "quadstack.h"
#include "camera.h"
#include "vertexmorph.h"
#include "wtimer.h"


// These are for our multitexture defines
#define GL_TEXTURE0_ARB                     0x84C0
#define GL_TEXTURE1_ARB                     0x84C1

#define GL_COMBINE_ARB						0x8570
#define GL_RGB_SCALE_ARB					0x8573

// Here are the multitexture function prototypes
typedef void (APIENTRY * PFNGLMULTITEXCOORD2FARBPROC) (GLenum target, GLfloat s, GLfloat t);
typedef void (APIENTRY * PFNGLACTIVETEXTUREARBPROC) (GLenum target);

// Here we extern our function pointers for multitexturing capabilities
extern PFNGLMULTITEXCOORD2FARBPROC			glMultiTexCoord2fARB;
extern PFNGLACTIVETEXTUREARBPROC			glActiveTextureARB;


class Volume;
class VertexMorph;
class Camera;

class DPatch{
    public:
        DPatch();
        ~DPatch();
        void Initialize(Volume*, int, int);
		void SetTextureCoord(float , float );
        int Render(const Camera*, WTimer *timer);
        void Tesselate(const Camera*);

#ifdef GEOMORPH 
        void Update();
#endif

        void SetPriority(float);
        void SetTextureIDs(int, int, int);
		void RenderWater(WTimer *timer);
        void ToggleSplitOnly();
        void DrawNormals(int);
        float GetPriority();

        int NumTri();
        int NumVertices();
        
        Vertex* newVertex();
#ifdef GEOMORPH 
        static void NewGeoMorph(VertexMorph&);
#endif
        
    protected:
        void InitMesh();
        void InitVariance();
        void Reset();
        void ResetMesh();
        void ResetQueues();
    
        // Recursive Split
        void Split(QuadTriTree*, const Camera*);
        // Recursive Merge
        void Merge(QuadTriTree*, const Camera*);
        bool InMergeQ(QuadTriTree*);
        bool InSplitQ(QuadTriTree*);
        int ComputeVariance(int, int, int, int, int, int, int, unsigned char*);
        void PriorityEnqueue(QuadTriTree*, int);
        
    protected:
        Volume* volume;
    
        QuadTriTree base1, base2;
        unsigned char* variance1, *variance2;
        LARGE_INTEGER TimeStart;
		float anim;
        int varsize;
        float splitpriority;
        bool drawNormals;        
        bool ResetDiamond;
        bool SplitOnly;
        int TextureID_base, TextureID_detail, TextureID_water;
               
        QuadStack SplitAbove;
        QuadStack SplitBelow;
        QuadStack MergeAbove;
        QuadStack MergeBelow;
#ifdef GEOMORPH         
        static list<VertexMorph> Morph;
#endif
};

class DScape {
    public:
        void Initialize(Volume*);
        void Tessellate();
        void Render();
    
    private:
        void Reset();
    
    protected:
//        static DPatch TriPool[][];
        
        
};

#endif

