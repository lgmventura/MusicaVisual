#ifndef LAYER_H
#define LAYER_H

#include "utils/colour.h"

class Layer
{
public:
    Layer();
    ~Layer();

    bool ActiveTracks[128] = { [0 ... 127] = 1 };

    // Class member functions (methods):
    void setActive(int track, bool value) {ActiveTracks[track] = value;}

};

#endif // LAYER_H
