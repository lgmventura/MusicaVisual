#include "importmidi.h"

ImportMidi::ImportMidi()
{

}

// other member functions:
void ImportMidi::importMidiMessagesText(const char *midiFileName) // useful for debugging
{
    MidiFile midifile;
    midifile.read(midiFileName);

    std::stringstream buffer;
    buffer << "TPQ: " << midifile.getTicksPerQuarterNote() << endl; // Get the Ticks Per QarterNote (TPQ), necessary for setting the tempo
    buffer << "TRACKS: " << midifile.getTrackCount() << endl;

    midifile.joinTracks();

    buffer << "TICK    DELTA   TRACK   MIDI MESSAGE\n";
    buffer << "____________________________________\n";

    MidiEvent* mev;
    int deltatick;
    for (int iEvent=0; iEvent < midifile[0].size(); iEvent++) {
        mev = &midifile[0][iEvent];
        if (iEvent == 0) {
            deltatick = mev->tick;
        } else {
            deltatick = mev->tick - midifile[0][iEvent-1].tick;
        }

        buffer << endl;
        buffer << dec << mev->tick;
        buffer << '\t' << deltatick;
        buffer << '\t' << mev->track;
        buffer << '\t' << hex;
        for (int i=0; i < mev->size(); i++) {
            buffer << (int)(*mev)[i] << ' ';
            if ((int)(*mev)[i] < 16) buffer << ' '; // this line adds a new space in case of (int) < 16 (or (hex) < 10) to ensure equal spacing between elements of messages.
        }
        buffer << '\t'; // Places a '\t' after each line.
        //          cout << endl;
        this->NEvents = iEvent; // number of iEvents in file opened. Calculated after file importing.
        this->MidiMessages = buffer.str();
    }
}
