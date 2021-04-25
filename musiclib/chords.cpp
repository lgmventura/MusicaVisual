#include "chords.h"
#include <iostream> // only for debugging


const std::string pitchDict::LettersFlat[12]  =        {"C", "Db", "D", "Eb", "E", "F", "Gb", "G", "Ab", "A", "Bb", "B"};
const std::string pitchDict::LettersSharp[12] =        {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};
const std::string pitchDict::GermanicNamesFlat[12]  =  {"C", "Des", "D", "Es", "E", "F", "Ges", "G", "Aes", "A", "B", "H"};
const std::string pitchDict::GermanicNamesSharp[12] =  {"C", "Cis", "D", "Dis", "E", "F", "Fis", "G", "Gis", "A", "Ais", "B"};
const std::string pitchDict::SolfeggioNamesFlat[12]  = {"Do", "Re bemol", "Re", "Mi bemol", "Mi", "Fa", "Sol bemol", "Sol", "La bemol", "La", "Si bemol", "Si"};
const std::string pitchDict::SolfeggioNamesSharp[12] = {"Do", "Do sustenido", "Re", "Re sustenido", "Mi", "Fa", "Fa sustenido", "Sol", "Sol sustenido", "La", "La sustenido", "Si"};



pitch::pitch(int midi)
{
    pitch::Midi = midi;
    pitch::Octave = midi/12;
    pitch::DistanceFromLastC = midi%12;
    pitch::LetterFlat = pitchDict::LettersFlat[midi%12];
    pitch::LetterSharp = pitchDict::LettersSharp[midi%12];
    pitch::GermanicFlat = pitchDict::GermanicNamesFlat[midi%12];
    pitch::GermanicSharp = pitchDict::GermanicNamesSharp[midi%12];
    pitch::SolfeggioFlat = pitchDict::SolfeggioNamesFlat[midi%12];
    pitch::SolfeggioSharp = pitchDict::SolfeggioNamesSharp[midi%12]; // pitch::DistanceFromLastC
}
bool pitch::isValidPitch()
{
    if (pitch::Midi >= 0 && pitch::Midi <= 127)
    {
        return true;
    }
    else
    {
        return false;
    }
}
int pitch::getOctave()
{
    return pitch::Octave;
}
int pitch::getDistanceFromLastC()
{
    return pitch::DistanceFromLastC;
}
std::string pitch::getLetterName(Accidental type)
{
    if (type == flat)
    {
        return pitch::LetterFlat;
    }
    else// if (type == sharp)
    {
        return pitch::LetterSharp;
    }
}
std::string pitch::getLetterNameWithOctave(Accidental type)
{
    if (type == flat)
    {
        return pitch::LetterFlat + "-" + std::to_string(pitch::getOctave());
    }
    else// if (type == sharp)
    {
        return pitch::LetterSharp + "-" + std::to_string(pitch::getOctave());
    }
}
int pitch::getMidiTrack()
{
    return this->MidiTrack;
}
void pitch::setTrack(int track)
{
    this->MidiTrack = track;
}


// Chord:
chord::chord() // nothing in the constructor
{

}
void chord::insertPitch(pitch pitch)
{
    chord::Pitches.insert(pitch);
    //chord::calculateName();
}
std::set<pitch> chord::getPitches()
{
    return chord::Pitches;
}
std::string chord::getPitchesStr(bool accidentalSharp, bool *tracks, bool includeUnsetTracks)
{
    std::string pitchesStr = "Pitches:";
    for (std::set<pitch>::iterator pt = chord::Pitches.begin(); pt!=chord::Pitches.end(); ++pt)
    {
        pitch p = *pt;
        if (p.getMidiTrack() == -1 && includeUnsetTracks == true)
        {
            if (accidentalSharp)
                pitchesStr = pitchesStr + " " + p.getLetterNameWithOctave(pitch::sharp);
            else
                pitchesStr = pitchesStr + " " + p.getLetterNameWithOctave(pitch::flat);
        }
        else if (tracks[p.getMidiTrack()] == true)
        {
            if (accidentalSharp)
                pitchesStr = pitchesStr + " " + p.getLetterNameWithOctave(pitch::sharp);
            else
                pitchesStr = pitchesStr + " " + p.getLetterNameWithOctave(pitch::flat);
        }
    }
    return pitchesStr;
}
std::string chord::getName()
{
    return chord::Name;
}
void chord::calculateName() // here it's where "the stick sings" - we have to use Python's library music21 or find another solution
{

}
std::set<float> chord::getAnglesDeg(circle type, bool *tracks, bool includeUnsetTracks) // idea: implement other temperaments
{
    std::set<float> angles;
    if (type == circle::circleOfSemitones)
    {
        for (std::set<pitch>::iterator pt = chord::Pitches.begin(); pt != chord::Pitches.end(); ++pt)
        {
            pitch p = *pt;
            if (p.getMidiTrack() == -1 && includeUnsetTracks == true)
            {
                float angle = p.getDistanceFromLastC()*360.0/12.0;
                angles.insert(angle);
            }
            else if (tracks[p.getMidiTrack()] == true)
            {
                float angle = p.getDistanceFromLastC()*360.0/12.0;
                angles.insert(angle);
            }
        }
    }
    else if (type == circle::circleOfFifths)
    {
        for (std::set<pitch>::iterator pt = chord::Pitches.begin(); pt != chord::Pitches.end(); ++pt)
        {
            pitch p = *pt;
            if (p.getMidiTrack() == -1 && includeUnsetTracks == true)
            {
                float angle = fmod(p.getDistanceFromLastC()*7*360.0/12.0, 360.0); // fmod = modulus with floats
                angles.insert(angle);
            }
            else if (tracks[p.getMidiTrack()] == true)
            {
                float angle = fmod(p.getDistanceFromLastC()*7*360.0/12.0, 360.0); // fmod = modulus with floats
                angles.insert(angle);
            }
        }
    }
    return angles;
}


// Chord with time:
chordWithTime::chordWithTime()
{

}

// Chords:
chords::chords() // nothing in the contructor
{

}

int chords::process_chords(std::list <MidiNote> notes, bool includeTrackInfo = true) // basically the main loop, but only to run python and get the list of chords
{
    for (std::list<MidiNote>::iterator it=notes.begin() ; it != notes.end(); ++it) // Run the list forwards
    {
        //bool in_track = (std::find(tracks.begin(), tracks.end(), (*it).track) != tracks.end()); // check if note is in the list of tracks to be considered
        //if ((*it).is_note == 1 && in_track )// && startMidiTime -50 < (*it).t_off && endMidiTime + 50 > (*it).t_on) // is_note checks if it's a real note to avoid getting trash.
        //std::vector<bool> vtracks; vtracks.reserve(tracks.size()); vtracks.insert(vtracks.end(), tracks.begin(), tracks.end());
        if ((*it).is_note == 1)
        {
            chords::Start_end_times.insert((*it).t_on); // every time a note begins, we have a new chord
            chords::Start_end_times.insert((*it).t_off);
        }
    }

    // now, let's run through the start_end_times and check what notes are being played ;)
    // first, let's convert it to a vector (or list) and sort it:
    chords::VStart_end_times.assign(chords::Start_end_times.begin(), chords::Start_end_times.end()); // converting to vector
    std::sort(chords::VStart_end_times.begin(), chords::VStart_end_times.end()); // sorting
    // for (std::set<int>::iterator it=chords::Start_end_times.begin(); it != chords::Start_end_times.end(); ++it) // for list, just replace set by list
    for (std::vector<long>::iterator start_end_time=std::begin(chords::VStart_end_times); start_end_time != std::end(chords::VStart_end_times); ++start_end_time)
    {
        chordWithTime current_chord; // create a current chord
        for (std::list<MidiNote>::iterator it=notes.begin() ; it != notes.end(); ++it) // Run the list forwards
        {
            //bool in_track = (std::find(tracks.begin(), tracks.end(), (*it).track) != tracks.end()); // check if note is in the list of tracks to be considered
            //if ((*it).is_note == 1 && in_track )// && startMidiTime -50 < (*it).t_off && endMidiTime + 50 > (*it).t_on) // is_note checks if it's a real note to avoid getting trash.
            //std::vector<bool> vtracks; vtracks.reserve(tracks.size()); vtracks.insert(vtracks.end(), tracks.begin(), tracks.end());
            if ((*it).is_note == 1)
            {
                if ((*it).t_on <= *start_end_time && (*it).t_off > *start_end_time) // check if the current note pointed by it is in the chord
                {
                    //current_chord.insertPitch((*it).pitch); // insert pitch in the current chord
                    pitch p = pitch((*it).pitch);
                    if (includeTrackInfo == true)
                    {
                        p.setTrack((*it).track);
                    }
                    current_chord.Chord.insertPitch(p);
                    current_chord.Start_time = *(1+start_end_time); // actually it corresponds to the next start_end_time, so summing 1 to the pointer
                }
            }
        }
        chords::Chords.push_front(current_chord); // add the current chord to the list of chords
    }
    return 0; // success exit code
}
