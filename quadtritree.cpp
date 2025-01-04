#include "quadtritree.h"
#include "types.h"
#include "dpatch.h"
#include "volume.h"
#include "utility.h"

/*	This code is only meant to be used as an internal datatype for the Diamond	Landscape class. 

*/

int QuadTriTree::triCount = 0;
int QuadTriTree::vertCount = 0;

// Miscelaneous math functions
Point3d midPoint(Point3d* p1, Point3d *p2){
    Point3d mid;
    mid.x = (p1->x + p2->x) / 2;
    mid.y = (p1->y + p2->y) / 2;    
    mid.z = (p1->z + p2->z) / 2;
    return mid;
};

/* Returns distance^2 */
float CalcDistance(Point3d p1, Point3d p2){
    float result;
    result = SQR(p1.x - p2.x) + SQR(p1.y - p2.y) + SQR( p1.z - p2.z);
    return result;
};      


QuadTriTree::QuadTriTree(){
    n1 = n2 = n3 = a = b = c = d = NULL;
    v1 = v2 = v3 = NULL; //Vertices
//    clipcode = 0;
    priority = 0;
    variance = NULL;  
    varindex = 0;
    queue = NULL;
    queueloc = NULL;
    
    triCount++;
};

QuadTriTree::~QuadTriTree(){


    if ( v1 )
    {
        delete v1;    
		vertCount--;
    }
    if ( v2 )
    {
        delete v2;
        vertCount--;
    }
    if ( v3 )
    {
        delete v3; 
        vertCount--;
    }
  
	triCount--;
};

// TODO: Cause these 2 functions to pull data from a common pool
Vertex* QuadTriTree::newVertex(){
    Vertex* newpos;
    newpos = new Vertex;
    vertCount++;
    return newpos;
};

QuadTriTree* QuadTriTree::NewTri(QuadTriTree* Dparent, int index){
    QuadTriTree* newTri = new QuadTriTree();
    if (newTri){
        newTri->variance = variance;
        newTri->varindex = (varindex << 2) + index;
        newTri->parent = Dparent;
        return newTri;
    }
    else
        return NULL;
};

// TODO: END

void QuadTriTree::DeleteTri(QuadTriTree* trytokill){
    if (trytokill)
        if ( !trytokill->HasChildren() )
            delete trytokill;
};

//Tells whether or not a triangle faces up or down
int QuadTriTree::Orientation()
{ 
  if ( v1->z > v2->z )
        return 1;
    else
        return -1; 
};

void QuadTriTree::Prioritize(const Point3d &pos){
    if ( this )
    {
        if ( varindex >= 262655 || varindex < 0)// || clipcode != 0 )
            priority = MINPRIORITY;
        else
        {
            float distance;
            distance = CalcDistance( pos, CenterPoint() );
            // the variance is squared since CalcDistance returns distance^2
            priority = SQR(variance[varindex]) / distance;
        }
    }
};


Vector3 QuadTriTree::Normal()
{
    Vector3 normal;
    Vector3 one, two;

    if ( Orientation() < 0 )
    {
        one.x = v3->x - v2->x;
        one.y = v3->y - v2->y;
        one.z = v3->z - v2->z;
        
        two.x = v1->x - v2->x;
        two.y = v1->y - v2->y;
        two.z = v1->z - v2->z;
    }
    else
    {
        one.x = v2->x - v3->x;
        one.y = v2->y - v3->y;
        one.z = v2->z - v3->z;
        
        two.x = v1->x - v3->x;
        two.y = v1->y - v3->y;
        two.z = v1->z - v3->z;
    }
    normal.x = (one.y * two.z) - (two.y * one.z);
    normal.y = (two.x * one.z) - (one.x * two.z);
    normal.z = (one.x * two.y) - (two.x * one.y);
    
    if ( normal.y < 0 )
    {
        normal.x *= -1;
        normal.y *= -1;        
        normal.z *= -1;
    }
    
    return normal;
};  

void QuadTriTree::Split(Volume* volumeptr){
//    if (qtree)
        if ( !HasChildren() )
        {
            a = NewTri(this, 1);
            b = NewTri(this, 2);
            c = NewTri(this, 3);
            d = NewTri(this, 4);
//            if (!a || !b || !c || !d)
                //exit(1);
                
            //triCount += 4;
            
            //Assign Vertices - Reusing if possible
            // Side 1 of parent //
            if ( n1->HasChildren() )
            {
                //Common vertices
                a->v1 = b->v2 = c->v1 = n1->a->v3;
                
                //Assign neighbors
                b->n1 = n1->c;
                n1->c->n3 = b;
                c->n1 = n1->d;
                n1->d->n3 = c;
            }
            else
            { //No neighbor or no children so compute midpoint
                Vertex* mid = newVertex();
				//RefCntPtr<Vertex> mid( newVertex() );
                Vertex from, to;
                //Compute new 3d position
                from.x = (v1->x + v2->x) / 2;
                from.z = (v1->z + v2->z) / 2;
                from.y = (v1->y + v2->y) / 2;

                to.x = from.x;
                to.z = from.z;
                to.y = volumeptr->GetHeight(to.x, to.z);
				//to.y = volumeptr->AvgHeight(to.x, to.z);
#ifdef GEOMORPH
                mid->x = from.x;
                mid->y = from.y;
                mid->z = from.z;
                
                DPatch::NewGeoMorph( VertexMorph(to, from, mid) );
#else
                mid->x = to.x;
                mid->y = to.y;
                mid->z = to.z;
#endif
                
                //Common vertices
                a->v1 = b->v2 = c->v1 = mid;
                
                //Assign neighbors
                b->n1 = n1;
                c->n1 = n1;
            }            
            // Side 2 of parent//
            if ( n2->HasChildren() )
            {
                //common vertices
                a->v2 = b->v3 = d->v1 = n2->a->v2;
                //Assign neighbors
                b->n2 = n2->b;
                n2->b->n2 = b;
                d->n2 = n2->d;
                n2->d->n2 = d;
            }
            else
            { //No neighbor or no children so compute midpoint
                Vertex* mid = newVertex();
				//RefCntPtr<Vertex> mid( newVertex() );
                Vertex from, to;
                //Compute new 3d position
                from.x = (v1->x + v3->x) / 2;
                from.z = (v1->z + v3->z) / 2;
                from.y = (v1->y + v3->y) / 2;

                to.x = from.x;
                to.z = from.z;
                to.y = volumeptr->GetHeight(to.x, to.z);
				//to.y = volumeptr->AvgHeight(to.x, to.z);
#ifdef GEOMORPH
                mid->x = from.x;
                mid->y = from.y;
                mid->z = from.z;
                
                DPatch::NewGeoMorph( VertexMorph(to, from, mid) );
#else
                mid->x = to.x;
                mid->y = to.y;
                mid->z = to.z;
#endif                            
                //common vertices
                a->v2 = b->v3 = d->v1 = mid;
                d->v3 = v3;
                //Assign neighbors
                b->n2 = n2;
                d->n2 = n2;
            }
            // Side 3 of parent//
            if ( n3->HasChildren() )
            {
                //common vertices
                a->v3 = d->v2 = c->v3 = n3->a->v1;
                //Assign neighbors
                c->n3 = n3->b;
                n3->b->n1 = c;
                d->n3 = n3->c;
                n3->c->n1 = d;
            }
            else
            { //No neighbor or no children so compute midpoint
                Vertex* mid = newVertex();
				//RefCntPtr<Vertex> mid( newVertex() );
                Vertex from, to;
                //Compute new 3d position
                from.x = (v2->x + v3->x) / 2;
                from.z = (v2->z + v3->z) / 2;
                from.y = (v2->y + v3->y) / 2;

                to.x = from.x;
                to.z = from.z;
                to.y = volumeptr->GetHeight(to.x, to.z);
				//to.y = volumeptr->AvgHeight(to.x, to.z);
#ifdef GEOMORPH
                mid->x = from.x;
                mid->y = from.y;
                mid->z = from.z;
                
                DPatch::NewGeoMorph( VertexMorph(to, from, mid) );
#else
                mid->x = to.x;
                mid->y = to.y;
                mid->z = to.z;
#endif
                           
                a->v3 = d->v2 = c->v3 = mid;
                //Assign neighbors
                c->n3 = n3;
                d->n3 = n3;
            }
            
            //Shared with parent
            b->v1 = v1;
            c->v2 = v2;
            d->v3 = v3;
            
            //Set children's neighbors
            a->n1 = b;
            a->n2 = c;
            a->n3 = d;
            
            b->n3 = a;
            c->n2 = a;
            d->n1 = a;
        }
};

/*****************************************************/
/* Function : Merge                                  */
/* Purpose  : Prunes a QuadTriTree's children while  */
/*            preserving neighbor pointers           */
/* Usage    : Do NOT call on a QuadTriTree in which  */
/*            one of its children possess children   */
/*****************************************************/
void QuadTriTree::Merge()
{
    if ( HasChildren() )
    {
		// Reset neighbor pointers to merged diamond if necessary
		// Saving any vertices shared with neighbors
		if ( n1 )
		if ( n1->HasChildren() )
		{
			a->v1 = NULL;
			n1->c->n3 = n1->d->n3 = this;
		}

		if ( n2 )
		if ( n2->HasChildren() )
		{
			a->v2 = NULL;
			n2->b->n2 = n2->d->n2 = this;
		}

		if ( n3 )
		if ( n3->HasChildren() )
		{
			a->v3 = NULL;
			n3->b->n1 = n3->c->n1 = this;
		}

		// Save vertices shared with parent from getting axed

		b->v1 = b->v2 = b->v3 = NULL;
		c->v1 = c->v2 = c->v3 = NULL;
		d->v1 = d->v2 = d->v3 = NULL;

		// Destroy the children

		delete a;
		delete b;
		delete c;
		delete d;
	        
		a = b = c = d = NULL;
	}
};

bool QuadTriTree::HasNeighbors(){
    if ( this )
        return ( n1 || n2 || n3 );
    else
        return false;
};

bool QuadTriTree::HasChildren(){
    if ( this )
        return a; //Children always appear in 4's so we can just check one
    else
        return false;
};

bool QuadTriTree::IsMergeAble(){
    if ( this )
    {
        if ( !HasChildren() || a->HasChildren() || b->HasChildren() || c->HasChildren() || d->HasChildren() )
            return false;
        if ( n1 )
            if ( n1->HasChildren() && n1->c->HasChildren() || n1->d->HasChildren() )
                return false;
        if ( n2 )
            if ( n2->HasChildren() && n2->b->HasChildren() || n2->d->HasChildren() )
                return false;
        if ( n3 )
            if ( n3->HasChildren() && n3->b->HasChildren() || n3->c->HasChildren() )
                return false;
            
        //Triangle is mergeable
        return true;
    }
    else
        return false;
};

// TODO: Dont think this is the correct method of finding centerpoint
// Want to use midpoints of two sides and find the intersection from the
// vertex opposite the midpoint to the midpoint of each side.  Hopefully
// where they cross is the value I need.  For equilateral triangles it should be 
// the true centroid.
Point3d QuadTriTree::CenterPoint(){
//    Point3d side1, side2;
        Point3d center;
        center.x = (v1->x + v2->x + v3->x) / 3;
        center.y = (v1->y + v2->y + v3->y) / 3;
        center.z = (v1->z + v2->z + v3->z) / 3;    

//        center.x = (v1->x + v3->x) / 2;
//        center.y = (v1->y + v3->y) / 2;
//        center.z = (v1->z + v3->z) / 2;    
    
        return center;
};

bool QuadTriTree::IsVisible(const Camera* camera)
{
//    int dotprod;
    if ( this ) 
    {
        //FIXME
    
        return 1; 
    }
    else
        return 0;
};      

void QuadTriTree::Destroy()
{
    if ( HasChildren() )
    {
		// Reset neighbor pointers to merged diamond if necessary
		// Saving any vertices shared with neighbors
		
		if ( n1 )
		{
			n1->n3 = NULL;
			if ( n1->HasChildren() )
			{
				
				a->v1 = NULL;
			//	n1->c->n3 = n1->d->n3 = NULL;
			}
		}
		if ( n2 )
		{
			n2->n2 = NULL;
			if ( n2->HasChildren() )
			{
				
				a->v2 = NULL;
				n2->b->n2 = n2->d->n2 = NULL;
			}
		}

		if ( n3 )
		{
			n3->n1 = NULL;
			if ( n3->HasChildren() )
			{
				
				a->v3 = NULL;
			//	n3->b->n1 = n3->c->n1 = NULL;
			}
		}
		// Save vertices shared with parent from getting axed

		
			b->v1 = b->v2 = b->v3 = NULL;
			c->v1 = c->v2 = c->v3 = NULL;
			d->v1 = d->v2 = d->v3 = NULL;

		// Destroy the children

		delete a;
		delete b;
		delete c;
		delete d;
	        
		a = b = c = d = NULL;
	}
};

/*****************************************************/
/* Function : Collapse								 */
/* Purpose  : To collapse a QuadTriTree to its roots */
/* Usage    : Do NOT call on a local QuadTriTree     */
/* Warning  : Object it is called upon is deleted    */
/*****************************************************/
void QuadTriTree::Collapse()
{
	if ( this )
	{
            if ( b->HasChildren() )
                b->Collapse();

            if ( c->HasChildren() )
                c->Collapse();

            if ( d->HasChildren() )
                d->Collapse();

            if ( a->HasChildren() )
                a->Collapse();

            Destroy();
        }
};

