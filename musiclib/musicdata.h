#ifndef MUSICDATA_H
#define MUSICDATA_H

#include <list>
#include "midimessages.h"
#include "chords.h"

class MusicData
{
public:
    MusicData();

    int NEvents; // Number of events
    std::list <MidiNote> Notes; // List of processed notes.
    std::list <TempoChange> *Tempos = new std::list <TempoChange>;
    std::list <TimeSignature> TSignatures; // list of time signatures (being used for vertical lines)
    unsigned long TotalTime = 0; // Global variable to store the total time of the current processed midi file.
    unsigned int PitchMax = 1, PitchMin = 128;
    unsigned int NTracks = 1;
    unsigned int Tpq; // Ticks per Quarter-Note
    chords GChords; // global var for chords - TODO: GET RID of global variables as much as possible, replace by oop

    std::vector<std::string> *TrackNames = new std::vector<std::string>; // = {"Track 1", "Track 2", "Track 3", "Track 4", "Track 5", "Track 6", "Track 7", "Track 8", "Track 9", "Track 10", "Track 11", "Track 12", "Track 13", "Track 14", "Track 15", "Track 16", "Track 17", "Track 18", "Track 19", "Track 20", "Track 21", "Track 22", "Track 23", "Track 24"}; // this works, but is variable in size. So save/load settings won't work if put in TracksP.

};

#endif // MUSICDATA_H
