#include <sys/types.h>
#include <sys/stat.h>
#include <string>
#include <iostream>
using namespace std;

#include "terrmesh.h"
#include "types.h"
#include "utility.h"

TMesh::TMesh(): filename(NULL), rows(0), cols(0), pixbuffer(NULL){};

TMesh::~TMesh(){
    if (pixbuffer)
        delete pixbuffer;
};

float TMesh::GetHeight(float xcoord, float ycoord){
    return 0.0;
};

char* TMesh::Name(){
    return filename;
};

int TMesh::GetDim(){
    return rows;
};

/*
Terragen terrains have their 0,0 coordinate at the bottom left of the map (south/west) corner.
X runs west to east/left to right.
Y runs south to north/bottom to top.

"XPTS" 4-byte marker. Must appear after the "SIZE" marker. Must appear before any altitude data. 

     The "XPTS" marker is followed by a 2-byte integer value xpts, followed by 2 bytes of padding. xpts is equal to the number of data
     points in the x-direction in the elevation image. 

"YPTS" 4-byte marker. Must appear after the "SIZE" marker. Must appear before any altitude data. 

     The "YPTS" marker is followed by a 2-byte integer value ypts, followed by 2 bytes of padding. ypts is equal to the number of data points
     in the y-direction in the elevation image. 

"SIZE" 4-byte marker, necessary. Must appear before any altitude data. 

     The "SIZE" marker is followed by a 2-byte integer value equal to n - 1, followed by 2 bytes of padding. In square terrains, n is the
     number of data points on a side of the elevation image. In non-square terrains, n is equal to the number of data points along the shortest
     side. 
     Example: a terrain with a heightfield 300 points in the x-direction and 400 points in the y-direction would have a size value of 299. 

"SCAL" 4-byte marker, optional. Must appear before any altitude data. 

     The "SCAL" marker is followed by three intel-ordered 4-byte floating point values (x,y,z). It represents the scale of the terrain in metres
     per terrain unit. Default scale is currently (30,30,30). At present, Terragen can not use non-cuboid scaling, so x, y and z must be equal. 

"CRAD" 4-byte marker, optional. Must appear before any altitude data. 

     The "CRAD" marker is followed by one intel-ordered 4-byte floating point value. It represents the radius of the planet being rendered
     and is measured in metres. The default value is 6370, which is the approximate radius of the Earth. 

"CRVM" 4-byte marker, optional. Must appear before any altitude data. 

     The "CRVM" marker is followed by one unsigned integer. Mode 0 means the terrain is rendered flat (default). Mode 1 means the terrain
     is draped (and stretched) over a sphere of radius CRAD*1000/zscale, centred at (midx, midy, -CRAD*1000/zscale), where
     midx=XSIZE/2 and midy=YSIZE/2. Terrain sizes are one less than the number of points on the side. 
     The x and y values are undistorted, therefore the map will still look normal when viewed from above, but geographic distances will be
     stretched towards the edge of the map if there is a lot of curvature. There is also an implicit limit on the size/curvature radius of a
     landscape before the landscape becomes unacceptably distorted (and wraps around onto itself!) 
     Other curve modes are currently undefined, and reserved for the future. 

"ALTW" 4-byte marker. Must appear after the "SIZE" marker. Must appear after the "XPTS" and "YPTS" markers (if they exist). 

     ALTW stands for 'Altitude in 16-bit Words'. After The "ALTW" marker, the following appear in order: 

          HeightScale, a 2-byte signed integer value. 
          BaseHeight, a 2-byte signed integer value. 
          Elevations, a sequence of 2-byte signed integers. 

     There are (xpts * ypts) elevation integers, where xpts and ypts will have been set earlier in the "SIZE" segment or the "XPTS" and
     "YPTS" segments. The elevations are ordered such that the first row (y = 0) is read first from left to right, then the second (y = 1), and so
     on. The values in Elevations are not absolute altitudes. The absolute altitude of a particular point (in the same scale as x and y) is equal to
     BaseHeight + Elevation * HeightScale / ddgTerragenScale. 
*/

bool TMesh::Load(const char* fname){
    filename = (char*)fname;
    
	struct stat fileStatus;
	void *buffer = 0;

	//ddgVector3 scale;			/// Terrain scale.
//	float minHexel = MAXFLOAT, maxHexel = -MAXFLOAT;

//	char *fullName = ddgFileHelper::getFullFileName(filename);


	FILE *fptr = fopen(filename,"rb");

	if (!fptr){
        cout<<"\nFailed to open "<<filename<<"\nExiting..";
        exit(-1);
    }

	if (stat(filename,&fileStatus) != 0){
        cout<<"\nFile contains no data";
        exit(-1);
    }
    else
        cout<<"\n"<<filename<<" contains : "<<fileStatus.st_size<<" bytes";

	buffer = new unsigned char[fileStatus.st_size+10];
	fread(buffer,fileStatus.st_size,1,fptr);

	fclose(fptr);
	fptr = 0;

	readTGN(buffer,fileStatus.st_size);

	if (buffer)
		delete [] buffer;

	if (fptr)
		fclose(fptr);
    
    return true;
};

void TMesh::readTGN(const void *buf, unsigned long size)
{
	bool done = false;
	unsigned char ch1, ch2;
	char name[9],type[9], segment[5];
	const char* ptr = (const char*)buf;
	const char* const bufend = ptr + size;
	const char* hexelPtr = 0;
	Vector3 scale;			/// Terrain scale.
	float minHexel = MAXFLOAT, maxHexel = -MAXFLOAT;
	int baseHeight, heightScale;
	scale.set(30,30,30);		// In case it is not provided, default to this.
	float _radius = 0;

	// Name 8 bytes.
	memcpy(name, ptr, 8);
	ptr += 8;
	name[8] = '\0';
	memcpy(type, ptr, 8);
	ptr += 8;
	type[8] = '\0';
	if (strcmp(name,"TERRAGEN") || strcmp(type,"TERRAIN ")){
//		ddgErrorSetExit(ddgE_DataType,"Invalid header on Terragen file");
        cout<<"\nInvalid Header on Terragen file";
        exit(-1);
    }

	while (!done)
	{
		// Segment 4 bytes.
		segment[4] = '\0';
		memcpy(segment,ptr,4);
		ptr += 4;

		if (!strcmp(segment,"SIZE"))
		{
			// SIZE 2 bytes.
			ch1 = *ptr++;
			ch2 = *ptr++;
			rows = cols = ch2*256+ch1 +1;
			// Padding 2 bytes.
			ptr += 2;
		}
		else if (!strcmp(segment,"XPTS"))
		{
			// XPTS.
			ch1 = *ptr++;
			ch2 = *ptr++;
			cols = ch2*256+ch1;
			// Padding 2 bytes.
			ptr += 2;
		}
		else if (!strcmp(segment,"YPTS"))
		{
			// YPTS.
			ch1 = *ptr++;
			ch2 = *ptr++;
			rows = ch2*256+ch1;
			// Padding 2 bytes.
			ptr += 2;
		}
		else if (!strcmp(segment,"SCAL"))
		{
			scale.v[0] = *((float*)ptr); ptr += sizeof(float);
			scale.v[1] = *((float*)ptr); ptr += sizeof(float);
			scale.v[2] = *((float*)ptr); ptr += sizeof(float);
 		}
		else if (!strcmp(segment,"CRAD"))
		{
			_radius = ((*(float*)ptr)) * 1000.0;
			ptr += 1 * sizeof(float);
		}
		else if (!strcmp(segment,"CRVM"))
		{
			// Ignore
			ptr += 1*sizeof(unsigned int);
		}
		else if (!strcmp(segment,"ALTW"))
		{
			// ALTW.
			// Absolute elevation for a given point is
			// BaseHeight + elevation* Scale / ddgTerragenScale.
			ch1 = *ptr++;
			ch2 = *ptr++;
			heightScale = (ch2*256+ch1);

			ch1 = *ptr++;
			ch2 = *ptr++;
			baseHeight = ( ch2*256+ch1);

			// Read height data
			// The absolute altitude of a particular point (in the same scale as x and y)
			// is equal to BaseHeight + Elevation * HeightScale / ddgTerragenScale
			hexelPtr = ptr;
			int i;
			// Scan through the data and find the min/max.
			for (i = 0; i < cols * rows; i++)
			{
				ch1 = *ptr++;
				ch2 = *ptr++;
				int data = ( ch2*256+ch1);
				if (data > 0x7FFF) data = (data - 0x10000);

				if (data < minHexel)
					minHexel = data;
				else if (data > maxHexel)
					maxHexel = data;
			}

			done = true;
		} // ALTW
	} // End while

	// Allocate hexel data, note this resets the bbox.
	if (! allocate(cols,rows) ){
		cout<<"Failed to allocate space for Terragen data"<<endl;
        exit(-1);
    }
/*
	// Set the world space bounding box.
	{
		Vector3 vMin, vMax;
		float tMin = (baseHeight + heightScale * minHexel/ddgTerragenScale) * scale[2];
		float tMax = (baseHeight + heightScale * maxHexel/ddgTerragenScale) * scale[2];
		setMinMax(tMin, tMax);
		vMin.set(0,0,tMin);
		vMax.set(scale[0] * (cols-1), scale[1] * (_rows-1), tMax);
		_bbox.set(&vMin,&vMax);
		_invalidValue = convert(DDG_INVALID_ELEVATION);
	}*/

	{
		// Load the data into pixbuffer.
		ptr = hexelPtr;
		int i,c=0,r=0;

		for (i = 0; i < cols * rows; i++)
		{
			ch1 = *ptr++;
			ch2 = *ptr++;
			int data = ( ch2*256+ch1);
			if (data > 0x7FFF) data = (data - 0x10000);
//			ddgAssert(c>=0 && r >=0 && c <_cols && r <_rows);

			setf(c,r,((float)baseHeight + (float)heightScale *((float)data)/(float)ddgTerragenScale)* scale[2]);
			c++;
			if (c == cols)
			{
				c = 0;
				r++;
			}
		}
	}

};

