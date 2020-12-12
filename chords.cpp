#include "chords.h"
#include <python_caller.h>


chord::chord() // nothing in the constructor
{

}
void chord::insertPitch(int pitch)
{
    chord::Pitches.insert(pitch);
    chord::calculateName();
}
std::set<int> chord::getPitches()
{
    return chord::Pitches;
}
std::string chord::getName()
{
    return chord::Name;
}
void chord::calculateName() // here it's where "the stick sings" - we have to use Python's library music21
{
    python_caller pyCaller;
    char * argv[] = {(char*)"chordNames", (char*)"getChordName"};
    pyCaller.callFuncFromModule(2, argv);
}


chords::chords() // nothing in the contructor
{

}

int chords::process_chords(std::list <MidiNote> notes, bool* tracks) // basically the main loop, but only to run python and get the list of chords
{
    for (std::list<MidiNote>::iterator it=notes.begin() ; it != notes.end(); ++it) // Run the list forwards
    {
        //bool in_track = (std::find(tracks.begin(), tracks.end(), (*it).track) != tracks.end()); // check if note is in the list of tracks to be considered
        //if ((*it).is_note == 1 && in_track )// && startMidiTime -50 < (*it).t_off && endMidiTime + 50 > (*it).t_on) // is_note checks if it's a real note to avoid getting trash.
        //std::vector<bool> vtracks; vtracks.reserve(tracks.size()); vtracks.insert(vtracks.end(), tracks.begin(), tracks.end());
        if ((*it).is_note == 1 && tracks[(*it).track] == true)
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
        chord current_chord; // create a current chord
        for (std::list<MidiNote>::iterator it=notes.begin() ; it != notes.end(); ++it) // Run the list forwards
        {
            //bool in_track = (std::find(tracks.begin(), tracks.end(), (*it).track) != tracks.end()); // check if note is in the list of tracks to be considered
            //if ((*it).is_note == 1 && in_track )// && startMidiTime -50 < (*it).t_off && endMidiTime + 50 > (*it).t_on) // is_note checks if it's a real note to avoid getting trash.
            //std::vector<bool> vtracks; vtracks.reserve(tracks.size()); vtracks.insert(vtracks.end(), tracks.begin(), tracks.end());
            if ((*it).is_note == 1 && tracks[(*it).track] == true)
            {
                if ((*it).t_on <= *start_end_time && (*it).t_off > *start_end_time) // check if the current note pointed by it is in the chord
                {
                    current_chord.insertPitch((*it).pitch); // insert pitch in the current chord
                }
            }
        }
        chords::Chords.push_front(current_chord); // add the current chord to the list of chords
    }
    return 0; // success exit code
}
