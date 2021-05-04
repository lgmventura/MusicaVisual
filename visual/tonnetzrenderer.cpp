#include "tonnetzrenderer.h"

using namespace Hexagon;

void TonnetzRenderer::renderGrid(cv::Mat mat, cv::Point centre, vector<Hex> GridPositions, int cellDiameter, Layout layout, Shape shape)
{
    if (shape == Shape::Circle)
    {
        std::vector<Hex>::iterator it = GridPositions.begin();
        for (int iHex = 0; it != GridPositions.end(); it++, iHex++)
        {
            cv::Point cvCentre;
            Point2d hexCentre = hex_to_pixel(layout, (*it));
            cvCentre.x = hexCentre.x;
            cvCentre.y = hexCentre.y;
            cvCentre = cvCentre + centre;
            cv::circle(mat, cvCentre, cellDiameter, cv::Scalar(200,200,200));
        }
    }
    if (shape == Shape::Hexagon)
    {
        std::vector<Hex>::iterator it = GridPositions.begin();
        for (int iHex = 0; it != GridPositions.end(); it++, iHex++)
        {
            cv::Point cvCentre;
            //Point2d hexCentre = hex_to_pixel(layout, (*it));
            std::vector<Point2d> polygon = polygon_corners(layout, (*it));
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
            cv::polylines(mat, pts, true, cv::Scalar(200,200,200));
        }
    }
}
