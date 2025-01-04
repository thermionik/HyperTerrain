#ifndef VOLUME_H
#define VOLUME_H

class Volume{
    public:
//        virtual ~Mesh();
        char* Name(){ return filename; };
        virtual short GetHeight(int, int) = 0;
        virtual float AvgHeight(float, float) = 0;
        virtual bool Load(char*) = 0; //Load Filename
        virtual int GetDim() = 0;
    protected:
        char* filename;
};

#endif