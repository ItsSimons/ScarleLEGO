#include "pch.h"
#include "UserInterface.h"

UserInterface::UserInterface()
= default;

UserInterface::~UserInterface()
{
    for (auto element : elements_UI)
    {
        delete element;
    }
}

void UserInterface::initialize(ID3D11Device* _d3dDevice, const Vector2& resolution)
{
    ImageGO2D* background = new ImageGO2D("background_UI", _d3dDevice);
    background->SetPos(resolution / 2);
    elements_UI.push_back(background);
}

void UserInterface::update(GameData* _GD)
{
    for (auto element : elements_UI)
    {
        element->Tick(_GD);
    }
}

void UserInterface::render(DrawData2D* _DD2D)
{
    for (auto element : elements_UI)
    {
        element->Draw(_DD2D);
    }
}
