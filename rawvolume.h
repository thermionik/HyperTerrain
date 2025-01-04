#ifndef RAWVOLUME_H
#define RAWVOLUME_H
/*

#include <string>
#include <iostream>
using namespace std;
*/
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <stdio.h>
#include "volume.h"
#include "gl/glaux.h"


extern AUX_RGBImageRec *LoadBMP(char *Filename);

class RawVolume : public Volume{
    public:
        RawVolume();
        virtual ~RawVolume();
        short GetHeight(int, int);
		float AvgHeight(float , float );
        bool Load(char*); //Load Filename
        int GetDim();
        
        int rows, cols;
        unsigned char* buffer;
};

RawVolume::RawVolume(): rows(0), cols(0), buffer(NULL){};

RawVolume::~RawVolume(){
    if(buffer)
        delete buffer;
};

int RawVolume::GetDim(){
    return cols;
};

short RawVolume::GetHeight(int x, int y){
    return buffer[y*cols + x];
};

float RawVolume::AvgHeight(float x, float y)
{/*
	float temp;
	float t = x % 1;
	float u = y % 1;

	if ( t != 0 && u != 0 )
	{
		int x1, x2, y1, y2;
		x1 = x; x2 = x + 1;
		y1 = y; y2 = y + 1;
		if ( t > .5 )
			temp = buffer[y1*cols + x1] * (1 - t) + buffer[y1*cols + x2] * t;
		else
			temp = buffer[y1*cols + x1] * t + buffer[y1*cols + x2] * (1 - t);

		if ( u > .5)
			temp += buffer[y1*cols + x1] * (1 - t) + buffer[y2*cols + x1] * t;
		else
			temp += buffer[y1*cols + x1] * t + buffer[y2*cols + x1] * (1 - t);
		
		return temp / 4;
	}
	
	return buffer[y*cols + x];*/
	return 0;
};

bool RawVolume::Load(char* fname){
    filename = (char*)fname;
    
	struct stat fileStatus;
//	void *buffer = 0;
	FILE *fptr = fopen(filename,"rb");

	if (!fptr){
//        cout<<"\nFailed to open "<<filename<<"\nExiting..";
        return false;
    }

	if (stat(filename,&fileStatus) != 0){
  //      cout<<"\nFile contains no data";
        return false;
    }
    else
    ///    cout<<"\n"<<filename<<" contains : "<<fileStatus.st_size<<" bytes";

	buffer = new unsigned char[fileStatus.st_size];
	fread(buffer,fileStatus.st_size,1,fptr);
    rows = cols = 1024;
    
    
	fclose(fptr);
	fptr = 0;
    return true;
};

#endif