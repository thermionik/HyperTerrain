
#include "dpatch.h"
#include "volume.h"
#include "utility.h"
#include "quadtritree.h"

// #ifdef WIN32
#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>
//#include "GL/glext.h"
/*
#else
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#endif
*/
#include <iostream>
#include <list>
using namespace std;

extern float g_Lighting;
#ifdef GEOMORPH 
list<VertexMorph> DPatch::Morph;








void DPatch::NewGeoMorph(VertexMorph& vmorph )
{
    Morph.push_front(vmorph);
};
#endif

DPatch::DPatch():volume(NULL),variance1(NULL),variance2(NULL),
    varsize(0),splitpriority(0.0),ResetDiamond(true),SplitOnly(true) {    QueryPerformanceCounter(&TimeStart);
};

DPatch::~DPatch()
{
	ResetQueues();
	ResetMesh();

	
    delete [] variance1;
    delete [] variance2;
    
	delete base1.v1;
	delete base1.v2;
	delete base1.v3;
	delete base2.v3;

	base1.v1 = base1.v2 = base1.v3 = NULL;
    base2.v1 = base2.v2 = base2.v3 = NULL;
/*
    base1.Collapse();
    base2.Collapse();
    */
    //base1.v1 = base1.v2 = base1.v3 = NULL;
    //base2.v1 = base2.v2 = base2.v3 = NULL;

};

void DPatch::Initialize(Volume* vol,int xoffset,int yoffset)
{

    volume = vol;
    cout<<"\nDPatch initializing with : "<< volume->Name();
    InitVariance();
    InitMesh();    
    ResetDiamond = true;

};

int DPatch::NumTri()
{
    return base1.triCount;
};

int DPatch::NumVertices()
{
    return base1.vertCount;
};

void DPatch::DrawNormals(int yesno)
{
    if ( yesno )
        drawNormals = true;
    else 
        drawNormals = false;
};

void DPatch::InitMesh()
{
	
	
    ResetDiamond = true;
    base1.v1 = newVertex();
    base1.v2 = newVertex();
    base1.v3 = newVertex();    
    base2.v3 = newVertex();
    

    base1.v1->x = 0;
    base1.v1->z = 0;
    base1.v1->y = volume->GetHeight(base1.v1->x, base1.v1->z);
    
    base1.v2->x = 0;
    base1.v2->z = volume->GetDim() - 1;
    base1.v2->y = volume->GetHeight(base1.v2->x, base1.v2->z);


    base1.v3->x = volume->GetDim() - 1;
    base1.v3->z = 0;    
    base1.v3->y = volume->GetHeight(base1.v3->x, base1.v3->z);

    
    
    base2.v1 = base1.v2;
    base2.v2 = base1.v3;
    
    base2.v3->x = volume->GetDim() - 1;
    base2.v3->z = volume->GetDim() - 1;
    base2.v3->y = volume->GetHeight(base2.v3->x, base2.v3->z);


    base1.n3 = &base2;
    base2.n1 = &base1;
    
    base1.variance = variance1;
    base1.varindex = 0;
    base2.variance = variance2;
    base2.varindex = 0;
};

void DPatch::InitVariance()
{
    int mapsize = 0;
    mapsize = volume->GetDim();
    cout<<mapsize;
    varsize = (mapsize * mapsize) / 4 - 1;
    cout<<" "<< varsize;
    variance1 = new unsigned char[varsize];
    variance2 = new unsigned char[varsize];
    
    ComputeVariance(0, 0, 0, mapsize-1, mapsize-1, 0, 0, variance1);
    ComputeVariance(0, mapsize-1, mapsize-1, 0, mapsize-1, mapsize-1, 0, variance2);
};

void DPatch::PriorityEnqueue(QuadTriTree* qtree, int queue)
{
        if ( queue == SPLIT )
            if ( qtree->priority > splitpriority )
                SplitAbove.push(qtree);
            else
                SplitBelow.push(qtree);
        
        if ( queue == MERGE )
            if ( qtree->priority > splitpriority )
                MergeAbove.push(qtree);
            else
                MergeBelow.push(qtree);
};

void DPatch::ResetMesh()
{
    base1.Collapse();
    base2.Collapse();
    
    base1.a = base1.b = base1.c = base1.d = NULL;
    base2.a = base2.b = base2.c = base2.d = NULL;
    
    ResetDiamond = true;
};

void DPatch::ResetQueues()
{
    SplitAbove.DeleteAll();
    MergeAbove.DeleteAll();
    SplitBelow.DeleteAll();
    MergeBelow.DeleteAll();
};

        
void DPatch::SetPriority(float prior)
{
    splitpriority = prior;
};

void DPatch::ToggleSplitOnly()
{
    if ( SplitOnly )
    {
        SplitOnly = false;
        ResetDiamond = false;
    }
    else
    {
        SplitOnly = true;
        ResetDiamond = true;
    }

};

void DPatch::SetTextureIDs(int TexID_base, int TexID_detail, int water){
    TextureID_base = TexID_base;
	TextureID_detail = TexID_detail;
	TextureID_water = water;
};
        
float DPatch::GetPriority()
{
    return splitpriority;
}
#ifdef GEOMORPH 
void DPatch::Update()
{
    int loopnum = 0;
    int arraysize = Morph.size();
    if ( arraysize )
    {
        list<VertexMorph>::iterator mlist=Morph.begin();
        list<VertexMorph>::iterator temp;
	while ( mlist != Morph.end() )
        {
            if ( mlist->Finished() )
            {
                temp = mlist;
                mlist++;
                Morph.erase(temp);
            }
            else
            {
                mlist->Update();
                mlist++;
            }
//            cout<<endl<<arraysize;
            
            arraysize--;
        }
    }
}
#endif

void DPatch::Tesselate(const Camera* camera)
{
    if (ResetDiamond){
        
        ResetMesh();
        ResetQueues();
    
        //Cull and Prioritize base tri's
        
        base1.Prioritize( camera->GetLocation() );
        base2.Prioritize( camera->GetLocation() );
        
        //Enqueue in SplitAbove or Below as appropriate
        PriorityEnqueue(&base1, SPLIT);
        PriorityEnqueue(&base2, SPLIT);

        ResetDiamond = false;
    }
    else{
        QuadStack::Iterator iter;
        QuadTriTree* qtree;

        iter = SplitBelow.begin();
        while (iter != NULL )
        {
            qtree = *iter;
            qtree->Prioritize( camera->GetLocation() );
            if ( qtree->priority > splitpriority )
            {
                iter++;	//Move iter so we dont invalidate it
                qtree->queue->UnlinkNode(qtree->queueloc);  //Remove it from SplitBelow
                SplitAbove.push(qtree);
            }
            else
                iter++;
        }
        iter = MergeAbove.begin();
        while (iter != NULL )
        {
            qtree = *iter;
            qtree->Prioritize( camera->GetLocation() );
            if ( qtree->priority <= splitpriority )
            {
                iter++;	//Move iter so we dont invalidate it
                qtree->queue->UnlinkNode(qtree->queueloc);  //Remove it from SplitBelow
                MergeBelow.push(qtree);
            }
            else
                iter++;
        } 
    }
    
    while ( !MergeBelow.IsEmpty() )
        Merge( MergeBelow.pop(), camera );
        
    while ( !SplitAbove.IsEmpty() )
        Split( SplitAbove.pop(), camera );
    
};

void DPatch::Split(QuadTriTree* diamond, const Camera* camera)
{
    if ( InSplitQ(diamond) )
        diamond->queue->UnlinkNode(diamond->queueloc);
    if ( InMergeQ(diamond->parent) )
        diamond->parent->queue->UnlinkNode( diamond->parent->queueloc );

    if ( diamond->n1 )
    {
        //Split neighbor 1 if necessary
    	if ( diamond->n1->n3 != diamond )
            Split( diamond->n1, camera );
        //Dequeue neighbor1's parent if it is in a MergeQueue
        if ( diamond->n1->parent && InMergeQ( diamond->n1->parent ) )
            diamond->n1->parent->queue->UnlinkNode( diamond->n1->parent->queueloc );
    }
            
    if ( diamond->n2 )
    {
        if ( diamond->n2->n2 != diamond )
            Split( diamond->n2, camera );
        //Dequeue neighbor 2's parent if it is in a MergeQueue
        if ( diamond->n2->parent && InMergeQ( diamond->n2->parent) )
            diamond->n2->parent->queue->UnlinkNode( diamond->n2->parent->queueloc );
    }
    
    if (diamond->n3 )
    {
        if ( diamond->n3->n1 != diamond )
            Split( diamond->n3, camera );
        if ( diamond->n3->parent && InMergeQ( diamond->n3->parent) )
            diamond->n3->parent->queue->UnlinkNode( diamond->n3->parent->queueloc );
    }
    
    diamond->Split(volume);
    //Culling of children goes here
    
    if ( diamond->a->IsVisible(camera) )
    {
        diamond->a->Prioritize( camera->GetLocation() );
        PriorityEnqueue( diamond->a, SPLIT);
    }
    
    if ( diamond->b->IsVisible(camera) )
    {
        diamond->b->Prioritize( camera->GetLocation() );
        PriorityEnqueue( diamond->b, SPLIT);
    }
    if ( diamond->c->IsVisible(camera) )
    {
        diamond->c->Prioritize( camera->GetLocation() );
        PriorityEnqueue( diamond->c, SPLIT);
    }
    if ( diamond->d->IsVisible(camera) )
    {
        diamond->d->Prioritize(camera->GetLocation() ); 
        PriorityEnqueue( diamond->d, SPLIT);
    }
    
    if ( diamond->varindex >= varsize || diamond->IsMergeAble() )
        MergeAbove.push( diamond );

};

void DPatch::Merge(QuadTriTree* diamond, const Camera* camera)
{
    if ( InSplitQ(diamond->a) )
        diamond->a->queue->UnlinkNode(diamond->a->queueloc);
    if ( InSplitQ(diamond->b) )
        diamond->b->queue->UnlinkNode(diamond->b->queueloc);
    if ( InSplitQ(diamond->c) )
        diamond->c->queue->UnlinkNode(diamond->c->queueloc);
    if ( InSplitQ(diamond->d) )
        diamond->d->queue->UnlinkNode(diamond->d->queueloc);

    if ( InMergeQ(diamond) )
        diamond->queue->UnlinkNode(diamond->queueloc);
    
    diamond->Merge();
    
    if ( diamond->n1 )
        if ( diamond->n1->parent && diamond->n1->parent != diamond->parent)
        if ( diamond->n1->parent->IsMergeAble() )
        {	
            diamond->n1->parent->Prioritize(camera->GetLocation());
            PriorityEnqueue(diamond->n1->parent, MERGE);
        }
    if ( diamond->n2 )
        if ( diamond->n2->parent && diamond->n2->parent != diamond->parent)
        if (diamond->n2->parent->IsMergeAble() ){
            diamond->n2->parent->Prioritize(camera->GetLocation());
            PriorityEnqueue(diamond->n2->parent, MERGE);
        }
    if ( diamond->n3 )
        if ( diamond->n3->parent && diamond->n3->parent != diamond->parent)
        if (diamond->n3->parent->IsMergeAble() )
        {
            diamond->n3->parent->Prioritize(camera->GetLocation());
            PriorityEnqueue(diamond->n3->parent, MERGE);
        }
    if ( diamond->parent )
        if ( diamond->parent->IsMergeAble() )
        {
            diamond->parent->Prioritize(camera->GetLocation());
            PriorityEnqueue(diamond->parent, MERGE);
        }
        
    SplitBelow.push(diamond);
};

int DPatch::ComputeVariance(int x1, int y1, int x2, int y2, int x3, int y3, int index, unsigned char* varcurrent)
{
    int xa, ya, za, vr;
    int ea, eb, ec, ed, et;
    int mx1, mx2, mx3, my1, my2, my3;

    if (index >= varsize)
        return 0;
    else{
        xa = (x1 + x2) >> 1;
        ya = (y1 + y2) >> 1;
        za = volume->GetHeight(x1, y1) + volume->GetHeight(x2, y2);
        vr = abs((volume->GetHeight(xa, ya) << 1) - za);  // vr is first variance value
        
        xa = (x1 + x3) >> 1;
        ya = (y1 + y3) >> 1;
        za = volume->GetHeight(x1, y1) + volume->GetHeight(x3, y3);
        vr = MAX(vr, abs((volume->GetHeight(xa, ya) << 1) - za));  // vr is current max variance value
    
        xa = (x2 + x3) >> 1;
        ya = (y2 + y3) >> 1;
        za = volume->GetHeight(x2, y2) + volume->GetHeight(x3, y3);
        vr = MAX(vr, abs((volume->GetHeight(xa, ya) << 1) - za));  // vr is current max variance value
    
        mx1 = (x1 + x2) >> 1;
        my1 = (y1 + y2) >> 1;
        mx2 = (x1 + x3) >> 1;
        my2 = (y1 + y3) >> 1;
        mx3 = (x2 + x3) >> 1;
        my3 = (y2 + y3) >> 1;
        ea = ComputeVariance(mx1, my1, mx2, my2, mx3, my3, (index << 2) + 1, varcurrent);
        eb = ComputeVariance(x1, y2, mx1, my1, mx2, my2, (index << 2) + 2, varcurrent);
        ec = ComputeVariance(mx1, my1, x2, y2, mx3, my3, (index << 2) + 3, varcurrent);
        ed = ComputeVariance(mx2, my2, mx3, my3, x3, y3, (index << 2) + 4, varcurrent);
        
        et = MAX(ea, eb);
        et = MAX(et, ec);
        et = MAX(et, ed);
        et += vr;
        if (et > 511)
            et = 511;
        
        varcurrent[index] = et >> 1;
        return et;
    }
};

bool DPatch::InMergeQ(QuadTriTree* diamond)
{	
    if ( diamond )
        if ( diamond->queue == &MergeAbove || diamond->queue == &MergeBelow )
            return true;
        else
            return false;
    else
        return false;
};

bool DPatch::InSplitQ(QuadTriTree* diamond)
{
    if ( diamond )
        if ( diamond->queue == &SplitAbove || diamond->queue == &SplitBelow )
            return true;
        else
            return false;
    else
        return false;
};

void DPatch::Reset()
{
    ResetMesh();
    ResetQueues();
};

Vertex* DPatch::newVertex(){
    Vertex* newpos;
    newpos = new Vertex;
    return newpos;
};

int DPatch::Render(const Camera* camera, WTimer *timer) 
{
    int triCount = 0;
    int cont = 0;
    float height;
    float vector[3];
    Vector3 normal;
    Point3d endpoint,centerpoint;
 	static GLfloat s_vector[4] = { 1.0/(GLfloat)1024, 0, 0, 0 };
	static GLfloat t_vector[4] = { 0, 0, 1.0/(GLfloat)1024, 0 };
   
    glPushMatrix();
    glTranslatef(0, 0, 0);

    glDisable(GL_LIGHTING);
//	glEnable(GL_MODULATE);

 //   glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	// Activate the first texture ID and bind the tree background to it
	glActiveTextureARB(GL_TEXTURE0_ARB);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, TextureID_base);

	glTexGeni( GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR );
	glTexGenfv( GL_S, GL_OBJECT_PLANE, s_vector );

	glTexGeni( GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR );
	glTexGenfv( GL_T, GL_OBJECT_PLANE, t_vector ); 



	// If we want detail texturing on, let's render the second texture
	if(true)
	{
		// Activate the second texture ID and bind the fog texture to it
		glActiveTextureARB(GL_TEXTURE1_ARB);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, TextureID_detail);
		
		// Here we turn on the COMBINE properties and increase our RGB
		// gamma for the detail texture.  2 seems to work just right.
		//glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_ARB);
		//glTexEnvi(GL_TEXTURE_ENV, GL_RGB_SCALE_ARB, 2);

		glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE, GL_COMBINE_EXT); 
		glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB_EXT, GL_ADD_SIGNED_EXT); 

		// Bind the detail texture
	glTexGeni( GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR );
	glTexGenfv( GL_S, GL_OBJECT_PLANE, s_vector );

	glTexGeni( GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR );
	glTexGenfv( GL_T, GL_OBJECT_PLANE, t_vector ); 
	
		// Now we want to enter the texture matrix.  This will allow us
		// to change the tiling of the detail texture.
		glMatrixMode(GL_TEXTURE);

			// Reset the current matrix and apply our chosen scale value
			glLoadIdentity();
			glScalef((float)32, (float)32, 1);

		// Leave the texture matrix and set us back in the model view matrix
		glMatrixMode(GL_MODELVIEW);
	}







//    glColor3f(g_Lighting, g_Lighting, g_Lighting);
    
    QuadTriTree* diamond;
    QuadStack::Iterator iter;

    for (iter = SplitBelow.begin(); iter != NULL; iter++)
    {
        diamond = *iter;
        cont = 0;
        if ( !diamond->HasChildren() )
        {
            if ( diamond->IsVisible(camera) )
            {    
    //            triCount++;
                if ( diamond->n1 )
                if ( diamond->n1->HasChildren() )
                    cont |= 1;
                if ( diamond->n2 )
                if ( diamond->n2->HasChildren() )
                    cont |= 2;
                if ( diamond->n3 )                
                if ( diamond->n3->HasChildren() )
                    cont |= 4;
                
                /*normal = diamond->Normal();
 
                vector[0] = normal.x;
                vector[1] = normal.y; 
                vector[2] = normal.z;
                
                ReduceToUnit(vector);
    
                glNormal3fv(vector);

                    // Remove Here
//                    cont = 0;
                if (drawNormals){
                            centerpoint = diamond->CenterPoint();
                            
                            endpoint.x = centerpoint.x + vector[0];// + .8;
                            endpoint.y = centerpoint.y + vector[1];// + .8;
                            endpoint.z = centerpoint.z + vector[2];// + .8;
                            if (diamond->Orientation() < 0 )
                                glColor3f(0.0f, 0.0f, 1.0f);
                            else
                                glColor3f(1.0f, 0.5f, 0.5f);
                            glBegin(GL_LINES);                            
                                glVertex3f(centerpoint.x, centerpoint.y, centerpoint.z);
                                glVertex3f(endpoint.x, endpoint.y, endpoint.z);
                            glEnd();
                }
                   */ // End remove
                    
                                
                if (diamond->Orientation() < 0)
                {
                 //   glColor3f(1.0f,0.0f,0.0f);
                     
                    switch (cont)
                    {  // Facing Down
                    
                        case 0: { //Neighboring triangles are at the same level j
                            glBegin(GL_TRIANGLES);
                              SetTextureCoord(diamond->v1->x, diamond->v1->z);  
								glVertex3f(diamond->v1->x, diamond->v1->y, diamond->v1->z);
                              SetTextureCoord(diamond->v2->x, diamond->v2->z);  
								glVertex3f(diamond->v2->x, diamond->v2->y, diamond->v2->z);
                              SetTextureCoord(diamond->v3->x, diamond->v3->z);  
								glVertex3f(diamond->v3->x, diamond->v3->y, diamond->v3->z);
                            glEnd(); 
                            triCount++;
                            break;
                        }
                        case 1: { //Split triangle in half j
                            glBegin(GL_TRIANGLES);
                                SetTextureCoord(diamond->v1->x, diamond->v1->z);				
									glVertex3f(diamond->v1->x, diamond->v1->y, diamond->v1->z);
                                SetTextureCoord(diamond->n1->a->v3->x, diamond->n1->a->v3->z);	
									glVertex3f(diamond->n1->a->v3->x, diamond->n1->a->v3->y, diamond->n1->a->v3->z);
                                SetTextureCoord(diamond->v3->x, diamond->v3->z);				
									glVertex3f(diamond->v3->x, diamond->v3->y, diamond->v3->z);

                                SetTextureCoord(diamond->n1->a->v3->x, diamond->n1->a->v3->z);	
									glVertex3f(diamond->n1->a->v3->x, diamond->n1->a->v3->y, diamond->n1->a->v3->z);
                                SetTextureCoord(diamond->v2->x, diamond->v2->z);				
									glVertex3f(diamond->v2->x, diamond->v2->y, diamond->v2->z);
                                SetTextureCoord(diamond->v3->x, diamond->v3->z);				
									glVertex3f(diamond->v3->x, diamond->v3->y, diamond->v3->z);
                            glEnd();
                            triCount += 2;
                            break;
                        }           
                        case 2: { // Two triangles
                            glBegin(GL_TRIANGLES);
                             SetTextureCoord(diamond->v1->x, diamond->v1->z);				   glVertex3f(diamond->v1->x, diamond->v1->y, diamond->v1->z);
                             SetTextureCoord(diamond->v2->x, diamond->v2->z);				   glVertex3f(diamond->v2->x, diamond->v2->y, diamond->v2->z);
                             SetTextureCoord(diamond->n2->a->v2->x, diamond->n2->a->v2->z);	   glVertex3f(diamond->n2->a->v2->x, diamond->n2->a->v2->y, diamond->n2->a->v2->z);
                                
                             SetTextureCoord(diamond->n2->a->v2->x, diamond->n2->a->v2->z);	   glVertex3f(diamond->n2->a->v2->x, diamond->n2->a->v2->y, diamond->n2->a->v2->z);
                             SetTextureCoord(diamond->v2->x, diamond->v2->z);				   glVertex3f(diamond->v2->x, diamond->v2->y, diamond->v2->z);
                             SetTextureCoord(diamond->v3->x, diamond->v3->z);				   glVertex3f(diamond->v3->x, diamond->v3->y, diamond->v3->z);
                            glEnd();
                            triCount += 2;
                            break;
                        }
                        case 3: { // Three Triangles                            
                            glBegin(GL_TRIANGLES);
                                // Begin first triangle
                              SetTextureCoord(diamond->v1->x, diamond->v1->z);				   glVertex3f(diamond->v1->x, diamond->v1->y, diamond->v1->z);                           
                              SetTextureCoord(diamond->n1->a->v3->x, diamond->n1->a->v3->z);	   glVertex3f(diamond->n1->a->v3->x, diamond->n1->a->v3->y, diamond->n1->a->v3->z);
                              SetTextureCoord(diamond->n2->a->v2->x, diamond->n2->a->v2->z);	   glVertex3f(diamond->n2->a->v2->x, diamond->n2->a->v2->y, diamond->n2->a->v2->z);
                              // Finish first triangle   // Finish first triangle
                                 
                              SetTextureCoord(diamond->n2->a->v2->x, diamond->n2->a->v2->z);	   glVertex3f(diamond->n2->a->v2->x, diamond->n2->a->v2->y, diamond->n2->a->v2->z);                                                                                                
                              SetTextureCoord(diamond->n1->a->v3->x, diamond->n1->a->v3->z);	   glVertex3f(diamond->n1->a->v3->x, diamond->n1->a->v3->y, diamond->n1->a->v3->z);
                              SetTextureCoord(diamond->v3->x, diamond->v3->z);				   glVertex3f(diamond->v3->x, diamond->v3->y, diamond->v3->z);                      
                              // End Second triangle   // End Second triangle
                              SetTextureCoord(diamond->v3->x, diamond->v3->z);				   glVertex3f(diamond->v3->x, diamond->v3->y, diamond->v3->z);
                              SetTextureCoord(diamond->n1->a->v3->x, diamond->n1->a->v3->z);	   glVertex3f(diamond->n1->a->v3->x, diamond->n1->a->v3->y, diamond->n1->a->v3->z);                                
                              SetTextureCoord(diamond->v2->x, diamond->v2->z);				   glVertex3f(diamond->v2->x, diamond->v2->y, diamond->v2->z);
                            glEnd();
                            triCount += 3;
                            break;
                        }
                        case 4:
                            {
                            glBegin(GL_TRIANGLES);
                               SetTextureCoord(diamond->v1->x, diamond->v1->z);				 glVertex3f(diamond->v1->x, diamond->v1->y, diamond->v1->z);                            
                               SetTextureCoord(diamond->n3->a->v1->x, diamond->n3->a->v1->z);	 glVertex3f(diamond->n3->a->v1->x, diamond->n3->a->v1->y, diamond->n3->a->v1->z);
                               SetTextureCoord(diamond->v3->x, diamond->v3->z);				 glVertex3f(diamond->v3->x, diamond->v3->y, diamond->v3->z);                                

                               SetTextureCoord(diamond->v1->x, diamond->v1->z);				 glVertex3f(diamond->v1->x, diamond->v1->y, diamond->v1->z);
                               SetTextureCoord(diamond->v2->x, diamond->v2->z);				 glVertex3f(diamond->v2->x, diamond->v2->y, diamond->v2->z);
                               SetTextureCoord(diamond->n3->a->v1->x, diamond->n3->a->v1->z);	 glVertex3f(diamond->n3->a->v1->x, diamond->n3->a->v1->y, diamond->n3->a->v1->z);
                            glEnd();
                            triCount += 2;
                            break;
                        }
                        case 5:
                            {
                            glBegin(GL_TRIANGLES);                            
                                // Begin first triangle
                          SetTextureCoord(diamond->v2->x, diamond->v2->z);				        glVertex3f(diamond->v2->x, diamond->v2->y, diamond->v2->z);
                          SetTextureCoord(diamond->n3->a->v1->x, diamond->n3->a->v1->z);	        glVertex3f(diamond->n3->a->v1->x, diamond->n3->a->v1->y, diamond->n3->a->v1->z);
                          SetTextureCoord(diamond->n1->a->v3->x, diamond->n1->a->v3->z);	        glVertex3f(diamond->n1->a->v3->x, diamond->n1->a->v3->y, diamond->n1->a->v3->z);
                          // End first triangle        // End first triangle
//                            glColor3f(1.0f, 0.5f, 0.5f);  //      glColor3f(1.0f, 0.5f, 0.5f);
                          SetTextureCoord(diamond->n1->a->v3->x, diamond->n1->a->v3->z);	        glVertex3f(diamond->n1->a->v3->x, diamond->n1->a->v3->y, diamond->n1->a->v3->z);                                                                
                          SetTextureCoord(diamond->n3->a->v1->x, diamond->n3->a->v1->z);	        glVertex3f(diamond->n3->a->v1->x, diamond->n3->a->v1->y, diamond->n3->a->v1->z);
                          SetTextureCoord(diamond->v1->x, diamond->v1->z);				        glVertex3f(diamond->v1->x, diamond->v1->y, diamond->v1->z);
                          // End Second Triangle        // End Second Triangle
//                            glColor3f(1.0f, 0.0f, 0.0f);  //      glColor3f(1.0f, 0.0f, 0.0f);
                          SetTextureCoord(diamond->v1->x, diamond->v1->z);				        glVertex3f(diamond->v1->x, diamond->v1->y, diamond->v1->z);            
                          SetTextureCoord(diamond->n3->a->v1->x, diamond->n3->a->v1->z);	        glVertex3f(diamond->n3->a->v1->x, diamond->n3->a->v1->y, diamond->n3->a->v1->z);
                          SetTextureCoord(diamond->v3->x, diamond->v3->z);				        glVertex3f(diamond->v3->x, diamond->v3->y, diamond->v3->z);
                            glEnd();
                            triCount += 3;
                            break;                            
                        }
                        case 6: {
                            glBegin(GL_TRIANGLES);                            
                                // Begin first triangle
                              SetTextureCoord(diamond->v1->x, diamond->v1->z);				     glVertex3f(diamond->v1->x, diamond->v1->y, diamond->v1->z);
                              SetTextureCoord(diamond->v2->x, diamond->v2->z);				     glVertex3f(diamond->v2->x, diamond->v2->y, diamond->v2->z);
                              SetTextureCoord(diamond->n3->a->v1->x, diamond->n3->a->v1->z);	    glVertex3f(diamond->n3->a->v1->x, diamond->n3->a->v1->y, diamond->n3->a->v1->z);                        
  //                            glColor3f(1.0f, 0.5f, 0.5f);                                      glColor3f(1.0f, 0.5f, 0.5f);                                
                              SetTextureCoord(diamond->v1->x, diamond->v1->z);				    glVertex3f(diamond->v1->x, diamond->v1->y, diamond->v1->z);                                                                                                                                                                
                              SetTextureCoord(diamond->n3->a->v1->x, diamond->n3->a->v1->z);	    glVertex3f(diamond->n3->a->v1->x, diamond->n3->a->v1->y, diamond->n3->a->v1->z);
                              SetTextureCoord(diamond->n2->a->v2->x, diamond->n2->a->v2->z);	    glVertex3f(diamond->n2->a->v2->x, diamond->n2->a->v2->y, diamond->n2->a->v2->z);
//                              glColor3f(1.0f, 0.0f, 0.0f);            glColor3f(1.0f, 0.0f, 0.0f);      
                              SetTextureCoord(diamond->n2->a->v2->x, diamond->n2->a->v2->z);	    glVertex3f(diamond->n2->a->v2->x, diamond->n2->a->v2->y, diamond->n2->a->v2->z);                                
                              SetTextureCoord(diamond->n3->a->v1->x, diamond->n3->a->v1->z);	    glVertex3f(diamond->n3->a->v1->x, diamond->n3->a->v1->y, diamond->n3->a->v1->z);
                              SetTextureCoord(diamond->v3->x, diamond->v3->z);				    glVertex3f(diamond->v3->x, diamond->v3->y, diamond->v3->z);
                            glEnd();
                            triCount += 3;
                            break;
                        }
                        case 7: {
                            glBegin(GL_TRIANGLES);
                               SetTextureCoord(diamond->v1->x, diamond->v1->z);				  glVertex3f(diamond->v1->x, diamond->v1->y, diamond->v1->z);
                               SetTextureCoord(diamond->n1->a->v3->x, diamond->n1->a->v3->z);	  glVertex3f(diamond->n1->a->v3->x, diamond->n1->a->v3->y, diamond->n1->a->v3->z);
                               SetTextureCoord(diamond->n2->a->v2->x, diamond->n2->a->v2->z);	  glVertex3f(diamond->n2->a->v2->x, diamond->n2->a->v2->y, diamond->n2->a->v2->z);
																	
                               SetTextureCoord(diamond->n1->a->v3->x, diamond->n1->a->v3->z);	  glVertex3f(diamond->n1->a->v3->x, diamond->n1->a->v3->y, diamond->n1->a->v3->z);
                               SetTextureCoord(diamond->n3->a->v1->x, diamond->n3->a->v1->z);	  glVertex3f(diamond->n3->a->v1->x, diamond->n3->a->v1->y, diamond->n3->a->v1->z);                            
                               SetTextureCoord(diamond->n2->a->v2->x, diamond->n2->a->v2->z);	  glVertex3f(diamond->n2->a->v2->x, diamond->n2->a->v2->y, diamond->n2->a->v2->z);
                              																	  
                               SetTextureCoord(diamond->n1->a->v3->x, diamond->n1->a->v3->z);	  glVertex3f(diamond->n1->a->v3->x, diamond->n1->a->v3->y, diamond->n1->a->v3->z);
                               SetTextureCoord(diamond->v2->x, diamond->v2->z);				  glVertex3f(diamond->v2->x, diamond->v2->y, diamond->v2->z);
                               SetTextureCoord(diamond->n3->a->v1->x, diamond->n3->a->v1->z);	  glVertex3f(diamond->n3->a->v1->x, diamond->n3->a->v1->y, diamond->n3->a->v1->z);                            
                              																	  
                               SetTextureCoord(diamond->n2->a->v2->x, diamond->n2->a->v2->z);	  glVertex3f(diamond->n2->a->v2->x, diamond->n2->a->v2->y, diamond->n2->a->v2->z);
                               SetTextureCoord(diamond->n3->a->v1->x, diamond->n3->a->v1->z);	  glVertex3f(diamond->n3->a->v1->x, diamond->n3->a->v1->y, diamond->n3->a->v1->z);
                               SetTextureCoord(diamond->v3->x, diamond->v3->z);				  glVertex3f(diamond->v3->x, diamond->v3->y, diamond->v3->z);
                            glEnd();
                            triCount += 4;
                        break;
                        }
                        default: 
                            break;
                    }

  /*
  
                  // TRIANGLE STRIP METHOD - Downward facing
                    glColor3f(1.0f,0.0f,0.0f);
                    switch (cont)
                    {  // Facing Down
                        case 0: { //Neighboring triangles are at the same level
                            glBegin(GL_TRIANGLES);
                                glVertex3f(diamond->v1->x, diamond->v1->z, diamond->v1->y);    
                                glVertex3f(diamond->v2->x, diamond->v2->z, diamond->v2->y);
                                glVertex3f(diamond->v3->x, diamond->v3->z, diamond->v3->y);
                            glEnd();
                            break;
                        }
                        case 1: { //Split triangle in half
                            glBegin(GL_TRIANGLE_STRIP);
                                glVertex3f(diamond->n1->a->v3->x, diamond->n1->a->v3->z, diamond->n1->a->v3->y);
                                glVertex3f(diamond->v1->x, diamond->v1->z, diamond->v1->y);  
                                glVertex3f(diamond->v3->x, diamond->v3->z, diamond->v3->y);
                                glVertex3f(diamond->v2->x, diamond->v2->z, diamond->v2->y);
                            glEnd();
                            break;
                        }           
                        case 2: { // Two triangles
                            glBegin(GL_TRIANGLE_STRIP);
                                glVertex3f(diamond->v2->x, diamond->v2->z, diamond->v2->y);
                                glVertex3f(diamond->v1->x, diamond->v1->z, diamond->v1->y);  
                                glVertex3f(diamond->n2->a->v2->x, diamond->n2->a->v2->z, diamond->n2->a->v2->y);                        
                                glVertex3f(diamond->v3->x, diamond->v3->z, diamond->v3->y);
                            glEnd();
                            break;
                        }
                        case 3: { // Three Triangles
                            
                            glBegin(GL_TRIANGLE_STRIP);
                                // Begin first triangle
                                glVertex3f(diamond->v3->x, diamond->v3->z, diamond->v3->y);                                
                                glVertex3f(diamond->n2->a->v2->x, diamond->n2->a->v2->z, diamond->n2->a->v2->y);
                                glVertex3f(diamond->v2->x, diamond->v2->z, diamond->v2->y);
                                // Finish first triangle
                                glVertex3f(diamond->n1->a->v3->x, diamond->n1->a->v3->z, diamond->n1->a->v3->y);
                                // End Second triangle                        
                                glVertex3f(diamond->v1->x, diamond->v1->z, diamond->v1->y);
                            glEnd();
                            break;
                        }
                        case 4:
                            {
                            glBegin(GL_TRIANGLE_STRIP);
                                glVertex3f(diamond->v1->x, diamond->v1->z, diamond->v1->y);                            
                                glVertex3f(diamond->v3->x, diamond->v3->z, diamond->v3->y);                                
                                glVertex3f(diamond->n3->a->v1->x, diamond->n3->a->v1->z, diamond->n3->a->v1->y);
                                glVertex3f(diamond->v2->x, diamond->v2->z, diamond->v2->y);
                            glEnd();
                            break;
                        }
                        case 5:
                            {
                            glBegin(GL_TRIANGLE_STRIP);                            
                                // Begin first triangle
                                glVertex3f(diamond->v1->x, diamond->v1->z, diamond->v1->y);
                                glVertex3f(diamond->n1->a->v3->x, diamond->n1->a->v3->z, diamond->n1->a->v3->y);
                                glVertex3f(diamond->v3->x, diamond->v3->z, diamond->v3->y);
                                // Finish first triangle
                                glVertex3f(diamond->n3->a->v1->x, diamond->n3->a->v1->z, diamond->n3->a->v1->y);
                                // End Second triangle
                                glVertex3f(diamond->v2->x, diamond->v2->z, diamond->v2->y);
                            glEnd();
                            break;                            
                        }
                        case 6: {
                            glBegin(GL_TRIANGLE_STRIP);                            
                                // Begin first triangle
                                glVertex3f(diamond->v3->x, diamond->v3->z, diamond->v3->y);
                                glVertex3f(diamond->n2->a->v2->x, diamond->n2->a->v2->z, diamond->n2->a->v2->y);
                                glVertex3f(diamond->n3->a->v1->x, diamond->n3->a->v1->z, diamond->n3->a->v1->y);                        
                                //End first triangle
                                glVertex3f(diamond->v2->x, diamond->v2->z, diamond->v2->y);
                                //End second triangle
                                glVertex3f(diamond->v1->x, diamond->v1->z, diamond->v1->y);
                            glEnd();
                            break;
                        }
                        case 7: {
                            glBegin(GL_TRIANGLE_STRIP);
                                glVertex3f(diamond->v1->x, diamond->v1->z, diamond->v1->y);
                                glVertex3f(diamond->n1->a->v3->x, diamond->n1->a->v3->z, diamond->n1->a->v3->y);
                                glVertex3f(diamond->n2->a->v2->x, diamond->n2->a->v2->z, diamond->n2->a->v2->y);
                                glVertex3f(diamond->n3->a->v1->x, diamond->n3->a->v1->z, diamond->n3->a->v1->y);                            
                                glVertex3f(diamond->v2->x, diamond->v2->z, diamond->v2->y);
                            glEnd();
                            
                            glBegin(GL_TRIANGLES);    
                                glVertex3f(diamond->v3->x, diamond->v3->z, diamond->v3->y);
                                glVertex3f(diamond->n2->a->v2->x, diamond->n2->a->v2->z, diamond->n2->a->v2->y);                            
                                glVertex3f(diamond->n3->a->v1->x, diamond->n3->a->v1->z, diamond->n3->a->v1->y);
                            glEnd();
                            
                        break;
                        }
                    } */
                }
                else
                {   // Facing upwards
//                    glColor3f(1.0f,0.0f,0.0f);                   
                   // glColor3f(1.0f, 1.0f, 0.0f);
                    // Remove here
//                    cont = 0;
                    //END remove
                    switch (cont)
                    {   //Always wind counter clockwise!
                        case 0: { //Neighboring triangles are at the same level
                            glBegin(GL_TRIANGLES);
                                SetTextureCoord(diamond->v1->x, diamond->v1->z);	 
									glVertex3f(diamond->v1->x, diamond->v1->y, diamond->v1->z);                             
                                SetTextureCoord(diamond->v3->x, diamond->v3->z);  
									glVertex3f(diamond->v3->x, diamond->v3->y, diamond->v3->z);
                                SetTextureCoord(diamond->v2->x, diamond->v2->z);   
									glVertex3f(diamond->v2->x, diamond->v2->y, diamond->v2->z);                               
                            glEnd();
                            triCount += 1;
                            break;
                        }
                        case 1: { //Split triangle in half
    
                            glBegin(GL_TRIANGLES);
                              SetTextureCoord(diamond->v1->x, diamond->v1->z);				   
								glVertex3f(diamond->v1->x, diamond->v1->y, diamond->v1->z);
                              SetTextureCoord(diamond->v3->x, diamond->v3->z);				  
								glVertex3f(diamond->v3->x, diamond->v3->y, diamond->v3->z);
                              SetTextureCoord(diamond->n1->a->v3->x, diamond->n1->a->v3->z);
								glVertex3f(diamond->n1->a->v3->x, diamond->n1->a->v3->y, diamond->n1->a->v3->z);

                              SetTextureCoord(diamond->n1->a->v3->x, diamond->n1->a->v3->z);
								glVertex3f(diamond->n1->a->v3->x, diamond->n1->a->v3->y, diamond->n1->a->v3->z);
                              SetTextureCoord(diamond->v3->x, diamond->v3->z);				
								glVertex3f(diamond->v3->x, diamond->v3->y, diamond->v3->z);
                              SetTextureCoord(diamond->v2->x, diamond->v2->z);			
								glVertex3f(diamond->v2->x, diamond->v2->y, diamond->v2->z);
                            glEnd();
                            triCount += 2;
                            break;
                        }           
                        case 2: { // Two triangles
                            glBegin(GL_TRIANGLES);
                                SetTextureCoord(diamond->v1->x, diamond->v1->z);				
									glVertex3f(diamond->v1->x, diamond->v1->y, diamond->v1->z);
                                SetTextureCoord(diamond->n2->a->v2->x, diamond->n2->a->v2->z);
									glVertex3f(diamond->n2->a->v2->x, diamond->n2->a->v2->y, diamond->n2->a->v2->z);
                                SetTextureCoord(diamond->v2->x, diamond->v2->z);
									glVertex3f(diamond->v2->x, diamond->v2->y, diamond->v2->z);

                                SetTextureCoord(diamond->v2->x, diamond->v2->z);	
									glVertex3f(diamond->v2->x, diamond->v2->y, diamond->v2->z);
                                SetTextureCoord(diamond->n2->a->v2->x, diamond->n2->a->v2->z);
									glVertex3f(diamond->n2->a->v2->x, diamond->n2->a->v2->y, diamond->n2->a->v2->z);
                                SetTextureCoord(diamond->v3->x, diamond->v3->z);
									glVertex3f(diamond->v3->x, diamond->v3->y, diamond->v3->z);
                            glEnd();
                            triCount += 2;
                            break;
                        }
                        case 3: { // Three Triangles
                            
                            glBegin(GL_TRIANGLES);
                                // Begin first triangle
                               SetTextureCoord(diamond->v1->x, diamond->v1->z);				  
								 glVertex3f(diamond->v1->x, diamond->v1->y, diamond->v1->z);
                               SetTextureCoord(diamond->n2->a->v2->x, diamond->n2->a->v2->z);
								 glVertex3f(diamond->n2->a->v2->x, diamond->n2->a->v2->y, diamond->n2->a->v2->z);
                               SetTextureCoord(diamond->n1->a->v3->x, diamond->n1->a->v3->z);
								 glVertex3f(diamond->n1->a->v3->x, diamond->n1->a->v3->y, diamond->n1->a->v3->z);
                                 
                               SetTextureCoord(diamond->n1->a->v3->x, diamond->n1->a->v3->z);
								 glVertex3f(diamond->n1->a->v3->x, diamond->n1->a->v3->y, diamond->n1->a->v3->z);
                               SetTextureCoord(diamond->n2->a->v2->x, diamond->n2->a->v2->z);
								 glVertex3f(diamond->n2->a->v2->x, diamond->n2->a->v2->y, diamond->n2->a->v2->z);
                               SetTextureCoord(diamond->v3->x, diamond->v3->z);			
								 glVertex3f(diamond->v3->x, diamond->v3->y, diamond->v3->z);
                                 
                               SetTextureCoord(diamond->n1->a->v3->x, diamond->n1->a->v3->z);
								  glVertex3f(diamond->n1->a->v3->x, diamond->n1->a->v3->y, diamond->n1->a->v3->z);
                               SetTextureCoord(diamond->v3->x, diamond->v3->z);		
								  glVertex3f(diamond->v3->x, diamond->v3->y, diamond->v3->z);
                               SetTextureCoord(diamond->v2->x, diamond->v2->z);	
								  glVertex3f(diamond->v2->x, diamond->v2->y, diamond->v2->z);
                            glEnd();

                            triCount += 3;                            
                            break;
                        }
                        case 4:
                            {
                            glBegin(GL_TRIANGLES);
                              SetTextureCoord(diamond->v1->x, diamond->v1->z);				 
								 glVertex3f(diamond->v1->x, diamond->v1->y, diamond->v1->z);
                              SetTextureCoord(diamond->v3->x, diamond->v3->z);		
								 glVertex3f(diamond->v3->x, diamond->v3->y, diamond->v3->z);
                              SetTextureCoord(diamond->n3->a->v1->x, diamond->n3->a->v1->z);
								 glVertex3f(diamond->n3->a->v1->x, diamond->n3->a->v1->y, diamond->n3->a->v1->z);

                              SetTextureCoord(diamond->v1->x, diamond->v1->z);		
								 glVertex3f(diamond->v1->x, diamond->v1->y, diamond->v1->z);
                              SetTextureCoord(diamond->n3->a->v1->x, diamond->n3->a->v1->z);
								 glVertex3f(diamond->n3->a->v1->x, diamond->n3->a->v1->y, diamond->n3->a->v1->z);
                              SetTextureCoord(diamond->v2->x, diamond->v2->z);	
								glVertex3f(diamond->v2->x, diamond->v2->y, diamond->v2->z);                                
                            glEnd();
                            triCount += 2;
                            break;
                        }
                        case 5:
                            {
                            glBegin(GL_TRIANGLES);                            
                                // Begin first triangle
                                SetTextureCoord(diamond->v1->x, diamond->v1->z);		
									glVertex3f(diamond->v1->x, diamond->v1->y, diamond->v1->z);
                                SetTextureCoord(diamond->v3->x, diamond->v3->z);	
									glVertex3f(diamond->v3->x, diamond->v3->y, diamond->v3->z);
                                SetTextureCoord(diamond->n1->a->v3->x, diamond->n1->a->v3->z); 
									glVertex3f(diamond->n1->a->v3->x, diamond->n1->a->v3->y, diamond->n1->a->v3->z);
                                  
                                SetTextureCoord(diamond->n1->a->v3->x, diamond->n1->a->v3->z);
									glVertex3f(diamond->n1->a->v3->x, diamond->n1->a->v3->y, diamond->n1->a->v3->z);
                                SetTextureCoord(diamond->v3->x, diamond->v3->z);	
									glVertex3f(diamond->v3->x, diamond->v3->y, diamond->v3->z);
                                SetTextureCoord(diamond->n3->a->v1->x, diamond->n3->a->v1->z);
									glVertex3f(diamond->n3->a->v1->x, diamond->n3->a->v1->y, diamond->n3->a->v1->z);

                                SetTextureCoord(diamond->n1->a->v3->x, diamond->n1->a->v3->z); 
									glVertex3f(diamond->n1->a->v3->x, diamond->n1->a->v3->y, diamond->n1->a->v3->z);
                                SetTextureCoord(diamond->n3->a->v1->x, diamond->n3->a->v1->z);
									glVertex3f(diamond->n3->a->v1->x, diamond->n3->a->v1->y, diamond->n3->a->v1->z);                                
                                SetTextureCoord(diamond->v2->x, diamond->v2->z);		
									glVertex3f(diamond->v2->x, diamond->v2->y, diamond->v2->z);
                            glEnd();
                            triCount += 3;                            
                            break;                            
                        }
                        case 6: {
                            glBegin(GL_TRIANGLES);
                                // Begin first triangle
                              SetTextureCoord(diamond->v1->x, diamond->v1->z);				 
								 glVertex3f(diamond->v1->x, diamond->v1->y, diamond->v1->z);
                              SetTextureCoord(diamond->n3->a->v1->x, diamond->n3->a->v1->z);
								 glVertex3f(diamond->n3->a->v1->x, diamond->n3->a->v1->y, diamond->n3->a->v1->z);                        
                              SetTextureCoord(diamond->v2->x, diamond->v2->z);			
								  glVertex3f(diamond->v2->x, diamond->v2->y, diamond->v2->z);
                                 
                              SetTextureCoord(diamond->v1->x, diamond->v1->z);		
								 glVertex3f(diamond->v1->x, diamond->v1->y, diamond->v1->z);                                                                                                
                              SetTextureCoord(diamond->n2->a->v2->x, diamond->n2->a->v2->z);
								 glVertex3f(diamond->n2->a->v2->x, diamond->n2->a->v2->y, diamond->n2->a->v2->z);
                              SetTextureCoord(diamond->n3->a->v1->x, diamond->n3->a->v1->z);
								  glVertex3f(diamond->n3->a->v1->x, diamond->n3->a->v1->y, diamond->n3->a->v1->z);                        

                              SetTextureCoord(diamond->n2->a->v2->x, diamond->n2->a->v2->z);
								 glVertex3f(diamond->n2->a->v2->x, diamond->n2->a->v2->y, diamond->n2->a->v2->z);
                              SetTextureCoord(diamond->v3->x, diamond->v3->z);		
								 glVertex3f(diamond->v3->x, diamond->v3->y, diamond->v3->z);
                              SetTextureCoord(diamond->n3->a->v1->x, diamond->n3->a->v1->z);
								glVertex3f(diamond->n3->a->v1->x, diamond->n3->a->v1->y, diamond->n3->a->v1->z);                        
                            glEnd();
                            triCount += 3;                            
                            break;
                        }
                        case 7: {
                            glBegin(GL_TRIANGLES);
                               SetTextureCoord(diamond->v1->x, diamond->v1->z);				  
								 glVertex3f(diamond->v1->x, diamond->v1->y, diamond->v1->z);
                               SetTextureCoord(diamond->n2->a->v2->x, diamond->n2->a->v2->z);
								  glVertex3f(diamond->n2->a->v2->x, diamond->n2->a->v2->y, diamond->n2->a->v2->z);
                               SetTextureCoord(diamond->n1->a->v3->x, diamond->n1->a->v3->z);
								 glVertex3f(diamond->n1->a->v3->x, diamond->n1->a->v3->y, diamond->n1->a->v3->z);
																	
                               SetTextureCoord(diamond->n1->a->v3->x, diamond->n1->a->v3->z);
								 glVertex3f(diamond->n1->a->v3->x, diamond->n1->a->v3->y, diamond->n1->a->v3->z);
                               SetTextureCoord(diamond->n2->a->v2->x, diamond->n2->a->v2->z);
								  glVertex3f(diamond->n2->a->v2->x, diamond->n2->a->v2->y, diamond->n2->a->v2->z);
                               SetTextureCoord(diamond->n3->a->v1->x, diamond->n3->a->v1->z);
								 glVertex3f(diamond->n3->a->v1->x, diamond->n3->a->v1->y, diamond->n3->a->v1->z);
																	
                               SetTextureCoord(diamond->n1->a->v3->x, diamond->n1->a->v3->z);
								 glVertex3f(diamond->n1->a->v3->x, diamond->n1->a->v3->y, diamond->n1->a->v3->z);
                               SetTextureCoord(diamond->n3->a->v1->x, diamond->n3->a->v1->z);
								 glVertex3f(diamond->n3->a->v1->x, diamond->n3->a->v1->y, diamond->n3->a->v1->z);                            
                               SetTextureCoord(diamond->v2->x, diamond->v2->z);				 
								 glVertex3f(diamond->v2->x, diamond->v2->y, diamond->v2->z);
                              																	  
                               SetTextureCoord(diamond->n2->a->v2->x, diamond->n2->a->v2->z);
								glVertex3f(diamond->n2->a->v2->x, diamond->n2->a->v2->y, diamond->n2->a->v2->z);                                
                               SetTextureCoord(diamond->v3->x, diamond->v3->z);				  
								glVertex3f(diamond->v3->x, diamond->v3->y, diamond->v3->z);
                               SetTextureCoord(diamond->n3->a->v1->x, diamond->n3->a->v1->z);
								glVertex3f(diamond->n3->a->v1->x, diamond->n3->a->v1->y, diamond->n3->a->v1->z);

                            triCount += 4;                            
                        break;
                        }
                    } //End Select  


                    /*  //Old TRIANGLE STRIP METHOD
                    glColor3f(1.0f, 1.0f, 0.0f);
                    switch (cont)
                    {   //Always wind counter clockwise!
                        case 0: { //Neighboring triangles are at the same level
                            glBegin(GL_TRIANGLES);
                                glVertex3f(diamond->v1->x, diamond->v1->z, diamond->v1->y);
                                glVertex3f(diamond->v3->x, diamond->v3->z, diamond->v3->y);
                                glVertex3f(diamond->v2->x, diamond->v2->z, diamond->v2->y);
                            glEnd();
                            break;
                        }
                        case 1: { //Split triangle in half
    
                            glBegin(GL_TRIANGLE_STRIP);
                                glVertex3f(diamond->n1->a->v3->x, diamond->n1->a->v3->z, diamond->n1->a->v3->y);
                                glVertex3f(diamond->v1->x, diamond->v1->z, diamond->v1->y);
                                glVertex3f(diamond->v3->x, diamond->v3->z, diamond->v3->y);
                                glVertex3f(diamond->v2->x, diamond->v2->z, diamond->v2->y);
                            glEnd();
                            break;
                        }           
                        case 2: { // Two triangles
                            glBegin(GL_TRIANGLE_STRIP);
                                glVertex3f(diamond->v2->x, diamond->v2->z, diamond->v2->y);
                                glVertex3f(diamond->v1->x, diamond->v1->z, diamond->v1->y);
                                glVertex3f(diamond->n2->a->v2->x, diamond->n2->a->v2->z, diamond->n2->a->v2->y);
                                glVertex3f(diamond->v3->x, diamond->v3->z, diamond->v3->y);
                            glEnd();
                            break;
                        }
                        case 3: { // Three Triangles
                            
                            glBegin(GL_TRIANGLE_STRIP);
                                // Begin first triangle
                                glVertex3f(diamond->v1->x, diamond->v1->z, diamond->v1->y);
                                glVertex3f(diamond->n2->a->v2->x, diamond->n2->a->v2->z, diamond->n2->a->v2->y);
                                glVertex3f(diamond->n1->a->v3->x, diamond->n1->a->v3->z, diamond->n1->a->v3->y);
                                // Finish first triangle                            
                                glVertex3f(diamond->v2->x, diamond->v2->z, diamond->v2->y);
                                glVertex3f(diamond->v3->x, diamond->v3->z, diamond->v3->y);
                            glEnd();
                            break;
                        }
                        case 4:
                            {
                            glBegin(GL_TRIANGLE_STRIP);
                                glVertex3f(diamond->v1->x, diamond->v1->z, diamond->v1->y);
                                glVertex3f(diamond->v3->x, diamond->v3->z, diamond->v3->y);
                                glVertex3f(diamond->n3->a->v1->x, diamond->n3->a->v1->z, diamond->n3->a->v1->y);
                                glVertex3f(diamond->v2->x, diamond->v2->z, diamond->v2->y);                                
                            glEnd();
                            break;
                        }
                        case 5:
                            {
                            glBegin(GL_TRIANGLE_STRIP);                            
                                // Begin first triangle
                                glVertex3f(diamond->v2->x, diamond->v2->z, diamond->v2->y);
                                glVertex3f(diamond->n1->a->v3->x, diamond->n1->a->v3->z, diamond->n1->a->v3->y);
                                glVertex3f(diamond->n3->a->v1->x, diamond->n3->a->v1->z, diamond->n3->a->v1->y);
                                // Finish first triangle
                                glVertex3f(diamond->v3->x, diamond->v3->z, diamond->v3->y);
                                // End Second triangle
                                glVertex3f(diamond->v1->x, diamond->v1->z, diamond->v1->y);
                            glEnd();
                            break;                            
                        }
                        case 6: {
                            glBegin(GL_TRIANGLE_STRIP);
                                // Begin first triangle
                                glVertex3f(diamond->v1->x, diamond->v1->z, diamond->v1->y);
                                glVertex3f(diamond->n2->a->v2->x, diamond->n2->a->v2->z, diamond->n2->a->v2->y);
                                glVertex3f(diamond->v2->x, diamond->v2->z, diamond->v2->y);
                                //End first triangle
                                glVertex3f(diamond->n3->a->v1->x, diamond->n3->a->v1->z, diamond->n3->a->v1->y);                        
                                //End second triangle
                                glVertex3f(diamond->v3->x, diamond->v3->z, diamond->v3->y);
                            glEnd();
                            break;
                        }
                        case 7: {
                            glBegin(GL_TRIANGLE_STRIP);
                                glVertex3f(diamond->v1->x, diamond->v1->z, diamond->v1->y);
                                glVertex3f(diamond->n1->a->v3->x, diamond->n1->a->v3->z, diamond->n1->a->v3->y);
                                glVertex3f(diamond->n2->a->v2->x, diamond->n2->a->v2->z, diamond->n2->a->v2->y);
                                glVertex3f(diamond->n3->a->v1->x, diamond->n3->a->v1->z, diamond->n3->a->v1->y);                            
                                glVertex3f(diamond->v2->x, diamond->v2->z, diamond->v2->y);
                            glEnd();
                            glBegin(GL_TRIANGLES);
                                glVertex3f(diamond->v3->x, diamond->v3->z, diamond->v3->y);
                                glVertex3f(diamond->n2->a->v2->x, diamond->n2->a->v2->z, diamond->n2->a->v2->y);                                                        
                                glVertex3f(diamond->n3->a->v1->x, diamond->n3->a->v1->z, diamond->n3->a->v1->y);
                            glEnd();
                            
                        break;
                        }
                    } //End Select  
                */
                }
            }
        }
    }
	// Turn the second multitexture pass off
	glActiveTextureARB(GL_TEXTURE1_ARB);
    glDisable(GL_TEXTURE_2D);

	// Turn the first multitexture pass off
	glActiveTextureARB(GL_TEXTURE0_ARB);		
    glDisable(GL_TEXTURE_2D);

	RenderWater(timer);
    glFlush();
    
    
    glPopMatrix();

    return triCount;
};

// set the texture coords for multitex arb...
void DPatch::SetTextureCoord(float x, float z)
{
	// Find the (u, v) coordinate for the current vertex
	float u =  (float)x / (float)1024;
	float v = -(float)z / (float)1024;
	
	// Give OpenGL the current terrain texture coordinate for our height map
	glMultiTexCoord2fARB(GL_TEXTURE0_ARB, u, v);

	// Give OpenGL the current detail texture coordinate for our height map
	glMultiTexCoord2fARB(GL_TEXTURE1_ARB, u, v);
}



void DPatch::RenderWater(WTimer *timer)
{
	float waterTile = 2*60.f;

#define	WATER_STRIP_LEN	15
#define WATER_STRIP_LEN_1	(WATER_STRIP_LEN-1)

	Vertex	temp;
	Vertex	dU, dV;
	
	//float sys_Time=0;
    // Get the current time, and update the time controller.
    LARGE_INTEGER TimeNow;
    QueryPerformanceCounter(&TimeNow);
    unsigned long sys_Time = ((TimeNow.QuadPart-TimeStart.QuadPart));


	dU.x = 24000;
		dU.y = 0;
		dU.z = 0;
		//= skyBoxVerts[5] - skyBoxVerts[4];
	dV.x = 0;
	dV.y = 0;
	dV.z = -24000;
	//= skyBoxVerts[7] - skyBoxVerts[4];

	//float anim = sys_Time*0.0000001f;
	anim+=0.0001;
	if(anim > 1)
		anim = 0;
	float xx, yy;

	glEnable(GL_BLEND);
	glDisable(GL_CULL_FACE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(1,1,1,0.45f);
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);

	glBindTexture(GL_TEXTURE_2D, TextureID_water);

/*
	for (int j=0; j<WATER_STRIP_LEN; j++) {
		glBegin( GL_QUAD_STRIP );
		for (int i=0; i<WATER_STRIP_LEN+1; i++) {
			for (int dy=1; dy>=0; dy--) {
				temp.x = -11500;
				temp.y = 65;
				temp.z = 12500;
				temp.x += dU.x * ((float)i / WATER_STRIP_LEN);
				temp.y += dU.y * ((float)i / WATER_STRIP_LEN);
				temp.z += dU.z * ((float)i / WATER_STRIP_LEN);
				temp.x += dV.x * ((float)(j+dy) / WATER_STRIP_LEN);
				temp.y += dV.y * ((float)(j+dy) / WATER_STRIP_LEN);
				temp.z += dV.z * ((float)(j+dy) / WATER_STRIP_LEN);
								
				glTexCoord2d((j+dy)*waterTile/WATER_STRIP_LEN, i*waterTile/WATER_STRIP_LEN);
				glVertex3fv((const float *) &temp);
			}
		}
		glEnd();
	}*/

//	glBlendFunc(GL_SRC_ALPHA, GL_DST_COLOR);//GL_SRC_ALPHA);
	glBlendFunc(GL_SRC_COLOR, GL_DST_COLOR);//GL_SRC_ALPHA);



	for (int j=0; j<WATER_STRIP_LEN; j++) {
		glBegin( GL_QUAD_STRIP );
		for (int i=0; i<WATER_STRIP_LEN+1; i++) {
			for (int dy=1; dy>=0; dy--) {
				temp.x = -11500;
				temp.y = 68;
				temp.z = 12500;
				temp.x += dU.x * ((float)i / WATER_STRIP_LEN);
				temp.y += dU.y * ((float)i / WATER_STRIP_LEN);
				temp.z += dU.z * ((float)i / WATER_STRIP_LEN);
				temp.x += dV.x * ((float)(j+dy) / WATER_STRIP_LEN);
				temp.y += dV.y * ((float)(j+dy) / WATER_STRIP_LEN);
				temp.z += dV.z * ((float)(j+dy) / WATER_STRIP_LEN);
				xx = anim+((j+dy)*waterTile/WATER_STRIP_LEN);
				yy =  anim+(i*waterTile/WATER_STRIP_LEN);
				glTexCoord2d( xx , yy);
			//	glTexCoord2d(anim, anim);
					
				glVertex3fv((const float *)&temp);
			}
		}
		glEnd();
	}

	glDisable(GL_BLEND);


/*	pTexSkyBox[5].Use();
	glBegin( GL_QUADS );
		glTexCoord2d(0, waterTile);
		glVertex3fv(skyBoxVerts[4].GetPtr());
		glTexCoord2d(waterTile, waterTile);
		glVertex3fv(skyBoxVerts[5].GetPtr());
		glTexCoord2d(waterTile, 0);
		glVertex3fv(skyBoxVerts[6].GetPtr());
		glTexCoord2d(0, 0);
		glVertex3fv(skyBoxVerts[7].GetPtr());
	glEnd();*/
}