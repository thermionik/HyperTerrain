#include "voxel.h"
#include <stdio.h>
#include <math.h>
#ifndef WIN32
#include <sys/uio.h>
#include <unistd.h>
#endif

Voxel::Voxel() : m_width(1024), m_breadth(1024), m_height(256), m_resolution(1024), space(NULL) {};

Voxel::Voxel(const char* filename)
{
    LoadVoxelFile(filename);
};
        
Voxel::~Voxel()
{
    if ( space )
        delete [] space;
}

void Voxel::ConvertHeightmap(const unsigned char* buffer, int buffsize, int bytespersample)
{
    int numsamples = buffsize / bytespersample;
    m_width = m_breadth = sqrt((float)numsamples);  // Get width and height (must be square)!
    int height;//, temp;

    m_height = 1 << (bytespersample * 8);

    space = new unsigned char [m_width * m_breadth * (m_height / 8)];
    unsigned char* byte = space;
    
    
    for (int x = 0; x < m_width; x++)
        for (int y=0; y < m_breadth; y++)
        {
            height = buffer[y*m_width + x];  // Height of current sample
            int numbytes = height / 8;
            for (int iter=0; iter < numbytes; iter++, byte++)
                *byte = 0xFF;					// Everything underneath the surface is solid
            int bitlocation = (height % 8) - 1;
            *byte = 0x80 >> bitlocation;			// Actual surface bit stored in last used byte
            while ( numbytes < (m_height / 8) )
            {
                *byte++ = 0x00;
                numbytes++;
            }
        }
};

void Voxel::SaveHeightmap(const char* filename)
{
    


};
        
bool Voxel::LoadVoxelFile(const char* filename)
{
//    int width, breadth, height, size;
    unsigned char buffer[8];
    
    struct stat fileStatus;

    FILE *fptr = fopen(filename,"rb");

    if (!fptr){
        cout<<"\nFailed to open "<<filename<<"\nExiting..";
        exit (1);
    }

    if (stat(filename,&fileStatus) != 0){
        cout<<"\nFile contains no data";
        exit (1);
    }
    
    VWDHeader head;
    cout<<"\n"<<filename<<" contains : "<<fileStatus.st_size<<" bytes";
    head.pad = 0x00;

    fread(&head,sizeof(VWDHeader),1,fptr);
    if ( !CheckSig(head) )
        return false;
 
    fread(&buffer,8 , 1, fptr);
    
    m_width = (buffer[0] << 8) + buffer[1];
    m_breadth = (buffer[2] << 8) + buffer[3];
    m_height = (buffer[4] << 8) + buffer[5];
    // For future use
    m_resolution = (buffer[6] << 8) + buffer[7];
    
    cout<<endl<<"Width : "<<m_width<<"\nBreadth : "<<m_breadth<<"\nHeight : "<<m_height;//<<"\nResolution : "<<m_resolution;

    space = new unsigned char[m_width * m_breadth * (m_height/8)];
    
    if (!space)
        exit(-1);
        
    fread(space, m_width * m_breadth * (m_height / 8), 1, fptr);

    fclose(fptr);
	return(true);
};

void Voxel::SaveVoxelFile(const char* filename)
{
    unsigned char temp;
    int itemp;
//    int file = open(filename, O_RDWR | O_CREAT, 0766);
    FILE *fptr = fopen(filename,"w+");

    if (!fptr){
        cout<<"\nFailed to open "<<filename<<"\nExiting..";
		exit (1);
    }
    // Write Header to file
    // VWD is signature of file
    // Next six bytes are Major Minor Revision (file version) Ascii format
    VWDHeader whead;
    whead.sig[0] = 'V';
    whead.sig[1] = 'W';
    whead.sig[2] = 'D';
    // Major
    whead.filever[0] = '0';
    whead.filever[1] = '0';
    // Minor
    whead.filever[2] = '0';
    whead.filever[3] = '0';
    // Revision
    whead.filever[4] = '0';
    whead.filever[5] = '0';
    
    fwrite(&whead, sizeof(VWDHeader), 1, fptr);
    
    itemp = m_width & 0xFF00;
    itemp = itemp >> 8;
    temp = itemp;
    fwrite(&temp, sizeof(unsigned char), 1, fptr);
    temp = m_width & 0x00FF;
    fwrite(&temp, sizeof(unsigned char), 1, fptr);
    temp = m_breadth >> 8;
    fwrite(&temp, sizeof(unsigned char), 1, fptr);
    temp = m_breadth & 0x0F >> 8;
    fwrite(&temp, sizeof(unsigned char), 1, fptr);
    temp = m_height >> 8;
    fwrite(&temp, sizeof(unsigned char), 1, fptr);
    temp = m_height & 0x0F >> 8;
    fwrite(&temp, sizeof(unsigned char), 1, fptr);
    
    // Write the actual data
    fwrite(space, m_width*m_breadth*(m_height/8), 1, fptr);
    fclose(fptr);
};

bool Voxel::CheckSig(const VWDHeader& header)
{
    if ( header.sig[0] != 'V' )
        return false;
    if ( header.sig[1] != 'W' )
        return false;
    if ( header.sig[2] != 'D' )
        return false;
    else
        return true;
};

bool Voxel::CheckVersion(const VWDHeader& header)
{
    return true;
};
