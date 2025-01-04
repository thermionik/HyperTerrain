
#include "vertexmorph.h"

float VertexMorph::stepAmount = (float)0.1;

VertexMorph::VertexMorph()// : finished(false), T(0.0), vertToMorph(NULL)
{
	finished = 0;
	T = 0.0;
	vertToMorph = NULL;

    to.x = to.y = to.z = 0.0;
    from.x = from.y = from.z = 0.0;
};

VertexMorph::VertexMorph(const Vertex& vertTo, const Vertex& vertFrom, RefCntPtr<Vertex> vert) : finished(false), T(0.0)
{
    vertToMorph = vert;
    to = vertTo;
    from = vertFrom;
    
    vertToMorph->x = from.x;
    vertToMorph->y = from.y;
    vertToMorph->z = from.z;

};

VertexMorph::VertexMorph(const VertexMorph& rhs) : finished(0), T(0.0)
{
    vertToMorph = rhs.vertToMorph;
    to = rhs.to;
    from = rhs.from;
};

VertexMorph::~VertexMorph() {
    vertToMorph = NULL;    
};

void VertexMorph::Update()
{
	if ( vertToMorph.Unique() )
    {
        finished = true;
        return;
    }
      
    //   vertToMorph->x = from.x + (to.x - from.x) * T;
    vertToMorph->y = from.y + (to.y - from.y) * T;
    //   vertToMorph->z = from.z + (to.z - from.z) * T;
    T += stepAmount;
    
    if ( T >= 1.1 ){
        finished = true;
    //             vertToMorph->x = to.x;
                vertToMorph->y = to.y;
    //             vertToMorph->z = to.z;
    }
};

bool VertexMorph::Finished()
{
    return finished;
};

VertexMorph& VertexMorph::operator=(const VertexMorph& rhs)
{
	T = rhs.T;
	vertToMorph = rhs.vertToMorph;
	to = rhs.to;
	from = rhs.from;
		
	return *this;
};
