#ifndef CHORDLAYERS_H
#define CHORDLAYERS_H

#include <string>

#include "layer.h"
#include "chords.h"
#include "tonnetzrenderer.h"

using namespace Hexagon;

class ChordLayers: public Layer
{
public:
    ChordLayers();

    enum ChordLayerType {PitchNames, ChordStar, Tonnetz};

    // Type:
    ChordLayerType CLType = PitchNames;

    // relative coordinates to the window and absolute sizes, in pixels:
    double x_pos = 0.25;
    double y_pos = 0.25;
    int w = 100;
    int h = 100;

    // more general options:
    bool NoteFadeOut = false;
    bool NoteCollapse = false;
    rgb GridColour = rgb(100, 100, 100);

    // Note names:
    bool AccidentalSharp = false; // 0 for flat, 1 for sharp
    //bool ChordNames = 0;

    // Chord star:
    bool NoteNamesOnStar = false;
    int TurnChordCircle = 0;
    Chord::circle ChordStarType = Chord::circleOfSemitones;
    rgb ChordStarColour = rgb(120, 120, 120);

    // Tonnetz:
    TonnetzOptions::Shape TonnetzShape = TonnetzOptions::Shape::Circle;
    int CellDiameter = 10;
    int NoteSize = 10;
    int CentralMidi = 60;
    Layout HexLayout = Layout(layout_pointy, Point2d(14, 14), Point2d(0, 0));

    void setTonnetzGridDiameter(int diameter);
};

class ChordSetupOptions
{
public:
    ChordSetupOptions();

    const static std::string ChordLayerTypes[3];
    const static std::string TonnetzShapes[2];
};

#endif // CHORDLAYERS_H
