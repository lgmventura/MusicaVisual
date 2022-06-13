#include "musicdata.h"

// Constructor method:
MusicData::MusicData()
{
    // initialising basic stuff that can be used before processing:
    TrackNames.push_back("Track 1");
}


// Other methods:
void MusicData::nameTracksReset()
{
    this->TrackNames.clear();
    for (int i = 1; i <= 128; i++)
        this->TrackNames.push_back("Track " + std::to_string(i));
}

void MusicData::processMidiString(string midiMessagesText) // legacy, still usefull for debug. ToDo: write new function, direct from import
{
    unsigned int delta, track;
    unsigned int id = 0;
    unsigned long time = 0;
    int messg;
    // reset track names, since we are processing a new midi file, to avoid garbage (old names, if you load a new midi file):
    this->nameTracksReset();

    this->Notes.clear(); // Clear notes list before adding new elements.
    this->Tempos.clear(); // Clear tempos list before adding new elements.
    this->NTracks = 1;
    this->PitchMax = 1;
//    this->PitchMin = 127;

    string str;
    stringstream midiMTSS;
    midiMTSS.str(midiMessagesText);
    for (int i = 0; getline(midiMTSS, str, '\t'); i++) // this loops horizontally and vertically through substrings. Tabs and new-lines will be a new get line, look at '\t'
    {
//        int time, delta, track;
//        int messg;
        if (i == 0)
        {
            this->Tpq = stoi(str.substr(5)); // getting the tpq (ticks per quarter-note) on the first line
        }

        string messg_str;
        if (i >= 5 && i%4 == 0) // first column
        {
            time = stoi(str, nullptr, 10); // getting the absolute time in ticks
        }
        if (i >= 5 && i%4 == 1) // second column
        {
            delta = stoi(str, nullptr, 10); // getting the delta time in ticks (relative time from last message)
        }
        if (i >= 5 && i%4 == 2) // third column
        {
            track = stoi(str, nullptr, 10); // getting the track number
        }
        if (i >= 5 && i%4 == 3) // fourth column - the hexadecimal midi message itself
        {
            messg = stoi(str, nullptr, 16); // getting the midi message type
            messg_str = str; // getting the midi message string
            //ui->plainTextEdit->appendPlainText(QString::fromStdString(messg_str)); // this line is only for verification
        }
        //ui->plainTextEdit->appendPlainText(QString::fromStdString(str)); // this line is only for verification
        //ui->plainTextEdit->appendPlainText(QString::number(i)); // this line is only for verification

        // Save notes in notes list:
        if (i >= 5 && i%4 == 3) // If the current element of the plain text is a midi message
        {

            if (messg_str[0] == '9' && stoi(messg_str.substr(6,2), nullptr, 16) > 0) // Condition for Note on is messg_str[0] == '9' and velocity > 0
            {
                MidiNote tempnote; // Create an object MidiNote
                tempnote.t_on = time;
                tempnote.pitch = stoi(messg_str.substr(3,2), nullptr, 16); // get the pitch from the midi message string
                tempnote.track = track;
                tempnote.channel = stoi(messg_str.substr(1,1), nullptr, 16); // channel is the second byte in hex
                tempnote.vel = stoi(messg_str.substr(6,2), nullptr, 16); // get velocity from the midi message string
                tempnote.id = id;
                id++;
                tempnote.is_note = false; // t_off still not found
                this->Notes.push_back(tempnote); // Insert the tempnote into the list "notes"
                if (tempnote.pitch > this->PitchMax)
                {
                        this->PitchMax = tempnote.pitch;
                        //cout << pitch_max << endl;
                }
                if (tempnote.pitch < this->PitchMin)
                        this->PitchMin = tempnote.pitch;
                if (tempnote.track > this->NTracks) // if there we see a track whose number is higher than the current number of tracks, we do number of tracks (aka tracks_count) = current track number
                        this->NTracks = tempnote.track;
            }
            else if ((messg_str[0] == '9' && stoi(messg_str.substr(6,2), nullptr, 16) == 0) || messg_str[0] == '8') // Condition for Note off is messg_str[0] == '9' and velocity == 0 or messg_str[0] == '8'
            {
                for (std::list<MidiNote>::iterator it=this->Notes.end() ; it != this->Notes.begin(); ) // note_off found: run the list backwards
                {
                    --it; // The pointer regress one position at the beginning, since the same message won't be on and off together.
                    if ((*it).track == track && (*it).pitch == (unsigned int)stoi(messg_str.substr(3,2), nullptr, 16)) // find the first note in the list with the same pitch and in the same track
                    {
                        (*it).t_off = time; // set its note_off time (the current time, since it has just found a note_off)
                        (*it).t_middle = (time + (*it).t_on)/2; // set its middle time ((t_on + t_off)/2)
                        (*it).duration = time - (*it).t_on; // Sets its duration
                        (*it).is_note = 1; // This is a real note!
                        break; // break this loop since the corresponding note_on to this current note_off was found!
                    }

                }
            }
            else if (messg_str[0] == 'f' && messg_str[1] == 'f' && messg_str.size() >= 9) // Checks if it's a meta-message
            {
                // meta message is ill-formed.
                // meta messages must have at least three bytes:
                //    0: 0xff == meta message marker
                //    1: meta message type
                //    2: meta message data bytes to follow
                if (messg_str.substr(3,2) == "51" && messg_str.size() == 18) // Checks if it is a tempo change message
                {
                    // Meta tempo message can only be 6 bytes long.      ^ 18 is the string size: 6bytes*2hexDigits + 6 spaces (one at the end of each)
                    // The new_tempo calculated is given in microseconds per quarter note.
                    //ui->plainTextEdit->appendPlainText("MetaTempoMessage!\n"); // Line just for checking!
                    TempoChange temp_tempo_change; // Create a temporary object for tempo change
                    //temp_tempo_change.new_tempo = ((stoi(messg_str.substr(6,2), nullptr, 16) << 16) + (stoi(messg_str.substr(9,2), nullptr, 16) << 8) + stoi(messg_str.substr(12,2), nullptr, 16)); // equivalent to: return ((*this)[3] << 16) + ((*this)[4] << 8) + (*this)[5];
                    temp_tempo_change.new_tempo = ((stol(messg_str.substr(9,2), nullptr, 16) << 16) + (stol(messg_str.substr(12,2), nullptr, 16) << 8) + stol(messg_str.substr(15,2), nullptr, 16)); // equivalent to: return ((*this)[3] << 16) + ((*this)[4] << 8) + (*this)[5];
                    temp_tempo_change.t_on = time;
                    //cout << temp_tempo_change.new_tempo << "\n";
                    this->Tempos.push_back(temp_tempo_change); // Stores the tempo change in the list
                }
                // reading track names - Changing track names for easing instrument finding
                else if (messg_str.substr(3,2) == "3 ")// && messg_str.size() > 4) // Check if it is a track name meta message. I don't know its size, I put 4 bytes to test.
                {
                    std::string t_name;
                    std::string straux = messg_str.substr(9);
                    straux.erase(remove_if(straux.begin(), straux.end(), ::isspace), straux.end()); // remove_if is declared in <algorithm>
                    hex2ascii(straux, t_name);
                    if (track <= 128) // the maximum number of tracks is 128
                    {
                        this->TrackNames.at(track) = t_name; // store the track names when the button Process is pressed.
                    }
                }
                else if (messg_str.substr(3,5) == "58 04" || messg_str.substr(3,4) == "58 4") // time signature messages start with this substring "FF 58 04"
                {
                    TimeSignature temp_tsig;
                    temp_tsig.numerator = stoi(messg_str.substr(9,2), nullptr, 16);
                    int denominator_exponent = stoi(messg_str.substr(12,2), nullptr, 16);
                    temp_tsig.denominator = std::pow(2, denominator_exponent);
                    temp_tsig.t_on = time;
                    this->TSignatures.push_back(temp_tsig);
                }
            }

        }
    }
    this->NTracks = this->NTracks + 1; // This defines the final number of tracks detected in the processed midi. It is the highest track + 1, since the tracks are numbered from 0 to n-1.

    this->TotalTime = time; // set the current time after processing as global variable total time from this midi processing.
}

string MusicData::squeezeTracksMidiStr(string midiMessages)
{
    std::stringstream stream; // for the whole text in the edit area
    std::stringstream streamCopy; // copy because the orig. will be exhausted using getline
    stream.str(midiMessages);
    streamCopy.str(midiMessages);
    string str; // one line

    std::vector <unsigned int> tracks_list;

    std::set <unsigned short> tracks_set;
    std::set<unsigned short>::iterator it;
    //std::pair<std::set<unsigned short>::iterator,bool> ret;

    unsigned short tnum_temp;
    std::string messg_str;
    //std::map <unsigned short, unsigned short> tracks_association;
    for (int i = 0; getline(stream,str,'\t'); i++)
    {
        if (i >= 5 && i%4 == 2) // position (column) of a track number in the plain text
        {
            tnum_temp = stoi(str, nullptr, 10);
        }
        if (i >= 5 && i%4 == 3) // getting midi message
        {
            //messg = stoi(str, nullptr, 16); // getting the midi message type
            messg_str = str; // getting the midi message string
            //ui->plainTextEdit->appendPlainText(QString::fromStdString(messg_str)); // this line is only for verification
        }
        if (messg_str[0] == '9' && stoi(messg_str.substr(6,2), nullptr, 16) > 0) // checking midi message. Consider the track not empty if there is at least one note_on message.
        {
            tracks_list.push_back(tnum_temp); // getting the track number and inserting into the list
            //ret = tracks_set.insert(tnum_temp); // getting the track number and inserting into the set. The set is like its mathematical definition, so it doesn't get repeated items
            tracks_set.insert(tnum_temp);
            //if (ret.second==false) // if no element was inserted into the set because the element is already there
            //    it=ret.first; // to improve inserting efficiency, see http://www.cplusplus.com/reference/set/set/insert/
        }
    }
    std::stringstream stream2;
    std::string str2;

    for (int i = 0; getline(streamCopy,str,'\t'); i++)
    {
        if (i < 5) // for the first lines
            str2.append(str); // nothing changes
        else if (i >= 5 && i%4 != 2) // for the following changes, if it is not in the track column,
            str2.append(str); // nothing changes
        else //if (i >= 5 && i%4 == 2) // else
        {
            it = tracks_set.find(stoi(str, nullptr, 10)); // find position of the track in the new set containing only non-empty tracks
//            if (it != tracks_set.end()) {
                str2.append(std::to_string(std::distance(tracks_set.begin(), it))); // insert it to the auxiliar string str2
//            }
//            else {
//                str2.append("-1");
//            }

        }
        stream2 << str2;
        stream2 << '\t';

        str2.clear();
    }

    return stream2.str();
}

string MusicData::splitChannels2Tracks(string midiMessages)
{
    std::stringstream stream; // for the whole text in the edit area
    std::stringstream streamCopy; // copy because the orig. will be exhausted using getline
    stream.str(midiMessages);
    streamCopy.str(midiMessages);
    string str; // one line

    unsigned int tnum; // track number
    unsigned int chan; // channel number

    std::string messg_str;
    std::stringstream stream2; // output stream
    std::string str2; // output string (only one chunks between two tabs (e.g. time, delta, track or midi_message) -> will be dropped into stream2 for each of them)

    // the first thing is to create an associative map between old and new track numbers. All midi messages have a track associated with (see the track column), but not all of them have
    // a channel. The midi messages that have a channel are only those starting with 8, 9, A, B, C, D or E. Then, the channel will be the second hex char. For instance, a message starting
    // with 93 is a note_on (or off if vel == 0) in channel 3. So, if we have notes in track 4 on channel 6 and 7 only, then, we only want the track 4 to be split into 2 tracks, one
    // for channel 6 and one for channel 7. If track 3 has notes only in channel 5, we do not want to split channel 3. So, we need to run through, check for all messages with a channel
    // and map them into a new track. Since there are 128 possible tracks and 16 possible channels, we can have a unique relationship by doing new_track = 16*old_track + channel.
    // we can only do this for messages with a channel. For those without a channel, we still want to map them into the channel, otherwise we have new_track = 16*old_track, which means
    // we are creating a new track only for those messages, resulting in a track without notes but with auxiliar or meta messages.
    // So, we first run through everything to map and run a second time to put the messages with channel into their unique location (16*t + c) and the ones without a channel into a location
    // of one with channel (16*t + c_from_another_message_from_same_track).
    // At the end, we can squeeze to get rid of empty tracks and we'll stay only with messages that have the lowest channel value among all messages within the same track, on its original track,
    // and messages with a higher channel value on a new track following its original track. For messages without a channel, they will end up on their original tracks, which corresponds to
    // the lowest channel.
    std::unordered_map<unsigned int, unsigned int> track_old_new;

    bool tab;

    for (int i = 0; getline(stream, str, '\t'); i++) // first run to map tnum (old_track) to tnum*16 + chan. Not writing anything yet.
    {
        if (i >= 5 && i%4 == 2) // position (column) of a track number in the plain text (index 2)
        {
            tnum = stoi(str, nullptr, 10);
        }
        else if (i >= 5 && i%4 == 3) // getting the midi message, which is the 4th column (index 3)
        {
            messg_str = str; // getting the midi message string

            if ((messg_str[0] == '9' || messg_str[0] == '8' || messg_str[0] == 'A' || messg_str[0] == 'B' || messg_str[0] == 'C' || messg_str[0] == 'D' || messg_str[0] == 'E')) // these are all the first bytes of a midi message that have a channel associated with
            {
                chan = stoi(messg_str.substr(1,1), nullptr, 16); // read out channel, which is the second byte (position 1, length 1) and transform to int (from base 16, hex)
                track_old_new.insert(std::pair<unsigned int, unsigned int>(tnum, tnum*16 + chan)); // map old track with new provisory track tnum*16 + chan (unique track number, since chan can vary within the range 0 to 15)
            }
        }
    }
    for (int i = 0; getline(streamCopy, str, '\t'); i++) // run through everything again, now to rewrite
    {
        tab = true; // insert a tab or not, to avoid double tabs
        if (i < 5) // for the first lines
            str2.append(str); // nothing changes
        else if (i >= 5 && i%4 == 2) // position (column) of a track number in the plain text
        {
            tnum = stoi(str, nullptr, 10); // get old track number
            tab = false; // we aren't writing anything yet into the final string, so in this case, we have to skip append("\t") so we don't end up with 2 consecutive tabs
        }
        else if (i >= 5 && i%4 == 3) // getting midi message
        {
            messg_str = str; // getting the midi message string

            if ((messg_str[0] == '9' || messg_str[0] == '8' || messg_str[0] == 'A' || messg_str[0] == 'B' || messg_str[0] == 'C' || messg_str[0] == 'D' || messg_str[0] == 'E')) // these are all the first bytes of a midi message that have a channel associated with
            {
                chan = stoi(messg_str.substr(1,1), nullptr, 16); // read out channel, which is the second byte (position 1, length 1) and transform to int (from base 16, hex)
                str2.append(std::to_string(tnum*16 + chan)); // write new provisory track number on the string, which is 16*tnum + chan (chan can vary between 0 and 15). The squeeze later will transform the provisory into the final track number.
            }
            else // any other message which doesn't have a channel associated
            {
                str2.append(std::to_string(track_old_new[tnum])); // insert first track occurence from the map, so these messages still end up in an existing track from the new ones, which are defined by 16*tnum + chan.
            }
            str2.append("\t"); // we appended the (new) track number only now after checking them from message or from map. We need a tabulator before appending the message itself.
            str2.append(messg_str); // append the entire message
        }
        else // no header, no track number, no message. Is either time or delta
        {
            str2.append(str); // just append, nothing changes
        }
        stream2 << str2; // put chunk str2 into stream2, which is the whole text
        if (tab == true) {
            stream2 << '\t';
        }

        str2.clear(); // already in stream2. Clearing to get the next one.
    }

    // now we can squeeze to remove empty tracks, since 16*tnum + chan will generate a lot of empty tracks,
    // since normally only one or few channels are used per track:
    return squeezeTracksMidiStr(stream2.str());
}
