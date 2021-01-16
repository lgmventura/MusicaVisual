/*
 *  This software takes a midi file and creates animations from its notes.
    Copyright (C) 2016  Luiz Guilherme de Medeiros Ventura, Belo Horizonte, Brazil

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Very, very, very helpful on working with midi messages: http://midifile.sapp.org/class/MidiMessage/
 * Thanks to Craiggsappmidi (sapp.org) for the library used here.
 * Thanks to OpenCV library used as well to work with images and videos.
 */

#ifndef MIDIMESSAGES_H
#define MIDIMESSAGES_H


// Classes for storing midi messages
class MidiMessages
{
public:
    long t_on;
    unsigned int id;
    MidiMessages();
};

class MidiNote: public MidiMessages
{
public:
    unsigned int track;
    long t_off;
    unsigned int vel;
    unsigned int pitch;
    unsigned int t_middle;// = t_on + t_off;
    unsigned int duration;
    int is_note; // 1 for yes. This is just to avoid trash.
    MidiNote();
};

class TempoChange: public MidiMessages
{
public:
    unsigned long new_tempo;
    TempoChange();
};

class TimeSignature: public MidiMessages
{
public:
    int numerator;
    int denominator;
    TimeSignature();
};

//
#endif // MIDIMESSAGES_H
