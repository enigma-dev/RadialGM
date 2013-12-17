namespace RGM {

#ifndef RESOURCE_H
#define RESOURCE_H

class Resource
{
public:
    virtual ~Resource();
    virtual void LoadDefaults() = 0;
    virtual void SaveDefaults() = 0;
    virtual void Initialize() = 0;
    virtual void Finalize() = 0;
};

#endif // RESOURCE_H

}
