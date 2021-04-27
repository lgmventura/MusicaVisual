#include "blocklayers.h"

BlockLayers::BlockLayers()
{

}

rgb BlockLayers::getColour(int track, int pitch)
{
    if (this->colorScheme[track] == 0) // Just one color for the track
    {
        rgb rgbcolor;
        rgbcolor.r = this->getCv(track,0);
        rgbcolor.g = this->getCv(track,1);
        rgbcolor.b = this->getCv(track,2);
        return (rgbcolor);
    }
    if (this->colorScheme[track] == 1) // Base color for the track and a certain pitch and, from them, increase 1/12 hue for each half note
    {
        rgb rgbcolor;
        rgbcolor.b = (double)this->getCv(track,2)/255;
        rgbcolor.g = (double)this->getCv(track,1)/255;
        rgbcolor.r = (double)this->getCv(track,0)/255;
        hsv hsvcolor = rgb2hsv(rgbcolor);
        //hsvcolor.h = (double)((int)(hsvcolor.h + 180*pitch/12)%180);// - 180*(int)hsvcolor.h/180;
        hsvcolor.h = (double)((int)(hsvcolor.h + 360*pitch/12)%360);// - 180*(int)hsvcolor.h/180;
        rgbcolor = hsv2rgb(hsvcolor);
        rgbcolor.r = rgbcolor.r*255;
        rgbcolor.g = rgbcolor.g*255;
        rgbcolor.b = rgbcolor.b*255;
        return (rgbcolor);
    }
    if (this->colorScheme[track] == 2) // Base color for the track and a certain pitch and, from them, increase 5/12 hue for each half note
    {
        rgb rgbcolor;
        rgbcolor.b = (double)this->getCv(track,2)/255;
        rgbcolor.g = (double)this->getCv(track,1)/255;
        rgbcolor.r = (double)this->getCv(track,0)/255;
        hsv hsvcolor = rgb2hsv(rgbcolor);
        //hsvcolor.h = (double)((int)(hsvcolor.h + 900*pitch/12)%180); // 180*5*pitch/12
        hsvcolor.h = (double)((int)(hsvcolor.h + 1800*pitch/12)%360); // 360*5*pitch/12
        rgbcolor = hsv2rgb(hsvcolor);
        rgbcolor.r = rgbcolor.r*255;
        rgbcolor.g = rgbcolor.g*255;
        rgbcolor.b = rgbcolor.b*255;
        return (rgbcolor);
    }
}

const std::string BlockSetupOptions::ColourSchemes[3] = {
    "Single colour",
    "1/12 hue shift per half tone",
    "1/12 hue shift per fifth"};
const std::string BlockSetupOptions::Shapes[19] = {
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
    "Triangles < (pf)"
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
