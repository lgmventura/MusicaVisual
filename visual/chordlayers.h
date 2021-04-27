#ifndef CHORDLAYERS_H
#define CHORDLAYERS_H


class ChordLayers
{
public:
    ChordLayers();

    bool ChordStarTrack[128] = { [0 ... 127 ] = 1 };

    // absolute coordinates, in pixels:
    int x_pos;
    int y_pos;
    int w;
    int h;
};

#endif // CHORDLAYERS_H
