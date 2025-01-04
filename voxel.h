#ifndef VOXELSPACE_H
#define VOXELSPACE_H


#include <sys/types.h>
#include <sys/stat.h>
#include <string>
#include <iostream>
using namespace std;

typedef struct header
{
    unsigned char sig[3];
    unsigned char filever[6];
    unsigned char pad;       //Present only to pad to 10 bytes
} VWDHeader;

class Voxel //: public CSpace
{
    public:
        Voxel();
        Voxel(const char*);
        ~Voxel();
        
        void ConvertHeightmap(const unsigned char*,int, int);
        void SaveHeightmap(const char*);
        
        bool LoadVoxelFile(const char*);
        void SaveVoxelFile(const char*);
    private:
        bool CheckSig(const VWDHeader&);
        bool CheckVersion(const VWDHeader&);
                
    private:
        int m_width, m_breadth, m_height, m_resolution;
        
        unsigned char *space;
};

#endif