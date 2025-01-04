#ifndef VERTEXMORPH_H
#define VERTEXMORPH_H

#include "types.h"
#include "refcntptr.h"

#ifndef NULL
#define NULL 0
#endif

class VertexMorph
{
    public:
        VertexMorph();
        VertexMorph(const Vertex& vertTo, const Vertex& vertFrom, RefCntPtr<Vertex> vert);
		VertexMorph(const VertexMorph&);
        ~VertexMorph();
        void Update();
        bool Finished();
        VertexMorph& operator=(const VertexMorph&);
    private:
        bool finished;
        float T;
        RefCntPtr<Vertex> vertToMorph;
        Vertex to;
        Vertex from;
        static float stepAmount;
};

#endif        
