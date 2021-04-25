#ifndef CHORDLAYERS_H
#define CHORDLAYERS_H


class ChordLayers
{
public:
    ChordLayers();

    bool ChordStarTrack[128] = { [0 ... 127 ] = 1 };
};

#endif // CHORDLAYERS_H
