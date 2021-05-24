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

    std::string getName();
    void setName(std::string);

private:
    char Name[128];

    // Ideas for later:
    // int BlendMode;
    // float Opacity;
};

#endif // LAYERCONTAINER_H
