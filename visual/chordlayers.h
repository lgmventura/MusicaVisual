#ifndef CHORDLAYERS_H
#define CHORDLAYERS_H

#include <string>

#include "chords.h"

class ChordLayers
{
public:
    ChordLayers();

    enum ChordLayerType {PitchNames, ChordStar};

    bool ChordStarTrack[128] = { [0 ... 127 ] = 1 };

    // Type:
    ChordLayerType CLType = PitchNames;

    // relative coordinates to the window and absolute sizes, in pixels:
    double x_pos = 0.25;
    double y_pos = 0.25;
    int w = 100;
    int h = 100;

    // Note names:
    bool AccidentalSharp = false; // 0 for flat, 1 for sharp
    //bool ChordNames = 0;

    // Chord star:
    bool NoteNamesOnStar = false;
    int TurnChordCircle = 0;
    chord::circle ChordStarType = chord::circleOfSemitones;
};

class ChordSetupOptions
{
public:
    ChordSetupOptions();

    const static std::string ChordLayerTypes[2];
};

#endif // CHORDLAYERS_H
