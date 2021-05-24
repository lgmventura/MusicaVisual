#include "animation.h"

RenderP::RenderP()
{

}

AnimPainter::AnimPainter(RenderBuffer *rBuffer)
{
    this->RBuffer = rBuffer;
}

void AnimPainter::paintBlocksNoShading(cv::Mat image, MusicData mdt, ShapePoints spts, char* window_name, AnimWindow aw)
{
    //std::cout << "Paint blocks! " << Mdt->PitchMin << ' ' << pitch_max << endl;
    for (std::list<MidiNote>::iterator it=mdt.Notes.begin() ; it != mdt.Notes.end(); ++it) // Run the list forwards
    {
        spts.x1 = (double)aw.Width*((double)(*it).t_on/((double)aw.EndMidiTime - (double)aw.StartMidiTime));
        spts.x2 = (double)aw.Width*((double)(*it).t_off/((double)aw.EndMidiTime - (double)aw.StartMidiTime));
        spts.y1 = (double)aw.Height - (double)aw.Height*((double)(*it).pitch/((double)mdt.PitchMax - (double)mdt.PitchMin + 20.0));
        spts.y2 = (double)aw.Height - (double)aw.Height*(((double)(*it).pitch + 1.0)/((double)mdt.PitchMax - (double)mdt.PitchMin + 20.0));
        spts.pt1.x = (int)(spts.x1); //aw.Width*((*it).t_on/(aw.EndMidiTime - aw.StartMidiTime));
        spts.pt2.x = (int)(spts.x2); //aw.Width*((*it).t_off/(aw.EndMidiTime - aw.StartMidiTime));
        spts.pt1.y = (int)(spts.y1); //aw.Height*((*it).pitch/(50));
        spts.pt2.y = (int)(spts.y2);//aw.Height*(((*it).pitch + 1)/(50));
        //std::cout << (*it).t_on << ' ' << (*it).t_off << ": " << (*it).pitch << '\n';
        //std::cout << spts.pt1.x << ' ' << spts.pt1.y << ": " << spts.pt2.x << ' ' << spts.pt2.y << '\n';
        cv::rectangle( image, spts.pt1, spts.pt2, {(double)(*it).vel,(double)(*it).vel,(double)(*it).vel}, 2, 8 );
    }
}

void AnimPainter::paintNotes(MusicData mdt, cv::Mat image, std::vector <cv::Mat> img_buffer_sep_tracks, AnimWindow aw, BlockLayers blockL, ChordLayers chordL, RenderP rProp, LayerContainer::LayerType ltype) // this function is called for every frame. aw.StartMidiTime is the time in the left side of the window, aw.EndMidiTime, at the right. These depend on playback position and zoom.
{
    // this function loops through all notes and call corresponding functions to paint them
    // finding current global zoom:
    int hZoomGlob = aw.EndMidiTime - aw.StartMidiTime;

    // finding current playback position:
    aw.CurrPosMiddle = (aw.StartMidiTime + (hZoomGlob)/2);

    // since all points are calculated taking into account aw.StartMidiTime (where points appear on screen) aw.EndMidiTime (where points disappear from screen), let's recalculate them, now considering the hZoomMultiplier:
    double hZoom = hZoomGlob*blockL.hZoomMult;
    // smt = cpm - hzg/2 -> smt = cpm - emt/2 + smt/2 -> smt = 2*cpm - emt -> smt = 2*cpm - hzg - smt -> smt = cpm - hzg/2
    // emt = smt + hzg -> emt = cpm + hzg/2
    aw.StartMidiTime = aw.CurrPosMiddle - (int)hZoom/2;
    aw.EndMidiTime = aw.CurrPosMiddle + (int)hZoom/2;

    // Calculating vertical zoom:
    aw.VZoom = blockL.vZoomMult*rProp.vertRange/50.0;

    // declaring points used to create geometry:
    ShapePoints spts;
    int x_max = aw.Width + 5000, y_max = aw.Height + 50;
    //int max_duration = window_width + 1000;
    //std::cout << "Paint blocks! " << Mdt->PitchMin << ' ' << pitch_max << endl;
    cv::Mat img_playing_notes = cv::Mat::zeros(aw.Height, aw.Width, CV_8UC3 );
    cv::Mat img_moving_notes = cv::Mat::zeros(aw.Height, aw.Width, CV_8UC3 );
    //cv::Mat img_buffer_sep_tracks[tracks_count] = cv::Mat::zeros( aw.Height, window_width, CV_8UC3 ); // Qt 5.7, OpenCV 2.4 (which uses C++98)
    //cv::Mat img_buffer_sep_tracks[tracks_count] = {cv::Mat::zeros( aw.Height, window_width, CV_8UC3 )}; // this does not solve for Qt 5.9, OpenCV 4.0 (which uses C++11)
    // http://answers.opencv.org/question/31665/creating-an-array-of-mats-of-size-n/ - Static arrays need the size for their construction at compile time. If you want to have the size controlled at run-time, you need to either create the mat-array via new or use std::vector (I'd prefer the latter one, since when using new you'll also need to call delete[] and it also prevents you from writing at non existent memory)
    //std::vector <cv::Mat> img_buffer_sep_tracks(tracks_count, cv::Mat::zeros( aw.Height, window_width, CV_8UC3 ));// this does not solve for Qt 5.9, OpenCV 4.0 (which uses C++11), see comments below
    // now, if you create a vector of cv::Mat objects, you are actually creating a vector of pointers to the SAME matrix. cv::Mat is not a matrix, but rather a pointer to one.
    // Solving this on the generation of the animation window
    cv::LineTypes lineType = rProp.shapeLineType;

    // for chord layer:
    int p_x = chordL.x_pos*aw.Width;
    int p_y = chordL.y_pos*aw.Height;

    cv::Point clCentre = cv::Point(p_x, p_y);

    TonnetzOptions opt(chordL.TonnetzShape, chordL.ActiveTracks, chordL.HexLayout);
    opt.Central = chordL.CentralMidi;
    opt.NoteSize = chordL.NoteSize;
    opt.NoteCollapse = chordL.NoteCollapse;
    opt.NoteFadeOut = chordL.NoteFadeOut;

    // iterating over all notes:
    for (std::list<MidiNote>::iterator it=mdt.Notes.begin() ; it != mdt.Notes.end(); ++it) // Run the list forwards
    {
        // taking only notes inside the visible area (tolerance of 50 ticks outside the screen)
        if ((*it).is_note == 1 && aw.StartMidiTime -50 < (*it).t_off && aw.EndMidiTime + 50 > (*it).t_on) // is_note checks if it's a real note to avoid getting trash.
        {

            spts.x1 = (float)aw.Width*((-(float)aw.StartMidiTime + (float)(*it).t_on)/((float)aw.EndMidiTime - (float)aw.StartMidiTime)); // note_on time
            spts.x2 = (float)aw.Width*((-(float)aw.StartMidiTime + (float)(*it).t_off)/((float)aw.EndMidiTime - (float)aw.StartMidiTime)); // note_off time
            spts.y1 = (float)aw.Height/2 - (float)aw.Height*((float)((*it).pitch - ((float)mdt.PitchMin + mdt.PitchMax)/2)/((float)mdt.PitchMax - (float)mdt.PitchMin))*aw.VZoom - rProp.vertShift;
            spts.y2 = (float)aw.Height/2 - (float)aw.Height*((float)((*it).pitch + 1.0 - ((float)mdt.PitchMin + mdt.PitchMax)/2)/((float)mdt.PitchMax - (float)mdt.PitchMin))*aw.VZoom - rProp.vertShift;
            spts.pt1.x = f2int_safe(spts.x1); //aw.Width*((*it).t_on/(aw.EndMidiTime - aw.StartMidiTime));
            spts.pt2.x = f2int_safe(spts.x2); //aw.Width*((*it).t_off/(aw.EndMidiTime - aw.StartMidiTime));
            spts.pt1.y = f2int_safe(spts.y1); //aw.Height*((*it).pitch/(50));
            spts.pt2.y = f2int_safe(spts.y2);//aw.Height*(((*it).pitch + 1)/(50));
            //std::cout << (*it).t_on << ' ' << (*it).t_off << ": " << (*it).pitch << '\n';
            //std::cout << spts.pt1.x << ' ' << spts.pt1.y << ": " << spts.pt2.x << ' ' << spts.pt2.y << '\n';
            if (ltype == LayerContainer::BlockLayer)
            {
                for (unsigned int tnum = 0; tnum < mdt.NTracks; tnum++)
                {
                    // -------------------------- Draw Interconnected Lines -------------------------
                    if ((*it).track == tnum && blockL.ActiveTracks[tnum] == true && blockL.interconnect[tnum] == 1) // All tracks - rectangle
                    {
                        spts.x3 = (float)aw.Width*((-(float)aw.StartMidiTime + (float)(*it).t_middle)/((float)aw.EndMidiTime - (float)aw.StartMidiTime));
                        spts.y3 = (float)aw.Height/2 - (float)aw.Height*((float)((*it).pitch + 0.5 - ((float)mdt.PitchMin + mdt.PitchMax)/2)/((float)mdt.PitchMax - (float)mdt.PitchMin))*aw.VZoom - rProp.vertShift;
                        spts.pt3.x = f2int_safe(spts.x3); // Center x
                        spts.pt3.y = f2int_safe(spts.y3); // Center y
                        if (abs(spts.pt3.x - spts.pt5[tnum].x) < rProp.max_connect_dist && spts.pt5[tnum] != cv::Point(0,0))
                            if ( ! rProp.sep_render[1])
                                cv::line( image, spts.pt3, spts.pt5[tnum], {(double)blockL.getCv(tnum,2)/4,  (double)blockL.getCv(tnum,1)/4,  (double)blockL.getCv(tnum,0)/4}, 1, lineType );
                            else
                                cv::line( img_buffer_sep_tracks[tnum], spts.pt3, spts.pt5[tnum], {(double)blockL.getCv(tnum,2)/4,  (double)blockL.getCv(tnum,1)/4,  (double)blockL.getCv(tnum,0)/4}, 1, lineType );
                        spts.pt5[tnum].x = f2int_safe(spts.x3); // Center x of the previous note
                        spts.pt5[tnum].y = f2int_safe(spts.y3); // Center y of the previous note
                    }



                    // ------------------------- Draw Rectangles -------------------------
                    if ((*it).track == tnum && blockL.ActiveTracks[tnum] == true && blockL.shape[tnum] == 0) // All tracks - rectangle
                    {
                        if (spts.pt1.x > aw.Width/2) // The note block is before (to the right of) the center line
                            if ( ! rProp.sep_render[1])
                                cv::rectangle( image, spts.pt1, spts.pt2, {blockL.getColour(tnum, (*it).pitch).b*(*it).vel*3/512, blockL.getColour(tnum, (*it).pitch).g*(*it).vel*3/512, blockL.getColour(tnum, (*it).pitch).r*(*it).vel*3/512}, -1, lineType );
                            else
                                cv::rectangle( img_buffer_sep_tracks[tnum], spts.pt1, spts.pt2, {blockL.getColour(tnum, (*it).pitch).b*(*it).vel*3/512, blockL.getColour(tnum, (*it).pitch).g*(*it).vel*3/512, blockL.getColour(tnum, (*it).pitch).r*(*it).vel*3/512}, -1, lineType );
                        else if (spts.pt1.x <= aw.Width/2 && spts.pt2.x > aw.Width/2) // The note block is inside the center line
                        {
                            if ( ! rProp.sep_render[0])
                                cv::rectangle( image, spts.pt1, spts.pt2, {blockL.getColour(tnum, (*it).pitch).b*(*it).vel/64, blockL.getColour(tnum, (*it).pitch).g*(*it).vel/64, blockL.getColour(tnum, (*it).pitch).r*(*it).vel/64}, -1, lineType );
                            else
                                cv::rectangle( img_playing_notes, spts.pt1, spts.pt2, {blockL.getColour(tnum, (*it).pitch).b*(*it).vel/64, blockL.getColour(tnum, (*it).pitch).g*(*it).vel/64, blockL.getColour(tnum, (*it).pitch).r*(*it).vel/64}, -1, lineType );
                        }
                        else // The note block is after (to the left of) the center line
                            if ( ! rProp.sep_render[1])
                                cv::rectangle( image, spts.pt1, spts.pt2, {blockL.getColour(tnum, (*it).pitch).b*(*it).vel/512, blockL.getColour(tnum, (*it).pitch).g*(*it).vel/512, blockL.getColour(tnum, (*it).pitch).r*(*it).vel/512}, 2, lineType );
                            else
                                cv::rectangle( img_buffer_sep_tracks[tnum], spts.pt1, spts.pt2, {blockL.getColour(tnum, (*it).pitch).b*(*it).vel/512, blockL.getColour(tnum, (*it).pitch).g*(*it).vel/512, blockL.getColour(tnum, (*it).pitch).r*(*it).vel/512}, 2, lineType );
                    }


                    // -------------------------- Draw Rhombus -------------------------
                    if ((*it).track == tnum && blockL.ActiveTracks[tnum] == true && blockL.shape[tnum] == 1) // All tracks - rectangle
                    {
                        spts.x3 = (float)aw.Width*((-(float)aw.StartMidiTime + (float)(*it).t_middle)/((float)aw.EndMidiTime - (float)aw.StartMidiTime));
                        spts.y3 = (float)aw.Height/2 - (float)aw.Height*((float)((*it).pitch + 0.5 - ((float)mdt.PitchMin + mdt.PitchMax)/2)/((float)mdt.PitchMax - (float)mdt.PitchMin))*aw.VZoom - rProp.vertShift;
                        spts.pt1.y = f2int_safe(spts.y3);
                        spts.pt2.y = f2int_safe(spts.y3);
                        spts.pt3.x = f2int_safe(spts.x3);
                        spts.pt3.y = f2int_safe(spts.y1);
                        spts.pt4.x = f2int_safe(spts.x3);
                        spts.pt4.y = f2int_safe(spts.y2);
                        cv::Point ptos[4];
                        ptos[0] = spts.pt1; ptos[1] = spts.pt3; ptos[2] = spts.pt2; ptos[3] = spts.pt4;
                        if (spts.pt1.x > aw.Width/2) // The note block is before (to the right of) the center line
                            if ( ! rProp.sep_render[1])
                                //cv::fillPoly( image, pts, );
                                cv::fillConvexPoly( image, ptos, 4, {blockL.getColour(tnum, (*it).pitch).b*(*it).vel*3/512, blockL.getColour(tnum, (*it).pitch).g*(*it).vel*3/512, blockL.getColour(tnum, (*it).pitch).r*(*it).vel*3/512}, lineType );
                            else
                                cv::fillConvexPoly( img_buffer_sep_tracks[tnum], ptos, 4, {blockL.getColour(tnum, (*it).pitch).b*(*it).vel*3/512, blockL.getColour(tnum, (*it).pitch).g*(*it).vel*3/512, blockL.getColour(tnum, (*it).pitch).r*(*it).vel*3/512}, lineType );
                        else if (spts.pt1.x <= aw.Width/2 && spts.pt2.x > aw.Width/2) // The note block is inside the center line
                        {
                            if ( ! rProp.sep_render[0])
                                cv::fillConvexPoly( image, ptos, 4, {blockL.getColour(tnum, (*it).pitch).b*(*it).vel/64, blockL.getColour(tnum, (*it).pitch).g*(*it).vel/64, blockL.getColour(tnum, (*it).pitch).r*(*it).vel/64}, lineType );
                            else
                                cv::fillConvexPoly( img_playing_notes, ptos, 4, {blockL.getColour(tnum, (*it).pitch).b*(*it).vel/64, blockL.getColour(tnum, (*it).pitch).g*(*it).vel/64, blockL.getColour(tnum, (*it).pitch).r*(*it).vel/64}, lineType );
                        }
                        else // The note block is after (to the left of) the center line
                            if ( ! rProp.sep_render[1])
                                cv::fillConvexPoly( image, ptos, 4, {blockL.getColour(tnum, (*it).pitch).b*(*it).vel/512, blockL.getColour(tnum, (*it).pitch).g*(*it).vel/512, blockL.getColour(tnum, (*it).pitch).r*(*it).vel/512}, lineType );
                            else
                                cv::fillConvexPoly( img_buffer_sep_tracks[tnum], ptos, 4, {blockL.getColour(tnum, (*it).pitch).b*(*it).vel/512, blockL.getColour(tnum, (*it).pitch).g*(*it).vel/512, blockL.getColour(tnum, (*it).pitch).r*(*it).vel/512}, lineType );
                    }


                    // -------------------------- Draw Ellipses -------------------------
                    if ((*it).track == tnum && blockL.ActiveTracks[tnum] == true && blockL.shape[tnum] == 2) // All tracks - rectangle
                    {
                        spts.x3 = (float)aw.Width*((-(float)aw.StartMidiTime + (float)(*it).t_middle)/((float)aw.EndMidiTime - (float)aw.StartMidiTime));
                        spts.y3 = (float)aw.Height/2 - (float)aw.Height*((float)((*it).pitch + 0.5 - ((float)mdt.PitchMin + mdt.PitchMax)/2)/((float)mdt.PitchMax - (float)mdt.PitchMin))*aw.VZoom - rProp.vertShift;
                        spts.pt3.x = f2int_safe(spts.x3); // Center x
                        spts.pt3.y = f2int_safe(spts.y3); // Center y
                        if (spts.pt1.x > aw.Width/2) // The note block is before (to the right of) the center line
                            if ( ! rProp.sep_render[1])
                                //cv::fillPoly( image, pts, );
                                cv::ellipse( image, spts.pt3, cv::Size(spts.x3-spts.x1, spts.y3-spts.y2), 0, 0, 360, {blockL.getColour(tnum, (*it).pitch).b*(*it).vel*3/512, blockL.getColour(tnum, (*it).pitch).g*(*it).vel*3/512, blockL.getColour(tnum, (*it).pitch).r*(*it).vel*3/512}, -1, lineType);
                            else
                                cv::ellipse( img_buffer_sep_tracks[tnum], spts.pt3, cv::Size(spts.x3-spts.x1, spts.y3-spts.y2), 0, 0, 360, {blockL.getColour(tnum, (*it).pitch).b*(*it).vel*3/512, blockL.getColour(tnum, (*it).pitch).g*(*it).vel*3/512, blockL.getColour(tnum, (*it).pitch).r*(*it).vel*3/512}, -1, lineType);
                        else if (spts.pt1.x <= aw.Width/2 && spts.pt2.x > aw.Width/2) // The note block is inside the center line
                        {
                            if ( ! rProp.sep_render[0])
                                cv::ellipse( image, spts.pt3, cv::Size(spts.x3-spts.x1, spts.y3-spts.y2), 0, 0, 360, {blockL.getColour(tnum, (*it).pitch).b*(*it).vel/64, blockL.getColour(tnum, (*it).pitch).g*(*it).vel/64, blockL.getColour(tnum, (*it).pitch).r*(*it).vel/64}, -1, lineType);
                            else
                                cv::ellipse( img_playing_notes, spts.pt3, cv::Size(spts.x3-spts.x1, spts.y3-spts.y2), 0, 0, 360, {blockL.getColour(tnum, (*it).pitch).b*(*it).vel/64, blockL.getColour(tnum, (*it).pitch).g*(*it).vel/64, blockL.getColour(tnum, (*it).pitch).r*(*it).vel/64}, -1, lineType);
                        }
                        else // The note block is after (to the left of) the center line
                            if ( ! rProp.sep_render[1])
                                cv::ellipse( image, spts.pt3, cv::Size(spts.x3-spts.x1, spts.y3-spts.y2), 0, 0, 360, {blockL.getColour(tnum, (*it).pitch).b*(*it).vel/512, blockL.getColour(tnum, (*it).pitch).g*(*it).vel/512, blockL.getColour(tnum, (*it).pitch).r*(*it).vel/512}, 2, lineType);
                            else
                                cv::ellipse( img_buffer_sep_tracks[tnum], spts.pt3, cv::Size(spts.x3-spts.x1, spts.y3-spts.y2), 0, 0, 360, {blockL.getColour(tnum, (*it).pitch).b*(*it).vel/512, blockL.getColour(tnum, (*it).pitch).g*(*it).vel/512, blockL.getColour(tnum, (*it).pitch).r*(*it).vel/512}, 2, lineType);
                    }


                    // -------------------------- Draw Circles -------------------------
                    if ((*it).track == tnum && blockL.ActiveTracks[tnum] == true && blockL.shape[tnum] == 3) // All tracks
                    {
                        spts.x3 = (float)aw.Width*((-(float)aw.StartMidiTime + (float)(*it).t_middle)/((float)aw.EndMidiTime - (float)aw.StartMidiTime));
                        spts.y3 = (float)aw.Height/2 - (float)aw.Height*((float)((*it).pitch + 0.5 - ((float)mdt.PitchMin + mdt.PitchMax)/2)/((float)mdt.PitchMax - (float)mdt.PitchMin))*aw.VZoom - rProp.vertShift;
                        spts.pt3.x = f2int_safe(spts.x3); // Center x
                        spts.pt3.y = f2int_safe(spts.y3); // Center y
                        int radius = (spts.x3-spts.x1) < 80 ? (spts.x3-spts.x1) : 80;
                        if (spts.pt1.x > aw.Width/2) // The note block is before (to the right of) the center line
                            if ( ! rProp.sep_render[1])
                                //cv::fillPoly( image, pts, );
                                cv::circle( image, spts.pt3, radius, {blockL.getColour(tnum, (*it).pitch).b*(*it).vel*3/512, blockL.getColour(tnum, (*it).pitch).g*(*it).vel*3/512, blockL.getColour(tnum, (*it).pitch).r*(*it).vel*3/512}, -1, lineType );
                            else
                                cv::circle( img_buffer_sep_tracks[tnum], spts.pt3, radius, {blockL.getColour(tnum, (*it).pitch).b*(*it).vel*3/512, blockL.getColour(tnum, (*it).pitch).g*(*it).vel*3/512, blockL.getColour(tnum, (*it).pitch).r*(*it).vel*3/512}, -1, lineType );
                        else if (spts.pt1.x <= aw.Width/2 && spts.pt2.x > aw.Width/2) // The note block is inside the center line
                        {
                            if ( ! rProp.sep_render[0])
                                cv::circle( image, spts.pt3, (spts.x2-aw.Width/2+3), {blockL.getColour(tnum, (*it).pitch).b*(*it).vel/64, blockL.getColour(tnum, (*it).pitch).g*(*it).vel/64, blockL.getColour(tnum, (*it).pitch).r*(*it).vel/64}, -1, lineType );
                            else
                                cv::circle( img_playing_notes, spts.pt3, (spts.x2-aw.Width/2+1), {blockL.getColour(tnum, (*it).pitch).b*(*it).vel/64, blockL.getColour(tnum, (*it).pitch).g*(*it).vel/64, blockL.getColour(tnum, (*it).pitch).r*(*it).vel/64}, -1, lineType );
                        }
                        else // The note block is after (to the left of) the center line
                            if ( ! rProp.sep_render[1])
                                cv::circle( image, spts.pt3, radius, {blockL.getColour(tnum, (*it).pitch).b*(*it).vel/512, blockL.getColour(tnum, (*it).pitch).g*(*it).vel/512, blockL.getColour(tnum, (*it).pitch).r*(*it).vel/512}, 2, lineType );
                            else
                                cv::circle( img_buffer_sep_tracks[tnum], spts.pt3, radius, {blockL.getColour(tnum, (*it).pitch).b*(*it).vel/512, blockL.getColour(tnum, (*it).pitch).g*(*it).vel/512, blockL.getColour(tnum, (*it).pitch).r*(*it).vel/512}, 2, lineType );
                    }





                    // -------------------------- Draw Circles with moving centred circle -------------------------
                    if ((*it).track == tnum && blockL.ActiveTracks[tnum] == true && blockL.shape[tnum] == 4) // All tracks
                    {
                        spts.x3 = (float)aw.Width*((-(float)aw.StartMidiTime + (float)(*it).t_middle)/((float)aw.EndMidiTime - (float)aw.StartMidiTime));
                        spts.y3 = (float)aw.Height/2 - (float)aw.Height*((float)((*it).pitch + 0.5 - ((float)mdt.PitchMin + mdt.PitchMax)/2)/((float)mdt.PitchMax - (float)mdt.PitchMin))*aw.VZoom - rProp.vertShift;
                        spts.pt3.x = f2int_safe(spts.x3); // Center x
                        spts.pt3.y = f2int_safe(spts.y3); // Center y
                        int radius = (spts.x3-spts.x1) < 80 ? (spts.x3-spts.x1) : 80;
                        if (spts.pt1.x > aw.Width/2) // The note block is before (to the right of) the center line
                            if ( ! rProp.sep_render[1])
                                //cv::fillPoly( image, pts, );
                                cv::circle( image, spts.pt3, radius, {blockL.getColour(tnum, (*it).pitch).b*(*it).vel*3/512, blockL.getColour(tnum, (*it).pitch).g*(*it).vel*3/512, blockL.getColour(tnum, (*it).pitch).r*(*it).vel*3/512}, -1, lineType);
                            else
                                cv::circle( img_buffer_sep_tracks[tnum], spts.pt3, radius, {blockL.getColour(tnum, (*it).pitch).b*(*it).vel*3/512, blockL.getColour(tnum, (*it).pitch).g*(*it).vel*3/512, blockL.getColour(tnum, (*it).pitch).r*(*it).vel*3/512}, -1, lineType);

                        if (spts.pt1.x < aw.Width/2) // The note block is after (to the left of) the center line
                            if ( ! rProp.sep_render[1])
                                cv::circle( image, spts.pt3, radius, {blockL.getColour(tnum, (*it).pitch).b*(*it).vel/512, blockL.getColour(tnum, (*it).pitch).g*(*it).vel/512, blockL.getColour(tnum, (*it).pitch).r*(*it).vel/512}, 2, lineType);
                            else
                                cv::circle( img_buffer_sep_tracks[tnum], spts.pt3, radius, {blockL.getColour(tnum, (*it).pitch).b*(*it).vel/512, blockL.getColour(tnum, (*it).pitch).g*(*it).vel/512, blockL.getColour(tnum, (*it).pitch).r*(*it).vel/512}, 2, lineType);


                        if (spts.pt1_prev[tnum].x <= aw.Width/2 && spts.pt1.x > aw.Width/2 && spts.pt2_prev[tnum].x + 5 > aw.Width/2) // The note block is inside the center line // no more else if
                            // this section corresponds to the "moving notes", i.e., the notes being played morphing from the current to the last (or from the next to the current if you prefer to say).
                            // In general, it works averaging current position and size with the last ones. A very important detail is to store the previous points p1_prev, spts.pt2_prev, ... in the
                            // corresponding track (so, spts.pt1_prev[tnum], spts.pt2_prev[tnum] etc.), because we don't want the very last note as previous note, but the previous note of the same track!
                        {
                            float deriv = (((float)aw.Width/2 - (float)spts.pt1_prev[tnum].x)/((float)spts.pt1.x - (float)spts.pt1_prev[tnum].x));
                            int y_mov = (int) ((float)spts.pt3_prev[tnum].y + deriv*((float)spts.pt3.y - (float)spts.pt3_prev[tnum].y));
                            int x_mov = aw.Width/2 + spts.pt3_prev[tnum].x - spts.pt1_prev[tnum].x + deriv*((spts.pt3.x - spts.pt1.x) - (spts.pt3_prev[tnum].x - spts.pt1_prev[tnum].x));
                            // Case 1: the distance to the next note is big (not a legato) -> note just fall off
                            if (spts.pt1.x - spts.pt2_prev[tnum].x > 5) // next_note t_on and old note t_off are far
                                if ( ! rProp.sep_render[2])
                                    cv::circle( image, spts.pt3_prev[tnum], floor1(spts.pt2_prev[tnum].x-aw.Width/2+1), {blockL.getColour(tnum, (*it).pitch).b*(*it).vel/64, blockL.getColour(tnum, (*it).pitch).g*(*it).vel/64, blockL.getColour(tnum, (*it).pitch).r*(*it).vel/64}, -1, lineType);
                                else
                                    cv::circle( img_moving_notes, spts.pt3_prev[tnum], floor1(spts.pt2_prev[tnum].x-aw.Width/2+1), {blockL.getColour(tnum, (*it).pitch).b*(*it).vel/64, blockL.getColour(tnum, (*it).pitch).g*(*it).vel/64, blockL.getColour(tnum, (*it).pitch).r*(*it).vel/64}, -1, lineType);
                            // Case 2: the end of the current note (prev) is close to the begin of the new note (legato) -> notes connect themselves
                            else
                                if ( ! rProp.sep_render[2])
                                    cv::circle( image, cv::Point(x_mov, y_mov), floor1(spts.RadiusPrev[tnum] + deriv*((float)radius - (float)spts.RadiusPrev[tnum])), {blockL.getColour(tnum, (*it).pitch).b*(*it).vel/64, blockL.getColour(tnum, (*it).pitch).g*(*it).vel/64, blockL.getColour(tnum, (*it).pitch).r*(*it).vel/64}, -1, lineType);
                                else
                                    cv::circle( img_moving_notes, cv::Point(x_mov, y_mov), floor1(spts.RadiusPrev[tnum] + deriv*((float)radius - (float)spts.RadiusPrev[tnum])), {blockL.getColour(tnum, (*it).pitch).b*(*it).vel/64, blockL.getColour(tnum, (*it).pitch).g*(*it).vel/64, blockL.getColour(tnum, (*it).pitch).r*(*it).vel/64}, -1, lineType);

                        }

                        spts.pt3_prev[tnum] = spts.pt3; // Next centre point = previous
                        spts.pt1_prev[tnum] = spts.pt1;
                        spts.pt2_prev[tnum] = spts.pt2;
                        spts.RadiusPrev[tnum] = radius;
                    }



                    // -------------------------- Draw Circles with moving centred circle and playing note highlight -------------------------
                    if ((*it).track == tnum && blockL.ActiveTracks[tnum] == true && blockL.shape[tnum] == 5) // All tracks
                    {
                        spts.x3 = (float)aw.Width*((-(float)aw.StartMidiTime + (float)(*it).t_middle)/((float)aw.EndMidiTime - (float)aw.StartMidiTime));
                        spts.y3 = (float)aw.Height/2 - (float)aw.Height*((float)((*it).pitch + 0.5 - ((float)mdt.PitchMin + mdt.PitchMax)/2)/((float)mdt.PitchMax - (float)mdt.PitchMin))*aw.VZoom - rProp.vertShift;
                        spts.pt3.x = f2int_safe(spts.x3); // Center x
                        spts.pt3.y = f2int_safe(spts.y3); // Center y
                        int radius = (spts.x3-spts.x1) < 80 ? (spts.x3-spts.x1) : 80;
                        if (spts.pt1.x > aw.Width/2) // The note block is before (to the right of) the center line
                            if ( ! rProp.sep_render[1])
                                //cv::fillPoly( image, pts, );
                                cv::circle( image, spts.pt3, radius, {blockL.getColour(tnum, (*it).pitch).b*(*it).vel*3/512, blockL.getColour(tnum, (*it).pitch).g*(*it).vel*3/512, blockL.getColour(tnum, (*it).pitch).r*(*it).vel*3/512}, -1, lineType);
                            else
                                cv::circle( img_buffer_sep_tracks[tnum], spts.pt3, radius, {blockL.getColour(tnum, (*it).pitch).b*(*it).vel*3/512, blockL.getColour(tnum, (*it).pitch).g*(*it).vel*3/512, blockL.getColour(tnum, (*it).pitch).r*(*it).vel*3/512}, -1, lineType);
                        else if (spts.pt1.x <= aw.Width/2 && spts.pt2.x > aw.Width/2) // The note block is inside the center line
                        {
                            if ( ! rProp.sep_render[0])
                                cv::circle( image, spts.pt3, radius, {blockL.getColour(tnum, (*it).pitch).b*(*it).vel/64, blockL.getColour(tnum, (*it).pitch).g*(*it).vel/64, blockL.getColour(tnum, (*it).pitch).r*(*it).vel/64}, 1, lineType);
                            else
                                cv::circle( img_playing_notes, spts.pt3, radius, {blockL.getColour(tnum, (*it).pitch).b*(*it).vel/64, blockL.getColour(tnum, (*it).pitch).g*(*it).vel/64, blockL.getColour(tnum, (*it).pitch).r*(*it).vel/64}, 2, lineType);
                        }
                        else // The note block is after (to the left of) the center line
                            if ( ! rProp.sep_render[1])
                                cv::circle( image, spts.pt3, radius, {blockL.getColour(tnum, (*it).pitch).b*(*it).vel/512, blockL.getColour(tnum, (*it).pitch).g*(*it).vel/512, blockL.getColour(tnum, (*it).pitch).r*(*it).vel/512}, 2, lineType);
                            else
                                cv::circle( img_buffer_sep_tracks[tnum], spts.pt3, radius, {blockL.getColour(tnum, (*it).pitch).b*(*it).vel/512, blockL.getColour(tnum, (*it).pitch).g*(*it).vel/512, blockL.getColour(tnum, (*it).pitch).r*(*it).vel/512}, 2, lineType);


                        if (spts.pt1_prev[tnum].x <= aw.Width/2 && spts.pt1.x > aw.Width/2 && spts.pt2_prev[tnum].x + 5 > aw.Width/2) // The note block is inside the center line // no more else if
                        {
                            float deriv = (((float)aw.Width/2 - (float)spts.pt1_prev[tnum].x)/((float)spts.pt1.x - (float)spts.pt1_prev[tnum].x));
                            int y_mov = (int) ((float)spts.pt3_prev[tnum].y + deriv*((float)spts.pt3.y - (float)spts.pt3_prev[tnum].y));
                            int x_mov = aw.Width/2 + spts.pt3_prev[tnum].x - spts.pt1_prev[tnum].x + deriv*((spts.pt3.x - spts.pt1.x) - (spts.pt3_prev[tnum].x - spts.pt1_prev[tnum].x));
                            // Case 1: the distance to the next note is big (not a legato) -> note just fall off
                            if (spts.pt1.x - spts.pt2_prev[tnum].x > 5) // next_note t_on and old note t_off are far
                                if ( ! rProp.sep_render[2])
                                    cv::circle( image, spts.pt3_prev[tnum], floor1(spts.pt2_prev[tnum].x-aw.Width/2+1), {blockL.getColour(tnum, (*it).pitch).b*(*it).vel/64, blockL.getColour(tnum, (*it).pitch).g*(*it).vel/64, blockL.getColour(tnum, (*it).pitch).r*(*it).vel/64}, -1, lineType);
                                else
                                    cv::circle( img_moving_notes, spts.pt3_prev[tnum], floor1(spts.pt2_prev[tnum].x-aw.Width/2+1), {blockL.getColour(tnum, (*it).pitch).b*(*it).vel/64, blockL.getColour(tnum, (*it).pitch).g*(*it).vel/64, blockL.getColour(tnum, (*it).pitch).r*(*it).vel/64}, -1, lineType);
                            // Case 2: the end of the current note (prev) is close to the begin of the new note (legato) -> notes connect themselves
                            else
                                if ( ! rProp.sep_render[2])
                                    cv::circle( image, cv::Point(x_mov, y_mov), floor1(spts.RadiusPrev[tnum] + deriv*((float)radius - (float)spts.RadiusPrev[tnum])), {blockL.getColour(tnum, (*it).pitch).b*(*it).vel/64, blockL.getColour(tnum, (*it).pitch).g*(*it).vel/64, blockL.getColour(tnum, (*it).pitch).r*(*it).vel/64}, -1, lineType);
                                else
                                    cv::circle( img_moving_notes, cv::Point(x_mov, y_mov), floor1(spts.RadiusPrev[tnum] + deriv*((float)radius - (float)spts.RadiusPrev[tnum])), {blockL.getColour(tnum, (*it).pitch).b*(*it).vel/64, blockL.getColour(tnum, (*it).pitch).g*(*it).vel/64, blockL.getColour(tnum, (*it).pitch).r*(*it).vel/64}, -1, lineType);

                        }

                        spts.pt3_prev[tnum] = spts.pt3; // Next centre point = previous
                        spts.pt1_prev[tnum] = spts.pt1;
                        spts.pt2_prev[tnum] = spts.pt2;
                        spts.RadiusPrev[tnum] = radius;
                    }



                    // -------------------------- Draw Rectangles with moving centred rectangle -------------------------
                    if ((*it).track == tnum && blockL.ActiveTracks[tnum] == true && blockL.shape[tnum] == 6) // All tracks
                    {
    //                        spts.x3 = (float)aw.Width*((-(float)aw.StartMidiTime + (float)(*it).t_middle)/((float)aw.EndMidiTime - (float)aw.StartMidiTime));
    //                        spts.y3 = (float)aw.Height/2 - (float)aw.Height*((float)((*it).pitch + 0.5 - ((float)mdt.PitchMin + pitch_max)/2)/((float)pitch_max - (float)mdt.PitchMin))*aw.VZoom - rProp.vertShift;
    //                        spts.pt3.x = f2int_safe(spts.x3); // Center x
    //                        spts.pt3.y = f2int_safe(spts.y3); // Center y
                        if (spts.pt1.x > aw.Width/2) // The note block is before (to the right of) the center line
                            if ( ! rProp.sep_render[1])
                                cv::rectangle( image, spts.pt1, spts.pt2, {blockL.getColour(tnum, (*it).pitch).b*(*it).vel*3/512, blockL.getColour(tnum, (*it).pitch).g*(*it).vel*3/512, blockL.getColour(tnum, (*it).pitch).r*(*it).vel*3/512}, -1, 8 );
                            else
                                cv::rectangle( img_buffer_sep_tracks[tnum], spts.pt1, spts.pt2, {blockL.getColour(tnum, (*it).pitch).b*(*it).vel*3/512, blockL.getColour(tnum, (*it).pitch).g*(*it).vel*3/512, blockL.getColour(tnum, (*it).pitch).r*(*it).vel*3/512}, -1, 8 );

                        else //if (spts.pt1.x <= aw.Width/2) // The note block is after (to the left of) the center line
                            if ( ! rProp.sep_render[1])
                                cv::rectangle( image, spts.pt1, spts.pt2, {blockL.getColour(tnum, (*it).pitch).b*(*it).vel/256, blockL.getColour(tnum, (*it).pitch).g*(*it).vel/256, blockL.getColour(tnum, (*it).pitch).r*(*it).vel/256}, 2, 8 );
                            else
                                cv::rectangle( img_buffer_sep_tracks[tnum], spts.pt1, spts.pt2, {blockL.getColour(tnum, (*it).pitch).b*(*it).vel/256, blockL.getColour(tnum, (*it).pitch).g*(*it).vel/256, blockL.getColour(tnum, (*it).pitch).r*(*it).vel/256}, 2, 8 );


                        if (spts.pt1_prev[tnum].x <= aw.Width/2 && spts.pt1.x > aw.Width/2 && spts.pt2_prev[tnum].x + 5 > aw.Width/2) // The note block is inside the center line // no more else if
                        {
                            float deriv = (((float)aw.Width/2 - (float)spts.pt1_prev[tnum].x)/((float)spts.pt1.x - (float)spts.pt1_prev[tnum].x));
                            //int y_mov = (int) ((float)spts.pt3_prev.y + deriv*((float)spts.pt3.y - (float)spts.pt3_prev.y));
                            spts.y1_mov = (int) ((float)spts.pt1_prev[tnum].y + deriv*((float)spts.pt1.y - (float)spts.pt1_prev[tnum].y));
                            spts.y2_mov = (int) spts.y1_mov + spts.y2 - spts.y1; // the difference among them remains the same
                            //int x_mov = aw.Width/2 + spts.pt3_prev.x - spts.pt1_prev.x + deriv*((spts.pt3.x - spts.pt1.x) - (spts.pt3_prev.x - spts.pt1_prev.x));
                            spts.x1_mov = aw.Width/2;
                            spts.x2_mov = aw.Width/2 + spts.pt2_prev[tnum].x - spts.pt1_prev[tnum].x - deriv*((spts.pt2_prev[tnum].x - spts.pt1_prev[tnum].x) - (spts.pt2.x - spts.pt1.x));
                            // Case 1: the distance to the next note is big (not a legato) -> note just fall off
                            if (spts.pt1.x - spts.pt2_prev[tnum].x > 5) // next_note t_on and old note t_off are far
                                if ( ! rProp.sep_render[2])
                                    cv::rectangle( image, cv::Point(- 2 + spts.pt1_prev[tnum].x + deriv*(spts.pt2_prev[tnum].x - spts.pt1_prev[tnum].x), 2 + spts.pt1_prev[tnum].y + deriv*(spts.pt2_prev[tnum].y - spts.pt1_prev[tnum].y)), cv::Point(2 + spts.pt2_prev[tnum].x - deriv*(spts.pt2_prev[tnum].x - spts.pt1_prev[tnum].x), - 2 + spts.pt2_prev[tnum].y - deriv*(spts.pt2_prev[tnum].y - spts.pt1_prev[tnum].y)), {blockL.getColour(tnum, (*it).pitch).b*(*it).vel/64, blockL.getColour(tnum, (*it).pitch).g*(*it).vel/64, blockL.getColour(tnum, (*it).pitch).r*(*it).vel/64}, -1, 8 );
                                else
                                    cv::rectangle( img_moving_notes, cv::Point(- 2 + spts.pt1_prev[tnum].x + deriv*(spts.pt2_prev[tnum].x - spts.pt1_prev[tnum].x), 2 + spts.pt1_prev[tnum].y + deriv*(spts.pt2_prev[tnum].y - spts.pt1_prev[tnum].y)), cv::Point(2 + spts.pt2_prev[tnum].x - deriv*(spts.pt2_prev[tnum].x - spts.pt1_prev[tnum].x), - 2 + spts.pt2_prev[tnum].y - deriv*(spts.pt2_prev[tnum].y - spts.pt1_prev[tnum].y)), {blockL.getColour(tnum, (*it).pitch).b*(*it).vel/64, blockL.getColour(tnum, (*it).pitch).g*(*it).vel/64, blockL.getColour(tnum, (*it).pitch).r*(*it).vel/64}, -1, 8 );
                            // Case 2: the end of the current note (prev) is close to the begin of the new note (legato) -> notes connect themselves
                            else
                                if ( ! rProp.sep_render[2])
                                    cv::rectangle( image, cv::Point(spts.x1_mov, spts.y1_mov), cv::Point(spts.x2_mov, spts.y2_mov), {blockL.getColour(tnum, (*it).pitch).b*(*it).vel/64, blockL.getColour(tnum, (*it).pitch).g*(*it).vel/64, blockL.getColour(tnum, (*it).pitch).r*(*it).vel/64}, -1, 8 );
                                else
                                    cv::rectangle( img_moving_notes, cv::Point(spts.x1_mov, spts.y1_mov), cv::Point(spts.x2_mov, spts.y2_mov), {blockL.getColour(tnum, (*it).pitch).b*(*it).vel/64, blockL.getColour(tnum, (*it).pitch).g*(*it).vel/64, blockL.getColour(tnum, (*it).pitch).r*(*it).vel/64}, -1, 8 );

                        }

                        //spts.pt3_prev = spts.pt3; // Next centre point = previous
                        spts.pt1_prev[tnum] = spts.pt1;
                        spts.pt2_prev[tnum] = spts.pt2;
    //                        spts.RadiusPrev = radius;
                    }



                    // -------------------------- Draw Rectangles with moving centred rectangle and highlighted borders -------------------------
                    if ((*it).track == tnum && blockL.ActiveTracks[tnum] == true && blockL.shape[tnum] == 7) // All tracks
                    {
    //                        spts.x3 = (float)aw.Width*((-(float)aw.StartMidiTime + (float)(*it).t_middle)/((float)aw.EndMidiTime - (float)aw.StartMidiTime));
    //                        spts.y3 = (float)aw.Height/2 - (float)aw.Height*((float)((*it).pitch + 0.5 - ((float)mdt.PitchMin + pitch_max)/2)/((float)pitch_max - (float)mdt.PitchMin))*aw.VZoom - rProp.vertShift;
    //                        spts.pt3.x = f2int_safe(spts.x3); // Center x
    //                        spts.pt3.y = f2int_safe(spts.y3); // Center y
                        if (spts.pt1.x > aw.Width/2) // The note block is before (to the right of) the center line
                            if ( ! rProp.sep_render[1])
                                cv::rectangle( image, spts.pt1, spts.pt2, {blockL.getColour(tnum, (*it).pitch).b*(*it).vel*3/512, blockL.getColour(tnum, (*it).pitch).g*(*it).vel*3/512, blockL.getColour(tnum, (*it).pitch).r*(*it).vel*3/512}, -1, 8 );
                            else
                                cv::rectangle( img_buffer_sep_tracks[tnum], spts.pt1, spts.pt2, {blockL.getColour(tnum, (*it).pitch).b*(*it).vel*3/512, blockL.getColour(tnum, (*it).pitch).g*(*it).vel*3/512, blockL.getColour(tnum, (*it).pitch).r*(*it).vel*3/512}, -1, 8 );
                        else if (spts.pt1.x <= aw.Width/2 && spts.pt2.x > aw.Width/2) // The note block is inside the center line
                        {
                            if ( ! rProp.sep_render[0])
                                cv::rectangle( image, spts.pt1, spts.pt2, {blockL.getColour(tnum, (*it).pitch).b*(*it).vel/64, blockL.getColour(tnum, (*it).pitch).g*(*it).vel/64, blockL.getColour(tnum, (*it).pitch).r*(*it).vel/64}, 2, 8 );
                            else
                                cv::rectangle( img_playing_notes, spts.pt1, spts.pt2, {blockL.getColour(tnum, (*it).pitch).b*(*it).vel/64, blockL.getColour(tnum, (*it).pitch).g*(*it).vel/64, blockL.getColour(tnum, (*it).pitch).r*(*it).vel/64}, 2, 8 );
                        }
                        else // The note block is after (to the left of) the center line
                            if ( ! rProp.sep_render[1])
                                cv::rectangle( image, spts.pt1, spts.pt2, {blockL.getColour(tnum, (*it).pitch).b*(*it).vel/512, blockL.getColour(tnum, (*it).pitch).g*(*it).vel/512, blockL.getColour(tnum, (*it).pitch).r*(*it).vel/512}, 2, 8 );
                            else
                                cv::rectangle( img_buffer_sep_tracks[tnum], spts.pt1, spts.pt2, {blockL.getColour(tnum, (*it).pitch).b*(*it).vel/512, blockL.getColour(tnum, (*it).pitch).g*(*it).vel/512, blockL.getColour(tnum, (*it).pitch).r*(*it).vel/512}, 2, 8 );


                        if (spts.pt1_prev[tnum].x <= aw.Width/2 && spts.pt1.x > aw.Width/2 && spts.pt2_prev[tnum].x + 5 > aw.Width/2) // The note block is inside the center line // no more else if
                        {
                            float deriv = (((float)aw.Width/2 - (float)spts.pt1_prev[tnum].x)/((float)spts.pt1.x - (float)spts.pt1_prev[tnum].x));
                            //int y_mov = (int) ((float)spts.pt3_prev.y + deriv*((float)spts.pt3.y - (float)spts.pt3_prev.y));
                            spts.y1_mov = (int) ((float)spts.pt1_prev[tnum].y + deriv*((float)spts.pt1.y - (float)spts.pt1_prev[tnum].y));
                            spts.y2_mov = (int) spts.y1_mov + spts.y2 - spts.y1; // the difference among them remains the same
                            //int x_mov = aw.Width/2 + spts.pt3_prev.x - spts.pt1_prev.x + deriv*((spts.pt3.x - spts.pt1.x) - (spts.pt3_prev.x - spts.pt1_prev.x));
                            spts.x1_mov = aw.Width/2;
                            spts.x2_mov = aw.Width/2 + spts.pt2_prev[tnum].x - spts.pt1_prev[tnum].x - deriv*((spts.pt2_prev[tnum].x - spts.pt1_prev[tnum].x) - (spts.pt2.x - spts.pt1.x));
                            // Case 1: the distance to the next note is big (not a legato) -> note just fall off
                            if (spts.pt1.x - spts.pt2_prev[tnum].x > 5) // next_note t_on and old note t_off are far
                                if ( ! rProp.sep_render[2])
                                    cv::rectangle( image, cv::Point(- 2 + spts.pt1_prev[tnum].x + deriv*(spts.pt2_prev[tnum].x - spts.pt1_prev[tnum].x), 2 + spts.pt1_prev[tnum].y + deriv*(spts.pt2_prev[tnum].y - spts.pt1_prev[tnum].y)), cv::Point(2 + spts.pt2_prev[tnum].x - deriv*(spts.pt2_prev[tnum].x - spts.pt1_prev[tnum].x), - 2 + spts.pt2_prev[tnum].y - deriv*(spts.pt2_prev[tnum].y - spts.pt1_prev[tnum].y)), {blockL.getColour(tnum, (*it).pitch).b*(*it).vel/64, blockL.getColour(tnum, (*it).pitch).g*(*it).vel/64, blockL.getColour(tnum, (*it).pitch).r*(*it).vel/64}, -1, 8 );
                                else
                                    cv::rectangle( img_moving_notes, cv::Point(- 2 + spts.pt1_prev[tnum].x + deriv*(spts.pt2_prev[tnum].x - spts.pt1_prev[tnum].x), 2 + spts.pt1_prev[tnum].y + deriv*(spts.pt2_prev[tnum].y - spts.pt1_prev[tnum].y)), cv::Point(2 + spts.pt2_prev[tnum].x - deriv*(spts.pt2_prev[tnum].x - spts.pt1_prev[tnum].x), - 2 + spts.pt2_prev[tnum].y - deriv*(spts.pt2_prev[tnum].y - spts.pt1_prev[tnum].y)), {blockL.getColour(tnum, (*it).pitch).b*(*it).vel/64, blockL.getColour(tnum, (*it).pitch).g*(*it).vel/64, blockL.getColour(tnum, (*it).pitch).r*(*it).vel/64}, -1, 8 );
                            // Case 2: the end of the current note (prev) is close to the begin of the new note (legato) -> notes connect themselves
                            else
                                if ( ! rProp.sep_render[2])
                                    cv::rectangle( image, cv::Point(spts.x1_mov, spts.y1_mov), cv::Point(spts.x2_mov, spts.y2_mov), {blockL.getColour(tnum, (*it).pitch).b*(*it).vel/64, blockL.getColour(tnum, (*it).pitch).g*(*it).vel/64, blockL.getColour(tnum, (*it).pitch).r*(*it).vel/64}, -1, 8 );
                                else
                                    cv::rectangle( img_moving_notes, cv::Point(spts.x1_mov, spts.y1_mov), cv::Point(spts.x2_mov, spts.y2_mov), {blockL.getColour(tnum, (*it).pitch).b*(*it).vel/64, blockL.getColour(tnum, (*it).pitch).g*(*it).vel/64, blockL.getColour(tnum, (*it).pitch).r*(*it).vel/64}, -1, 8 );

                        }

                        //spts.pt3_prev = spts.pt3; // Next centre point = previous
                        spts.pt1_prev[tnum] = spts.pt1;
                        spts.pt2_prev[tnum] = spts.pt2;
    //                        spts.RadiusPrev = radius;
                    }




                    // -------------------------- Draw Rhombus with moving rhombus -------------------------
                    if ((*it).track == tnum && blockL.ActiveTracks[tnum] == true && blockL.shape[tnum] == 8) // All tracks - rectangle
                    {
                        spts.x3 = (float)aw.Width*((-(float)aw.StartMidiTime + (float)(*it).t_middle)/((float)aw.EndMidiTime - (float)aw.StartMidiTime));
                        spts.y3 = (float)aw.Height/2 - (float)aw.Height*((float)((*it).pitch + 0.5 - ((float)mdt.PitchMin + mdt.PitchMax)/2)/((float)mdt.PitchMax - (float)mdt.PitchMin))*aw.VZoom - rProp.vertShift;
                        spts.pt1.y = f2int_safe(spts.y3);
                        spts.pt2.y = f2int_safe(spts.y3);
                        spts.pt3.x = f2int_safe(spts.x3);
                        spts.pt3.y = f2int_safe(spts.y1);
                        spts.pt4.x = f2int_safe(spts.x3);
                        spts.pt4.y = f2int_safe(spts.y2);
                        cv::Point ptos[4];
                        ptos[0] = spts.pt1; ptos[1] = spts.pt3; ptos[2] = spts.pt2; ptos[3] = spts.pt4;
                        if (spts.pt1.x > aw.Width/2) // The note block is before (to the right of) the center line
                            if ( ! rProp.sep_render[1])
                                //cv::fillPoly( image, pts, );
                                cv::fillConvexPoly( image, ptos, 4, {blockL.getColour(tnum, (*it).pitch).b*(*it).vel*3/512, blockL.getColour(tnum, (*it).pitch).g*(*it).vel*3/512, blockL.getColour(tnum, (*it).pitch).r*(*it).vel*3/512}, lineType);
                            else
                                cv::fillConvexPoly( img_buffer_sep_tracks[tnum], ptos, 4, {blockL.getColour(tnum, (*it).pitch).b*(*it).vel*3/512, blockL.getColour(tnum, (*it).pitch).g*(*it).vel*3/512, blockL.getColour(tnum, (*it).pitch).r*(*it).vel*3/512}, lineType);
                        else // The note block is after (to the left of) the center line
                            if ( ! rProp.sep_render[1])
                                cv::fillConvexPoly( image, ptos, 4, {blockL.getColour(tnum, (*it).pitch).b*(*it).vel/512, blockL.getColour(tnum, (*it).pitch).g*(*it).vel/512, blockL.getColour(tnum, (*it).pitch).r*(*it).vel/512}, lineType);
                            else
                                cv::fillConvexPoly( img_buffer_sep_tracks[tnum], ptos, 4, {blockL.getColour(tnum, (*it).pitch).b*(*it).vel/512, blockL.getColour(tnum, (*it).pitch).g*(*it).vel/512, blockL.getColour(tnum, (*it).pitch).r*(*it).vel/512}, lineType);
    //                        else if (spts.pt1.x <= aw.Width/2 && spts.pt2.x > aw.Width/2) // The note block is inside the center line
    //                        {
    //                            if ( ! rProp.sep_render[0])
    //                                cv::fillConvexPoly( image, ptos, 4, {tProp.getColour(tnum, (*it).pitch).b*(*it).vel/64, tProp.getColour(tnum, (*it).pitch).g*(*it).vel/64, tProp.getColour(tnum, (*it).pitch).r*(*it).vel/64}, 8 );
    //                            else
    //                                cv::fillConvexPoly( img_playing_notes, ptos, 4, {tProp.getColour(tnum, (*it).pitch).b*(*it).vel/64, tProp.getColour(tnum, (*it).pitch).g*(*it).vel/64, tProp.getColour(tnum, (*it).pitch).r*(*it).vel/64}, 8 );
    //                        }
                        if (spts.pt1_prev[tnum].x <= aw.Width/2 && spts.pt1.x > aw.Width/2 && spts.pt2_prev[tnum].x + 5 > aw.Width/2) // The note block is inside the center line // no more else if
                        {
                            float deriv = (((float)aw.Width/2 - (float)spts.pt1_prev[tnum].x)/((float)spts.pt1.x - (float)spts.pt1_prev[tnum].x));
                            //
                            spts.y1_mov = (int) ((float)spts.pt1_prev[tnum].y + deriv*((float)spts.pt1.y - (float)spts.pt1_prev[tnum].y));
                            // spts.y2_mov = spts.y1_mov;
                            spts.y3_mov = spts.y1_mov + spts.y1 - spts.y3; // = (int) ((float)spts.pt3_prev.y + deriv*((float)spts.pt3.y - (float)spts.pt3_prev.y));
                            int y4_mov = spts.y1_mov - spts.y1 + spts.y3;
                            //
                            spts.x1_mov = aw.Width/2;
                            spts.x2_mov = aw.Width/2 + spts.pt2_prev[tnum].x - spts.pt1_prev[tnum].x - deriv*((spts.pt2_prev[tnum].x - spts.pt1_prev[tnum].x) - (spts.pt2.x - spts.pt1.x));
                            spts.x3_mov = aw.Width/2 + (spts.pt2_prev[tnum].x - spts.pt1_prev[tnum].x - deriv*((spts.pt2_prev[tnum].x - spts.pt1_prev[tnum].x) - (spts.pt2.x - spts.pt1.x)))/2;
                            //spts.x3_mov = aw.Width/2 + spts.pt3_prev.x - spts.pt1_prev.x + deriv*((spts.pt3.x - spts.pt1.x) - (spts.pt3_prev.x - spts.pt1_prev.x));
                            // int x4_mov = spts.x3_mov
                            cv::Point ptos_mov[4];
                            ptos_mov[0] = cv::Point(spts.x1_mov, spts.y1_mov); ptos_mov[1] = cv::Point(spts.x3_mov, spts.y3_mov); ptos_mov[2] = cv::Point(spts.x2_mov, spts.y1_mov); ptos_mov[3] = cv::Point(spts.x3_mov, y4_mov);

                            // Case 1: the distance to the next note is big (not a legato) -> note just fall off
                            if (spts.pt1.x - spts.pt2_prev[tnum].x > 5) // next_note t_on and old note t_off are far
                                if ( ! rProp.sep_render[2])
                                    cv::fillConvexPoly( image, spts.PtosPrev[tnum], 4, {(0.5 - deriv)*blockL.getColour(tnum, (*it).pitch).b*(*it).vel/32, (0.5 - deriv)*blockL.getColour(tnum, (*it).pitch).g*(*it).vel/32, (0.5 - deriv)*blockL.getColour(tnum, (*it).pitch).r*(*it).vel/32}, lineType);
                                else
                                    cv::fillConvexPoly( img_moving_notes, spts.PtosPrev[tnum], 4, {(1 - deriv)*blockL.getColour(tnum, (*it).pitch).b*(*it).vel/32, (1 - deriv)*blockL.getColour(tnum, (*it).pitch).g*(*it).vel/32, (1 - deriv)*blockL.getColour(tnum, (*it).pitch).r*(*it).vel/32}, lineType);
                            // Case 2: the end of the current note (prev) is close to the begin of the new note (legato) -> notes connect themselves
                            else
                                if ( ! rProp.sep_render[2])
                                    cv::fillConvexPoly( image, ptos_mov, 4, {blockL.getColour(tnum, (*it).pitch).b*(*it).vel/64, blockL.getColour(tnum, (*it).pitch).g*(*it).vel/64, blockL.getColour(tnum, (*it).pitch).r*(*it).vel/64}, lineType);
                                else
                                    cv::fillConvexPoly( img_moving_notes, ptos_mov, 4, {blockL.getColour(tnum, (*it).pitch).b*(*it).vel/64, blockL.getColour(tnum, (*it).pitch).g*(*it).vel/64, blockL.getColour(tnum, (*it).pitch).r*(*it).vel/64}, lineType);

                        }
                        spts.pt3_prev[tnum] = spts.pt3; // Next centre point = previous
                        spts.pt1_prev[tnum] = spts.pt1;
                        spts.pt2_prev[tnum] = spts.pt2;
                        spts.PtosPrev[tnum][0] = ptos[0];
                        spts.PtosPrev[tnum][1] = ptos[1];
                        spts.PtosPrev[tnum][2] = ptos[2];
                        spts.PtosPrev[tnum][3] = ptos[3];
                    }




                    // -------------------------- Draw Rhombus with moving rhombus and playing highlight (select sep playing notes render) -------------------------
                    if ((*it).track == tnum && blockL.ActiveTracks[tnum] == true && blockL.shape[tnum] == 9) // All tracks - rectangle
                    {
                        spts.x3 = (float)aw.Width*((-(float)aw.StartMidiTime + (float)(*it).t_middle)/((float)aw.EndMidiTime - (float)aw.StartMidiTime));
                        spts.y3 = (float)aw.Height/2 - (float)aw.Height*((float)((*it).pitch + 0.5 - ((float)mdt.PitchMin + mdt.PitchMax)/2)/((float)mdt.PitchMax - (float)mdt.PitchMin))*aw.VZoom - rProp.vertShift;
                        spts.pt1.y = f2int_safe(spts.y3);
                        spts.pt2.y = f2int_safe(spts.y3);
                        spts.pt3.x = f2int_safe(spts.x3);
                        spts.pt3.y = f2int_safe(spts.y1);
                        spts.pt4.x = f2int_safe(spts.x3);
                        spts.pt4.y = f2int_safe(spts.y2);
                        cv::Point ptos[4];
                        ptos[0] = spts.pt1; ptos[1] = spts.pt3; ptos[2] = spts.pt2; ptos[3] = spts.pt4;
                        if (spts.pt1.x > aw.Width/2) // The note block is before (to the right of) the center line
                            if ( ! rProp.sep_render[1])
                                //cv::fillPoly( image, pts, );
                                cv::fillConvexPoly( image, ptos, 4, {blockL.getColour(tnum, (*it).pitch).b*(*it).vel*3/512, blockL.getColour(tnum, (*it).pitch).g*(*it).vel*3/512, blockL.getColour(tnum, (*it).pitch).r*(*it).vel*3/512}, lineType);
                            else
                                cv::fillConvexPoly( img_buffer_sep_tracks[tnum], ptos, 4, {blockL.getColour(tnum, (*it).pitch).b*(*it).vel*3/512, blockL.getColour(tnum, (*it).pitch).g*(*it).vel*3/512, blockL.getColour(tnum, (*it).pitch).r*(*it).vel*3/512}, lineType);
                        else if (spts.pt1.x <= aw.Width/2 && spts.pt2.x > aw.Width/2) // The note block is inside the center line
                        {
                            if ( ! rProp.sep_render[0])
                                cv::fillConvexPoly( image, ptos, 4, {blockL.getColour(tnum, (*it).pitch).b*(*it).vel/64, blockL.getColour(tnum, (*it).pitch).g*(*it).vel/64, blockL.getColour(tnum, (*it).pitch).r*(*it).vel/64}, lineType);
                            else
                                cv::fillConvexPoly( img_playing_notes, ptos, 4, {blockL.getColour(tnum, (*it).pitch).b*(*it).vel/64, blockL.getColour(tnum, (*it).pitch).g*(*it).vel/64, blockL.getColour(tnum, (*it).pitch).r*(*it).vel/64}, lineType);
                        }
                        if (spts.pt1.x <= aw.Width/2) // The note block is after (to the left of) the center line
                            if ( ! rProp.sep_render[1])
                                cv::fillConvexPoly( image, ptos, 4, {blockL.getColour(tnum, (*it).pitch).b*(*it).vel/512, blockL.getColour(tnum, (*it).pitch).g*(*it).vel/512, blockL.getColour(tnum, (*it).pitch).r*(*it).vel/512}, lineType);
                            else
                                cv::fillConvexPoly( img_buffer_sep_tracks[tnum], ptos, 4, {blockL.getColour(tnum, (*it).pitch).b*(*it).vel/512, blockL.getColour(tnum, (*it).pitch).g*(*it).vel/512, blockL.getColour(tnum, (*it).pitch).r*(*it).vel/512}, lineType);
    //                        else if (spts.pt1.x <= aw.Width/2 && spts.pt2.x > aw.Width/2) // The note block is inside the center line
    //                        {
    //                            if ( ! rProp.sep_render[0])
    //                                cv::fillConvexPoly( image, ptos, 4, {tProp.getColour(tnum, (*it).pitch).b*(*it).vel/64, tProp.getColour(tnum, (*it).pitch).g*(*it).vel/64, tProp.getColour(tnum, (*it).pitch).r*(*it).vel/64}, 8 );
    //                            else
    //                                cv::fillConvexPoly( img_playing_notes, ptos, 4, {tProp.getColour(tnum, (*it).pitch).b*(*it).vel/64, tProp.getColour(tnum, (*it).pitch).g*(*it).vel/64, tProp.getColour(tnum, (*it).pitch).r*(*it).vel/64}, 8 );
    //                        }
                        if (spts.pt1_prev[tnum].x <= aw.Width/2 && spts.pt1.x > aw.Width/2 && spts.pt2_prev[tnum].x + 5 > aw.Width/2) // The note block is inside the center line // no more else if
                        {
                            float deriv = (((float)aw.Width/2 - (float)spts.pt1_prev[tnum].x)/((float)spts.pt1.x - (float)spts.pt1_prev[tnum].x));
                            //
                            spts.y1_mov = (int) ((float)spts.pt1_prev[tnum].y + deriv*((float)spts.pt1.y - (float)spts.pt1_prev[tnum].y));
                            // spts.y2_mov = spts.y1_mov;
                            spts.y3_mov = spts.y1_mov + spts.y1 - spts.y3; // = (int) ((float)spts.pt3_prev.y + deriv*((float)spts.pt3.y - (float)spts.pt3_prev.y));
                            int y4_mov = spts.y1_mov - spts.y1 + spts.y3;
                            //
                            spts.x1_mov = aw.Width/2;
                            spts.x2_mov = aw.Width/2 + spts.pt2_prev[tnum].x - spts.pt1_prev[tnum].x - deriv*((spts.pt2_prev[tnum].x - spts.pt1_prev[tnum].x) - (spts.pt2.x - spts.pt1.x));
                            spts.x3_mov = aw.Width/2 + (spts.pt2_prev[tnum].x - spts.pt1_prev[tnum].x - deriv*((spts.pt2_prev[tnum].x - spts.pt1_prev[tnum].x) - (spts.pt2.x - spts.pt1.x)))/2;
                            //spts.x3_mov = aw.Width/2 + spts.pt3_prev.x - spts.pt1_prev.x + deriv*((spts.pt3.x - spts.pt1.x) - (spts.pt3_prev.x - spts.pt1_prev.x));
                            // int x4_mov = spts.x3_mov
                            cv::Point ptos_mov[4];
                            ptos_mov[0] = cv::Point(spts.x1_mov, spts.y1_mov); ptos_mov[1] = cv::Point(spts.x3_mov, spts.y3_mov); ptos_mov[2] = cv::Point(spts.x2_mov, spts.y1_mov); ptos_mov[3] = cv::Point(spts.x3_mov, y4_mov);

                            // Case 1: the distance to the next note is big (not a legato) -> note just fall off
                            if (spts.pt1.x - spts.pt2_prev[tnum].x > 5) // next_note t_on and old note t_off are far
                                if ( ! rProp.sep_render[2])
                                    cv::fillConvexPoly( image, spts.PtosPrev[tnum], 4, {(0.5 - deriv)*blockL.getColour(tnum, (*it).pitch).b*(*it).vel/32, (0.5 - deriv)*blockL.getColour(tnum, (*it).pitch).g*(*it).vel/32, (0.5 - deriv)*blockL.getColour(tnum, (*it).pitch).r*(*it).vel/32}, lineType);
                                else
                                    cv::fillConvexPoly( img_moving_notes, spts.PtosPrev[tnum], 4, {(1 - deriv)*blockL.getColour(tnum, (*it).pitch).b*(*it).vel/32, (1 - deriv)*blockL.getColour(tnum, (*it).pitch).g*(*it).vel/32, (1 - deriv)*blockL.getColour(tnum, (*it).pitch).r*(*it).vel/32}, lineType);
                            // Case 2: the end of the current note (prev) is close to the begin of the new note (legato) -> notes connect themselves
                            else
                                if ( ! rProp.sep_render[2])
                                    cv::fillConvexPoly( image, ptos_mov, 4, {blockL.getColour(tnum, (*it).pitch).b*(*it).vel/64, blockL.getColour(tnum, (*it).pitch).g*(*it).vel/64, blockL.getColour(tnum, (*it).pitch).r*(*it).vel/64}, lineType);
                                else
                                    cv::fillConvexPoly( img_moving_notes, ptos_mov, 4, {blockL.getColour(tnum, (*it).pitch).b*(*it).vel/64, blockL.getColour(tnum, (*it).pitch).g*(*it).vel/64, blockL.getColour(tnum, (*it).pitch).r*(*it).vel/64}, lineType);

                        }
                        spts.pt3_prev[tnum] = spts.pt3; // Next centre point = previous
                        spts.pt1_prev[tnum] = spts.pt1;
                        spts.pt2_prev[tnum] = spts.pt2;
                        spts.PtosPrev[tnum][0] = ptos[0];
                        spts.PtosPrev[tnum][1] = ptos[1];
                        spts.PtosPrev[tnum][2] = ptos[2];
                        spts.PtosPrev[tnum][3] = ptos[3];
                    }



                    // -------------------------- Draw ellipses with moving centred ellipse -------------------------
                    if ((*it).track == tnum && blockL.ActiveTracks[tnum] == true && blockL.shape[tnum] == 10) // All tracks
                    {
                        spts.x3 = (float)aw.Width*((-(float)aw.StartMidiTime + (float)(*it).t_middle)/((float)aw.EndMidiTime - (float)aw.StartMidiTime));
                        spts.y3 = (float)aw.Height/2 - (float)aw.Height*((float)((*it).pitch + 0.5 - ((float)mdt.PitchMin + mdt.PitchMax)/2)/((float)mdt.PitchMax - (float)mdt.PitchMin))*aw.VZoom - rProp.vertShift;
                        spts.pt3.x = f2int_safe(spts.x3); // Center x
                        spts.pt3.y = f2int_safe(spts.y3); // Center y
                        int radius = (spts.x3-spts.x1) < 80 ? (spts.x3-spts.x1) : 80;
                        if (spts.pt1.x > aw.Width/2) // The note block is before (to the right of) the center line
                            if ( ! rProp.sep_render[1])
                                //cv::fillPoly( image, pts, );
                                cv::ellipse( image, spts.pt3, cv::Size(spts.x3-spts.x1, spts.y3-spts.y2), 0, 0, 360, {blockL.getColour(tnum, (*it).pitch).b*(*it).vel*3/512, blockL.getColour(tnum, (*it).pitch).g*(*it).vel*3/512, blockL.getColour(tnum, (*it).pitch).r*(*it).vel*3/512}, -1, lineType);
                            else
                                cv::ellipse( img_buffer_sep_tracks[tnum], spts.pt3, cv::Size(spts.x3-spts.x1, spts.y3-spts.y2), 0, 0, 360, {blockL.getColour(tnum, (*it).pitch).b*(*it).vel*3/512, blockL.getColour(tnum, (*it).pitch).g*(*it).vel*3/512, blockL.getColour(tnum, (*it).pitch).r*(*it).vel*3/512}, -1, lineType);
    //                        else if (spts.pt1.x <= aw.Width/2 && spts.pt2.x > aw.Width/2) // The note block is inside the center line
    //                        {
    //                            if ( ! rProp.sep_render[0])
    //                                cv::ellipse( image, spts.pt3, cv::Size(spts.x3-spts.x1, spts.y3-spts.y2), 0, 0, 360, {tProp.getColour(tnum, (*it).pitch).b*(*it).vel/64, tProp.getColour(tnum, (*it).pitch).g*(*it).vel/64, tProp.getColour(tnum, (*it).pitch).r*(*it).vel/64}, -1, 8 );
    //                            else
    //                                cv::ellipse( img_playing_notes, spts.pt3, cv::Size(spts.x3-spts.x1, spts.y3-spts.y2), 0, 0, 360, {tProp.getColour(tnum, (*it).pitch).b*(*it).vel/64, tProp.getColour(tnum, (*it).pitch).g*(*it).vel/64, tProp.getColour(tnum, (*it).pitch).r*(*it).vel/64}, -1, 8 );
    //                        }
                        else // The note block is after (to the left of) the center line
                            if ( ! rProp.sep_render[1])
                                cv::ellipse( image, spts.pt3, cv::Size(spts.x3-spts.x1, spts.y3-spts.y2), 0, 0, 360, {blockL.getColour(tnum, (*it).pitch).b*(*it).vel/512, blockL.getColour(tnum, (*it).pitch).g*(*it).vel/512, blockL.getColour(tnum, (*it).pitch).r*(*it).vel/512}, 2, lineType);
                            else
                                cv::ellipse( img_buffer_sep_tracks[tnum], spts.pt3, cv::Size(spts.x3-spts.x1, spts.y3-spts.y2), 0, 0, 360, {blockL.getColour(tnum, (*it).pitch).b*(*it).vel/512, blockL.getColour(tnum, (*it).pitch).g*(*it).vel/512, blockL.getColour(tnum, (*it).pitch).r*(*it).vel/512}, 2, lineType);


                        if (spts.pt1_prev[tnum].x <= aw.Width/2 && spts.pt1.x > aw.Width/2 && spts.pt2_prev[tnum].x + 5 > aw.Width/2) // The note block is inside the center line // no more else if
                        {
                            float deriv = (((float)aw.Width/2 - (float)spts.pt1_prev[tnum].x)/((float)spts.pt1.x - (float)spts.pt1_prev[tnum].x));
                            spts.y3_mov = (int) ((float)spts.pt3_prev[tnum].y + deriv*((float)spts.pt3.y - (float)spts.pt3_prev[tnum].y));
                            spts.x3_mov = aw.Width/2 + spts.pt3_prev[tnum].x - spts.pt1_prev[tnum].x + deriv*((spts.pt3.x - spts.pt1.x) - (spts.pt3_prev[tnum].x - spts.pt1_prev[tnum].x));
                            spts.y1_mov = (int) ((float)spts.pt1_prev[tnum].y + deriv*((float)spts.pt1.y - (float)spts.pt1_prev[tnum].y));
                            spts.y2_mov = (int) spts.y1_mov + spts.y2 - spts.y1; // the difference among them remains the same
                            //int x_mov = aw.Width/2 + spts.pt3_prev.x - spts.pt1_prev.x + deriv*((spts.pt3.x - spts.pt1.x) - (spts.pt3_prev.x - spts.pt1_prev.x));
                            spts.x1_mov = aw.Width/2;
                            //spts.x2_mov = aw.Width/2 + spts.pt2_prev[tnum].x - spts.pt1_prev[tnum].x - deriv*((spts.pt2_prev[tnum].x - spts.pt1_prev[tnum].x) - (spts.pt2.x - spts.pt1.x));
                            // Case 1: the distance to the next note is big (not a legato) -> note just fall off
                            if (spts.pt1.x - spts.pt2_prev[tnum].x > 5) // next_note t_on and old note t_off are far
                                if ( ! rProp.sep_render[2])
                                    cv::ellipse( image, spts.pt3_prev[tnum], cv::Size(spts.x3_mov-spts.x1_mov, spts.y3-spts.y2), 0, 0, 360, {(0.5-deriv)*blockL.getColour(tnum, (*it).pitch).b*(*it).vel*3/64, (0.5-deriv)*blockL.getColour(tnum, (*it).pitch).g*(*it).vel*3/64, (0.5-deriv)*blockL.getColour(tnum, (*it).pitch).r*(*it).vel*3/64}, -1, lineType);
                                else
                                    cv::ellipse( img_moving_notes, spts.pt3_prev[tnum], cv::Size(spts.x3_mov-spts.x1_mov, spts.y3-spts.y2), 0, 0, 360, {(0.5-deriv)*blockL.getColour(tnum, (*it).pitch).b*(*it).vel*3/64, (0.5-deriv)*blockL.getColour(tnum, (*it).pitch).g*(*it).vel*3/64, (0.5-deriv)*blockL.getColour(tnum, (*it).pitch).r*(*it).vel*3/64}, -1, lineType);
                            // Case 2: the end of the current note (prev) is close to the begin of the new note (legato) -> notes connect themselves
                            else
                                if ( ! rProp.sep_render[2])
                                    cv::ellipse( image, cv::Point(spts.x3_mov, spts.y3_mov), cv::Size(spts.x3_mov-spts.x1_mov, spts.y3-spts.y2), 0, 0, 360, {blockL.getColour(tnum, (*it).pitch).b*(*it).vel*3/64, blockL.getColour(tnum, (*it).pitch).g*(*it).vel*3/64, blockL.getColour(tnum, (*it).pitch).r*(*it).vel*3/64}, -1, lineType);
                                else
                                    cv::ellipse( img_moving_notes, cv::Point(spts.x3_mov, spts.y3_mov), cv::Size(spts.x3_mov-spts.x1_mov, spts.y3-spts.y2), 0, 0, 360, {blockL.getColour(tnum, (*it).pitch).b*(*it).vel*3/64, blockL.getColour(tnum, (*it).pitch).g*(*it).vel*3/64, blockL.getColour(tnum, (*it).pitch).r*(*it).vel*3/64}, -1, lineType);

                        }

                        spts.pt3_prev[tnum] = spts.pt3; // Next centre point = previous
                        spts.pt1_prev[tnum] = spts.pt1;
                        spts.pt2_prev[tnum] = spts.pt2;
                        spts.RadiusPrev[tnum] = radius;
                    }




                    // -------------------------- Draw ellipses with moving centred ellipse and highlighted playing note -------------------------
                    if ((*it).track == tnum && blockL.ActiveTracks[tnum] == true && blockL.shape[tnum] == 11) // All tracks
                    {
                        spts.x3 = (float)aw.Width*((-(float)aw.StartMidiTime + (float)(*it).t_middle)/((float)aw.EndMidiTime - (float)aw.StartMidiTime));
                        spts.y3 = (float)aw.Height/2 - (float)aw.Height*((float)((*it).pitch + 0.5 - ((float)mdt.PitchMin + mdt.PitchMax)/2)/((float)mdt.PitchMax - (float)mdt.PitchMin))*aw.VZoom - rProp.vertShift;
                        spts.pt3.x = f2int_safe(spts.x3); // Center x
                        spts.pt3.y = f2int_safe(spts.y3); // Center y
                        int radius = (spts.x3-spts.x1) < 80 ? (spts.x3-spts.x1) : 80;
                        if (spts.pt1.x > aw.Width/2) // The note block is before (to the right of) the center line
                            if ( ! rProp.sep_render[1])
                                //cv::fillPoly( image, pts, );
                                cv::ellipse( image, spts.pt3, cv::Size(spts.x3-spts.x1, spts.y3-spts.y2), 0, 0, 360, {blockL.getColour(tnum, (*it).pitch).b*(*it).vel*3/512, blockL.getColour(tnum, (*it).pitch).g*(*it).vel*3/512, blockL.getColour(tnum, (*it).pitch).r*(*it).vel*3/512}, -1, lineType);
                            else
                                cv::ellipse( img_buffer_sep_tracks[tnum], spts.pt3, cv::Size(spts.x3-spts.x1, spts.y3-spts.y2), 0, 0, 360, {blockL.getColour(tnum, (*it).pitch).b*(*it).vel*3/512, blockL.getColour(tnum, (*it).pitch).g*(*it).vel*3/512, blockL.getColour(tnum, (*it).pitch).r*(*it).vel*3/512}, -1, lineType);
                        else if (spts.pt1.x <= aw.Width/2 && spts.pt2.x > aw.Width/2) // The note block is inside the center line
                        {
                            if ( ! rProp.sep_render[0])
                                cv::ellipse( image, spts.pt3, cv::Size(spts.x3-spts.x1, spts.y3-spts.y2), 0, 0, 360, {blockL.getColour(tnum, (*it).pitch).b*(*it).vel/64, blockL.getColour(tnum, (*it).pitch).g*(*it).vel/64, blockL.getColour(tnum, (*it).pitch).r*(*it).vel/64}, 1, lineType);
                            else
                                cv::ellipse( img_playing_notes, spts.pt3, cv::Size(spts.x3-spts.x1, spts.y3-spts.y2), 0, 0, 360, {blockL.getColour(tnum, (*it).pitch).b*(*it).vel/64, blockL.getColour(tnum, (*it).pitch).g*(*it).vel/64, blockL.getColour(tnum, (*it).pitch).r*(*it).vel/64}, 1, lineType);
                        }
                        else // The note block is after (to the left of) the center line
                            if ( ! rProp.sep_render[1])
                                cv::ellipse( image, spts.pt3, cv::Size(spts.x3-spts.x1, spts.y3-spts.y2), 0, 0, 360, {blockL.getColour(tnum, (*it).pitch).b*(*it).vel/512, blockL.getColour(tnum, (*it).pitch).g*(*it).vel/512, blockL.getColour(tnum, (*it).pitch).r*(*it).vel/512}, 2, lineType);
                            else
                                cv::ellipse( img_buffer_sep_tracks[tnum], spts.pt3, cv::Size(spts.x3-spts.x1, spts.y3-spts.y2), 0, 0, 360, {blockL.getColour(tnum, (*it).pitch).b*(*it).vel/512, blockL.getColour(tnum, (*it).pitch).g*(*it).vel/512, blockL.getColour(tnum, (*it).pitch).r*(*it).vel/512}, 2, lineType);


                        if (spts.pt1_prev[tnum].x <= aw.Width/2 && spts.pt1.x > aw.Width/2 && spts.pt2_prev[tnum].x + 5 > aw.Width/2) // The note block is inside the center line // no more else if
                        {
                            float deriv = (((float)aw.Width/2 - (float)spts.pt1_prev[tnum].x)/((float)spts.pt1.x - (float)spts.pt1_prev[tnum].x));
                            spts.y3_mov = (int) ((float)spts.pt3_prev[tnum].y + deriv*((float)spts.pt3.y - (float)spts.pt3_prev[tnum].y));
                            spts.x3_mov = aw.Width/2 + spts.pt3_prev[tnum].x - spts.pt1_prev[tnum].x + deriv*((spts.pt3.x - spts.pt1.x) - (spts.pt3_prev[tnum].x - spts.pt1_prev[tnum].x));
                            spts.y1_mov = (int) ((float)spts.pt1_prev[tnum].y + deriv*((float)spts.pt1.y - (float)spts.pt1_prev[tnum].y));
                            spts.y2_mov = (int) spts.y1_mov + spts.y2 - spts.y1; // the difference among them remains the same
                            //int x_mov = aw.Width/2 + spts.pt3_prev.x - spts.pt1_prev.x + deriv*((spts.pt3.x - spts.pt1.x) - (spts.pt3_prev.x - spts.pt1_prev.x));
                            spts.x1_mov = aw.Width/2;
                            //spts.x2_mov = aw.Width/2 + spts.pt2_prev[tnum].x - spts.pt1_prev[tnum].x - deriv*((spts.pt2_prev[tnum].x - spts.pt1_prev[tnum].x) - (spts.pt2.x - spts.pt1.x));
                            // Case 1: the distance to the next note is big (not a legato) -> note just fall off
                            if (spts.pt1.x - spts.pt2_prev[tnum].x > 5) // next_note t_on and old note t_off are far
                                if ( ! rProp.sep_render[2])
                                    cv::ellipse( image, spts.pt3_prev[tnum], cv::Size(spts.x3_mov-spts.x1_mov, spts.y3-spts.y2), 0, 0, 360, {(0.5-deriv)*blockL.getColour(tnum, (*it).pitch).b*(*it).vel*3/64, (0.5-deriv)*blockL.getColour(tnum, (*it).pitch).g*(*it).vel*3/64, (0.5-deriv)*blockL.getColour(tnum, (*it).pitch).r*(*it).vel*3/64}, -1, lineType);
                                else
                                    cv::ellipse( img_moving_notes, spts.pt3_prev[tnum], cv::Size(spts.x3_mov-spts.x1_mov, spts.y3-spts.y2), 0, 0, 360, {(0.5-deriv)*blockL.getColour(tnum, (*it).pitch).b*(*it).vel*3/64, (0.5-deriv)*blockL.getColour(tnum, (*it).pitch).g*(*it).vel*3/64, (0.5-deriv)*blockL.getColour(tnum, (*it).pitch).r*(*it).vel*3/64}, -1, lineType);
                            // Case 2: the end of the current note (prev) is close to the begin of the new note (legato) -> notes connect themselves
                            else
                                if ( ! rProp.sep_render[2])
                                    cv::ellipse( image, cv::Point(spts.x3_mov, spts.y3_mov), cv::Size(spts.x3_mov-spts.x1_mov, spts.y3-spts.y2), 0, 0, 360, {blockL.getColour(tnum, (*it).pitch).b*(*it).vel*3/64, blockL.getColour(tnum, (*it).pitch).g*(*it).vel*3/64, blockL.getColour(tnum, (*it).pitch).r*(*it).vel*3/64}, -1, lineType);
                                else
                                    cv::ellipse( img_moving_notes, cv::Point(spts.x3_mov, spts.y3_mov), cv::Size(spts.x3_mov-spts.x1_mov, spts.y3-spts.y2), 0, 0, 360, {blockL.getColour(tnum, (*it).pitch).b*(*it).vel*3/64, blockL.getColour(tnum, (*it).pitch).g*(*it).vel*3/64, blockL.getColour(tnum, (*it).pitch).r*(*it).vel*3/64}, -1, lineType);

                        }

                        spts.pt3_prev[tnum] = spts.pt3; // Next centre point = previous
                        spts.pt1_prev[tnum] = spts.pt1;
                        spts.pt2_prev[tnum] = spts.pt2;
                        spts.RadiusPrev[tnum] = radius;
                    }




                    // -------------------------- Draw Circles with size proportional to the velocity -------------------------
                    if ((*it).track == tnum && blockL.ActiveTracks[tnum] == true && blockL.shape[tnum] == 12) // All tracks
                    {
                        spts.x3 = (float)aw.Width*((-(float)aw.StartMidiTime + (float)(*it).t_middle)/((float)aw.EndMidiTime - (float)aw.StartMidiTime));
                        spts.y3 = (float)aw.Height/2 - (float)aw.Height*((float)((*it).pitch + 0.5 - ((float)mdt.PitchMin + mdt.PitchMax)/2)/((float)mdt.PitchMax - (float)mdt.PitchMin))*aw.VZoom - rProp.vertShift;
                        spts.pt3.x = f2int_safe(spts.x3); // Center x
                        spts.pt3.y = f2int_safe(spts.y3); // Center y
                        int radius = (*it).vel/2; // size proportion
                        if (spts.pt1.x > aw.Width/2) // The note block is before (to the right of) the center line
                            if ( ! rProp.sep_render[1])
                                //cv::fillPoly( image, pts, );
                                cv::circle( image, spts.pt3, radius, {blockL.getColour(tnum, (*it).pitch).b*(*it).vel*3/512, blockL.getColour(tnum, (*it).pitch).g*(*it).vel*3/512, blockL.getColour(tnum, (*it).pitch).r*(*it).vel*3/512}, -1, lineType);
                            else
                                cv::circle( img_buffer_sep_tracks[tnum], spts.pt3, radius, {blockL.getColour(tnum, (*it).pitch).b*(*it).vel*3/512, blockL.getColour(tnum, (*it).pitch).g*(*it).vel*3/512, blockL.getColour(tnum, (*it).pitch).r*(*it).vel*3/512}, -1, lineType);
                        else if (spts.pt1.x <= aw.Width/2 && spts.pt2.x > aw.Width/2) // The note block is inside the center line
                        {
                            if ( ! rProp.sep_render[0])
                                cv::circle( image, spts.pt3, 1.1*radius*(spts.x2-aw.Width/2)/(spts.x2-spts.x1), {blockL.getColour(tnum, (*it).pitch).b*(*it).vel/64, blockL.getColour(tnum, (*it).pitch).g*(*it).vel/64, blockL.getColour(tnum, (*it).pitch).r*(*it).vel/64}, -1, lineType);
                            else
                                cv::circle( img_playing_notes, spts.pt3, 1.1*radius*(spts.x2-aw.Width/2)/(spts.x2-spts.x1), {blockL.getColour(tnum, (*it).pitch).b*(*it).vel/64, blockL.getColour(tnum, (*it).pitch).g*(*it).vel/64, blockL.getColour(tnum, (*it).pitch).r*(*it).vel/64}, -1, lineType);
                        }
                        else // The note block is after (to the left of) the center line
                            if ( ! rProp.sep_render[1])
                                cv::circle( image, spts.pt3, radius, {blockL.getColour(tnum, (*it).pitch).b*(*it).vel/512, blockL.getColour(tnum, (*it).pitch).g*(*it).vel/512, blockL.getColour(tnum, (*it).pitch).r*(*it).vel/512}, 2, lineType);
                            else
                                cv::circle( img_buffer_sep_tracks[tnum], spts.pt3, radius, {blockL.getColour(tnum, (*it).pitch).b*(*it).vel/512, blockL.getColour(tnum, (*it).pitch).g*(*it).vel/512, blockL.getColour(tnum, (*it).pitch).r*(*it).vel/512}, 2, lineType);
                    }



                    // -------------------------- Draw Ellipses with height proportional to the velocity -------------------------
                    if ((*it).track == tnum && blockL.ActiveTracks[tnum] == true && blockL.shape[tnum] == 13) // All tracks - rectangle
                    {
                        spts.x3 = (float)aw.Width*((-(float)aw.StartMidiTime + (float)(*it).t_middle)/((float)aw.EndMidiTime - (float)aw.StartMidiTime));
                        spts.y3 = (float)aw.Height/2 - (float)aw.Height*((float)((*it).pitch + 0.5 - ((float)mdt.PitchMin + mdt.PitchMax)/2)/((float)mdt.PitchMax - (float)mdt.PitchMin))*aw.VZoom - rProp.vertShift;
                        spts.pt3.x = f2int_safe(spts.x3); // Center x
                        spts.pt3.y = f2int_safe(spts.y3); // Center y
                        if (spts.pt1.x > aw.Width/2) // The note block is before (to the right of) the center line
                            if ( ! rProp.sep_render[1])
                                //cv::fillPoly( image, pts, );
                                cv::ellipse( image, spts.pt3, cv::Size(spts.x3-spts.x1, 1 + (*it).vel/2 ), 0, 0, 360, {blockL.getColour(tnum, (*it).pitch).b*(*it).vel*3/512, blockL.getColour(tnum, (*it).pitch).g*(*it).vel*3/512, blockL.getColour(tnum, (*it).pitch).r*(*it).vel*3/512}, -1, lineType);
                            else
                                cv::ellipse( img_buffer_sep_tracks[tnum], spts.pt3, cv::Size(spts.x3-spts.x1, 1 + (*it).vel/2), 0, 0, 360, {blockL.getColour(tnum, (*it).pitch).b*(*it).vel*3/512, blockL.getColour(tnum, (*it).pitch).g*(*it).vel*3/512, blockL.getColour(tnum, (*it).pitch).r*(*it).vel*3/512}, -1, lineType);
                        else if (spts.pt1.x <= aw.Width/2 && spts.pt2.x > aw.Width/2) // The note block is inside the center line
                        {
                            if ( ! rProp.sep_render[0])
                                cv::ellipse( image, spts.pt3, cv::Size(spts.x3-spts.x1, 1 + (*it).vel/2 ), 0, 0, 360, {blockL.getColour(tnum, (*it).pitch).b*(*it).vel/64, blockL.getColour(tnum, (*it).pitch).g*(*it).vel/64, blockL.getColour(tnum, (*it).pitch).r*(*it).vel/64}, -1, lineType);
                            else
                                cv::ellipse( img_playing_notes, spts.pt3, cv::Size(spts.x3-spts.x1, 1 + (*it).vel/2 ), 0, 0, 360, {blockL.getColour(tnum, (*it).pitch).b*(*it).vel/64, blockL.getColour(tnum, (*it).pitch).g*(*it).vel/64, blockL.getColour(tnum, (*it).pitch).r*(*it).vel/64}, -1, lineType);
                        }
                        else // The note block is after (to the left of) the center line
                            if ( ! rProp.sep_render[1])
                                cv::ellipse( image, spts.pt3, cv::Size(spts.x3-spts.x1, 1 + (*it).vel/2 ), 0, 0, 360, {blockL.getColour(tnum, (*it).pitch).b*(*it).vel/512, blockL.getColour(tnum, (*it).pitch).g*(*it).vel/512, blockL.getColour(tnum, (*it).pitch).r*(*it).vel/512}, 2, lineType);
                            else
                                cv::ellipse( img_buffer_sep_tracks[tnum], spts.pt3, cv::Size(spts.x3-spts.x1, 1 + (*it).vel/2 ), 0, 0, 360, {blockL.getColour(tnum, (*it).pitch).b*(*it).vel/512, blockL.getColour(tnum, (*it).pitch).g*(*it).vel/512, blockL.getColour(tnum, (*it).pitch).r*(*it).vel/512}, 2, lineType);
                    }


                    // -------------------------- Draw Rhombus with height proportional to the velocity -------------------------
                    if ((*it).track == tnum && blockL.ActiveTracks[tnum] == true && blockL.shape[tnum] == 14) // All tracks - rectangle
                    {
                        spts.x3 = (float)aw.Width*((-(float)aw.StartMidiTime + (float)(*it).t_middle)/((float)aw.EndMidiTime - (float)aw.StartMidiTime));
                        spts.y3 = (float)aw.Height/2 - (float)aw.Height*((float)((*it).pitch + 0.5 - ((float)mdt.PitchMin + mdt.PitchMax)/2)/((float)mdt.PitchMax - (float)mdt.PitchMin))*aw.VZoom - rProp.vertShift;
                        spts.pt1.y = f2int_safe(spts.y3);
                        spts.pt2.y = f2int_safe(spts.y3);
                        spts.pt3.x = f2int_safe(spts.x3);
                        spts.pt3.y = f2int_safe(spts.y1) + (*it).vel/2;
                        spts.pt4.x = f2int_safe(spts.x3);
                        spts.pt4.y = f2int_safe(spts.y2) - (*it).vel/2;
                        cv::Point ptos[4];
                        ptos[0] = spts.pt1; ptos[1] = spts.pt3; ptos[2] = spts.pt2; ptos[3] = spts.pt4;
                        if (spts.pt1.x > aw.Width/2) // The note block is before (to the right of) the center line
                            if ( ! rProp.sep_render[1])
                                //cv::fillPoly( image, pts, );
                                cv::fillConvexPoly( image, ptos, 4, {blockL.getColour(tnum, (*it).pitch).b*(*it).vel*3/512, blockL.getColour(tnum, (*it).pitch).g*(*it).vel*3/512, blockL.getColour(tnum, (*it).pitch).r*(*it).vel*3/512}, lineType);
                            else
                                cv::fillConvexPoly( img_buffer_sep_tracks[tnum], ptos, 4, {blockL.getColour(tnum, (*it).pitch).b*(*it).vel*3/512, blockL.getColour(tnum, (*it).pitch).g*(*it).vel*3/512, blockL.getColour(tnum, (*it).pitch).r*(*it).vel*3/512}, lineType);
                        else if (spts.pt1.x <= aw.Width/2 && spts.pt2.x > aw.Width/2) // The note block is inside the center line
                        {
                            if ( ! rProp.sep_render[0])
                                cv::fillConvexPoly( image, ptos, 4, {blockL.getColour(tnum, (*it).pitch).b*(*it).vel/64, blockL.getColour(tnum, (*it).pitch).g*(*it).vel/64, blockL.getColour(tnum, (*it).pitch).r*(*it).vel/64}, lineType);
                            else
                                cv::fillConvexPoly( img_playing_notes, ptos, 4, {blockL.getColour(tnum, (*it).pitch).b*(*it).vel/64, blockL.getColour(tnum, (*it).pitch).g*(*it).vel/64, blockL.getColour(tnum, (*it).pitch).r*(*it).vel/64}, lineType);
                        }
                        else // The note block is after (to the left of) the center line
                            if ( ! rProp.sep_render[1])
                                cv::fillConvexPoly( image, ptos, 4, {blockL.getColour(tnum, (*it).pitch).b*(*it).vel/512, blockL.getColour(tnum, (*it).pitch).g*(*it).vel/512, blockL.getColour(tnum, (*it).pitch).r*(*it).vel/512}, lineType);
                            else
                                cv::fillConvexPoly( img_buffer_sep_tracks[tnum], ptos, 4, {blockL.getColour(tnum, (*it).pitch).b*(*it).vel/512, blockL.getColour(tnum, (*it).pitch).g*(*it).vel/512, blockL.getColour(tnum, (*it).pitch).r*(*it).vel/512}, lineType);
                    }



                    // -------------------------- Draw Triangles V with height proportional to the velocity -------------------------
                    if ((*it).track == tnum && blockL.ActiveTracks[tnum] == true && blockL.shape[tnum] == 15) // All tracks - rectangle
                    {
                        spts.x3 = (float)aw.Width*((-(float)aw.StartMidiTime + (float)(*it).t_middle)/((float)aw.EndMidiTime - (float)aw.StartMidiTime));
                        spts.y3 = (float)aw.Height/2 - (float)aw.Height*((float)((*it).pitch + 0.5 - ((float)mdt.PitchMin + mdt.PitchMax)/2)/((float)mdt.PitchMax - (float)mdt.PitchMin))*aw.VZoom - rProp.vertShift;
                        spts.pt1.y = f2int_safe(spts.y3);
                        spts.pt2.y = f2int_safe(spts.y3);
                        spts.pt3.x = f2int_safe(spts.x3);
                        spts.pt3.y = f2int_safe(spts.y1) + (*it).vel/2;
                        cv::Point ptos[3];
                        ptos[0] = spts.pt1; ptos[1] = spts.pt3; ptos[2] = spts.pt2;
                        if (spts.pt1.x > aw.Width/2) // The note block is before (to the right of) the center line
                            if ( ! rProp.sep_render[1])
                                //cv::fillPoly( image, pts, );
                                cv::fillConvexPoly( image, ptos, 3, {blockL.getColour(tnum, (*it).pitch).b*(*it).vel*3/512, blockL.getColour(tnum, (*it).pitch).g*(*it).vel*3/512, blockL.getColour(tnum, (*it).pitch).r*(*it).vel*3/512}, lineType);
                            else
                                cv::fillConvexPoly( img_buffer_sep_tracks[tnum], ptos, 3, {blockL.getColour(tnum, (*it).pitch).b*(*it).vel*3/512, blockL.getColour(tnum, (*it).pitch).g*(*it).vel*3/512, blockL.getColour(tnum, (*it).pitch).r*(*it).vel*3/512}, lineType);
                        else if (spts.pt1.x <= aw.Width/2 && spts.pt2.x > aw.Width/2) // The note block is inside the center line
                        {
                            if ( ! rProp.sep_render[0])
                                cv::fillConvexPoly( image, ptos, 3, {blockL.getColour(tnum, (*it).pitch).b*(*it).vel/64, blockL.getColour(tnum, (*it).pitch).g*(*it).vel/64, blockL.getColour(tnum, (*it).pitch).r*(*it).vel/64}, lineType);
                            else
                                cv::fillConvexPoly( img_playing_notes, ptos, 3, {blockL.getColour(tnum, (*it).pitch).b*(*it).vel/64, blockL.getColour(tnum, (*it).pitch).g*(*it).vel/64, blockL.getColour(tnum, (*it).pitch).r*(*it).vel/64}, lineType);
                        }
                        else // The note block is after (to the left of) the center line
                            if ( ! rProp.sep_render[1])
                                cv::fillConvexPoly( image, ptos, 3, {blockL.getColour(tnum, (*it).pitch).b*(*it).vel/512, blockL.getColour(tnum, (*it).pitch).g*(*it).vel/512, blockL.getColour(tnum, (*it).pitch).r*(*it).vel/512}, lineType);
                            else
                                cv::fillConvexPoly( img_buffer_sep_tracks[tnum], ptos, 3, {blockL.getColour(tnum, (*it).pitch).b*(*it).vel/512, blockL.getColour(tnum, (*it).pitch).g*(*it).vel/512, blockL.getColour(tnum, (*it).pitch).r*(*it).vel/512}, lineType);
                    }



                    // -------------------------- Draw Triangles ^ with height proportional to the velocity -------------------------
                    if ((*it).track == tnum && blockL.ActiveTracks[tnum] == true && blockL.shape[tnum] == 16) // All tracks - rectangle
                    {
                        spts.x3 = (float)aw.Width*((-(float)aw.StartMidiTime + (float)(*it).t_middle)/((float)aw.EndMidiTime - (float)aw.StartMidiTime));
                        spts.y3 = (float)aw.Height/2 - (float)aw.Height*((float)((*it).pitch + 0.5 - ((float)mdt.PitchMin + mdt.PitchMax)/2)/((float)mdt.PitchMax - (float)mdt.PitchMin))*aw.VZoom - rProp.vertShift;
                        spts.pt1.y = f2int_safe(spts.y3);
                        spts.pt2.y = f2int_safe(spts.y3);
                        spts.pt3.x = f2int_safe(spts.x3);
                        spts.pt3.y = f2int_safe(spts.y2) - (*it).vel/2;
                        cv::Point ptos[3];
                        ptos[0] = spts.pt1; ptos[1] = spts.pt3; ptos[2] = spts.pt2;
                        if (spts.pt1.x > aw.Width/2) // The note block is before (to the right of) the center line
                            if ( ! rProp.sep_render[1])
                                //cv::fillPoly( image, pts, );
                                cv::fillConvexPoly( image, ptos, 3, {blockL.getColour(tnum, (*it).pitch).b*(*it).vel*3/512, blockL.getColour(tnum, (*it).pitch).g*(*it).vel*3/512, blockL.getColour(tnum, (*it).pitch).r*(*it).vel*3/512}, lineType);
                            else
                                cv::fillConvexPoly( img_buffer_sep_tracks[tnum], ptos, 3, {blockL.getColour(tnum, (*it).pitch).b*(*it).vel*3/512, blockL.getColour(tnum, (*it).pitch).g*(*it).vel*3/512, blockL.getColour(tnum, (*it).pitch).r*(*it).vel*3/512}, lineType);
                        else if (spts.pt1.x <= aw.Width/2 && spts.pt2.x > aw.Width/2) // The note block is inside the center line
                        {
                            if ( ! rProp.sep_render[0])
                                cv::fillConvexPoly( image, ptos, 3, {blockL.getColour(tnum, (*it).pitch).b*(*it).vel/64, blockL.getColour(tnum, (*it).pitch).g*(*it).vel/64, blockL.getColour(tnum, (*it).pitch).r*(*it).vel/64}, lineType);
                            else
                                cv::fillConvexPoly( img_playing_notes, ptos, 3, {blockL.getColour(tnum, (*it).pitch).b*(*it).vel/64, blockL.getColour(tnum, (*it).pitch).g*(*it).vel/64, blockL.getColour(tnum, (*it).pitch).r*(*it).vel/64}, lineType);
                        }
                        else // The note block is after (to the left of) the center line
                            if ( ! rProp.sep_render[1])
                                cv::fillConvexPoly( image, ptos, 3, {blockL.getColour(tnum, (*it).pitch).b*(*it).vel/512, blockL.getColour(tnum, (*it).pitch).g*(*it).vel/512, blockL.getColour(tnum, (*it).pitch).r*(*it).vel/512}, lineType);
                            else
                                cv::fillConvexPoly( img_buffer_sep_tracks[tnum], ptos, 3, {blockL.getColour(tnum, (*it).pitch).b*(*it).vel/512, blockL.getColour(tnum, (*it).pitch).g*(*it).vel/512, blockL.getColour(tnum, (*it).pitch).r*(*it).vel/512}, lineType);
                    }




                    // -------------------------- Draw Triangles |> (forte-piano) -------------------------
                    if ((*it).track == tnum && blockL.ActiveTracks[tnum] == true && blockL.shape[tnum] == 17) // All tracks - rectangle
                    {
                        spts.x3 = (float)aw.Width*((-(float)aw.StartMidiTime + (float)(*it).t_middle)/((float)aw.EndMidiTime - (float)aw.StartMidiTime));
                        spts.y3 = (float)aw.Height/2 - (float)aw.Height*((float)((*it).pitch + 0.5 - ((float)mdt.PitchMin + mdt.PitchMax)/2)/((float)mdt.PitchMax - (float)mdt.PitchMin))*aw.VZoom - rProp.vertShift;
                        spts.pt1.y = f2int_safe(spts.y1);
                        spts.pt2.y = f2int_safe(spts.y2);
                        spts.pt3.x = f2int_safe(spts.x2);
                        spts.pt3.y = f2int_safe(spts.y3);
                        cv::Point ptos[3];
                        ptos[0] = cv::Point(spts.pt1.x, spts.pt1.y); ptos[1] = spts.pt3; ptos[2] = cv::Point(spts.pt1.x, spts.pt2.y);
                        if (spts.pt1.x > aw.Width/2) // The note block is before (to the right of) the center line
                            if ( ! rProp.sep_render[1])
                                //cv::fillPoly( image, pts, );
                                cv::fillConvexPoly( image, ptos, 3, {blockL.getColour(tnum, (*it).pitch).b*(*it).vel*3/512, blockL.getColour(tnum, (*it).pitch).g*(*it).vel*3/512, blockL.getColour(tnum, (*it).pitch).r*(*it).vel*3/512}, lineType);
                            else
                                cv::fillConvexPoly( img_buffer_sep_tracks[tnum], ptos, 3, {blockL.getColour(tnum, (*it).pitch).b*(*it).vel*3/512, blockL.getColour(tnum, (*it).pitch).g*(*it).vel*3/512, blockL.getColour(tnum, (*it).pitch).r*(*it).vel*3/512}, lineType);
                        else if (spts.pt1.x <= aw.Width/2 && spts.pt2.x > aw.Width/2) // The note block is inside the center line
                        {
                            float nprogress = (float)(spts.x2-aw.Width/2)/(spts.x2-spts.x1);
                            if ( ! rProp.sep_render[0])
                                cv::fillConvexPoly( image, ptos, 3, {nprogress*blockL.getColour(tnum, (*it).pitch).b*(*it).vel/64, nprogress*blockL.getColour(tnum, (*it).pitch).g*(*it).vel/64, nprogress*blockL.getColour(tnum, (*it).pitch).r*(*it).vel/64}, lineType);
                            else
                                cv::fillConvexPoly( img_playing_notes, ptos, 3, {nprogress*blockL.getColour(tnum, (*it).pitch).b*(*it).vel/32, nprogress*blockL.getColour(tnum, (*it).pitch).g*(*it).vel/32, nprogress*blockL.getColour(tnum, (*it).pitch).r*(*it).vel/32}, lineType);
                        }
                        else // The note block is after (to the left of) the center line
                            if ( ! rProp.sep_render[1])
                                cv::fillConvexPoly( image, ptos, 3, {blockL.getColour(tnum, (*it).pitch).b*(*it).vel/512, blockL.getColour(tnum, (*it).pitch).g*(*it).vel/512, blockL.getColour(tnum, (*it).pitch).r*(*it).vel/512}, lineType);
                            else
                                cv::fillConvexPoly( img_buffer_sep_tracks[tnum], ptos, 3, {blockL.getColour(tnum, (*it).pitch).b*(*it).vel/512, blockL.getColour(tnum, (*it).pitch).g*(*it).vel/512, blockL.getColour(tnum, (*it).pitch).r*(*it).vel/512}, lineType);
                    }



                    // -------------------------- Draw Triangles <| (piano-forte) -------------------------
                    if ((*it).track == tnum && blockL.ActiveTracks[tnum] == true && blockL.shape[tnum] == 18) // All tracks - rectangle
                    {
                        spts.x3 = (float)aw.Width*((-(float)aw.StartMidiTime + (float)(*it).t_middle)/((float)aw.EndMidiTime - (float)aw.StartMidiTime));
                        spts.y3 = (float)aw.Height/2 - (float)aw.Height*((float)((*it).pitch + 0.5 - ((float)mdt.PitchMin + mdt.PitchMax)/2)/((float)mdt.PitchMax - (float)mdt.PitchMin))*aw.VZoom - rProp.vertShift;
                        spts.pt1.y = f2int_safe(spts.y1);
                        spts.pt2.y = f2int_safe(spts.y2);
                        spts.pt3.x = f2int_safe(spts.x2);
                        spts.pt3.y = f2int_safe(spts.y3);
                        cv::Point ptos[3];
                        ptos[0] = cv::Point(spts.pt1.x, spts.pt3.y); ptos[1] = cv::Point(spts.pt2.x, spts.pt1.y); ptos[2] = cv::Point(spts.pt2.x, spts.pt2.y);
                        if (spts.pt1.x > aw.Width/2) // The note block is before (to the right of) the center line
                            if ( ! rProp.sep_render[1])
                                //cv::fillPoly( image, pts, );
                                cv::fillConvexPoly( image, ptos, 3, {blockL.getColour(tnum, (*it).pitch).b*(*it).vel*3/512, blockL.getColour(tnum, (*it).pitch).g*(*it).vel*3/512, blockL.getColour(tnum, (*it).pitch).r*(*it).vel*3/512}, lineType);
                            else
                                cv::fillConvexPoly( img_buffer_sep_tracks[tnum], ptos, 3, {blockL.getColour(tnum, (*it).pitch).b*(*it).vel*3/512, blockL.getColour(tnum, (*it).pitch).g*(*it).vel*3/512, blockL.getColour(tnum, (*it).pitch).r*(*it).vel*3/512}, lineType);
                        else if (spts.pt1.x <= aw.Width/2 && spts.pt2.x > aw.Width/2) // The note block is inside the center line
                        {
                            float nprogress = 1.5 - (float)(spts.x2-aw.Width/2)/(spts.x2-spts.x1);
                            if ( ! rProp.sep_render[0])
                                cv::fillConvexPoly( image, ptos, 3, {nprogress*blockL.getColour(tnum, (*it).pitch).b*(*it).vel/64, nprogress*blockL.getColour(tnum, (*it).pitch).g*(*it).vel/64, nprogress*blockL.getColour(tnum, (*it).pitch).r*(*it).vel/64}, lineType);
                            else
                                cv::fillConvexPoly( img_playing_notes, ptos, 3, {nprogress*blockL.getColour(tnum, (*it).pitch).b*(*it).vel/32, nprogress*blockL.getColour(tnum, (*it).pitch).g*(*it).vel/32, nprogress*blockL.getColour(tnum, (*it).pitch).r*(*it).vel/32}, lineType);
                        }
                        else // The note block is after (to the left of) the center line
                            if ( ! rProp.sep_render[1])
                                cv::fillConvexPoly( image, ptos, 3, {blockL.getColour(tnum, (*it).pitch).b*(*it).vel/512, blockL.getColour(tnum, (*it).pitch).g*(*it).vel/512, blockL.getColour(tnum, (*it).pitch).r*(*it).vel/512}, lineType);
                            else
                                cv::fillConvexPoly( img_buffer_sep_tracks[tnum], ptos, 3, {blockL.getColour(tnum, (*it).pitch).b*(*it).vel/512, blockL.getColour(tnum, (*it).pitch).g*(*it).vel/512, blockL.getColour(tnum, (*it).pitch).r*(*it).vel/512}, lineType);
                    }




                    // End shape drawings ------------------------------------------------


                    // ============= Vertical Lines from tracks =============

                    if ((*it).track == tnum && rProp.lines[3] && rProp.vlines_track_n == (int)tnum && blockL.vLines)
                    {
                        cv::line(image, cv::Point(spts.pt1.x, aw.Height), cv::Point(spts.pt1.x, 0), {(double)(double)rProp.vlines_colour[2]*(*it).vel/128, (double)(double)rProp.vlines_colour[1]*(*it).vel/128, (double)(double)rProp.vlines_colour[0]*(*it).vel/128});
                    }
                }
            }
            else  // chordLayer
            {
                if (chordL.CLType == ChordLayers::ChordLayerType::Tonnetz)
                {
                    if (chordL.ActiveTracks[(*it).track] == true)
                    {
                        if (spts.pt1.x <= aw.Width/2 && spts.pt2.x > aw.Width/2) // The note block is inside the center line
                        {
                            float noteProgress = (float)(aw.Width/2 - spts.pt1.x)/(spts.pt2.x - spts.pt1.x);
                            if ( ! rProp.sep_render[0])
                                TonnetzRenderer::renderNote((*it).pitch, noteProgress, image, clCentre, opt, RBuffer->TonnetzMap, chordL.getColour((*it).track, (*it).pitch));
                            else
                                TonnetzRenderer::renderNote((*it).pitch, noteProgress, img_playing_notes, clCentre, opt, RBuffer->TonnetzMap, chordL.getColour((*it).track, (*it).pitch));
                        }
                    }
                }
                if (chordL.CLType == ChordLayers::ChordLayerType::ChordStar)
                {
                    if (chordL.ActiveTracks[(*it).track] == true)
                    {
                        if (spts.pt1.x <= aw.Width/2 && spts.pt2.x > aw.Width/2) // The note block is inside the center line
                        {
                            float noteProgress = (float)(aw.Width/2 - spts.pt1.x)/(spts.pt2.x - spts.pt1.x);
                            if ( ! rProp.sep_render[0])
                                ChordStar::renderNote((*it).pitch, noteProgress, (Pitch::circle)chordL.ChordStarType, 10, chordL.getColour((*it).track, (*it).pitch), image, clCentre, chordL.w, chordL.TurnChordCircle);
                            else
                                ChordStar::renderNote((*it).pitch, noteProgress, (Pitch::circle)chordL.ChordStarType, 10, chordL.getColour((*it).track, (*it).pitch), img_playing_notes, clCentre, chordL.w, chordL.TurnChordCircle);
                        }
                    }
                }
            }
        }
    }
    //    spts.pt3.x = window_width/2;
    //    spts.pt4.x = window_width/2;
    //    spts.pt3.y = aw.Height;
    //    spts.pt4.y = 0;
    // =================== Vertical Lines =============
        if (rProp.lines[0] && blockL.vLines) // cetered line
            cv::line(image, cv::Point(aw.Width/2, aw.Height), cv::Point(aw.Width/2, 0), {100,100,100});
        if (rProp.lines[1] && blockL.vLines) // vertical lines from time signatures saved in the midi file
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
                        cv::line(image, cv::Point((int)((float)aw.Width*((-(float)aw.StartMidiTime + (float)i)/((float)aw.EndMidiTime - (float)aw.StartMidiTime))), aw.Height), cv::Point((int)((double)aw.Width*((-(double)aw.StartMidiTime + (double)i)/((double)aw.EndMidiTime - (double)aw.StartMidiTime))), 0), {(double)rProp.vlines_colour[2], (double)rProp.vlines_colour[1], (double)rProp.vlines_colour[0]});
                    else
                        cv::line(image, cv::Point((int)((float)aw.Width*((-(float)aw.StartMidiTime + (float)i)/((float)aw.EndMidiTime - (float)aw.StartMidiTime))), aw.Height), cv::Point((int)((double)aw.Width*((-(double)aw.StartMidiTime + (double)i)/((double)aw.EndMidiTime - (double)aw.StartMidiTime))), 0), {0.6*(double)rProp.vlines_colour[2], 0.6*(double)rProp.vlines_colour[1], 0.6*(double)rProp.vlines_colour[0]});
                    k++;
                }
                tsig_next = tsig;
            }
        }
        if (rProp.lines[2] && blockL.vLines) // manual time signature, given measure
        {
            int denominator = std::pow(2, rProp.beat_measure_manual[1]);
            for (unsigned int i = 0; i < aw.EndMidiTime; i = i + 4*rProp.beat_measure_manual[0]*mdt.Tpq/denominator)
            {
                cv::line(image, cv::Point((int)((float)aw.Width*((-(float)aw.StartMidiTime + (float)i)/((float)aw.EndMidiTime - (float)aw.StartMidiTime))), aw.Height), cv::Point((int)((double)aw.Width*((-(double)aw.StartMidiTime + (double)i)/((double)aw.EndMidiTime - (double)aw.StartMidiTime))), 0), {(double)rProp.vlines_colour[2], (double)rProp.vlines_colour[1], (double)rProp.vlines_colour[0]});
            }
        }


    // ==================== Horizontal Lines =============
        if (rProp.hlines && blockL.hLines)
        {
            float note_height = (float)aw.Height*((float)(1.0)/((float)mdt.PitchMax - (float)mdt.PitchMin))*aw.VZoom; // this is the height of a note in pixels, i.e. the vertical space between 2 midi notes in the image
            int basePitchRef = mdt.PitchMax%12; // this is the reference for the pitch
            int pitch_span = mdt.PitchMax - mdt.PitchMin; // this is the whole pitch extent from the midi file
            if(rProp.hlines_type == 0) // One line every n semitones + half shift
            {
                for (int i = 0; i < pitch_span/rProp.hlines_n + 1; i++)
                {
                    int hline_y = (float)aw.Height/2 - note_height*(pitch_span)/2 + (note_height*i*rProp.hlines_n) - rProp.vertShift - rProp.hlines_offset*note_height + rProp.half_shift*note_height/2;
                    hline_y = hline_y + basePitchRef*note_height;
                    cv::line(image, cv::Point(0, hline_y), cv::Point(aw.Width, hline_y), {(double)rProp.hlines_colour[2], (double)rProp.hlines_colour[1], (double)rProp.hlines_colour[0]});
                }
            }
            if(rProp.hlines_type == 1) // One line every semitone and a brighter line every n semitones
            {
                for (int i = 0; i < pitch_span; i++)
                {
                    if (i%rProp.hlines_n == basePitchRef)
                    {
                       int hline_y = (float)aw.Height/2 - note_height*(pitch_span)/2 + (note_height*i) - rProp.vertShift - rProp.hlines_offset*note_height - rProp.half_shift*note_height/2;
                       cv::line(image, cv::Point(0, hline_y), cv::Point(aw.Width, hline_y), {(double)rProp.hlines_colour[2], (double)rProp.hlines_colour[1], (double)rProp.hlines_colour[0]});
                    }
                    else
                    {
                        int hline_y = (float)aw.Height/2 - note_height*(pitch_span)/2 + (note_height*i) - rProp.vertShift - rProp.hlines_offset*note_height - rProp.half_shift*note_height/2;
                        cv::line(image, cv::Point(0, hline_y), cv::Point(aw.Width, hline_y), {(double)rProp.hlines_colour[2]/2, (double)rProp.hlines_colour[1]/2, (double)rProp.hlines_colour[0]/2});
                    }
                }
            }
            if(rProp.hlines_type == 2) // Keyboard
            {
                for (int i = 0; i < pitch_span + 1; i++)
                {
                    int ii = (i - basePitchRef)%12;
                    if (ii == 1 || ii == 3 || ii == 5 || ii == 8 || ii == 10)
                    {
                        int hline_y = (float)aw.Height/2 - note_height*(pitch_span)/2 + (note_height*i) - rProp.vertShift - rProp.hlines_offset*note_height - rProp.half_shift*note_height/2;
                        cv::rectangle(image, cv::Point(0, hline_y + 1), cv::Point(aw.Width, hline_y + note_height), {(double)rProp.hlines_colour[2], (double)rProp.hlines_colour[1], (double)rProp.hlines_colour[0]}, -1);
                    }
                }
            }
            if(rProp.hlines_type == 3) // Inverse Keyboard
            {
                for (int i = 0; i < pitch_span + 1; i++)
                {
                    int ii = (i - basePitchRef)%12;
                    if (ii == 0 || ii == 2 || ii == 4 || ii == 6 || ii == 7 || ii == 9 || ii == 11)
                    {
                        int hline_y = (float)aw.Height/2 - note_height*(pitch_span)/2 + (note_height*i) - rProp.vertShift - rProp.hlines_offset*note_height - rProp.half_shift*note_height/2;
                        cv::rectangle(image, cv::Point(0, hline_y + 1), cv::Point(aw.Width, hline_y + note_height), {(double)rProp.hlines_colour[2], (double)rProp.hlines_colour[1], (double)rProp.hlines_colour[0]}, -1);
                    }
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
            for (int j = 0; j < (mdt.NTracks); j++)
            {
                if (blockL.track_blur[j] > 0)
                    cv::boxFilter(img_buffer_sep_tracks[j], img_buffer_sep_tracks[j], -1, cv::Size(blockL.track_blur[j], blockL.track_blur[j]));
                image = image + img_buffer_sep_tracks[j];
                img_buffer_sep_tracks[j] = cv::Mat::zeros(aw.Height, aw.Width, CV_8UC3);
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
}

void AnimPainter::paintChords(Chord chord, float chordProgress, cv::Mat image, AnimWindow aw, ChordLayers chordL, RenderP rProp)
{
    int p_x = chordL.x_pos*aw.Width;
    int p_y = chordL.y_pos*aw.Height;

    cv::Point centre = cv::Point(p_x, p_y);
    int diam = chordL.w;
    cv::LineTypes lineType = rProp.shapeLineType;

    // ============ Displaying note names ==============
    if (chordL.CLType == ChordLayers::ChordLayerType::PitchNames) // ToDo: create a new class for chord analysis, generate chord names, currently displaying only pitches
    {
        std::string ptStr = "Pitches:";
        ptStr = chord.getPitchesStr(chordL.AccidentalSharp, chordL.ActiveTracks);
        cv::putText(image,
                ptStr,//"Here is some text",
                centre, // Coordinates
                cv::FONT_HERSHEY_COMPLEX_SMALL, // Font
                (float) chordL.w/100, // Scale. 2.0 = 2x bigger
                cv::Scalar(255,255,255), // BGR Color
                1, // Line Thickness (Optional)
                cv::LINE_AA); // Anti-alias (Optional)
    }

    // ============ Displaying chord names ============== ToDo

    // ============ Displaying circle / star ==============
    if (chordL.CLType == ChordLayers::ChordLayerType::ChordStar)
    {
        Chord::circle type = chordL.ChordStarType;
        ChordStar::renderChordStar(chord, type, image, centre, diam, chordL.ActiveTracks, chordL.TurnChordCircle);
    }
    // ============ Displaying tonnetz in function paint notes, since notes are not interdependent ==============
}

void AnimPainter::paintLayers(MusicData mdt, cv::Mat image, std::vector<cv::Mat> img_buffer_sep_tracks, AnimWindow aw, std::list<LayerContainer> layers, RenderP renderS)
{
    std::list<LayerContainer>::reverse_iterator lit = layers.rbegin();
    for(int iLayer = 0; lit != layers.rend(); lit++, iLayer++)
    {
        // (*it) is now the layer of the current iteration
        if ((*lit).LType == LayerContainer::LayerType::BlockLayer && (*lit).LayerActive == true)
        {
            this->paintNotes(mdt, image, img_buffer_sep_tracks, aw, (*lit).Bl, (*lit).Cl, renderS, LayerContainer::BlockLayer);
        }
        else if ((*lit).LType == LayerContainer::LayerType::ChordLayer && (*lit).LayerActive == true)
        {
            int zoom = aw.EndMidiTime - aw.StartMidiTime;
            aw.CurrPosMiddle = (aw.StartMidiTime + (zoom)/2);

            int p_x = (*lit).Cl.x_pos*aw.Width;
            int p_y = (*lit).Cl.y_pos*aw.Height;
            cv::Point centre = cv::Point(p_x, p_y);
            int diam = (*lit).Cl.w;

            // ============ Displaying circle / star grid ==============
            if ((*lit).Cl.CLType == ChordLayers::ChordLayerType::ChordStar)
            {
                Chord::circle type = (*lit).Cl.ChordStarType;
                if ( ! ((*lit).Cl.NoteNamesOnStar))
                {
                    ChordStar::dispChordDisc(type, image, centre, diam, (*lit).Cl.GridColour, false, (*lit).Cl.TurnChordCircle, (*lit).Cl.AccidentalSharp);
                }
                else if ((*lit).Cl.NoteNamesOnStar)
                {
                    ChordStar::dispChordDisc(type, image, centre, diam, (*lit).Cl.GridColour, true, (*lit).Cl.TurnChordCircle, (*lit).Cl.AccidentalSharp);
                }
            }
            // ============ Displaying tonnetz grid ==============
            else if ((*lit).Cl.CLType == ChordLayers::ChordLayerType::Tonnetz)
            {
                TonnetzRenderer::renderGrid(image, centre, RBuffer->TonnetzGridPositions, (*lit).Cl.CellDiameter, (*lit).Cl.HexLayout, (*lit).Cl.TonnetzShape, (*lit).Cl.GridColour);
            }

            std::list<ChordWithTime>::iterator cit;
            std::list<ChordWithTime>::iterator cit_next;
            for (cit = mdt.GChords.ChordsWTime.begin(), cit_next = ++mdt.GChords.ChordsWTime.begin(); cit_next!=(mdt.GChords.ChordsWTime.end()); ++cit, ++cit_next) // run through all chords
            {
                ChordWithTime chordWT = *cit;
                ChordWithTime chordWT_next = *cit_next;
                if (aw.CurrPosMiddle > chordWT_next.Start_time && (aw.CurrPosMiddle < chordWT.Start_time) && cit!=mdt.GChords.ChordsWTime.begin() && cit!=mdt.GChords.ChordsWTime.end())
                {
                    float chordProgress = (float)(aw.CurrPosMiddle - chordWT.Start_time)/(chordWT_next.Start_time - chordWT.Start_time);
                    this->paintChords((*cit).chord, chordProgress, image, aw, (*lit).Cl, renderS);
                    this->paintNotes(mdt, image, img_buffer_sep_tracks, aw, (*lit).Bl, (*lit).Cl, renderS, LayerContainer::ChordLayer);
                    break;
                }
            }
        }
    }
}

void AnimPainter::appendFrame(cv::Mat image, VideoRecorder *vRec)
{
    if (vRec != nullptr)
    {
        vRec->writeFrame(image);
    }
}
