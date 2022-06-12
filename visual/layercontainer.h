#ifndef LAYERCONTAINER_H
#define LAYERCONTAINER_H

#include "visual/blocklayers.h"
#include "visual/chordlayers.h"

class LayerContainer
{
public:
    LayerContainer();
    ~LayerContainer();

    enum LayerType {BlockLayer = 0, ChordLayer = 1};

    bool LayerActive = true;

    LayerType LType = BlockLayer;

    BlockLayers Bl;
    ChordLayers Cl;

    int TColours[128][3]= { // pre-initializing the first tracks, idea: use an ini file.
        {60, 60, 240} ,   //1 (index 0)
        {0, 120, 120} ,   //2
        {0, 220, 0},    //3
        {120, 120, 0},    //4
        {220, 20, 20},    //5
        {120, 0, 120},    //6
        {80, 80, 200} ,   //7
        {0, 200, 200} , //8
        {80, 200, 80},    //9
        {100, 100, 100},//10
        {100, 100, 0},  //11
        {200, 0, 0},    //12
        {100, 0, 100},  //13
        {50, 100, 150}, //14
        {150, 100, 50}, //15
        {100, 200, 100}, //16 (index 15)
        {30, 60, 90},
        {60, 30, 90},
        {30, 90, 60},
        {90, 30, 60},
        {60, 90, 30},
        {90, 60, 30},
        {200, 100, 50},
        {50, 100, 200},
        {60, 60, 120} ,   //25
        {40, 120, 120} ,   //26
        {40, 160, 40},
        {120, 120, 40},
        {160, 60, 60},
        {120, 40, 120},
        {80, 80, 160} ,
        {40, 160, 160} ,
        {80, 160, 80},
        {100, 100, 100},
        {80, 80, 60},
        {160, 40, 40},
        {100, 40, 100},
        {80, 100, 120},
        {120, 100, 80},
        {100, 160, 100},
        {40, 60, 90},
        {60, 40, 90},
        {40, 90, 60},
        {90, 40, 60},
        {60, 90, 40},
        {90, 60, 40},
        {160, 100, 80},
        {80, 100, 160} // 48
        // initialize more if needed, up to 128 (index 127)
    };

    int ColourScheme[128] = {0};

public: // methods
    std::string getName();
    void setName(std::string);

    rgb getColour(int track, int pitch);
    int getCv(int a, int b) {return (TColours[a][b]);}
    void setCv(int a, int b, int v) {TColours[a][b] = v;}

private:
    char Name[128];

    // Ideas for later:
    // int BlendMode;
    // float Opacity;
};

#endif // LAYERCONTAINER_H
