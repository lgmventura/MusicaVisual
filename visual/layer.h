#ifndef LAYER_H
#define LAYER_H

#include "visual/blocklayers.h"
#include "visual/chordlayers.h"

class Layer
{
public:
    Layer();
    ~Layer();

    enum LayerType {BlockLayer = 0, ChordLayer = 1};

    bool LayerActive = true;

    LayerType LType = BlockLayer;

    BlockLayers Bl;
    ChordLayers Cl;

    std::string getName();
    void setName(std::string);

private:
    char Name[128];

    // Ideas for later:
    // int BlendMode;
    // float Opacity;
};

#endif // LAYER_H
