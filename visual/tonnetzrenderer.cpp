#include "tonnetzrenderer.h"

using namespace Hexagon;

TonnetzOptions::TonnetzOptions(Shape shape, bool *tracks, Hexagon::Layout layout)
{
    this->Shp = shape;
    this->Tracks = tracks;
    this->Layout = layout;
}

void TonnetzRenderer::renderGrid(cv::Mat mat, cv::Point centre, vector<Hex> GridPositions, int cellDiameter, Layout layout, TonnetzOptions::Shape shape)
{
    if (shape == TonnetzOptions::Shape::Circle)
    {
        std::vector<Hex>::iterator it = GridPositions.begin();
        for (int iHex = 0; it != GridPositions.end(); it++, iHex++)
        {
            cv::Point cvCentre;
            Point2d hexCentre = hex_to_pixel(layout, (*it));
            cvCentre.x = hexCentre.x;
            cvCentre.y = hexCentre.y;
            cvCentre = cvCentre + centre;
            cv::circle(mat, cvCentre, cellDiameter, cv::Scalar(200,200,200), 1, cv::LineTypes::LINE_AA);
        }
    }
    else if (shape == TonnetzOptions::Shape::Hexagon)
    {
        std::vector<Hex>::iterator it = GridPositions.begin();
        for (int iHex = 0; it != GridPositions.end(); it++, iHex++)
        {
            std::vector<Point2d> polygon = polygon_corners(layout, (*it), (double)cellDiameter/10);
            std::vector<Point2d>::iterator jt = polygon.begin();
            std::array<cv::Point, 6> pts;
            for (int k = 0; jt != polygon.end(); jt++, k++)
            {
                cv::Point p;
                p.x = (*jt).x;
                p.y = (*jt).y;
                p = p + centre;
                pts.at(k) = p;
            }
            cv::polylines(mat, pts, true, cv::Scalar(200,200,200), 1, cv::LineTypes::LINE_AA);
        }
    }
}

void TonnetzRenderer::renderChord(Chord currChord, float chordProgress, cv::Mat mat, cv::Point centre, TonnetzOptions options, std::unordered_map<int, Hexagon::Hex> eulerTonnerzMap)
{
    int size = options.NoteSize - (options.NoteCollapse * chordProgress * options.NoteSize);
    // toDo: fadeOut
    std::vector<Hex> hexagons = EulerTonnetz::getHexagonsTracks(currChord, options.Tracks, false, eulerTonnerzMap, options.Central);
    if (options.Shp == TonnetzOptions::Shape::Circle)
    {
        std::vector<Hex>::iterator it = hexagons.begin();
        for (int iHex = 0; it != hexagons.end(); it++)
        {
            cv::Point cvCentre;
            Point2d hexCentre = hex_to_pixel(options.Layout, (*it));
            cvCentre.x = hexCentre.x;
            cvCentre.y = hexCentre.y;
            cvCentre = cvCentre + centre;
            cv::circle(mat, cvCentre, size, cv::Scalar(180,180,180), cv::LineTypes::FILLED);
        }
    }
    else if (options.Shp == TonnetzOptions::Shape::Hexagon)
    {
        std::vector<Hex>::iterator it = hexagons.begin();
        for (int iHex = 0; it != hexagons.end(); it++)
        {
            std::vector<Point2d> polygon = polygon_corners(options.Layout, (*it));
            std::vector<Point2d>::iterator jt = polygon.begin();
            std::array<cv::Point, 6> pts;
            for (int k = 0; jt != polygon.end(); jt++, k++)
            {
                cv::Point p;
                p.x = (*jt).x;
                p.y = (*jt).y;
                p = p + centre;
                pts.at(k) = p;
            }
            cv::fillConvexPoly(mat, pts, cv::Scalar(180,180,180), cv::LineTypes::LINE_AA);
        }
    }
}

void TonnetzRenderer::renderNote(Pitch note, float noteProgress, cv::Mat mat, cv::Point centre, TonnetzOptions options, std::unordered_map<int, Hexagon::Hex> eulerTonnerzMap, rgb colour)
{
    int size = options.NoteSize - (options.NoteCollapse * noteProgress * options.NoteSize);
    rgb currentColour = colour * (1 - options.NoteFadeOut * noteProgress);
    // toDo: fadeOut
    Hex hex = EulerTonnetz::getHexagon(note, false, eulerTonnerzMap, options.Central);
    if (options.Shp == TonnetzOptions::Shape::Circle)
    {
        cv::Point cvCentre;
        Point2d hexCentre = hex_to_pixel(options.Layout, (hex));
        cvCentre.x = hexCentre.x;
        cvCentre.y = hexCentre.y;
        cvCentre = cvCentre + centre;
        cv::circle(mat, cvCentre, size, cv::Scalar(currentColour.b, currentColour.g, currentColour.r), cv::LineTypes::FILLED);
    }
    else if (options.Shp == TonnetzOptions::Shape::Hexagon)
    {
        std::vector<Point2d> polygon = polygon_corners(options.Layout, (hex), (double)size/10);
        std::vector<Point2d>::iterator jt = polygon.begin();
        std::array<cv::Point, 6> pts;
        for (int k = 0; jt != polygon.end(); jt++, k++)
        {
            cv::Point p;
            p.x = (*jt).x;
            p.y = (*jt).y;
            p = p + centre;
            pts.at(k) = p;
        }
        cv::fillConvexPoly(mat, pts, cv::Scalar(currentColour.b, currentColour.g, currentColour.r), cv::LineTypes::LINE_AA);
    }
}
