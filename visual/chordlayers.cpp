#include "chordlayers.h"

ChordLayers::ChordLayers()
{

}

void ChordLayers::setTonnetzGridDiameter(int diameter)
{
    this->HexLayout.size.x = diameter;
    this->HexLayout.size.y = diameter;
}

const std::string ChordSetupOptions::ChordLayerTypes[3] = {
    "Note names",
    "Chord stars",
    "Tonnetz"
};

const std::string ChordSetupOptions::TonnetzShapes[2] = {
    "Circles",
    "Hexagons"
};
