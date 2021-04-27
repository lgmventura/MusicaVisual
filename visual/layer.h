#ifndef LAYER_H
#define LAYER_H

#include "visual/blocklayers.h"
#include "visual/chordlayers.h"

class Layer
{
public:
    Layer();

    enum LayerType {BlockLayer, ChordLayer};

    std::string Name;

    bool LayerActive = true;

    LayerType LType;

    BlockLayers Bl;
    ChordLayers Cl;

    // Ideas for later:
    // int BlendMode;
    // float Opacity;
};

#endif // LAYER_H
