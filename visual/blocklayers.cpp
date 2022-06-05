#include "blocklayers.h"

BlockLayers::BlockLayers()
{

}

const std::string BlockSetupOptions::ColourSchemes[3] = {
    "Single colour",
    "1/12 hue shift per half tone",
    "1/12 hue shift per fifth"};
const std::string BlockSetupOptions::Shapes[21] = {
    "Rectangle",
    "Rhombus (Diamonds)",
    "Ellipses",
    "Circles",
    "Circles with tracker",
    "Circles with tracker and playing note",
    "Rectangles with tracker",
    "Rectangles with tracker and playing note",
    "Rhombus with tracker",
    "Rhombus with tracker and playing note",
    "Ellipses with tracker",
    "Ellipses with tracker and playing note",
    "Circles sized by velocity",
    "Ellipses with height prop to velocity",
    "Rhombus with height prop to vel",
    "Triangles Nabla with height prop to vel",
    "Triangles Delta with height prop to vel",
    "Triangles > (fp)",
    "Triangles < (pf)",
    "Attack rhombus",
    "Circle miriapod"
};
const std::string BlockSetupOptions::Interconnection[2] = {
    "No connection",
    "Straight line"
};

void saveIni(std::string savePath)
{
    //TODO
}

void loadIni(std::string iniPath)
{
    //TODO
}
