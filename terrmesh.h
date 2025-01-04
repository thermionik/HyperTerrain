#ifndef TERRAGEN_MESH_H
#define TERRAGEN_MESH_H

#include "terrmesh.h"

//Handles Terragen files
class TMesh {
    public:
        TMesh();
        ~TMesh();
        float GetHeight(float, float);
        char* Name();
        bool Load(const char*); //Load Filename
        int GetDim();
    protected:
        void readTGN(const void *, unsigned long);
    
    private:
        char* filename;
        int rows;
        int cols;
        unsigned short* pixbuffer;
        
};




#endif