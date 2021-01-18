#include "musicdata.h"

// Constructor method:
MusicData::MusicData()
{

}


// Other methods:
void MusicData::nameTracksReset()
{
    this->TrackNames.clear();
    for (int i = 1; i <= 24; i++)
        this->TrackNames.push_back("Track " + std::to_string(i));
}
