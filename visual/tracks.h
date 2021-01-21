#ifndef TRACKS_H
#define TRACKS_H

#include "utils/colour.h"

class TracksP // ATTENTION: TracksP (class) or tracksproperties (objects) and RenderP (class) and renderproperties (object) have to be of a fixed static size. Otherwise, edit -> save/load settings won't work!
{
public:
    TracksP();
    int tColours[24][3]= {
        {0, 0, 200} ,   //1 (index 0)
        {0, 120, 120} ,   //2
        {0, 200, 0},    //3
        {120, 120, 0},    //4
        {200, 0, 0},    //5
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
        // new tracks
        {30, 60, 90},
        {60, 30, 90},
        {30, 90, 60},
        {90, 30, 60},
        {60, 90, 30},
        {90, 60, 30},
        {200, 100, 50},
        {50, 100, 200}
    };
    bool active[24] = {1,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1};
    short shape[24] = {0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0};
    short track_blur[24] = {0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0};
    short interconnect[24] = {0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0};
    int max_connect_dist = 100;
    short colorScheme[24] = {0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0};

public: // member functions
    rgb getColour(int track, int pitch);
    int getCv(int a, int b) {return (tColours[a][b]);}
    void setCv(int a, int b, int v) {tColours[a][b] = v;}
};

#endif // TRACKS_H
