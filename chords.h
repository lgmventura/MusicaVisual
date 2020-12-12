#ifndef CHORDNAMES_H
#define CHORDNAMES_H

#include <QMainWindow>
#include "MidiFile.h"
#include "midimessages.h"
#include <set>

/*
 * We consider a chord any set of notes, even below 3, e.g. a unison note. Algorithm: imagine all notes start times and end times with a big vertical line in the note blocks.
 * All these vertical lines would be an end of a chord and a begin of a new chord. Given that all these horizontal regions are chords, including empty ones, we can later
 * check if chords have more than 3 elements to be a "real" chord.
 */

class chord
{
public:
    chord();

private:
    std::set<int> Pitches = {}; // midi pitch 0 to 127
    std::string Name = "";

public:
    // methods:
    void insertPitch(int pitch);
    std::set<int> getPitches();
    std::string getName();

private:
    void calculateName();
};


class chords
{
public:
    chords();

    //std::list<std::string> Chordnamess = {};
    std::list<chord> Chords = {};
    std::set<long> Start_end_times = {}; // every time a new note appears or ends we consider having a new chord. If 2 or more notes start or end at the same time, it is still the same chord, so we have a set
    std::vector<long> VStart_end_times = {}; // this will be sorted

    // Methods
    //int process_chords(std::list <MidiNote> notes, std::list<int> tracks);
    int process_chords(std::list <MidiNote> notes, bool* tracks);
};

#endif // CHORDNAMES_H
