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

#include "playthread.h"

#include <iostream>


PlayThread::PlayThread(QObject *parent) :
    QThread(parent)
{
    this->fps = 30;
    this->isReadyToDrawFrame = true;
}

PlayThread::PlayThread(QObject *parent, float fps) :
    QThread(parent)
{
    this->fps = fps;
    this->isReadyToDrawFrame = true;
}

QMutex PlayThread::mutex;

void PlayThread::run()
{
    //std::cout << "run!";
    for(int i = 0; true; i++)
    {

        QMutexLocker lock(&mutex);
        if (this->stop)
            break;

        if (isReadyToDrawFrame)
        {
            emit NumberChanged(i);

            this->msleep(1000/fps);
        }

        //this->wait();
    }
}
