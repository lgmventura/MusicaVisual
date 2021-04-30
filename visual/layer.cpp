#include "layer.h"

Layer::Layer()
{

}

Layer::~Layer()
{

}

std::string Layer::getName()
{
    std::string name;
    name = this->Name;
    return name;
}

void Layer::setName(std::string newName)
{
    newName.copy(Name, 128);
}
