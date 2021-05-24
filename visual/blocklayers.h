#ifndef BLOCKLAYERS_H
#define BLOCKLAYERS_H

#include <string>
#include "layer.h"

class BlockLayers: public Layer // ATTENTION: TracksP (class) or tracksproperties (objects) and RenderP (class) and renderproperties (object) have to be of a fixed static size. Otherwise, edit -> save/load settings won't work!
{
public:
    BlockLayers();

    int shape[128] = {0};
    int track_blur[128] = {0};
    int interconnect[128] = {0};
    int maxBlur = 20;

    bool hLines = true;
    bool vLines = true;
    double hZoomMult = 1;
    double vZoomMult = 1;

public: // member functions

    void saveIni(std::string savePath);
    void loadIni(std::string iniPath);
};

class BlockSetupOptions // static
{
private:
    BlockSetupOptions() {}

public:
    const static std::string ColourSchemes[3];
    const static std::string Shapes[19];
    const static std::string Interconnection[2];
};

#endif // BLOCKLAYERS_H
