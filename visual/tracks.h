#ifndef TRACKS_H
#define TRACKS_H

#include "utils/colour.h"
#include <string>

class TracksP // ATTENTION: TracksP (class) or tracksproperties (objects) and RenderP (class) and renderproperties (object) have to be of a fixed static size. Otherwise, edit -> save/load settings won't work!
{
public:
    TracksP();
    int tColours[128][3]= { // pre-initializing the first 24 tracks, idea: use an ini file.
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
        {50, 100, 200}
    };
    bool active[128] = { [0 ... 127 ] = 1 };
    short shape[128] = {0};
    short track_blur[128] = {0};
    short interconnect[128] = {0};
    short colorScheme[128] = {0};
    int max_connect_dist = 100;
    int maxBlur = 20;

public: // member functions
    rgb getColour(int track, int pitch);
    int getCv(int a, int b) {return (tColours[a][b]);}
    void setCv(int a, int b, int v) {tColours[a][b] = v;}

    void setActive(int track, bool value) {active[track] = value;}

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

#endif // TRACKS_H
