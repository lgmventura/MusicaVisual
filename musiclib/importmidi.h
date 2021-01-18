#ifndef IMPORTMIDI_H
#define IMPORTMIDI_H

#include <sstream>
#include "MidiFile.h"

class ImportMidi
{
public:
    ImportMidi();

    // std::list(...) ToDo: list of messages with the respective timestamp
    std::string MidiMessages;
    int NEvents;

public:
    void importMidiMessagesText(const char *midiFileName);
};

#endif // IMPORTMIDI_H
