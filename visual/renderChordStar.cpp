#include "renderChordStar.h"

void ChordStar::renderChordStar(Chord chord, Chord::circle type, cv::Mat mat, cv::Point centre, int diameter, rgb lineColour, bool *tracks, int turn) // no need to pass by reference or return mat, since cv::Mat is already a reference
{
    std::set<float> angles = chord.getAnglesDeg(type, tracks);

    for (std::set<float>::iterator ptr = angles.begin(); ptr != angles.end(); ++ptr)
    {
        float currAngle = (*ptr) + turn*30;
        float currAngleRad = currAngle * M_PI/180;
        cv::Point pointRel = cv::Point((float)diameter*cos(currAngleRad), (float)diameter*sin(currAngleRad));
        cv::Point pointAbs = centre + pointRel;
//        cv::circle(mat, pointAbs, 3, cv::Scalar(100,100,100), -1); // drawing circle - update: now, being drawn as notes (for colour and other stuff)
        for (std::set<float>::iterator ptr2 = ptr; ptr2 != angles.end(); ++ptr2) // let's run through the next points and draw a line between then
        {
            float currAngle2 = (*ptr2) + turn*30;
            float currAngleRad2 = currAngle2 * M_PI/180;
            cv::Point pointRel2 = cv::Point(diameter*cos(currAngleRad2), diameter*sin(currAngleRad2));
            cv::Point pointAbs2 = centre + pointRel2;
            cv::line(mat, pointAbs, pointAbs2, cv::Scalar(lineColour.b, lineColour.g, lineColour.r), 2);
        }
    }
}

void ChordStar::renderNote(Pitch note, float noteProgress, rgb colour, cv::Mat mat, cv::Point centre, int diameter, ChordStarOptions opt)
{
    rgb currentColour = colour * (1 - opt.NoteFadeOut * noteProgress);
    int radius = opt.Radius - (opt.NoteCollapse * noteProgress * opt.Radius);
    float angle = note.getAngleDeg(opt.Type) + opt.TurnCircle*30;
    float currAngleRad = angle * M_PI/180;
    cv::Point pointRel = cv::Point((float)diameter*cos(currAngleRad), (float)diameter*sin(currAngleRad));
    cv::Point pointAbs = centre + pointRel;
    cv::circle(mat, pointAbs, radius, cv::Scalar(currentColour.b, currentColour.g, currentColour.r), -1); // drawing circle
}

void ChordStar::dispChordDisc(Chord::circle type, cv::Mat mat, cv::Point centre, int diameter, rgb colour, bool dispPitchNames, int turn, bool accidentalSharp)
{
    cv::circle(mat, centre, diameter + 10, cv::Scalar(0, 0, 0), -1);
    std::list<Pitch> allPitches = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11}; // init
    int i = turn;
    for (std::list<Pitch>::iterator ptr = allPitches.begin(); ptr != allPitches.end(); ++ptr)
    {
        Pitch p = (*ptr);
        float currAngleRad = i*30 * M_PI/180;
        cv::Point pointRel = cv::Point((float)diameter*cos(currAngleRad), (float)diameter*sin(currAngleRad));
        cv::Point pointAbs = centre + pointRel;
        cv::circle(mat, pointAbs, 5, cv::Scalar(colour.b, colour.g, colour.r));
        if (dispPitchNames)
        {
            if (accidentalSharp == false)
                cv::putText(mat,
                        p.getLetterName(Pitch::Accidental::flat),//"Here is some text",
                        pointAbs, // Coordinates
                        cv::FONT_HERSHEY_COMPLEX_SMALL, // Font
                        1.0, // Scale. 2.0 = 2x bigger
                        cv::Scalar(colour.b, colour.g, colour.r), // BGR Color
                        1, // Line Thickness (Optional)
                        cv::LINE_AA); // Anti-alias (Optional)
            else
                cv::putText(mat,
                        p.getLetterName(Pitch::Accidental::sharp),//"Here is some text",
                        pointAbs, // Coordinates
                        cv::FONT_HERSHEY_COMPLEX_SMALL, // Font
                        1.0, // Scale. 2.0 = 2x bigger
                        cv::Scalar(colour.b, colour.g, colour.r), // BGR Color
                        1, // Line Thickness (Optional)
                        cv::LINE_AA); // Anti-alias (Optional)
        }
        if (type == Chord::circleOfSemitones)
            {i++;}
        else if (type == Chord::circleOfFifths)
            {i = i + 7;}
    }
}
