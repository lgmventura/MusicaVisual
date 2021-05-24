#include "layercontainer.h"

LayerContainer::LayerContainer()
{

}

LayerContainer::~LayerContainer()
{

}

std::string LayerContainer::getName()
{
    std::string name;
    name = this->Name;
    return name;
}

void LayerContainer::setName(std::string newName)
{
    newName.copy(Name, 128);
}
