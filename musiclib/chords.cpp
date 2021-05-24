#include "chords.h"
#include <iostream> // only for debugging


const std::string PitchDict::LettersFlat[12]  =        {"C", "Db", "D", "Eb", "E", "F", "Gb", "G", "Ab", "A", "Bb", "B"};
const std::string PitchDict::LettersSharp[12] =        {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};
const std::string PitchDict::GermanicNamesFlat[12]  =  {"C", "Des", "D", "Es", "E", "F", "Ges", "G", "Aes", "A", "B", "H"};
const std::string PitchDict::GermanicNamesSharp[12] =  {"C", "Cis", "D", "Dis", "E", "F", "Fis", "G", "Gis", "A", "Ais", "B"};
const std::string PitchDict::SolfeggioNamesFlat[12]  = {"Do", "Re bemol", "Re", "Mi bemol", "Mi", "Fa", "Sol bemol", "Sol", "La bemol", "La", "Si bemol", "Si"};
const std::string PitchDict::SolfeggioNamesSharp[12] = {"Do", "Do sustenido", "Re", "Re sustenido", "Mi", "Fa", "Fa sustenido", "Sol", "Sol sustenido", "La", "La sustenido", "Si"};



Pitch::Pitch(int midi)
{
    Pitch::Midi = midi;
    Pitch::Octave = midi/12;
    Pitch::DistanceFromLastC = midi%12;
    Pitch::LetterFlat = PitchDict::LettersFlat[midi%12];
    Pitch::LetterSharp = PitchDict::LettersSharp[midi%12];
    Pitch::GermanicFlat = PitchDict::GermanicNamesFlat[midi%12];
    Pitch::GermanicSharp = PitchDict::GermanicNamesSharp[midi%12];
    Pitch::SolfeggioFlat = PitchDict::SolfeggioNamesFlat[midi%12];
    Pitch::SolfeggioSharp = PitchDict::SolfeggioNamesSharp[midi%12]; // pitch::DistanceFromLastC
}
bool Pitch::isValidPitch()
{
    if (Pitch::Midi >= 0 && Pitch::Midi <= 127)
    {
        return true;
    }
    else
    {
        return false;
    }
}
int Pitch::getOctave()
{
    return Pitch::Octave;
}
int Pitch::getDistanceFromLastC()
{
    return Pitch::DistanceFromLastC;
}
std::string Pitch::getLetterName(Accidental type)
{
    if (type == flat)
    {
        return Pitch::LetterFlat;
    }
    else// if (type == sharp)
    {
        return Pitch::LetterSharp;
    }
}
std::string Pitch::getLetterNameWithOctave(Accidental type)
{
    if (type == flat)
    {
        return Pitch::LetterFlat + "-" + std::to_string(Pitch::getOctave());
    }
    else// if (type == sharp)
    {
        return Pitch::LetterSharp + "-" + std::to_string(Pitch::getOctave());
    }
}
float Pitch::getAngleDeg(circle type)
{
    float angle;
    if (type == circle::circleOfSemitones)
    {
         angle = this->getDistanceFromLastC()*360.0/12.0;
    }
    else if (type == circle::circleOfFifths)
    {
         angle = fmod(this->getDistanceFromLastC()*7*360.0/12.0, 360.0); // fmod = modulus with floats
    }
    else
    {
        angle = 0;
    }
    return angle;
}
void Pitch::setTrack(int track)
{
    this->MidiTrack = track;
}


// Chord:
Chord::Chord() // nothing in the constructor
{

}
void Chord::insertPitch(Pitch pitch)
{
    this->Pitches.insert(pitch);
    //chord::calculateName();
}
std::set<Pitch> Chord::getPitches()
{
    return this->Pitches;
}
std::string Chord::getPitchesStr(bool accidentalSharp, bool *tracks, bool includeUnsetTracks)
{
    std::string pitchesStr = "Pitches:";
    for (std::set<Pitch>::iterator pt = Chord::Pitches.begin(); pt!=Chord::Pitches.end(); ++pt)
    {
        Pitch p = *pt;
        if (p.getMidiTrack() == -1 && includeUnsetTracks == true)
        {
            if (accidentalSharp)
                pitchesStr = pitchesStr + " " + p.getLetterNameWithOctave(Pitch::sharp);
            else
                pitchesStr = pitchesStr + " " + p.getLetterNameWithOctave(Pitch::flat);
        }
        else if (tracks[p.getMidiTrack()] == true)
        {
            if (accidentalSharp)
                pitchesStr = pitchesStr + " " + p.getLetterNameWithOctave(Pitch::sharp);
            else
                pitchesStr = pitchesStr + " " + p.getLetterNameWithOctave(Pitch::flat);
        }
    }
    return pitchesStr;
}
std::string Chord::getName()
{
    return Chord::Name;
}
void Chord::calculateName() // here it's where "the stick sings" - we have to use Python's library music21 or find another solution
{

}
std::set<float> Chord::getAnglesDeg(circle type, bool *tracks, bool includeUnsetTracks) // idea: implement other temperaments
{
    std::set<float> angles;

    for (std::set<Pitch>::iterator pt = Chord::Pitches.begin(); pt != Chord::Pitches.end(); ++pt)
    {
        Pitch p = *pt;
        if (p.getMidiTrack() == -1 && includeUnsetTracks == true)
        {
            Pitch::circle pCircType = (Pitch::circle)type;
            float angle = p.getAngleDeg(pCircType);
            angles.insert(angle);
        }
        else if (tracks[p.getMidiTrack()] == true)
        {
            Pitch::circle pCircType = (Pitch::circle)type;
            float angle = p.getAngleDeg(pCircType);
            angles.insert(angle);
        }
    }
    return angles;
}


// Chord with time:
ChordWithTime::ChordWithTime()
{

}

// Chords:
Chords::Chords() // nothing in the contructor
{

}

int Chords::process_chords(std::list <MidiNote> notes, bool includeTrackInfo = true) // basically the main loop, but only to run python and get the list of chords
{
    for (std::list<MidiNote>::iterator it=notes.begin() ; it != notes.end(); ++it) // Run the list forwards
    {
        //bool in_track = (std::find(tracks.begin(), tracks.end(), (*it).track) != tracks.end()); // check if note is in the list of tracks to be considered
        //if ((*it).is_note == 1 && in_track )// && aw.startMidiTime -50 < (*it).t_off && aw.endMidiTime + 50 > (*it).t_on) // is_note checks if it's a real note to avoid getting trash.
        //std::vector<bool> vtracks; vtracks.reserve(tracks.size()); vtracks.insert(vtracks.end(), tracks.begin(), tracks.end());
        if ((*it).is_note == 1)
        {
            Chords::Start_end_times.insert((*it).t_on); // every time a note begins, we have a new chord
            Chords::Start_end_times.insert((*it).t_off);
        }
    }

    // now, let's run through the start_end_times and check what notes are being played ;)
    // first, let's convert it to a vector (or list) and sort it:
    Chords::VStart_end_times.assign(Chords::Start_end_times.begin(), Chords::Start_end_times.end()); // converting to vector
    std::sort(Chords::VStart_end_times.begin(), Chords::VStart_end_times.end()); // sorting
    // for (std::set<int>::iterator it=chords::Start_end_times.begin(); it != chords::Start_end_times.end(); ++it) // for list, just replace set by list
    for (std::vector<long>::iterator start_end_time=std::begin(Chords::VStart_end_times); start_end_time != std::end(Chords::VStart_end_times); ++start_end_time)
    {
        ChordWithTime current_chord; // create a current chord
        for (std::list<MidiNote>::iterator it=notes.begin() ; it != notes.end(); ++it) // Run the list forwards
        {
            //bool in_track = (std::find(tracks.begin(), tracks.end(), (*it).track) != tracks.end()); // check if note is in the list of tracks to be considered
            //if ((*it).is_note == 1 && in_track )// && aw.startMidiTime -50 < (*it).t_off && aw.endMidiTime + 50 > (*it).t_on) // is_note checks if it's a real note to avoid getting trash.
            //std::vector<bool> vtracks; vtracks.reserve(tracks.size()); vtracks.insert(vtracks.end(), tracks.begin(), tracks.end());
            if ((*it).is_note == 1)
            {
                if ((*it).t_on <= *start_end_time && (*it).t_off > *start_end_time) // check if the current note pointed by it is in the chord
                {
                    //current_chord.insertPitch((*it).pitch); // insert pitch in the current chord
                    Pitch p = Pitch((*it).pitch);
                    if (includeTrackInfo == true)
                    {
                        p.setTrack((*it).track);
                    }
                    current_chord.chord.insertPitch(p);
                    current_chord.Start_time = *(1+start_end_time); // actually it corresponds to the next start_end_time, so summing 1 to the pointer
                }
            }
        }
        Chords::ChordsWTime.push_front(current_chord); // add the current chord to the list of chords
    }
    return 0; // success exit code
}
