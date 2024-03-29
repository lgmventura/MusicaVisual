#ifndef CHORDNAMES_H
#define CHORDNAMES_H

#include <QMainWindow>
#include "MidiFile.h"
#include "midimessages.h"
#include <set>
#include <math.h>

/*
 * We consider a chord any set of notes, even below 3, e.g. a unison note. Algorithm: imagine all notes start times and end times with a big vertical line in the note blocks.
 * All these vertical lines would be an end of a chord and a begin of a new chord. Given that all these horizontal regions are chords, including empty ones, we can later
 * check if chords have more than 3 elements to be a "real" chord.
 */
class PitchDict // static
{
private:
    PitchDict() {}

public:
    const static char LettersFlat[12][3];
    const static char LettersSharp[12][3];
    const static std::string GermanicNamesFlat[12];
    const static std::string GermanicNamesSharp[12];
    const static std::string SolfeggioNamesFlat[12];
    const static std::string SolfeggioNamesSharp[12];

};

//std::string LettersFlat[12]  =        {"C", "Db", "D", "Eb", "E", "F", "Gb", "G", "Ab", "A", "Bb", "B"};
//std::string LettersSharp[12] =        {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};
//std::string GermanicNamesFlat[12]  =  {"C", "Des", "D", "Es", "E", "F", "Ges", "G", "Aes", "A", "B", "H"};
//std::string GermanicNamesSharp[12] =  {"C", "Cis", "D", "Dis", "E", "F", "Fis", "G", "Gis", "A", "Ais", "B"};
//std::string SolfeggioNamesFlat[12]  = {"Do", "Re bemol", "Re", "Mi bemol", "Mi", "Fa", "Sol bemol", "Sol", "La bemol", "La", "Si bemol", "Si"};
//std::string SolfeggioNamesSharp[12] = {"Do", "Do sustenido", "Re", "Re sustenido", "Mi", "Fa", "Fa sustenido", "Sol", "Sol sustenido", "La", "La sustenido", "Si"};

class Pitch
{
public:
    Pitch(int midi = -1);

    enum circle {circleOfSemitones, circleOfFifths};
// members:
public:
//    enum Letters {C, D, E, F, G, A, B};
    enum Accidental {flat, sharp}; // bemol, sustenido
private:
    int Midi; // 0 to 127

    int Octave;
    int DistanceFromLastC; // 0 to 11
    std::string LetterFlat; // A to G
    std::string LetterSharp;
//    std::string GermanicFlat; // A to H
//    std::string GermanicSharp;
//    std::string SolfeggioFlat; // Do to Si
//    std::string SolfeggioSharp;

    int MidiTrack = -1; // if not set or not relevant, the default value is -1

// get methods:
public:
    int getMidiPitch() {return this->Midi;};
    int getOctave();
    int getDistanceFromLastC();
    std::string getLetterName(Accidental type);
    std::string getLetterNameWithOctave(Accidental type);
    int getMidiTrack() const {return this->MidiTrack;};
    float getAngleDeg(circle type);

// set methods:
    void setTrack(int track);

// other:
    bool isValidPitch();

// operators:
    bool operator<(const Pitch &other) const {return this->Midi < other.Midi;};
    bool operator>(const Pitch &other) const {return this->Midi > other.Midi;};
    bool operator<=(const Pitch &other) const {return this->Midi <= other.Midi;};
    bool operator>=(const Pitch &other) const {return this->Midi >= other.Midi;};
    bool operator==(const Pitch &other) const {return this->Midi == other.Midi;};
};


class Chord
{
public:
    Chord();
    enum circle {circleOfSemitones, circleOfFifths};

private:
    std::set<Pitch> Pitches = {}; // midi pitch 0 to 127.
    std::string Name = "";

public:
    // methods:
    void insertPitch(Pitch pitch);
    std::set<Pitch> getPitches();
    std::string getPitchesStr(bool accidentalSharp = false, bool *tracks = new bool[128], bool includeUnsetTracks = true);
    std::string getName();
    std::set<float> getAnglesDeg(circle type, bool *tracks, bool includeUnsetTracks = true); // for visual representation of chords

private:
    void calculateName();
};

class ChordWithTime
{
public:
    ChordWithTime();

    Chord chord;
    long Start_time;
};

class Chords
{
public:
    Chords();

    //std::list<std::string> Chordnamess = {};
    std::list<ChordWithTime> ChordsWTime = {};
    std::set<long> Start_end_times = {}; // every time a new note appears or ends we consider having a new chord. If 2 or more notes start or end at the same time, it is still the same chord, so we have a set
    std::vector<long> VStart_end_times = {}; // this will be sorted

    // Methods
    //int process_chords(std::list <MidiNote> notes, std::list<int> tracks);
    int process_chords(std::list <MidiNote> notes, bool includeTrackInfo);
};

#endif // CHORDNAMES_H
