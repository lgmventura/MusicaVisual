#include "animation.h"

RenderP::RenderP()
{

}

void AnimPainter::blocks_paint(MusicData mdt, cv::Mat image, std::vector <cv::Mat> img_buffer_sep_tracks, int startMidiTime, int endMidiTime, int window_width, int window_height, TracksP tProp, RenderP rProp, VideoRecorder *vRec) // this function is called for every frame. startMidiTime is the time in the left side of the window, endMidiTime, at the right. These depend on playback position and zoom.
{
    int zoom = endMidiTime - startMidiTime;
    int curr_pos_middle = (startMidiTime + (zoom)/2);
    cv::Point pt1, pt2, pt3, pt4;//, pt5;// , pt6;
    cv::Point pt5[mdt.NTracks]; // for the interconnected lines. It has to be a vector of tracks_count length to make lines be connected only with notes within the same track.
    cv::Point pt1_prev[mdt.NTracks], pt2_prev[mdt.NTracks], pt3_prev[mdt.NTracks]; // these points are used for storing the previous respective points to make "moving notes" possible averaging current position/size with last position/size. There must be one for each track so that they are kept independent, i.e., moving notes in one track don't influence mov. notes in another (same idea as for interconn. lines).
    cv::Point ptos_prev[mdt.NTracks][4];
    int radius_prev[mdt.NTracks];// = 0;
    for (int i = 0; i < mdt.NTracks; i++) // start lines at zero. If not set, a rubish value may appear.
    {
        pt5[i].x = 0;
        pt5[i].y = 0;
    }
    float x1, x2, y1, y2, x3, y3;
    int x_max = window_width + 5000, y_max = window_height + 50;
    //int max_duration = window_width + 1000;
    //std::cout << "Paint blocks! " << Mdt->PitchMin << ' ' << pitch_max << endl;
    cv::Mat img_playing_notes = cv::Mat::zeros( window_height, window_width, CV_8UC3 );
    cv::Mat img_moving_notes = cv::Mat::zeros( window_height, window_width, CV_8UC3 );
    //cv::Mat img_buffer_sep_tracks[tracks_count] = cv::Mat::zeros( window_height, window_width, CV_8UC3 ); // Qt 5.7, OpenCV 2.4 (which uses C++98)
    //cv::Mat img_buffer_sep_tracks[tracks_count] = {cv::Mat::zeros( window_height, window_width, CV_8UC3 )}; // this does not solve for Qt 5.9, OpenCV 4.0 (which uses C++11)
    // http://answers.opencv.org/question/31665/creating-an-array-of-mats-of-size-n/ - Static arrays need the size for their construction at compile time. If you want to have the size controlled at run-time, you need to either create the mat-array via new or use std::vector (I'd prefer the latter one, since when using new you'll also need to call delete[] and it also prevents you from writing at non existent memory)
    //std::vector <cv::Mat> img_buffer_sep_tracks(tracks_count, cv::Mat::zeros( window_height, window_width, CV_8UC3 ));// this does not solve for Qt 5.9, OpenCV 4.0 (which uses C++11), see comments below
    // now, if you create a vector of cv::Mat objects, you are actually creating a vector of pointers to the SAME matrix. cv::Mat is not a matrix, but rather a pointer to one.
    // Solving this on the generation of the animation window
    cv::LineTypes lineType = rProp.shapeLineType;

    for (std::list<MidiNote>::iterator it=mdt.Notes.begin() ; it != mdt.Notes.end(); ++it) // Run the list forwards
    {
        if ((*it).is_note == 1 && startMidiTime -50 < (*it).t_off && endMidiTime + 50 > (*it).t_on) // is_note checks if it's a real note to avoid getting trash.
        {
            x1 = (float)window_width*((-(float)startMidiTime + (float)(*it).t_on)/((float)endMidiTime - (float)startMidiTime)); // note_on time
            x2 = (float)window_width*((-(float)startMidiTime + (float)(*it).t_off)/((float)endMidiTime - (float)startMidiTime)); // note_off time
            y1 = (float)window_height/2 - (float)window_height*((float)((*it).pitch - ((float)mdt.PitchMin + mdt.PitchMax)/2)/((float)mdt.PitchMax - (float)mdt.PitchMin))*rProp.vertRange/50.0 - rProp.vertShift;
            y2 = (float)window_height/2 - (float)window_height*((float)((*it).pitch + 1.0 - ((float)mdt.PitchMin + mdt.PitchMax)/2)/((float)mdt.PitchMax - (float)mdt.PitchMin))*rProp.vertRange/50.0 - rProp.vertShift;
            pt1.x = f2int_safe(x1); //window_width*((*it).t_on/(endMidiTime - startMidiTime));
            pt2.x = f2int_safe(x2); //window_width*((*it).t_off/(endMidiTime - startMidiTime));
            pt1.y = f2int_safe(y1); //window_height*((*it).pitch/(50));
            pt2.y = f2int_safe(y2);//window_height*(((*it).pitch + 1)/(50));
            //std::cout << (*it).t_on << ' ' << (*it).t_off << ": " << (*it).pitch << '\n';
            //std::cout << pt1.x << ' ' << pt1.y << ": " << pt2.x << ' ' << pt2.y << '\n';
            if (true) //(std::abs(pt1.x) < x_max && std::abs(pt1.y) < y_max && std::abs(pt2.x) < x_max && std::abs(pt2.y) < y_max);// && (*it).duration < max_duration) // This "if" is just to bypass an issue (certainly about big doubles being coverted to int), what causes random boxes appearing on the screen when one zooms in very close.
            {
                for (unsigned short tnum = 0; tnum < mdt.NTracks; tnum++)
                {
                    // -------------------------- Draw Interconnected Lines -------------------------
                    if ((*it).track == tnum && tProp.active[tnum] == true && tProp.interconnect[tnum] == 1) // All tracks - rectangle
                    {
                        x3 = (float)window_width*((-(float)startMidiTime + (float)(*it).t_middle)/((float)endMidiTime - (float)startMidiTime));
                        y3 = (float)window_height/2 - (float)window_height*((float)((*it).pitch + 0.5 - ((float)mdt.PitchMin + mdt.PitchMax)/2)/((float)mdt.PitchMax - (float)mdt.PitchMin))*rProp.vertRange/50.0 - rProp.vertShift;
                        pt3.x = f2int_safe(x3); // Center x
                        pt3.y = f2int_safe(y3); // Center y
                        if (abs(pt3.x - pt5[tnum].x) < tProp.max_connect_dist && pt5[tnum] != cv::Point(0,0))
                            if ( ! rProp.sep_render[1])
                                cv::line( image, pt3, pt5[tnum], {tProp.getCv(tnum,2)/4,  tProp.getCv(tnum,1)/4,  tProp.getCv(tnum,0)/4}, 1, lineType );
                            else
                                cv::line( img_buffer_sep_tracks[tnum], pt3, pt5[tnum], {tProp.getCv(tnum,2)/4,  tProp.getCv(tnum,1)/4,  tProp.getCv(tnum,0)/4}, 1, lineType );
                        pt5[tnum].x = f2int_safe(x3); // Center x of the previous note
                        pt5[tnum].y = f2int_safe(y3); // Center y of the previous note
                    }



                    // ------------------------- Draw Rectangles -------------------------
                    if ((*it).track == tnum && tProp.active[tnum] == true && tProp.shape[tnum] == 0) // All tracks - rectangle
                    {
                        if (pt1.x > window_width/2) // The note block is before (to the right of) the center line
                            if ( ! rProp.sep_render[1])
                                cv::rectangle( image, pt1, pt2, {tProp.getColour(tnum, (*it).pitch).b*(*it).vel*3/512, tProp.getColour(tnum, (*it).pitch).g*(*it).vel*3/512, tProp.getColour(tnum, (*it).pitch).r*(*it).vel*3/512}, -1, lineType );
                            else
                                cv::rectangle( img_buffer_sep_tracks[tnum], pt1, pt2, {tProp.getColour(tnum, (*it).pitch).b*(*it).vel*3/512, tProp.getColour(tnum, (*it).pitch).g*(*it).vel*3/512, tProp.getColour(tnum, (*it).pitch).r*(*it).vel*3/512}, -1, lineType );
                        else if (pt1.x <= window_width/2 && pt2.x > window_width/2) // The note block is inside the center line
                        {
                            if ( ! rProp.sep_render[0])
                                cv::rectangle( image, pt1, pt2, {tProp.getColour(tnum, (*it).pitch).b*(*it).vel/64, tProp.getColour(tnum, (*it).pitch).g*(*it).vel/64, tProp.getColour(tnum, (*it).pitch).r*(*it).vel/64}, -1, lineType );
                            else
                                cv::rectangle( img_playing_notes, pt1, pt2, {tProp.getColour(tnum, (*it).pitch).b*(*it).vel/64, tProp.getColour(tnum, (*it).pitch).g*(*it).vel/64, tProp.getColour(tnum, (*it).pitch).r*(*it).vel/64}, -1, lineType );
                        }
                        else // The note block is after (to the left of) the center line
                            if ( ! rProp.sep_render[1])
                                cv::rectangle( image, pt1, pt2, {tProp.getColour(tnum, (*it).pitch).b*(*it).vel/512, tProp.getColour(tnum, (*it).pitch).g*(*it).vel/512, tProp.getColour(tnum, (*it).pitch).r*(*it).vel/512}, 2, lineType );
                            else
                                cv::rectangle( img_buffer_sep_tracks[tnum], pt1, pt2, {tProp.getColour(tnum, (*it).pitch).b*(*it).vel/512, tProp.getColour(tnum, (*it).pitch).g*(*it).vel/512, tProp.getColour(tnum, (*it).pitch).r*(*it).vel/512}, 2, lineType );
                    }


                    // -------------------------- Draw Rhombus -------------------------
                    if ((*it).track == tnum && tProp.active[tnum] == true && tProp.shape[tnum] == 1) // All tracks - rectangle
                    {
                        x3 = (float)window_width*((-(float)startMidiTime + (float)(*it).t_middle)/((float)endMidiTime - (float)startMidiTime));
                        y3 = (float)window_height/2 - (float)window_height*((float)((*it).pitch + 0.5 - ((float)mdt.PitchMin + mdt.PitchMax)/2)/((float)mdt.PitchMax - (float)mdt.PitchMin))*rProp.vertRange/50.0 - rProp.vertShift;
                        pt1.y = f2int_safe(y3);
                        pt2.y = f2int_safe(y3);
                        pt3.x = f2int_safe(x3);
                        pt3.y = f2int_safe(y1);
                        pt4.x = f2int_safe(x3);
                        pt4.y = f2int_safe(y2);
                        cv::Point ptos[4];
                        ptos[0] = pt1; ptos[1] = pt3; ptos[2] = pt2; ptos[3] = pt4;
                        if (pt1.x > window_width/2) // The note block is before (to the right of) the center line
                            if ( ! rProp.sep_render[1])
                                //cv::fillPoly( image, pts, );
                                cv::fillConvexPoly( image, ptos, 4, {tProp.getColour(tnum, (*it).pitch).b*(*it).vel*3/512, tProp.getColour(tnum, (*it).pitch).g*(*it).vel*3/512, tProp.getColour(tnum, (*it).pitch).r*(*it).vel*3/512}, lineType );
                            else
                                cv::fillConvexPoly( img_buffer_sep_tracks[tnum], ptos, 4, {tProp.getColour(tnum, (*it).pitch).b*(*it).vel*3/512, tProp.getColour(tnum, (*it).pitch).g*(*it).vel*3/512, tProp.getColour(tnum, (*it).pitch).r*(*it).vel*3/512}, lineType );
                        else if (pt1.x <= window_width/2 && pt2.x > window_width/2) // The note block is inside the center line
                        {
                            if ( ! rProp.sep_render[0])
                                cv::fillConvexPoly( image, ptos, 4, {tProp.getColour(tnum, (*it).pitch).b*(*it).vel/64, tProp.getColour(tnum, (*it).pitch).g*(*it).vel/64, tProp.getColour(tnum, (*it).pitch).r*(*it).vel/64}, lineType );
                            else
                                cv::fillConvexPoly( img_playing_notes, ptos, 4, {tProp.getColour(tnum, (*it).pitch).b*(*it).vel/64, tProp.getColour(tnum, (*it).pitch).g*(*it).vel/64, tProp.getColour(tnum, (*it).pitch).r*(*it).vel/64}, lineType );
                        }
                        else // The note block is after (to the left of) the center line
                            if ( ! rProp.sep_render[1])
                                cv::fillConvexPoly( image, ptos, 4, {tProp.getColour(tnum, (*it).pitch).b*(*it).vel/512, tProp.getColour(tnum, (*it).pitch).g*(*it).vel/512, tProp.getColour(tnum, (*it).pitch).r*(*it).vel/512}, lineType );
                            else
                                cv::fillConvexPoly( img_buffer_sep_tracks[tnum], ptos, 4, {tProp.getColour(tnum, (*it).pitch).b*(*it).vel/512, tProp.getColour(tnum, (*it).pitch).g*(*it).vel/512, tProp.getColour(tnum, (*it).pitch).r*(*it).vel/512}, lineType );
                    }


                    // -------------------------- Draw Ellipses -------------------------
                    if ((*it).track == tnum && tProp.active[tnum] == true && tProp.shape[tnum] == 2) // All tracks - rectangle
                    {
                        x3 = (float)window_width*((-(float)startMidiTime + (float)(*it).t_middle)/((float)endMidiTime - (float)startMidiTime));
                        y3 = (float)window_height/2 - (float)window_height*((float)((*it).pitch + 0.5 - ((float)mdt.PitchMin + mdt.PitchMax)/2)/((float)mdt.PitchMax - (float)mdt.PitchMin))*rProp.vertRange/50.0 - rProp.vertShift;
                        pt3.x = f2int_safe(x3); // Center x
                        pt3.y = f2int_safe(y3); // Center y
                        if (pt1.x > window_width/2) // The note block is before (to the right of) the center line
                            if ( ! rProp.sep_render[1])
                                //cv::fillPoly( image, pts, );
                                cv::ellipse( image, pt3, cv::Size(x3-x1, y3-y2), 0, 0, 360, {tProp.getColour(tnum, (*it).pitch).b*(*it).vel*3/512, tProp.getColour(tnum, (*it).pitch).g*(*it).vel*3/512, tProp.getColour(tnum, (*it).pitch).r*(*it).vel*3/512}, -1, lineType);
                            else
                                cv::ellipse( img_buffer_sep_tracks[tnum], pt3, cv::Size(x3-x1, y3-y2), 0, 0, 360, {tProp.getColour(tnum, (*it).pitch).b*(*it).vel*3/512, tProp.getColour(tnum, (*it).pitch).g*(*it).vel*3/512, tProp.getColour(tnum, (*it).pitch).r*(*it).vel*3/512}, -1, lineType);
                        else if (pt1.x <= window_width/2 && pt2.x > window_width/2) // The note block is inside the center line
                        {
                            if ( ! rProp.sep_render[0])
                                cv::ellipse( image, pt3, cv::Size(x3-x1, y3-y2), 0, 0, 360, {tProp.getColour(tnum, (*it).pitch).b*(*it).vel/64, tProp.getColour(tnum, (*it).pitch).g*(*it).vel/64, tProp.getColour(tnum, (*it).pitch).r*(*it).vel/64}, -1, lineType);
                            else
                                cv::ellipse( img_playing_notes, pt3, cv::Size(x3-x1, y3-y2), 0, 0, 360, {tProp.getColour(tnum, (*it).pitch).b*(*it).vel/64, tProp.getColour(tnum, (*it).pitch).g*(*it).vel/64, tProp.getColour(tnum, (*it).pitch).r*(*it).vel/64}, -1, lineType);
                        }
                        else // The note block is after (to the left of) the center line
                            if ( ! rProp.sep_render[1])
                                cv::ellipse( image, pt3, cv::Size(x3-x1, y3-y2), 0, 0, 360, {tProp.getColour(tnum, (*it).pitch).b*(*it).vel/512, tProp.getColour(tnum, (*it).pitch).g*(*it).vel/512, tProp.getColour(tnum, (*it).pitch).r*(*it).vel/512}, 2, lineType);
                            else
                                cv::ellipse( img_buffer_sep_tracks[tnum], pt3, cv::Size(x3-x1, y3-y2), 0, 0, 360, {tProp.getColour(tnum, (*it).pitch).b*(*it).vel/512, tProp.getColour(tnum, (*it).pitch).g*(*it).vel/512, tProp.getColour(tnum, (*it).pitch).r*(*it).vel/512}, 2, lineType);
                    }


                    // -------------------------- Draw Circles -------------------------
                    if ((*it).track == tnum && tProp.active[tnum] == true && tProp.shape[tnum] == 3) // All tracks
                    {
                        x3 = (float)window_width*((-(float)startMidiTime + (float)(*it).t_middle)/((float)endMidiTime - (float)startMidiTime));
                        y3 = (float)window_height/2 - (float)window_height*((float)((*it).pitch + 0.5 - ((float)mdt.PitchMin + mdt.PitchMax)/2)/((float)mdt.PitchMax - (float)mdt.PitchMin))*rProp.vertRange/50.0 - rProp.vertShift;
                        pt3.x = f2int_safe(x3); // Center x
                        pt3.y = f2int_safe(y3); // Center y
                        int radius = (x3-x1) < 80 ? (x3-x1) : 80;
                        if (pt1.x > window_width/2) // The note block is before (to the right of) the center line
                            if ( ! rProp.sep_render[1])
                                //cv::fillPoly( image, pts, );
                                cv::circle( image, pt3, radius, {tProp.getColour(tnum, (*it).pitch).b*(*it).vel*3/512, tProp.getColour(tnum, (*it).pitch).g*(*it).vel*3/512, tProp.getColour(tnum, (*it).pitch).r*(*it).vel*3/512}, -1, lineType );
                            else
                                cv::circle( img_buffer_sep_tracks[tnum], pt3, radius, {tProp.getColour(tnum, (*it).pitch).b*(*it).vel*3/512, tProp.getColour(tnum, (*it).pitch).g*(*it).vel*3/512, tProp.getColour(tnum, (*it).pitch).r*(*it).vel*3/512}, -1, lineType );
                        else if (pt1.x <= window_width/2 && pt2.x > window_width/2) // The note block is inside the center line
                        {
                            if ( ! rProp.sep_render[0])
                                cv::circle( image, pt3, (x2-window_width/2+3), {tProp.getColour(tnum, (*it).pitch).b*(*it).vel/64, tProp.getColour(tnum, (*it).pitch).g*(*it).vel/64, tProp.getColour(tnum, (*it).pitch).r*(*it).vel/64}, -1, lineType );
                            else
                                cv::circle( img_playing_notes, pt3, (x2-window_width/2+1), {tProp.getColour(tnum, (*it).pitch).b*(*it).vel/64, tProp.getColour(tnum, (*it).pitch).g*(*it).vel/64, tProp.getColour(tnum, (*it).pitch).r*(*it).vel/64}, -1, lineType );
                        }
                        else // The note block is after (to the left of) the center line
                            if ( ! rProp.sep_render[1])
                                cv::circle( image, pt3, radius, {tProp.getColour(tnum, (*it).pitch).b*(*it).vel/512, tProp.getColour(tnum, (*it).pitch).g*(*it).vel/512, tProp.getColour(tnum, (*it).pitch).r*(*it).vel/512}, 2, lineType );
                            else
                                cv::circle( img_buffer_sep_tracks[tnum], pt3, radius, {tProp.getColour(tnum, (*it).pitch).b*(*it).vel/512, tProp.getColour(tnum, (*it).pitch).g*(*it).vel/512, tProp.getColour(tnum, (*it).pitch).r*(*it).vel/512}, 2, lineType );
                    }





                    // -------------------------- Draw Circles with moving centred circle -------------------------
                    if ((*it).track == tnum && tProp.active[tnum] == true && tProp.shape[tnum] == 4) // All tracks
                    {
                        x3 = (float)window_width*((-(float)startMidiTime + (float)(*it).t_middle)/((float)endMidiTime - (float)startMidiTime));
                        y3 = (float)window_height/2 - (float)window_height*((float)((*it).pitch + 0.5 - ((float)mdt.PitchMin + mdt.PitchMax)/2)/((float)mdt.PitchMax - (float)mdt.PitchMin))*rProp.vertRange/50.0 - rProp.vertShift;
                        pt3.x = f2int_safe(x3); // Center x
                        pt3.y = f2int_safe(y3); // Center y
                        int radius = (x3-x1) < 80 ? (x3-x1) : 80;
                        if (pt1.x > window_width/2) // The note block is before (to the right of) the center line
                            if ( ! rProp.sep_render[1])
                                //cv::fillPoly( image, pts, );
                                cv::circle( image, pt3, radius, {tProp.getColour(tnum, (*it).pitch).b*(*it).vel*3/512, tProp.getColour(tnum, (*it).pitch).g*(*it).vel*3/512, tProp.getColour(tnum, (*it).pitch).r*(*it).vel*3/512}, -1, lineType);
                            else
                                cv::circle( img_buffer_sep_tracks[tnum], pt3, radius, {tProp.getColour(tnum, (*it).pitch).b*(*it).vel*3/512, tProp.getColour(tnum, (*it).pitch).g*(*it).vel*3/512, tProp.getColour(tnum, (*it).pitch).r*(*it).vel*3/512}, -1, lineType);

                        if (pt1.x < window_width/2) // The note block is after (to the left of) the center line
                            if ( ! rProp.sep_render[1])
                                cv::circle( image, pt3, radius, {tProp.getColour(tnum, (*it).pitch).b*(*it).vel/512, tProp.getColour(tnum, (*it).pitch).g*(*it).vel/512, tProp.getColour(tnum, (*it).pitch).r*(*it).vel/512}, 2, lineType);
                            else
                                cv::circle( img_buffer_sep_tracks[tnum], pt3, radius, {tProp.getColour(tnum, (*it).pitch).b*(*it).vel/512, tProp.getColour(tnum, (*it).pitch).g*(*it).vel/512, tProp.getColour(tnum, (*it).pitch).r*(*it).vel/512}, 2, lineType);


                        if (pt1_prev[tnum].x <= window_width/2 && pt1.x > window_width/2 && pt2_prev[tnum].x + 5 > window_width/2) // The note block is inside the center line // no more else if
                            // this section corresponds to the "moving notes", i.e., the notes being played morphing from the current to the last (or from the next to the current if you prefer to say).
                            // In general, it works averaging current position and size with the last ones. A very important detail is to store the previous points p1_prev, pt2_prev, ... in the
                            // corresponding track (so, pt1_prev[tnum], pt2_prev[tnum] etc.), because we don't want the very last note as previous note, but the previous note of the same track!
                        {
                            float deriv = (((float)window_width/2 - (float)pt1_prev[tnum].x)/((float)pt1.x - (float)pt1_prev[tnum].x));
                            int y_mov = (int) ((float)pt3_prev[tnum].y + deriv*((float)pt3.y - (float)pt3_prev[tnum].y));
                            int x_mov = window_width/2 + pt3_prev[tnum].x - pt1_prev[tnum].x + deriv*((pt3.x - pt1.x) - (pt3_prev[tnum].x - pt1_prev[tnum].x));
                            // Case 1: the distance to the next note is big (not a legato) -> note just fall off
                            if (pt1.x - pt2_prev[tnum].x > 5) // next_note t_on and old note t_off are far
                                if ( ! rProp.sep_render[2])
                                    cv::circle( image, pt3_prev[tnum], floor1(pt2_prev[tnum].x-window_width/2+1), {tProp.getColour(tnum, (*it).pitch).b*(*it).vel/64, tProp.getColour(tnum, (*it).pitch).g*(*it).vel/64, tProp.getColour(tnum, (*it).pitch).r*(*it).vel/64}, -1, lineType);
                                else
                                    cv::circle( img_moving_notes, pt3_prev[tnum], floor1(pt2_prev[tnum].x-window_width/2+1), {tProp.getColour(tnum, (*it).pitch).b*(*it).vel/64, tProp.getColour(tnum, (*it).pitch).g*(*it).vel/64, tProp.getColour(tnum, (*it).pitch).r*(*it).vel/64}, -1, lineType);
                            // Case 2: the end of the current note (prev) is close to the begin of the new note (legato) -> notes connect themselves
                            else
                                if ( ! rProp.sep_render[2])
                                    cv::circle( image, cv::Point(x_mov, y_mov), floor1(radius_prev[tnum] + deriv*((float)radius - (float)radius_prev[tnum])), {tProp.getColour(tnum, (*it).pitch).b*(*it).vel/64, tProp.getColour(tnum, (*it).pitch).g*(*it).vel/64, tProp.getColour(tnum, (*it).pitch).r*(*it).vel/64}, -1, lineType);
                                else
                                    cv::circle( img_moving_notes, cv::Point(x_mov, y_mov), floor1(radius_prev[tnum] + deriv*((float)radius - (float)radius_prev[tnum])), {tProp.getColour(tnum, (*it).pitch).b*(*it).vel/64, tProp.getColour(tnum, (*it).pitch).g*(*it).vel/64, tProp.getColour(tnum, (*it).pitch).r*(*it).vel/64}, -1, lineType);

                        }

                        pt3_prev[tnum] = pt3; // Next centre point = previous
                        pt1_prev[tnum] = pt1;
                        pt2_prev[tnum] = pt2;
                        radius_prev[tnum] = radius;
                    }



                    // -------------------------- Draw Circles with moving centred circle and playing note highlight -------------------------
                    if ((*it).track == tnum && tProp.active[tnum] == true && tProp.shape[tnum] == 5) // All tracks
                    {
                        x3 = (float)window_width*((-(float)startMidiTime + (float)(*it).t_middle)/((float)endMidiTime - (float)startMidiTime));
                        y3 = (float)window_height/2 - (float)window_height*((float)((*it).pitch + 0.5 - ((float)mdt.PitchMin + mdt.PitchMax)/2)/((float)mdt.PitchMax - (float)mdt.PitchMin))*rProp.vertRange/50.0 - rProp.vertShift;
                        pt3.x = f2int_safe(x3); // Center x
                        pt3.y = f2int_safe(y3); // Center y
                        int radius = (x3-x1) < 80 ? (x3-x1) : 80;
                        if (pt1.x > window_width/2) // The note block is before (to the right of) the center line
                            if ( ! rProp.sep_render[1])
                                //cv::fillPoly( image, pts, );
                                cv::circle( image, pt3, radius, {tProp.getColour(tnum, (*it).pitch).b*(*it).vel*3/512, tProp.getColour(tnum, (*it).pitch).g*(*it).vel*3/512, tProp.getColour(tnum, (*it).pitch).r*(*it).vel*3/512}, -1, lineType);
                            else
                                cv::circle( img_buffer_sep_tracks[tnum], pt3, radius, {tProp.getColour(tnum, (*it).pitch).b*(*it).vel*3/512, tProp.getColour(tnum, (*it).pitch).g*(*it).vel*3/512, tProp.getColour(tnum, (*it).pitch).r*(*it).vel*3/512}, -1, lineType);
                        else if (pt1.x <= window_width/2 && pt2.x > window_width/2) // The note block is inside the center line
                        {
                            if ( ! rProp.sep_render[0])
                                cv::circle( image, pt3, radius, {tProp.getColour(tnum, (*it).pitch).b*(*it).vel/64, tProp.getColour(tnum, (*it).pitch).g*(*it).vel/64, tProp.getColour(tnum, (*it).pitch).r*(*it).vel/64}, 1, lineType);
                            else
                                cv::circle( img_playing_notes, pt3, radius, {tProp.getColour(tnum, (*it).pitch).b*(*it).vel/64, tProp.getColour(tnum, (*it).pitch).g*(*it).vel/64, tProp.getColour(tnum, (*it).pitch).r*(*it).vel/64}, 2, lineType);
                        }
                        else // The note block is after (to the left of) the center line
                            if ( ! rProp.sep_render[1])
                                cv::circle( image, pt3, radius, {tProp.getColour(tnum, (*it).pitch).b*(*it).vel/512, tProp.getColour(tnum, (*it).pitch).g*(*it).vel/512, tProp.getColour(tnum, (*it).pitch).r*(*it).vel/512}, 2, lineType);
                            else
                                cv::circle( img_buffer_sep_tracks[tnum], pt3, radius, {tProp.getColour(tnum, (*it).pitch).b*(*it).vel/512, tProp.getColour(tnum, (*it).pitch).g*(*it).vel/512, tProp.getColour(tnum, (*it).pitch).r*(*it).vel/512}, 2, lineType);


                        if (pt1_prev[tnum].x <= window_width/2 && pt1.x > window_width/2 && pt2_prev[tnum].x + 5 > window_width/2) // The note block is inside the center line // no more else if
                        {
                            float deriv = (((float)window_width/2 - (float)pt1_prev[tnum].x)/((float)pt1.x - (float)pt1_prev[tnum].x));
                            int y_mov = (int) ((float)pt3_prev[tnum].y + deriv*((float)pt3.y - (float)pt3_prev[tnum].y));
                            int x_mov = window_width/2 + pt3_prev[tnum].x - pt1_prev[tnum].x + deriv*((pt3.x - pt1.x) - (pt3_prev[tnum].x - pt1_prev[tnum].x));
                            // Case 1: the distance to the next note is big (not a legato) -> note just fall off
                            if (pt1.x - pt2_prev[tnum].x > 5) // next_note t_on and old note t_off are far
                                if ( ! rProp.sep_render[2])
                                    cv::circle( image, pt3_prev[tnum], floor1(pt2_prev[tnum].x-window_width/2+1), {tProp.getColour(tnum, (*it).pitch).b*(*it).vel/64, tProp.getColour(tnum, (*it).pitch).g*(*it).vel/64, tProp.getColour(tnum, (*it).pitch).r*(*it).vel/64}, -1, lineType);
                                else
                                    cv::circle( img_moving_notes, pt3_prev[tnum], floor1(pt2_prev[tnum].x-window_width/2+1), {tProp.getColour(tnum, (*it).pitch).b*(*it).vel/64, tProp.getColour(tnum, (*it).pitch).g*(*it).vel/64, tProp.getColour(tnum, (*it).pitch).r*(*it).vel/64}, -1, lineType);
                            // Case 2: the end of the current note (prev) is close to the begin of the new note (legato) -> notes connect themselves
                            else
                                if ( ! rProp.sep_render[2])
                                    cv::circle( image, cv::Point(x_mov, y_mov), floor1(radius_prev[tnum] + deriv*((float)radius - (float)radius_prev[tnum])), {tProp.getColour(tnum, (*it).pitch).b*(*it).vel/64, tProp.getColour(tnum, (*it).pitch).g*(*it).vel/64, tProp.getColour(tnum, (*it).pitch).r*(*it).vel/64}, -1, lineType);
                                else
                                    cv::circle( img_moving_notes, cv::Point(x_mov, y_mov), floor1(radius_prev[tnum] + deriv*((float)radius - (float)radius_prev[tnum])), {tProp.getColour(tnum, (*it).pitch).b*(*it).vel/64, tProp.getColour(tnum, (*it).pitch).g*(*it).vel/64, tProp.getColour(tnum, (*it).pitch).r*(*it).vel/64}, -1, lineType);

                        }

                        pt3_prev[tnum] = pt3; // Next centre point = previous
                        pt1_prev[tnum] = pt1;
                        pt2_prev[tnum] = pt2;
                        radius_prev[tnum] = radius;
                    }



                    // -------------------------- Draw Rectangles with moving centred rectangle -------------------------
                    if ((*it).track == tnum && tProp.active[tnum] == true && tProp.shape[tnum] == 6) // All tracks
                    {
//                        x3 = (float)window_width*((-(float)startMidiTime + (float)(*it).t_middle)/((float)endMidiTime - (float)startMidiTime));
//                        y3 = (float)window_height/2 - (float)window_height*((float)((*it).pitch + 0.5 - ((float)mdt.PitchMin + pitch_max)/2)/((float)pitch_max - (float)mdt.PitchMin))*rProp.vertRange/50.0 - rProp.vertShift;
//                        pt3.x = f2int_safe(x3); // Center x
//                        pt3.y = f2int_safe(y3); // Center y
                        if (pt1.x > window_width/2) // The note block is before (to the right of) the center line
                            if ( ! rProp.sep_render[1])
                                cv::rectangle( image, pt1, pt2, {tProp.getColour(tnum, (*it).pitch).b*(*it).vel*3/512, tProp.getColour(tnum, (*it).pitch).g*(*it).vel*3/512, tProp.getColour(tnum, (*it).pitch).r*(*it).vel*3/512}, -1, 8 );
                            else
                                cv::rectangle( img_buffer_sep_tracks[tnum], pt1, pt2, {tProp.getColour(tnum, (*it).pitch).b*(*it).vel*3/512, tProp.getColour(tnum, (*it).pitch).g*(*it).vel*3/512, tProp.getColour(tnum, (*it).pitch).r*(*it).vel*3/512}, -1, 8 );

                        else //if (pt1.x <= window_width/2) // The note block is after (to the left of) the center line
                            if ( ! rProp.sep_render[1])
                                cv::rectangle( image, pt1, pt2, {tProp.getColour(tnum, (*it).pitch).b*(*it).vel/256, tProp.getColour(tnum, (*it).pitch).g*(*it).vel/256, tProp.getColour(tnum, (*it).pitch).r*(*it).vel/256}, 2, 8 );
                            else
                                cv::rectangle( img_buffer_sep_tracks[tnum], pt1, pt2, {tProp.getColour(tnum, (*it).pitch).b*(*it).vel/256, tProp.getColour(tnum, (*it).pitch).g*(*it).vel/256, tProp.getColour(tnum, (*it).pitch).r*(*it).vel/256}, 2, 8 );


                        if (pt1_prev[tnum].x <= window_width/2 && pt1.x > window_width/2 && pt2_prev[tnum].x + 5 > window_width/2) // The note block is inside the center line // no more else if
                        {
                            float deriv = (((float)window_width/2 - (float)pt1_prev[tnum].x)/((float)pt1.x - (float)pt1_prev[tnum].x));
                            //int y_mov = (int) ((float)pt3_prev.y + deriv*((float)pt3.y - (float)pt3_prev.y));
                            int y1_mov = (int) ((float)pt1_prev[tnum].y + deriv*((float)pt1.y - (float)pt1_prev[tnum].y));
                            int y2_mov = (int) y1_mov + y2 - y1; // the difference among them remains the same
                            //int x_mov = window_width/2 + pt3_prev.x - pt1_prev.x + deriv*((pt3.x - pt1.x) - (pt3_prev.x - pt1_prev.x));
                            int x1_mov = window_width/2;
                            int x2_mov = window_width/2 + pt2_prev[tnum].x - pt1_prev[tnum].x - deriv*((pt2_prev[tnum].x - pt1_prev[tnum].x) - (pt2.x - pt1.x));
                            // Case 1: the distance to the next note is big (not a legato) -> note just fall off
                            if (pt1.x - pt2_prev[tnum].x > 5) // next_note t_on and old note t_off are far
                                if ( ! rProp.sep_render[2])
                                    cv::rectangle( image, cv::Point(- 2 + pt1_prev[tnum].x + deriv*(pt2_prev[tnum].x - pt1_prev[tnum].x), 2 + pt1_prev[tnum].y + deriv*(pt2_prev[tnum].y - pt1_prev[tnum].y)), cv::Point(2 + pt2_prev[tnum].x - deriv*(pt2_prev[tnum].x - pt1_prev[tnum].x), - 2 + pt2_prev[tnum].y - deriv*(pt2_prev[tnum].y - pt1_prev[tnum].y)), {tProp.getColour(tnum, (*it).pitch).b*(*it).vel/64, tProp.getColour(tnum, (*it).pitch).g*(*it).vel/64, tProp.getColour(tnum, (*it).pitch).r*(*it).vel/64}, -1, 8 );
                                else
                                    cv::rectangle( img_moving_notes, cv::Point(- 2 + pt1_prev[tnum].x + deriv*(pt2_prev[tnum].x - pt1_prev[tnum].x), 2 + pt1_prev[tnum].y + deriv*(pt2_prev[tnum].y - pt1_prev[tnum].y)), cv::Point(2 + pt2_prev[tnum].x - deriv*(pt2_prev[tnum].x - pt1_prev[tnum].x), - 2 + pt2_prev[tnum].y - deriv*(pt2_prev[tnum].y - pt1_prev[tnum].y)), {tProp.getColour(tnum, (*it).pitch).b*(*it).vel/64, tProp.getColour(tnum, (*it).pitch).g*(*it).vel/64, tProp.getColour(tnum, (*it).pitch).r*(*it).vel/64}, -1, 8 );
                            // Case 2: the end of the current note (prev) is close to the begin of the new note (legato) -> notes connect themselves
                            else
                                if ( ! rProp.sep_render[2])
                                    cv::rectangle( image, cv::Point(x1_mov, y1_mov), cv::Point(x2_mov, y2_mov), {tProp.getColour(tnum, (*it).pitch).b*(*it).vel/64, tProp.getColour(tnum, (*it).pitch).g*(*it).vel/64, tProp.getColour(tnum, (*it).pitch).r*(*it).vel/64}, -1, 8 );
                                else
                                    cv::rectangle( img_moving_notes, cv::Point(x1_mov, y1_mov), cv::Point(x2_mov, y2_mov), {tProp.getColour(tnum, (*it).pitch).b*(*it).vel/64, tProp.getColour(tnum, (*it).pitch).g*(*it).vel/64, tProp.getColour(tnum, (*it).pitch).r*(*it).vel/64}, -1, 8 );

                        }

                        //pt3_prev = pt3; // Next centre point = previous
                        pt1_prev[tnum] = pt1;
                        pt2_prev[tnum] = pt2;
//                        radius_prev = radius;
                    }



                    // -------------------------- Draw Rectangles with moving centred rectangle and highlighted borders -------------------------
                    if ((*it).track == tnum && tProp.active[tnum] == true && tProp.shape[tnum] == 7) // All tracks
                    {
//                        x3 = (float)window_width*((-(float)startMidiTime + (float)(*it).t_middle)/((float)endMidiTime - (float)startMidiTime));
//                        y3 = (float)window_height/2 - (float)window_height*((float)((*it).pitch + 0.5 - ((float)mdt.PitchMin + pitch_max)/2)/((float)pitch_max - (float)mdt.PitchMin))*rProp.vertRange/50.0 - rProp.vertShift;
//                        pt3.x = f2int_safe(x3); // Center x
//                        pt3.y = f2int_safe(y3); // Center y
                        if (pt1.x > window_width/2) // The note block is before (to the right of) the center line
                            if ( ! rProp.sep_render[1])
                                cv::rectangle( image, pt1, pt2, {tProp.getColour(tnum, (*it).pitch).b*(*it).vel*3/512, tProp.getColour(tnum, (*it).pitch).g*(*it).vel*3/512, tProp.getColour(tnum, (*it).pitch).r*(*it).vel*3/512}, -1, 8 );
                            else
                                cv::rectangle( img_buffer_sep_tracks[tnum], pt1, pt2, {tProp.getColour(tnum, (*it).pitch).b*(*it).vel*3/512, tProp.getColour(tnum, (*it).pitch).g*(*it).vel*3/512, tProp.getColour(tnum, (*it).pitch).r*(*it).vel*3/512}, -1, 8 );
                        else if (pt1.x <= window_width/2 && pt2.x > window_width/2) // The note block is inside the center line
                        {
                            if ( ! rProp.sep_render[0])
                                cv::rectangle( image, pt1, pt2, {tProp.getColour(tnum, (*it).pitch).b*(*it).vel/64, tProp.getColour(tnum, (*it).pitch).g*(*it).vel/64, tProp.getColour(tnum, (*it).pitch).r*(*it).vel/64}, 2, 8 );
                            else
                                cv::rectangle( img_playing_notes, pt1, pt2, {tProp.getColour(tnum, (*it).pitch).b*(*it).vel/64, tProp.getColour(tnum, (*it).pitch).g*(*it).vel/64, tProp.getColour(tnum, (*it).pitch).r*(*it).vel/64}, 2, 8 );
                        }
                        else // The note block is after (to the left of) the center line
                            if ( ! rProp.sep_render[1])
                                cv::rectangle( image, pt1, pt2, {tProp.getColour(tnum, (*it).pitch).b*(*it).vel/512, tProp.getColour(tnum, (*it).pitch).g*(*it).vel/512, tProp.getColour(tnum, (*it).pitch).r*(*it).vel/512}, 2, 8 );
                            else
                                cv::rectangle( img_buffer_sep_tracks[tnum], pt1, pt2, {tProp.getColour(tnum, (*it).pitch).b*(*it).vel/512, tProp.getColour(tnum, (*it).pitch).g*(*it).vel/512, tProp.getColour(tnum, (*it).pitch).r*(*it).vel/512}, 2, 8 );


                        if (pt1_prev[tnum].x <= window_width/2 && pt1.x > window_width/2 && pt2_prev[tnum].x + 5 > window_width/2) // The note block is inside the center line // no more else if
                        {
                            float deriv = (((float)window_width/2 - (float)pt1_prev[tnum].x)/((float)pt1.x - (float)pt1_prev[tnum].x));
                            //int y_mov = (int) ((float)pt3_prev.y + deriv*((float)pt3.y - (float)pt3_prev.y));
                            int y1_mov = (int) ((float)pt1_prev[tnum].y + deriv*((float)pt1.y - (float)pt1_prev[tnum].y));
                            int y2_mov = (int) y1_mov + y2 - y1; // the difference among them remains the same
                            //int x_mov = window_width/2 + pt3_prev.x - pt1_prev.x + deriv*((pt3.x - pt1.x) - (pt3_prev.x - pt1_prev.x));
                            int x1_mov = window_width/2;
                            int x2_mov = window_width/2 + pt2_prev[tnum].x - pt1_prev[tnum].x - deriv*((pt2_prev[tnum].x - pt1_prev[tnum].x) - (pt2.x - pt1.x));
                            // Case 1: the distance to the next note is big (not a legato) -> note just fall off
                            if (pt1.x - pt2_prev[tnum].x > 5) // next_note t_on and old note t_off are far
                                if ( ! rProp.sep_render[2])
                                    cv::rectangle( image, cv::Point(- 2 + pt1_prev[tnum].x + deriv*(pt2_prev[tnum].x - pt1_prev[tnum].x), 2 + pt1_prev[tnum].y + deriv*(pt2_prev[tnum].y - pt1_prev[tnum].y)), cv::Point(2 + pt2_prev[tnum].x - deriv*(pt2_prev[tnum].x - pt1_prev[tnum].x), - 2 + pt2_prev[tnum].y - deriv*(pt2_prev[tnum].y - pt1_prev[tnum].y)), {tProp.getColour(tnum, (*it).pitch).b*(*it).vel/64, tProp.getColour(tnum, (*it).pitch).g*(*it).vel/64, tProp.getColour(tnum, (*it).pitch).r*(*it).vel/64}, -1, 8 );
                                else
                                    cv::rectangle( img_moving_notes, cv::Point(- 2 + pt1_prev[tnum].x + deriv*(pt2_prev[tnum].x - pt1_prev[tnum].x), 2 + pt1_prev[tnum].y + deriv*(pt2_prev[tnum].y - pt1_prev[tnum].y)), cv::Point(2 + pt2_prev[tnum].x - deriv*(pt2_prev[tnum].x - pt1_prev[tnum].x), - 2 + pt2_prev[tnum].y - deriv*(pt2_prev[tnum].y - pt1_prev[tnum].y)), {tProp.getColour(tnum, (*it).pitch).b*(*it).vel/64, tProp.getColour(tnum, (*it).pitch).g*(*it).vel/64, tProp.getColour(tnum, (*it).pitch).r*(*it).vel/64}, -1, 8 );
                            // Case 2: the end of the current note (prev) is close to the begin of the new note (legato) -> notes connect themselves
                            else
                                if ( ! rProp.sep_render[2])
                                    cv::rectangle( image, cv::Point(x1_mov, y1_mov), cv::Point(x2_mov, y2_mov), {tProp.getColour(tnum, (*it).pitch).b*(*it).vel/64, tProp.getColour(tnum, (*it).pitch).g*(*it).vel/64, tProp.getColour(tnum, (*it).pitch).r*(*it).vel/64}, -1, 8 );
                                else
                                    cv::rectangle( img_moving_notes, cv::Point(x1_mov, y1_mov), cv::Point(x2_mov, y2_mov), {tProp.getColour(tnum, (*it).pitch).b*(*it).vel/64, tProp.getColour(tnum, (*it).pitch).g*(*it).vel/64, tProp.getColour(tnum, (*it).pitch).r*(*it).vel/64}, -1, 8 );

                        }

                        //pt3_prev = pt3; // Next centre point = previous
                        pt1_prev[tnum] = pt1;
                        pt2_prev[tnum] = pt2;
//                        radius_prev = radius;
                    }




                    // -------------------------- Draw Rhombus with moving rhombus -------------------------
                    if ((*it).track == tnum && tProp.active[tnum] == true && tProp.shape[tnum] == 8) // All tracks - rectangle
                    {
                        x3 = (float)window_width*((-(float)startMidiTime + (float)(*it).t_middle)/((float)endMidiTime - (float)startMidiTime));
                        y3 = (float)window_height/2 - (float)window_height*((float)((*it).pitch + 0.5 - ((float)mdt.PitchMin + mdt.PitchMax)/2)/((float)mdt.PitchMax - (float)mdt.PitchMin))*rProp.vertRange/50.0 - rProp.vertShift;
                        pt1.y = f2int_safe(y3);
                        pt2.y = f2int_safe(y3);
                        pt3.x = f2int_safe(x3);
                        pt3.y = f2int_safe(y1);
                        pt4.x = f2int_safe(x3);
                        pt4.y = f2int_safe(y2);
                        cv::Point ptos[4];
                        ptos[0] = pt1; ptos[1] = pt3; ptos[2] = pt2; ptos[3] = pt4;
                        if (pt1.x > window_width/2) // The note block is before (to the right of) the center line
                            if ( ! rProp.sep_render[1])
                                //cv::fillPoly( image, pts, );
                                cv::fillConvexPoly( image, ptos, 4, {tProp.getColour(tnum, (*it).pitch).b*(*it).vel*3/512, tProp.getColour(tnum, (*it).pitch).g*(*it).vel*3/512, tProp.getColour(tnum, (*it).pitch).r*(*it).vel*3/512}, lineType);
                            else
                                cv::fillConvexPoly( img_buffer_sep_tracks[tnum], ptos, 4, {tProp.getColour(tnum, (*it).pitch).b*(*it).vel*3/512, tProp.getColour(tnum, (*it).pitch).g*(*it).vel*3/512, tProp.getColour(tnum, (*it).pitch).r*(*it).vel*3/512}, lineType);
                        else // The note block is after (to the left of) the center line
                            if ( ! rProp.sep_render[1])
                                cv::fillConvexPoly( image, ptos, 4, {tProp.getColour(tnum, (*it).pitch).b*(*it).vel/512, tProp.getColour(tnum, (*it).pitch).g*(*it).vel/512, tProp.getColour(tnum, (*it).pitch).r*(*it).vel/512}, lineType);
                            else
                                cv::fillConvexPoly( img_buffer_sep_tracks[tnum], ptos, 4, {tProp.getColour(tnum, (*it).pitch).b*(*it).vel/512, tProp.getColour(tnum, (*it).pitch).g*(*it).vel/512, tProp.getColour(tnum, (*it).pitch).r*(*it).vel/512}, lineType);
//                        else if (pt1.x <= window_width/2 && pt2.x > window_width/2) // The note block is inside the center line
//                        {
//                            if ( ! rProp.sep_render[0])
//                                cv::fillConvexPoly( image, ptos, 4, {tProp.getColour(tnum, (*it).pitch).b*(*it).vel/64, tProp.getColour(tnum, (*it).pitch).g*(*it).vel/64, tProp.getColour(tnum, (*it).pitch).r*(*it).vel/64}, 8 );
//                            else
//                                cv::fillConvexPoly( img_playing_notes, ptos, 4, {tProp.getColour(tnum, (*it).pitch).b*(*it).vel/64, tProp.getColour(tnum, (*it).pitch).g*(*it).vel/64, tProp.getColour(tnum, (*it).pitch).r*(*it).vel/64}, 8 );
//                        }
                        if (pt1_prev[tnum].x <= window_width/2 && pt1.x > window_width/2 && pt2_prev[tnum].x + 5 > window_width/2) // The note block is inside the center line // no more else if
                        {
                            float deriv = (((float)window_width/2 - (float)pt1_prev[tnum].x)/((float)pt1.x - (float)pt1_prev[tnum].x));
                            //
                            int y1_mov = (int) ((float)pt1_prev[tnum].y + deriv*((float)pt1.y - (float)pt1_prev[tnum].y));
                            // int y2_mov = y1_mov;
                            int y3_mov = y1_mov + y1 - y3; // = (int) ((float)pt3_prev.y + deriv*((float)pt3.y - (float)pt3_prev.y));
                            int y4_mov = y1_mov - y1 + y3;
                            //
                            int x1_mov = window_width/2;
                            int x2_mov = window_width/2 + pt2_prev[tnum].x - pt1_prev[tnum].x - deriv*((pt2_prev[tnum].x - pt1_prev[tnum].x) - (pt2.x - pt1.x));
                            int x3_mov = window_width/2 + (pt2_prev[tnum].x - pt1_prev[tnum].x - deriv*((pt2_prev[tnum].x - pt1_prev[tnum].x) - (pt2.x - pt1.x)))/2;
                            //int x3_mov = window_width/2 + pt3_prev.x - pt1_prev.x + deriv*((pt3.x - pt1.x) - (pt3_prev.x - pt1_prev.x));
                            // int x4_mov = x3_mov
                            cv::Point ptos_mov[4];
                            ptos_mov[0] = cv::Point(x1_mov, y1_mov); ptos_mov[1] = cv::Point(x3_mov, y3_mov); ptos_mov[2] = cv::Point(x2_mov, y1_mov); ptos_mov[3] = cv::Point(x3_mov, y4_mov);

                            // Case 1: the distance to the next note is big (not a legato) -> note just fall off
                            if (pt1.x - pt2_prev[tnum].x > 5) // next_note t_on and old note t_off are far
                                if ( ! rProp.sep_render[2])
                                    cv::fillConvexPoly( image, ptos_prev[tnum], 4, {(0.5 - deriv)*tProp.getColour(tnum, (*it).pitch).b*(*it).vel/32, (0.5 - deriv)*tProp.getColour(tnum, (*it).pitch).g*(*it).vel/32, (0.5 - deriv)*tProp.getColour(tnum, (*it).pitch).r*(*it).vel/32}, lineType);
                                else
                                    cv::fillConvexPoly( img_moving_notes, ptos_prev[tnum], 4, {(1 - deriv)*tProp.getColour(tnum, (*it).pitch).b*(*it).vel/32, (1 - deriv)*tProp.getColour(tnum, (*it).pitch).g*(*it).vel/32, (1 - deriv)*tProp.getColour(tnum, (*it).pitch).r*(*it).vel/32}, lineType);
                            // Case 2: the end of the current note (prev) is close to the begin of the new note (legato) -> notes connect themselves
                            else
                                if ( ! rProp.sep_render[2])
                                    cv::fillConvexPoly( image, ptos_mov, 4, {tProp.getColour(tnum, (*it).pitch).b*(*it).vel/64, tProp.getColour(tnum, (*it).pitch).g*(*it).vel/64, tProp.getColour(tnum, (*it).pitch).r*(*it).vel/64}, lineType);
                                else
                                    cv::fillConvexPoly( img_moving_notes, ptos_mov, 4, {tProp.getColour(tnum, (*it).pitch).b*(*it).vel/64, tProp.getColour(tnum, (*it).pitch).g*(*it).vel/64, tProp.getColour(tnum, (*it).pitch).r*(*it).vel/64}, lineType);

                        }
                        pt3_prev[tnum] = pt3; // Next centre point = previous
                        pt1_prev[tnum] = pt1;
                        pt2_prev[tnum] = pt2;
                        ptos_prev[tnum][0] = ptos[0];
                        ptos_prev[tnum][1] = ptos[1];
                        ptos_prev[tnum][2] = ptos[2];
                        ptos_prev[tnum][3] = ptos[3];
                    }




                    // -------------------------- Draw Rhombus with moving rhombus and playing highlight (select sep playing notes render) -------------------------
                    if ((*it).track == tnum && tProp.active[tnum] == true && tProp.shape[tnum] == 9) // All tracks - rectangle
                    {
                        x3 = (float)window_width*((-(float)startMidiTime + (float)(*it).t_middle)/((float)endMidiTime - (float)startMidiTime));
                        y3 = (float)window_height/2 - (float)window_height*((float)((*it).pitch + 0.5 - ((float)mdt.PitchMin + mdt.PitchMax)/2)/((float)mdt.PitchMax - (float)mdt.PitchMin))*rProp.vertRange/50.0 - rProp.vertShift;
                        pt1.y = f2int_safe(y3);
                        pt2.y = f2int_safe(y3);
                        pt3.x = f2int_safe(x3);
                        pt3.y = f2int_safe(y1);
                        pt4.x = f2int_safe(x3);
                        pt4.y = f2int_safe(y2);
                        cv::Point ptos[4];
                        ptos[0] = pt1; ptos[1] = pt3; ptos[2] = pt2; ptos[3] = pt4;
                        if (pt1.x > window_width/2) // The note block is before (to the right of) the center line
                            if ( ! rProp.sep_render[1])
                                //cv::fillPoly( image, pts, );
                                cv::fillConvexPoly( image, ptos, 4, {tProp.getColour(tnum, (*it).pitch).b*(*it).vel*3/512, tProp.getColour(tnum, (*it).pitch).g*(*it).vel*3/512, tProp.getColour(tnum, (*it).pitch).r*(*it).vel*3/512}, lineType);
                            else
                                cv::fillConvexPoly( img_buffer_sep_tracks[tnum], ptos, 4, {tProp.getColour(tnum, (*it).pitch).b*(*it).vel*3/512, tProp.getColour(tnum, (*it).pitch).g*(*it).vel*3/512, tProp.getColour(tnum, (*it).pitch).r*(*it).vel*3/512}, lineType);
                        else if (pt1.x <= window_width/2 && pt2.x > window_width/2) // The note block is inside the center line
                        {
                            if ( ! rProp.sep_render[0])
                                cv::fillConvexPoly( image, ptos, 4, {tProp.getColour(tnum, (*it).pitch).b*(*it).vel/64, tProp.getColour(tnum, (*it).pitch).g*(*it).vel/64, tProp.getColour(tnum, (*it).pitch).r*(*it).vel/64}, lineType);
                            else
                                cv::fillConvexPoly( img_playing_notes, ptos, 4, {tProp.getColour(tnum, (*it).pitch).b*(*it).vel/64, tProp.getColour(tnum, (*it).pitch).g*(*it).vel/64, tProp.getColour(tnum, (*it).pitch).r*(*it).vel/64}, lineType);
                        }
                        if (pt1.x <= window_width/2) // The note block is after (to the left of) the center line
                            if ( ! rProp.sep_render[1])
                                cv::fillConvexPoly( image, ptos, 4, {tProp.getColour(tnum, (*it).pitch).b*(*it).vel/512, tProp.getColour(tnum, (*it).pitch).g*(*it).vel/512, tProp.getColour(tnum, (*it).pitch).r*(*it).vel/512}, lineType);
                            else
                                cv::fillConvexPoly( img_buffer_sep_tracks[tnum], ptos, 4, {tProp.getColour(tnum, (*it).pitch).b*(*it).vel/512, tProp.getColour(tnum, (*it).pitch).g*(*it).vel/512, tProp.getColour(tnum, (*it).pitch).r*(*it).vel/512}, lineType);
//                        else if (pt1.x <= window_width/2 && pt2.x > window_width/2) // The note block is inside the center line
//                        {
//                            if ( ! rProp.sep_render[0])
//                                cv::fillConvexPoly( image, ptos, 4, {tProp.getColour(tnum, (*it).pitch).b*(*it).vel/64, tProp.getColour(tnum, (*it).pitch).g*(*it).vel/64, tProp.getColour(tnum, (*it).pitch).r*(*it).vel/64}, 8 );
//                            else
//                                cv::fillConvexPoly( img_playing_notes, ptos, 4, {tProp.getColour(tnum, (*it).pitch).b*(*it).vel/64, tProp.getColour(tnum, (*it).pitch).g*(*it).vel/64, tProp.getColour(tnum, (*it).pitch).r*(*it).vel/64}, 8 );
//                        }
                        if (pt1_prev[tnum].x <= window_width/2 && pt1.x > window_width/2 && pt2_prev[tnum].x + 5 > window_width/2) // The note block is inside the center line // no more else if
                        {
                            float deriv = (((float)window_width/2 - (float)pt1_prev[tnum].x)/((float)pt1.x - (float)pt1_prev[tnum].x));
                            //
                            int y1_mov = (int) ((float)pt1_prev[tnum].y + deriv*((float)pt1.y - (float)pt1_prev[tnum].y));
                            // int y2_mov = y1_mov;
                            int y3_mov = y1_mov + y1 - y3; // = (int) ((float)pt3_prev.y + deriv*((float)pt3.y - (float)pt3_prev.y));
                            int y4_mov = y1_mov - y1 + y3;
                            //
                            int x1_mov = window_width/2;
                            int x2_mov = window_width/2 + pt2_prev[tnum].x - pt1_prev[tnum].x - deriv*((pt2_prev[tnum].x - pt1_prev[tnum].x) - (pt2.x - pt1.x));
                            int x3_mov = window_width/2 + (pt2_prev[tnum].x - pt1_prev[tnum].x - deriv*((pt2_prev[tnum].x - pt1_prev[tnum].x) - (pt2.x - pt1.x)))/2;
                            //int x3_mov = window_width/2 + pt3_prev.x - pt1_prev.x + deriv*((pt3.x - pt1.x) - (pt3_prev.x - pt1_prev.x));
                            // int x4_mov = x3_mov
                            cv::Point ptos_mov[4];
                            ptos_mov[0] = cv::Point(x1_mov, y1_mov); ptos_mov[1] = cv::Point(x3_mov, y3_mov); ptos_mov[2] = cv::Point(x2_mov, y1_mov); ptos_mov[3] = cv::Point(x3_mov, y4_mov);

                            // Case 1: the distance to the next note is big (not a legato) -> note just fall off
                            if (pt1.x - pt2_prev[tnum].x > 5) // next_note t_on and old note t_off are far
                                if ( ! rProp.sep_render[2])
                                    cv::fillConvexPoly( image, ptos_prev[tnum], 4, {(0.5 - deriv)*tProp.getColour(tnum, (*it).pitch).b*(*it).vel/32, (0.5 - deriv)*tProp.getColour(tnum, (*it).pitch).g*(*it).vel/32, (0.5 - deriv)*tProp.getColour(tnum, (*it).pitch).r*(*it).vel/32}, lineType);
                                else
                                    cv::fillConvexPoly( img_moving_notes, ptos_prev[tnum], 4, {(1 - deriv)*tProp.getColour(tnum, (*it).pitch).b*(*it).vel/32, (1 - deriv)*tProp.getColour(tnum, (*it).pitch).g*(*it).vel/32, (1 - deriv)*tProp.getColour(tnum, (*it).pitch).r*(*it).vel/32}, lineType);
                            // Case 2: the end of the current note (prev) is close to the begin of the new note (legato) -> notes connect themselves
                            else
                                if ( ! rProp.sep_render[2])
                                    cv::fillConvexPoly( image, ptos_mov, 4, {tProp.getColour(tnum, (*it).pitch).b*(*it).vel/64, tProp.getColour(tnum, (*it).pitch).g*(*it).vel/64, tProp.getColour(tnum, (*it).pitch).r*(*it).vel/64}, lineType);
                                else
                                    cv::fillConvexPoly( img_moving_notes, ptos_mov, 4, {tProp.getColour(tnum, (*it).pitch).b*(*it).vel/64, tProp.getColour(tnum, (*it).pitch).g*(*it).vel/64, tProp.getColour(tnum, (*it).pitch).r*(*it).vel/64}, lineType);

                        }
                        pt3_prev[tnum] = pt3; // Next centre point = previous
                        pt1_prev[tnum] = pt1;
                        pt2_prev[tnum] = pt2;
                        ptos_prev[tnum][0] = ptos[0];
                        ptos_prev[tnum][1] = ptos[1];
                        ptos_prev[tnum][2] = ptos[2];
                        ptos_prev[tnum][3] = ptos[3];
                    }



                    // -------------------------- Draw ellipses with moving centred ellipse -------------------------
                    if ((*it).track == tnum && tProp.active[tnum] == true && tProp.shape[tnum] == 10) // All tracks
                    {
                        x3 = (float)window_width*((-(float)startMidiTime + (float)(*it).t_middle)/((float)endMidiTime - (float)startMidiTime));
                        y3 = (float)window_height/2 - (float)window_height*((float)((*it).pitch + 0.5 - ((float)mdt.PitchMin + mdt.PitchMax)/2)/((float)mdt.PitchMax - (float)mdt.PitchMin))*rProp.vertRange/50.0 - rProp.vertShift;
                        pt3.x = f2int_safe(x3); // Center x
                        pt3.y = f2int_safe(y3); // Center y
                        int radius = (x3-x1) < 80 ? (x3-x1) : 80;
                        if (pt1.x > window_width/2) // The note block is before (to the right of) the center line
                            if ( ! rProp.sep_render[1])
                                //cv::fillPoly( image, pts, );
                                cv::ellipse( image, pt3, cv::Size(x3-x1, y3-y2), 0, 0, 360, {tProp.getColour(tnum, (*it).pitch).b*(*it).vel*3/512, tProp.getColour(tnum, (*it).pitch).g*(*it).vel*3/512, tProp.getColour(tnum, (*it).pitch).r*(*it).vel*3/512}, -1, lineType);
                            else
                                cv::ellipse( img_buffer_sep_tracks[tnum], pt3, cv::Size(x3-x1, y3-y2), 0, 0, 360, {tProp.getColour(tnum, (*it).pitch).b*(*it).vel*3/512, tProp.getColour(tnum, (*it).pitch).g*(*it).vel*3/512, tProp.getColour(tnum, (*it).pitch).r*(*it).vel*3/512}, -1, lineType);
//                        else if (pt1.x <= window_width/2 && pt2.x > window_width/2) // The note block is inside the center line
//                        {
//                            if ( ! rProp.sep_render[0])
//                                cv::ellipse( image, pt3, cv::Size(x3-x1, y3-y2), 0, 0, 360, {tProp.getColour(tnum, (*it).pitch).b*(*it).vel/64, tProp.getColour(tnum, (*it).pitch).g*(*it).vel/64, tProp.getColour(tnum, (*it).pitch).r*(*it).vel/64}, -1, 8 );
//                            else
//                                cv::ellipse( img_playing_notes, pt3, cv::Size(x3-x1, y3-y2), 0, 0, 360, {tProp.getColour(tnum, (*it).pitch).b*(*it).vel/64, tProp.getColour(tnum, (*it).pitch).g*(*it).vel/64, tProp.getColour(tnum, (*it).pitch).r*(*it).vel/64}, -1, 8 );
//                        }
                        else // The note block is after (to the left of) the center line
                            if ( ! rProp.sep_render[1])
                                cv::ellipse( image, pt3, cv::Size(x3-x1, y3-y2), 0, 0, 360, {tProp.getColour(tnum, (*it).pitch).b*(*it).vel/512, tProp.getColour(tnum, (*it).pitch).g*(*it).vel/512, tProp.getColour(tnum, (*it).pitch).r*(*it).vel/512}, 2, lineType);
                            else
                                cv::ellipse( img_buffer_sep_tracks[tnum], pt3, cv::Size(x3-x1, y3-y2), 0, 0, 360, {tProp.getColour(tnum, (*it).pitch).b*(*it).vel/512, tProp.getColour(tnum, (*it).pitch).g*(*it).vel/512, tProp.getColour(tnum, (*it).pitch).r*(*it).vel/512}, 2, lineType);


                        if (pt1_prev[tnum].x <= window_width/2 && pt1.x > window_width/2 && pt2_prev[tnum].x + 5 > window_width/2) // The note block is inside the center line // no more else if
                        {
                            float deriv = (((float)window_width/2 - (float)pt1_prev[tnum].x)/((float)pt1.x - (float)pt1_prev[tnum].x));
                            int y3_mov = (int) ((float)pt3_prev[tnum].y + deriv*((float)pt3.y - (float)pt3_prev[tnum].y));
                            int x3_mov = window_width/2 + pt3_prev[tnum].x - pt1_prev[tnum].x + deriv*((pt3.x - pt1.x) - (pt3_prev[tnum].x - pt1_prev[tnum].x));
                            int y1_mov = (int) ((float)pt1_prev[tnum].y + deriv*((float)pt1.y - (float)pt1_prev[tnum].y));
                            int y2_mov = (int) y1_mov + y2 - y1; // the difference among them remains the same
                            //int x_mov = window_width/2 + pt3_prev.x - pt1_prev.x + deriv*((pt3.x - pt1.x) - (pt3_prev.x - pt1_prev.x));
                            int x1_mov = window_width/2;
                            //int x2_mov = window_width/2 + pt2_prev[tnum].x - pt1_prev[tnum].x - deriv*((pt2_prev[tnum].x - pt1_prev[tnum].x) - (pt2.x - pt1.x));
                            // Case 1: the distance to the next note is big (not a legato) -> note just fall off
                            if (pt1.x - pt2_prev[tnum].x > 5) // next_note t_on and old note t_off are far
                                if ( ! rProp.sep_render[2])
                                    cv::ellipse( image, pt3_prev[tnum], cv::Size(x3_mov-x1_mov, y3-y2), 0, 0, 360, {(0.5-deriv)*tProp.getColour(tnum, (*it).pitch).b*(*it).vel*3/64, (0.5-deriv)*tProp.getColour(tnum, (*it).pitch).g*(*it).vel*3/64, (0.5-deriv)*tProp.getColour(tnum, (*it).pitch).r*(*it).vel*3/64}, -1, lineType);
                                else
                                    cv::ellipse( img_moving_notes, pt3_prev[tnum], cv::Size(x3_mov-x1_mov, y3-y2), 0, 0, 360, {(0.5-deriv)*tProp.getColour(tnum, (*it).pitch).b*(*it).vel*3/64, (0.5-deriv)*tProp.getColour(tnum, (*it).pitch).g*(*it).vel*3/64, (0.5-deriv)*tProp.getColour(tnum, (*it).pitch).r*(*it).vel*3/64}, -1, lineType);
                            // Case 2: the end of the current note (prev) is close to the begin of the new note (legato) -> notes connect themselves
                            else
                                if ( ! rProp.sep_render[2])
                                    cv::ellipse( image, cv::Point(x3_mov, y3_mov), cv::Size(x3_mov-x1_mov, y3-y2), 0, 0, 360, {tProp.getColour(tnum, (*it).pitch).b*(*it).vel*3/64, tProp.getColour(tnum, (*it).pitch).g*(*it).vel*3/64, tProp.getColour(tnum, (*it).pitch).r*(*it).vel*3/64}, -1, lineType);
                                else
                                    cv::ellipse( img_moving_notes, cv::Point(x3_mov, y3_mov), cv::Size(x3_mov-x1_mov, y3-y2), 0, 0, 360, {tProp.getColour(tnum, (*it).pitch).b*(*it).vel*3/64, tProp.getColour(tnum, (*it).pitch).g*(*it).vel*3/64, tProp.getColour(tnum, (*it).pitch).r*(*it).vel*3/64}, -1, lineType);

                        }

                        pt3_prev[tnum] = pt3; // Next centre point = previous
                        pt1_prev[tnum] = pt1;
                        pt2_prev[tnum] = pt2;
                        radius_prev[tnum] = radius;
                    }




                    // -------------------------- Draw ellipses with moving centred ellipse and highlighted playing note -------------------------
                    if ((*it).track == tnum && tProp.active[tnum] == true && tProp.shape[tnum] == 11) // All tracks
                    {
                        x3 = (float)window_width*((-(float)startMidiTime + (float)(*it).t_middle)/((float)endMidiTime - (float)startMidiTime));
                        y3 = (float)window_height/2 - (float)window_height*((float)((*it).pitch + 0.5 - ((float)mdt.PitchMin + mdt.PitchMax)/2)/((float)mdt.PitchMax - (float)mdt.PitchMin))*rProp.vertRange/50.0 - rProp.vertShift;
                        pt3.x = f2int_safe(x3); // Center x
                        pt3.y = f2int_safe(y3); // Center y
                        int radius = (x3-x1) < 80 ? (x3-x1) : 80;
                        if (pt1.x > window_width/2) // The note block is before (to the right of) the center line
                            if ( ! rProp.sep_render[1])
                                //cv::fillPoly( image, pts, );
                                cv::ellipse( image, pt3, cv::Size(x3-x1, y3-y2), 0, 0, 360, {tProp.getColour(tnum, (*it).pitch).b*(*it).vel*3/512, tProp.getColour(tnum, (*it).pitch).g*(*it).vel*3/512, tProp.getColour(tnum, (*it).pitch).r*(*it).vel*3/512}, -1, lineType);
                            else
                                cv::ellipse( img_buffer_sep_tracks[tnum], pt3, cv::Size(x3-x1, y3-y2), 0, 0, 360, {tProp.getColour(tnum, (*it).pitch).b*(*it).vel*3/512, tProp.getColour(tnum, (*it).pitch).g*(*it).vel*3/512, tProp.getColour(tnum, (*it).pitch).r*(*it).vel*3/512}, -1, lineType);
                        else if (pt1.x <= window_width/2 && pt2.x > window_width/2) // The note block is inside the center line
                        {
                            if ( ! rProp.sep_render[0])
                                cv::ellipse( image, pt3, cv::Size(x3-x1, y3-y2), 0, 0, 360, {tProp.getColour(tnum, (*it).pitch).b*(*it).vel/64, tProp.getColour(tnum, (*it).pitch).g*(*it).vel/64, tProp.getColour(tnum, (*it).pitch).r*(*it).vel/64}, 1, lineType);
                            else
                                cv::ellipse( img_playing_notes, pt3, cv::Size(x3-x1, y3-y2), 0, 0, 360, {tProp.getColour(tnum, (*it).pitch).b*(*it).vel/64, tProp.getColour(tnum, (*it).pitch).g*(*it).vel/64, tProp.getColour(tnum, (*it).pitch).r*(*it).vel/64}, 1, lineType);
                        }
                        else // The note block is after (to the left of) the center line
                            if ( ! rProp.sep_render[1])
                                cv::ellipse( image, pt3, cv::Size(x3-x1, y3-y2), 0, 0, 360, {tProp.getColour(tnum, (*it).pitch).b*(*it).vel/512, tProp.getColour(tnum, (*it).pitch).g*(*it).vel/512, tProp.getColour(tnum, (*it).pitch).r*(*it).vel/512}, 2, lineType);
                            else
                                cv::ellipse( img_buffer_sep_tracks[tnum], pt3, cv::Size(x3-x1, y3-y2), 0, 0, 360, {tProp.getColour(tnum, (*it).pitch).b*(*it).vel/512, tProp.getColour(tnum, (*it).pitch).g*(*it).vel/512, tProp.getColour(tnum, (*it).pitch).r*(*it).vel/512}, 2, lineType);


                        if (pt1_prev[tnum].x <= window_width/2 && pt1.x > window_width/2 && pt2_prev[tnum].x + 5 > window_width/2) // The note block is inside the center line // no more else if
                        {
                            float deriv = (((float)window_width/2 - (float)pt1_prev[tnum].x)/((float)pt1.x - (float)pt1_prev[tnum].x));
                            int y3_mov = (int) ((float)pt3_prev[tnum].y + deriv*((float)pt3.y - (float)pt3_prev[tnum].y));
                            int x3_mov = window_width/2 + pt3_prev[tnum].x - pt1_prev[tnum].x + deriv*((pt3.x - pt1.x) - (pt3_prev[tnum].x - pt1_prev[tnum].x));
                            int y1_mov = (int) ((float)pt1_prev[tnum].y + deriv*((float)pt1.y - (float)pt1_prev[tnum].y));
                            int y2_mov = (int) y1_mov + y2 - y1; // the difference among them remains the same
                            //int x_mov = window_width/2 + pt3_prev.x - pt1_prev.x + deriv*((pt3.x - pt1.x) - (pt3_prev.x - pt1_prev.x));
                            int x1_mov = window_width/2;
                            //int x2_mov = window_width/2 + pt2_prev[tnum].x - pt1_prev[tnum].x - deriv*((pt2_prev[tnum].x - pt1_prev[tnum].x) - (pt2.x - pt1.x));
                            // Case 1: the distance to the next note is big (not a legato) -> note just fall off
                            if (pt1.x - pt2_prev[tnum].x > 5) // next_note t_on and old note t_off are far
                                if ( ! rProp.sep_render[2])
                                    cv::ellipse( image, pt3_prev[tnum], cv::Size(x3_mov-x1_mov, y3-y2), 0, 0, 360, {(0.5-deriv)*tProp.getColour(tnum, (*it).pitch).b*(*it).vel*3/64, (0.5-deriv)*tProp.getColour(tnum, (*it).pitch).g*(*it).vel*3/64, (0.5-deriv)*tProp.getColour(tnum, (*it).pitch).r*(*it).vel*3/64}, -1, lineType);
                                else
                                    cv::ellipse( img_moving_notes, pt3_prev[tnum], cv::Size(x3_mov-x1_mov, y3-y2), 0, 0, 360, {(0.5-deriv)*tProp.getColour(tnum, (*it).pitch).b*(*it).vel*3/64, (0.5-deriv)*tProp.getColour(tnum, (*it).pitch).g*(*it).vel*3/64, (0.5-deriv)*tProp.getColour(tnum, (*it).pitch).r*(*it).vel*3/64}, -1, lineType);
                            // Case 2: the end of the current note (prev) is close to the begin of the new note (legato) -> notes connect themselves
                            else
                                if ( ! rProp.sep_render[2])
                                    cv::ellipse( image, cv::Point(x3_mov, y3_mov), cv::Size(x3_mov-x1_mov, y3-y2), 0, 0, 360, {tProp.getColour(tnum, (*it).pitch).b*(*it).vel*3/64, tProp.getColour(tnum, (*it).pitch).g*(*it).vel*3/64, tProp.getColour(tnum, (*it).pitch).r*(*it).vel*3/64}, -1, lineType);
                                else
                                    cv::ellipse( img_moving_notes, cv::Point(x3_mov, y3_mov), cv::Size(x3_mov-x1_mov, y3-y2), 0, 0, 360, {tProp.getColour(tnum, (*it).pitch).b*(*it).vel*3/64, tProp.getColour(tnum, (*it).pitch).g*(*it).vel*3/64, tProp.getColour(tnum, (*it).pitch).r*(*it).vel*3/64}, -1, lineType);

                        }

                        pt3_prev[tnum] = pt3; // Next centre point = previous
                        pt1_prev[tnum] = pt1;
                        pt2_prev[tnum] = pt2;
                        radius_prev[tnum] = radius;
                    }




                    // -------------------------- Draw Circles with size proportional to the velocity -------------------------
                    if ((*it).track == tnum && tProp.active[tnum] == true && tProp.shape[tnum] == 12) // All tracks
                    {
                        x3 = (float)window_width*((-(float)startMidiTime + (float)(*it).t_middle)/((float)endMidiTime - (float)startMidiTime));
                        y3 = (float)window_height/2 - (float)window_height*((float)((*it).pitch + 0.5 - ((float)mdt.PitchMin + mdt.PitchMax)/2)/((float)mdt.PitchMax - (float)mdt.PitchMin))*rProp.vertRange/50.0 - rProp.vertShift;
                        pt3.x = f2int_safe(x3); // Center x
                        pt3.y = f2int_safe(y3); // Center y
                        int radius = (*it).vel/2; // size proportion
                        if (pt1.x > window_width/2) // The note block is before (to the right of) the center line
                            if ( ! rProp.sep_render[1])
                                //cv::fillPoly( image, pts, );
                                cv::circle( image, pt3, radius, {tProp.getColour(tnum, (*it).pitch).b*(*it).vel*3/512, tProp.getColour(tnum, (*it).pitch).g*(*it).vel*3/512, tProp.getColour(tnum, (*it).pitch).r*(*it).vel*3/512}, -1, lineType);
                            else
                                cv::circle( img_buffer_sep_tracks[tnum], pt3, radius, {tProp.getColour(tnum, (*it).pitch).b*(*it).vel*3/512, tProp.getColour(tnum, (*it).pitch).g*(*it).vel*3/512, tProp.getColour(tnum, (*it).pitch).r*(*it).vel*3/512}, -1, lineType);
                        else if (pt1.x <= window_width/2 && pt2.x > window_width/2) // The note block is inside the center line
                        {
                            if ( ! rProp.sep_render[0])
                                cv::circle( image, pt3, 1.1*radius*(x2-window_width/2)/(x2-x1), {tProp.getColour(tnum, (*it).pitch).b*(*it).vel/64, tProp.getColour(tnum, (*it).pitch).g*(*it).vel/64, tProp.getColour(tnum, (*it).pitch).r*(*it).vel/64}, -1, lineType);
                            else
                                cv::circle( img_playing_notes, pt3, 1.1*radius*(x2-window_width/2)/(x2-x1), {tProp.getColour(tnum, (*it).pitch).b*(*it).vel/64, tProp.getColour(tnum, (*it).pitch).g*(*it).vel/64, tProp.getColour(tnum, (*it).pitch).r*(*it).vel/64}, -1, lineType);
                        }
                        else // The note block is after (to the left of) the center line
                            if ( ! rProp.sep_render[1])
                                cv::circle( image, pt3, radius, {tProp.getColour(tnum, (*it).pitch).b*(*it).vel/512, tProp.getColour(tnum, (*it).pitch).g*(*it).vel/512, tProp.getColour(tnum, (*it).pitch).r*(*it).vel/512}, 2, lineType);
                            else
                                cv::circle( img_buffer_sep_tracks[tnum], pt3, radius, {tProp.getColour(tnum, (*it).pitch).b*(*it).vel/512, tProp.getColour(tnum, (*it).pitch).g*(*it).vel/512, tProp.getColour(tnum, (*it).pitch).r*(*it).vel/512}, 2, lineType);
                    }



                    // -------------------------- Draw Ellipses with height proportional to the velocity -------------------------
                    if ((*it).track == tnum && tProp.active[tnum] == true && tProp.shape[tnum] == 13) // All tracks - rectangle
                    {
                        x3 = (float)window_width*((-(float)startMidiTime + (float)(*it).t_middle)/((float)endMidiTime - (float)startMidiTime));
                        y3 = (float)window_height/2 - (float)window_height*((float)((*it).pitch + 0.5 - ((float)mdt.PitchMin + mdt.PitchMax)/2)/((float)mdt.PitchMax - (float)mdt.PitchMin))*rProp.vertRange/50.0 - rProp.vertShift;
                        pt3.x = f2int_safe(x3); // Center x
                        pt3.y = f2int_safe(y3); // Center y
                        if (pt1.x > window_width/2) // The note block is before (to the right of) the center line
                            if ( ! rProp.sep_render[1])
                                //cv::fillPoly( image, pts, );
                                cv::ellipse( image, pt3, cv::Size(x3-x1, 1 + (*it).vel/2 ), 0, 0, 360, {tProp.getColour(tnum, (*it).pitch).b*(*it).vel*3/512, tProp.getColour(tnum, (*it).pitch).g*(*it).vel*3/512, tProp.getColour(tnum, (*it).pitch).r*(*it).vel*3/512}, -1, lineType);
                            else
                                cv::ellipse( img_buffer_sep_tracks[tnum], pt3, cv::Size(x3-x1, 1 + (*it).vel/2), 0, 0, 360, {tProp.getColour(tnum, (*it).pitch).b*(*it).vel*3/512, tProp.getColour(tnum, (*it).pitch).g*(*it).vel*3/512, tProp.getColour(tnum, (*it).pitch).r*(*it).vel*3/512}, -1, lineType);
                        else if (pt1.x <= window_width/2 && pt2.x > window_width/2) // The note block is inside the center line
                        {
                            if ( ! rProp.sep_render[0])
                                cv::ellipse( image, pt3, cv::Size(x3-x1, 1 + (*it).vel/2 ), 0, 0, 360, {tProp.getColour(tnum, (*it).pitch).b*(*it).vel/64, tProp.getColour(tnum, (*it).pitch).g*(*it).vel/64, tProp.getColour(tnum, (*it).pitch).r*(*it).vel/64}, -1, lineType);
                            else
                                cv::ellipse( img_playing_notes, pt3, cv::Size(x3-x1, 1 + (*it).vel/2 ), 0, 0, 360, {tProp.getColour(tnum, (*it).pitch).b*(*it).vel/64, tProp.getColour(tnum, (*it).pitch).g*(*it).vel/64, tProp.getColour(tnum, (*it).pitch).r*(*it).vel/64}, -1, lineType);
                        }
                        else // The note block is after (to the left of) the center line
                            if ( ! rProp.sep_render[1])
                                cv::ellipse( image, pt3, cv::Size(x3-x1, 1 + (*it).vel/2 ), 0, 0, 360, {tProp.getColour(tnum, (*it).pitch).b*(*it).vel/512, tProp.getColour(tnum, (*it).pitch).g*(*it).vel/512, tProp.getColour(tnum, (*it).pitch).r*(*it).vel/512}, 2, lineType);
                            else
                                cv::ellipse( img_buffer_sep_tracks[tnum], pt3, cv::Size(x3-x1, 1 + (*it).vel/2 ), 0, 0, 360, {tProp.getColour(tnum, (*it).pitch).b*(*it).vel/512, tProp.getColour(tnum, (*it).pitch).g*(*it).vel/512, tProp.getColour(tnum, (*it).pitch).r*(*it).vel/512}, 2, lineType);
                    }


                    // -------------------------- Draw Rhombus with height proportional to the velocity -------------------------
                    if ((*it).track == tnum && tProp.active[tnum] == true && tProp.shape[tnum] == 14) // All tracks - rectangle
                    {
                        x3 = (float)window_width*((-(float)startMidiTime + (float)(*it).t_middle)/((float)endMidiTime - (float)startMidiTime));
                        y3 = (float)window_height/2 - (float)window_height*((float)((*it).pitch + 0.5 - ((float)mdt.PitchMin + mdt.PitchMax)/2)/((float)mdt.PitchMax - (float)mdt.PitchMin))*rProp.vertRange/50.0 - rProp.vertShift;
                        pt1.y = f2int_safe(y3);
                        pt2.y = f2int_safe(y3);
                        pt3.x = f2int_safe(x3);
                        pt3.y = f2int_safe(y1) + (*it).vel/2;
                        pt4.x = f2int_safe(x3);
                        pt4.y = f2int_safe(y2) - (*it).vel/2;
                        cv::Point ptos[4];
                        ptos[0] = pt1; ptos[1] = pt3; ptos[2] = pt2; ptos[3] = pt4;
                        if (pt1.x > window_width/2) // The note block is before (to the right of) the center line
                            if ( ! rProp.sep_render[1])
                                //cv::fillPoly( image, pts, );
                                cv::fillConvexPoly( image, ptos, 4, {tProp.getColour(tnum, (*it).pitch).b*(*it).vel*3/512, tProp.getColour(tnum, (*it).pitch).g*(*it).vel*3/512, tProp.getColour(tnum, (*it).pitch).r*(*it).vel*3/512}, lineType);
                            else
                                cv::fillConvexPoly( img_buffer_sep_tracks[tnum], ptos, 4, {tProp.getColour(tnum, (*it).pitch).b*(*it).vel*3/512, tProp.getColour(tnum, (*it).pitch).g*(*it).vel*3/512, tProp.getColour(tnum, (*it).pitch).r*(*it).vel*3/512}, lineType);
                        else if (pt1.x <= window_width/2 && pt2.x > window_width/2) // The note block is inside the center line
                        {
                            if ( ! rProp.sep_render[0])
                                cv::fillConvexPoly( image, ptos, 4, {tProp.getColour(tnum, (*it).pitch).b*(*it).vel/64, tProp.getColour(tnum, (*it).pitch).g*(*it).vel/64, tProp.getColour(tnum, (*it).pitch).r*(*it).vel/64}, lineType);
                            else
                                cv::fillConvexPoly( img_playing_notes, ptos, 4, {tProp.getColour(tnum, (*it).pitch).b*(*it).vel/64, tProp.getColour(tnum, (*it).pitch).g*(*it).vel/64, tProp.getColour(tnum, (*it).pitch).r*(*it).vel/64}, lineType);
                        }
                        else // The note block is after (to the left of) the center line
                            if ( ! rProp.sep_render[1])
                                cv::fillConvexPoly( image, ptos, 4, {tProp.getColour(tnum, (*it).pitch).b*(*it).vel/512, tProp.getColour(tnum, (*it).pitch).g*(*it).vel/512, tProp.getColour(tnum, (*it).pitch).r*(*it).vel/512}, lineType);
                            else
                                cv::fillConvexPoly( img_buffer_sep_tracks[tnum], ptos, 4, {tProp.getColour(tnum, (*it).pitch).b*(*it).vel/512, tProp.getColour(tnum, (*it).pitch).g*(*it).vel/512, tProp.getColour(tnum, (*it).pitch).r*(*it).vel/512}, lineType);
                    }



                    // -------------------------- Draw Triangles V with height proportional to the velocity -------------------------
                    if ((*it).track == tnum && tProp.active[tnum] == true && tProp.shape[tnum] == 15) // All tracks - rectangle
                    {
                        x3 = (float)window_width*((-(float)startMidiTime + (float)(*it).t_middle)/((float)endMidiTime - (float)startMidiTime));
                        y3 = (float)window_height/2 - (float)window_height*((float)((*it).pitch + 0.5 - ((float)mdt.PitchMin + mdt.PitchMax)/2)/((float)mdt.PitchMax - (float)mdt.PitchMin))*rProp.vertRange/50.0 - rProp.vertShift;
                        pt1.y = f2int_safe(y3);
                        pt2.y = f2int_safe(y3);
                        pt3.x = f2int_safe(x3);
                        pt3.y = f2int_safe(y1) + (*it).vel/2;
                        cv::Point ptos[3];
                        ptos[0] = pt1; ptos[1] = pt3; ptos[2] = pt2;
                        if (pt1.x > window_width/2) // The note block is before (to the right of) the center line
                            if ( ! rProp.sep_render[1])
                                //cv::fillPoly( image, pts, );
                                cv::fillConvexPoly( image, ptos, 3, {tProp.getColour(tnum, (*it).pitch).b*(*it).vel*3/512, tProp.getColour(tnum, (*it).pitch).g*(*it).vel*3/512, tProp.getColour(tnum, (*it).pitch).r*(*it).vel*3/512}, lineType);
                            else
                                cv::fillConvexPoly( img_buffer_sep_tracks[tnum], ptos, 3, {tProp.getColour(tnum, (*it).pitch).b*(*it).vel*3/512, tProp.getColour(tnum, (*it).pitch).g*(*it).vel*3/512, tProp.getColour(tnum, (*it).pitch).r*(*it).vel*3/512}, lineType);
                        else if (pt1.x <= window_width/2 && pt2.x > window_width/2) // The note block is inside the center line
                        {
                            if ( ! rProp.sep_render[0])
                                cv::fillConvexPoly( image, ptos, 3, {tProp.getColour(tnum, (*it).pitch).b*(*it).vel/64, tProp.getColour(tnum, (*it).pitch).g*(*it).vel/64, tProp.getColour(tnum, (*it).pitch).r*(*it).vel/64}, lineType);
                            else
                                cv::fillConvexPoly( img_playing_notes, ptos, 3, {tProp.getColour(tnum, (*it).pitch).b*(*it).vel/64, tProp.getColour(tnum, (*it).pitch).g*(*it).vel/64, tProp.getColour(tnum, (*it).pitch).r*(*it).vel/64}, lineType);
                        }
                        else // The note block is after (to the left of) the center line
                            if ( ! rProp.sep_render[1])
                                cv::fillConvexPoly( image, ptos, 3, {tProp.getColour(tnum, (*it).pitch).b*(*it).vel/512, tProp.getColour(tnum, (*it).pitch).g*(*it).vel/512, tProp.getColour(tnum, (*it).pitch).r*(*it).vel/512}, lineType);
                            else
                                cv::fillConvexPoly( img_buffer_sep_tracks[tnum], ptos, 3, {tProp.getColour(tnum, (*it).pitch).b*(*it).vel/512, tProp.getColour(tnum, (*it).pitch).g*(*it).vel/512, tProp.getColour(tnum, (*it).pitch).r*(*it).vel/512}, lineType);
                    }



                    // -------------------------- Draw Triangles ^ with height proportional to the velocity -------------------------
                    if ((*it).track == tnum && tProp.active[tnum] == true && tProp.shape[tnum] == 16) // All tracks - rectangle
                    {
                        x3 = (float)window_width*((-(float)startMidiTime + (float)(*it).t_middle)/((float)endMidiTime - (float)startMidiTime));
                        y3 = (float)window_height/2 - (float)window_height*((float)((*it).pitch + 0.5 - ((float)mdt.PitchMin + mdt.PitchMax)/2)/((float)mdt.PitchMax - (float)mdt.PitchMin))*rProp.vertRange/50.0 - rProp.vertShift;
                        pt1.y = f2int_safe(y3);
                        pt2.y = f2int_safe(y3);
                        pt3.x = f2int_safe(x3);
                        pt3.y = f2int_safe(y2) - (*it).vel/2;
                        cv::Point ptos[3];
                        ptos[0] = pt1; ptos[1] = pt3; ptos[2] = pt2;
                        if (pt1.x > window_width/2) // The note block is before (to the right of) the center line
                            if ( ! rProp.sep_render[1])
                                //cv::fillPoly( image, pts, );
                                cv::fillConvexPoly( image, ptos, 3, {tProp.getColour(tnum, (*it).pitch).b*(*it).vel*3/512, tProp.getColour(tnum, (*it).pitch).g*(*it).vel*3/512, tProp.getColour(tnum, (*it).pitch).r*(*it).vel*3/512}, lineType);
                            else
                                cv::fillConvexPoly( img_buffer_sep_tracks[tnum], ptos, 3, {tProp.getColour(tnum, (*it).pitch).b*(*it).vel*3/512, tProp.getColour(tnum, (*it).pitch).g*(*it).vel*3/512, tProp.getColour(tnum, (*it).pitch).r*(*it).vel*3/512}, lineType);
                        else if (pt1.x <= window_width/2 && pt2.x > window_width/2) // The note block is inside the center line
                        {
                            if ( ! rProp.sep_render[0])
                                cv::fillConvexPoly( image, ptos, 3, {tProp.getColour(tnum, (*it).pitch).b*(*it).vel/64, tProp.getColour(tnum, (*it).pitch).g*(*it).vel/64, tProp.getColour(tnum, (*it).pitch).r*(*it).vel/64}, lineType);
                            else
                                cv::fillConvexPoly( img_playing_notes, ptos, 3, {tProp.getColour(tnum, (*it).pitch).b*(*it).vel/64, tProp.getColour(tnum, (*it).pitch).g*(*it).vel/64, tProp.getColour(tnum, (*it).pitch).r*(*it).vel/64}, lineType);
                        }
                        else // The note block is after (to the left of) the center line
                            if ( ! rProp.sep_render[1])
                                cv::fillConvexPoly( image, ptos, 3, {tProp.getColour(tnum, (*it).pitch).b*(*it).vel/512, tProp.getColour(tnum, (*it).pitch).g*(*it).vel/512, tProp.getColour(tnum, (*it).pitch).r*(*it).vel/512}, lineType);
                            else
                                cv::fillConvexPoly( img_buffer_sep_tracks[tnum], ptos, 3, {tProp.getColour(tnum, (*it).pitch).b*(*it).vel/512, tProp.getColour(tnum, (*it).pitch).g*(*it).vel/512, tProp.getColour(tnum, (*it).pitch).r*(*it).vel/512}, lineType);
                    }




                    // -------------------------- Draw Triangles |> (forte-piano) -------------------------
                    if ((*it).track == tnum && tProp.active[tnum] == true && tProp.shape[tnum] == 17) // All tracks - rectangle
                    {
                        x3 = (float)window_width*((-(float)startMidiTime + (float)(*it).t_middle)/((float)endMidiTime - (float)startMidiTime));
                        y3 = (float)window_height/2 - (float)window_height*((float)((*it).pitch + 0.5 - ((float)mdt.PitchMin + mdt.PitchMax)/2)/((float)mdt.PitchMax - (float)mdt.PitchMin))*rProp.vertRange/50.0 - rProp.vertShift;
                        pt1.y = f2int_safe(y1);
                        pt2.y = f2int_safe(y2);
                        pt3.x = f2int_safe(x2);
                        pt3.y = f2int_safe(y3);
                        cv::Point ptos[3];
                        ptos[0] = cv::Point(pt1.x, pt1.y); ptos[1] = pt3; ptos[2] = cv::Point(pt1.x, pt2.y);
                        if (pt1.x > window_width/2) // The note block is before (to the right of) the center line
                            if ( ! rProp.sep_render[1])
                                //cv::fillPoly( image, pts, );
                                cv::fillConvexPoly( image, ptos, 3, {tProp.getColour(tnum, (*it).pitch).b*(*it).vel*3/512, tProp.getColour(tnum, (*it).pitch).g*(*it).vel*3/512, tProp.getColour(tnum, (*it).pitch).r*(*it).vel*3/512}, lineType);
                            else
                                cv::fillConvexPoly( img_buffer_sep_tracks[tnum], ptos, 3, {tProp.getColour(tnum, (*it).pitch).b*(*it).vel*3/512, tProp.getColour(tnum, (*it).pitch).g*(*it).vel*3/512, tProp.getColour(tnum, (*it).pitch).r*(*it).vel*3/512}, lineType);
                        else if (pt1.x <= window_width/2 && pt2.x > window_width/2) // The note block is inside the center line
                        {
                            float nprogress = (float)(x2-window_width/2)/(x2-x1);
                            if ( ! rProp.sep_render[0])
                                cv::fillConvexPoly( image, ptos, 3, {nprogress*tProp.getColour(tnum, (*it).pitch).b*(*it).vel/64, nprogress*tProp.getColour(tnum, (*it).pitch).g*(*it).vel/64, nprogress*tProp.getColour(tnum, (*it).pitch).r*(*it).vel/64}, lineType);
                            else
                                cv::fillConvexPoly( img_playing_notes, ptos, 3, {nprogress*tProp.getColour(tnum, (*it).pitch).b*(*it).vel/32, nprogress*tProp.getColour(tnum, (*it).pitch).g*(*it).vel/32, nprogress*tProp.getColour(tnum, (*it).pitch).r*(*it).vel/32}, lineType);
                        }
                        else // The note block is after (to the left of) the center line
                            if ( ! rProp.sep_render[1])
                                cv::fillConvexPoly( image, ptos, 3, {tProp.getColour(tnum, (*it).pitch).b*(*it).vel/512, tProp.getColour(tnum, (*it).pitch).g*(*it).vel/512, tProp.getColour(tnum, (*it).pitch).r*(*it).vel/512}, lineType);
                            else
                                cv::fillConvexPoly( img_buffer_sep_tracks[tnum], ptos, 3, {tProp.getColour(tnum, (*it).pitch).b*(*it).vel/512, tProp.getColour(tnum, (*it).pitch).g*(*it).vel/512, tProp.getColour(tnum, (*it).pitch).r*(*it).vel/512}, lineType);
                    }



                    // -------------------------- Draw Triangles <| (piano-forte) -------------------------
                    if ((*it).track == tnum && tProp.active[tnum] == true && tProp.shape[tnum] == 18) // All tracks - rectangle
                    {
                        x3 = (float)window_width*((-(float)startMidiTime + (float)(*it).t_middle)/((float)endMidiTime - (float)startMidiTime));
                        y3 = (float)window_height/2 - (float)window_height*((float)((*it).pitch + 0.5 - ((float)mdt.PitchMin + mdt.PitchMax)/2)/((float)mdt.PitchMax - (float)mdt.PitchMin))*rProp.vertRange/50.0 - rProp.vertShift;
                        pt1.y = f2int_safe(y1);
                        pt2.y = f2int_safe(y2);
                        pt3.x = f2int_safe(x2);
                        pt3.y = f2int_safe(y3);
                        cv::Point ptos[3];
                        ptos[0] = cv::Point(pt1.x, pt3.y); ptos[1] = cv::Point(pt2.x, pt1.y); ptos[2] = cv::Point(pt2.x, pt2.y);
                        if (pt1.x > window_width/2) // The note block is before (to the right of) the center line
                            if ( ! rProp.sep_render[1])
                                //cv::fillPoly( image, pts, );
                                cv::fillConvexPoly( image, ptos, 3, {tProp.getColour(tnum, (*it).pitch).b*(*it).vel*3/512, tProp.getColour(tnum, (*it).pitch).g*(*it).vel*3/512, tProp.getColour(tnum, (*it).pitch).r*(*it).vel*3/512}, lineType);
                            else
                                cv::fillConvexPoly( img_buffer_sep_tracks[tnum], ptos, 3, {tProp.getColour(tnum, (*it).pitch).b*(*it).vel*3/512, tProp.getColour(tnum, (*it).pitch).g*(*it).vel*3/512, tProp.getColour(tnum, (*it).pitch).r*(*it).vel*3/512}, lineType);
                        else if (pt1.x <= window_width/2 && pt2.x > window_width/2) // The note block is inside the center line
                        {
                            float nprogress = 1.5 - (float)(x2-window_width/2)/(x2-x1);
                            if ( ! rProp.sep_render[0])
                                cv::fillConvexPoly( image, ptos, 3, {nprogress*tProp.getColour(tnum, (*it).pitch).b*(*it).vel/64, nprogress*tProp.getColour(tnum, (*it).pitch).g*(*it).vel/64, nprogress*tProp.getColour(tnum, (*it).pitch).r*(*it).vel/64}, lineType);
                            else
                                cv::fillConvexPoly( img_playing_notes, ptos, 3, {nprogress*tProp.getColour(tnum, (*it).pitch).b*(*it).vel/32, nprogress*tProp.getColour(tnum, (*it).pitch).g*(*it).vel/32, nprogress*tProp.getColour(tnum, (*it).pitch).r*(*it).vel/32}, lineType);
                        }
                        else // The note block is after (to the left of) the center line
                            if ( ! rProp.sep_render[1])
                                cv::fillConvexPoly( image, ptos, 3, {tProp.getColour(tnum, (*it).pitch).b*(*it).vel/512, tProp.getColour(tnum, (*it).pitch).g*(*it).vel/512, tProp.getColour(tnum, (*it).pitch).r*(*it).vel/512}, lineType);
                            else
                                cv::fillConvexPoly( img_buffer_sep_tracks[tnum], ptos, 3, {tProp.getColour(tnum, (*it).pitch).b*(*it).vel/512, tProp.getColour(tnum, (*it).pitch).g*(*it).vel/512, tProp.getColour(tnum, (*it).pitch).r*(*it).vel/512}, lineType);
                    }




                    // End shape drawings ------------------------------------------------


                    // ============= Vertical Lines from tracks =============

                    if ((*it).track == tnum && rProp.lines[3] && rProp.vlines_track_n == (int)tnum)
                    {
                        cv::line(image, cv::Point(pt1.x, window_height), cv::Point(pt1.x, 0), {rProp.vlines_colour[2]*(*it).vel/128, rProp.vlines_colour[1]*(*it).vel/128, rProp.vlines_colour[0]*(*it).vel/128});
                    }
                }
            }
        }
    }

//    pt3.x = window_width/2;
//    pt4.x = window_width/2;
//    pt3.y = window_height;
//    pt4.y = 0;
// =================== Vertical Lines =============
    if (rProp.lines[0]) // cetered line
        cv::line(image, cv::Point(window_width/2, window_height), cv::Point(window_width/2, 0), {100,100,100});
    if (rProp.lines[1]) // vertical lines from time signatures saved in the midi file
    {
        std::list<TimeSignature> tsigs = mdt.TSignatures; // for debugging
        TimeSignature tsig_next = TimeSignature(); // next physically, previous with reference to the backward loop:
        tsig_next.t_on = mdt.TotalTime + 100; // since TimeSignatures don't have a t_off time, we associate the total_time from the midi file to the very last time signature. The +100 is to force a last line t appear. +1 should already work, but this is still a mystery
        for (std::list<TimeSignature>::reverse_iterator ptsig = mdt.TSignatures.rbegin(); ptsig != mdt.TSignatures.rend(); ++ptsig) // run from last time signature to first
        {
            TimeSignature tsig = *ptsig;
            int k = 0; // for counting sub-beats and beats
            for (int i = tsig.t_on; i < tsig_next.t_on; i = i + 4*mdt.Tpq/tsig.denominator) // a sub-beat is 4*ticks_per_quarter_note/denominator
            {
                if (k % tsig.numerator == 0) // for a whole beat, we consider the numerator
                    cv::line(image, cv::Point((int)((float)window_width*((-(float)startMidiTime + (float)i)/((float)endMidiTime - (float)startMidiTime))), window_height), cv::Point((int)((double)window_width*((-(double)startMidiTime + (double)i)/((double)endMidiTime - (double)startMidiTime))), 0), {rProp.vlines_colour[2], rProp.vlines_colour[1], rProp.vlines_colour[0]});
                else
                    cv::line(image, cv::Point((int)((float)window_width*((-(float)startMidiTime + (float)i)/((float)endMidiTime - (float)startMidiTime))), window_height), cv::Point((int)((double)window_width*((-(double)startMidiTime + (double)i)/((double)endMidiTime - (double)startMidiTime))), 0), {0.6*rProp.vlines_colour[2], 0.6*rProp.vlines_colour[1], 0.6*rProp.vlines_colour[0]});
                k++;
            }
            tsig_next = tsig;
        }
    }
    if (rProp.lines[2]) // manual time signature, given measure
    {
        int denominator = std::pow(2, rProp.beat_measure_manual[1]);
        for (unsigned int i = 0; i < endMidiTime; i = i + 4*rProp.beat_measure_manual[0]*mdt.Tpq/denominator)
        {
            cv::line(image, cv::Point((int)((float)window_width*((-(float)startMidiTime + (float)i)/((float)endMidiTime - (float)startMidiTime))), window_height), cv::Point((int)((double)window_width*((-(double)startMidiTime + (double)i)/((double)endMidiTime - (double)startMidiTime))), 0), {rProp.vlines_colour[2], rProp.vlines_colour[1], rProp.vlines_colour[0]});
        }
    }


// ==================== Horizontal Lines =============
    if (rProp.hlines)
    {
        float note_height = (float)window_height*((float)(1.0)/((float)mdt.PitchMax - (float)mdt.PitchMin))*rProp.vertRange/50.0; // this is the height of a note in pixels, i.e. the vertical space between 2 midi notes in the image
        int basePitchRef = mdt.PitchMax%12; // this is the reference for the pitch
        int pitch_span = mdt.PitchMax - mdt.PitchMin; // this is the whole pitch extent from the midi file
        if(rProp.hlines_type == 0) // One line every n semitones + half shift
        {
            for (int i = 0; i < pitch_span/rProp.hlines_n + 1; i++)
            {
                int hline_y = (float)window_height/2 - note_height*(pitch_span)/2 + (note_height*i*rProp.hlines_n) - rProp.vertShift - rProp.hlines_offset*note_height + rProp.half_shift*note_height/2;
                hline_y = hline_y + basePitchRef*note_height;
                cv::line(image, cv::Point(0, hline_y), cv::Point(window_width, hline_y), {rProp.hlines_colour[2], rProp.hlines_colour[1], rProp.hlines_colour[0]});
            }
        }
        if(rProp.hlines_type == 1) // One line every semitone and a brighter line every n semitones
        {
            for (int i = 0; i < pitch_span; i++)
            {
                if (i%rProp.hlines_n == basePitchRef)
                {
                   int hline_y = (float)window_height/2 - note_height*(pitch_span)/2 + (note_height*i) - rProp.vertShift - rProp.hlines_offset*note_height - rProp.half_shift*note_height/2;
                   cv::line(image, cv::Point(0, hline_y), cv::Point(window_width, hline_y), {rProp.hlines_colour[2], rProp.hlines_colour[1], rProp.hlines_colour[0]});
                }
                else
                {
                    int hline_y = (float)window_height/2 - note_height*(pitch_span)/2 + (note_height*i) - rProp.vertShift - rProp.hlines_offset*note_height - rProp.half_shift*note_height/2;
                    cv::line(image, cv::Point(0, hline_y), cv::Point(window_width, hline_y), {rProp.hlines_colour[2]/2, rProp.hlines_colour[1]/2, rProp.hlines_colour[0]/2});
                }
            }
        }
        if(rProp.hlines_type == 2) // Keyboard
        {
            for (int i = 0; i < pitch_span + 1; i++)
            {
                short ii = (i - basePitchRef)%12;
                if (ii == 1 || ii == 3 || ii == 5 || ii == 8 || ii == 10)
                {
                    int hline_y = (float)window_height/2 - note_height*(pitch_span)/2 + (note_height*i) - rProp.vertShift - rProp.hlines_offset*note_height - rProp.half_shift*note_height/2;
                    cv::rectangle(image, cv::Point(0, hline_y + 1), cv::Point(window_width, hline_y + note_height), {rProp.hlines_colour[2], rProp.hlines_colour[1], rProp.hlines_colour[0]}, -1);
                }
            }
        }
        if(rProp.hlines_type == 3) // Inverse Keyboard
        {
            for (int i = 0; i < pitch_span + 1; i++)
            {
                short ii = (i - basePitchRef)%12;
                if (ii == 0 || ii == 2 || ii == 4 || ii == 6 || ii == 7 || ii == 9 || ii == 11)
                {
                    int hline_y = (float)window_height/2 - note_height*(pitch_span)/2 + (note_height*i) - rProp.vertShift - rProp.hlines_offset*note_height - rProp.half_shift*note_height/2;
                    cv::rectangle(image, cv::Point(0, hline_y + 1), cv::Point(window_width, hline_y + note_height), {rProp.hlines_colour[2], rProp.hlines_colour[1], rProp.hlines_colour[0]}, -1);
                }
            }
        }
    }



    // ============ Displaying note names ==============
    if (rProp.note_names && rProp.note_names_where == 0) // ToDo: create a new class for chord analysis, generate chord names, currently displaying only pitches
    {
        std::list<chordWithTime>::iterator it;
        std::list<chordWithTime>::iterator it_next;
        for (it = mdt.GChords.Chords.begin(), it_next = ++mdt.GChords.Chords.begin(); it_next!=(mdt.GChords.Chords.end()); ++it, ++it_next)
        {
            chordWithTime chordWT = *it;
            chordWithTime chordWT_next = *it_next;
            if (curr_pos_middle > chordWT_next.Start_time && (curr_pos_middle < chordWT.Start_time) && it!=mdt.GChords.Chords.begin() && it!=mdt.GChords.Chords.end())
            {
                std::string ptStr = "Pitches:";
                ptStr = chordWT.Chord.getPitchesStr(rProp.accidentalSharp);
                cv::putText(image,
                        ptStr,//"Here is some text",
                        cv::Point(10,30), // Coordinates
                        cv::FONT_HERSHEY_COMPLEX_SMALL, // Font
                        1.0, // Scale. 2.0 = 2x bigger
                        cv::Scalar(255,255,255), // BGR Color
                        1, // Line Thickness (Optional)
                        cv::LINE_AA); // Anti-alias (Optional)
            }

        }

    }

    // ============ Displaying chord names ============== ToDo

    // ============ Displaying circle / star ==============
    if (rProp.chord_star)
    {
        std::list<chordWithTime>::iterator it;
        std::list<chordWithTime>::iterator it_next;
        for (it = mdt.GChords.Chords.begin(), it_next = ++mdt.GChords.Chords.begin(); it_next!=(mdt.GChords.Chords.end()); ++it, ++it_next) // run through all chords
        {
            chordWithTime chordWT = *it;
            chordWithTime chordWT_next = *it_next;
            if (curr_pos_middle > chordWT_next.Start_time && (curr_pos_middle < chordWT.Start_time) && it!=mdt.GChords.Chords.begin() && it!=mdt.GChords.Chords.end()) // if it is the chord currently being played
            {
                int diam = 100;
                chord::circle type = rProp.chord_star_type;
                cv::Point centre = cv::Point(window_width/4, window_height/4);
                if ( ! (rProp.note_names_where == 1 && rProp.note_names))
                {
                    dispChordDisc(type, image, centre, diam, false, rProp.turn_chord_circle, rProp.accidentalSharp);
                }
                else if (rProp.note_names_where == 1 && rProp.note_names)
                {
                    dispChordDisc(type, image, centre, diam, true, rProp.turn_chord_circle, rProp.accidentalSharp);
                }
                chord currChord = chordWT.Chord;
                renderChordStar(currChord, type, image, centre, diam, rProp.turn_chord_circle);
            }

        }

    }

    // =========== Including separate layers ==========
    if (rProp.sep_render[0])
    {
        if (rProp.blur_size[0] > 0 && rProp.blur_size[1] > 0)
            cv::boxFilter(img_playing_notes, img_playing_notes, -1, cv::Size(rProp.blur_size[0], rProp.blur_size[1]));
        image = image + img_playing_notes;
    }
    if (rProp.sep_render[1])
    {
        for (short j = 0; j < (mdt.NTracks); j++)
        {
            if (tProp.track_blur[j] > 0)
                cv::boxFilter(img_buffer_sep_tracks[j], img_buffer_sep_tracks[j], -1, cv::Size(tProp.track_blur[j], tProp.track_blur[j]));
            image = image + img_buffer_sep_tracks[j];
            img_buffer_sep_tracks[j] = cv::Mat::zeros(window_height, window_width, CV_8UC3);
            //cv::add(image, img_buffer_sep_tracks[j], image); // this should do the same job as image = image + img_buffer_sep_tracks[j];

        }
    }
    //img_buffer_sep_tracks.clear();
    if (rProp.sep_render[2])
    {
        if (rProp.blur_size_movnotes[0] > 0 && rProp.blur_size_movnotes[1] > 0)
            cv::boxFilter(img_moving_notes, img_moving_notes, -1, cv::Size(rProp.blur_size_movnotes[0], rProp.blur_size_movnotes[1]));
        image = image + img_moving_notes;
    }
    if (vRec->RecordVideo == 1 && vRec != nullptr)
    {
        vRec->writeFrame(image);
    }
}
