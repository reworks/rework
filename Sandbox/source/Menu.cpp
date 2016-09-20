//
//  Menu.cpp
//  REngine3
//
//  Created by reworks on 1/08/2016.
//  Copyright (c) 2016 reworks. All rights reserved.
//

#include "Menu.hpp"

#include <SFML/Window/Event.hpp>

#include "re/entity/World.hpp"
#include "re/entity/Entity.hpp"
#include "re/graphics/Window.hpp"
#include "re/systems/RenderSystem.hpp"
#include "re/services/ServiceLocator.hpp"
#include "re/systems/state/StateIdentifiers.hpp"

#include "re/graphics/ui/UISystem.hpp"
#include "re/graphics/ui/UILabel.hpp"

using namespace re;

Menu::Menu()
	:State()
{
	Locator::Get<World>()->AddEntity("menu", std::make_shared<Entity>("menu.lua"));
	Locator::Get<World>()->AddEntity("person", std::make_shared<Entity>("person.lua"));

	Locator::Get<World>()->GetSystem<RenderSystem>()->Submit();

	Locator::Get<World>()->GetSystem<UISystem>()->AddPanel("menupanel", std::make_shared<UIPanel>("menupanel.lua"));
	Locator::Get<World>()->GetSystem<UISystem>()->GetPanel("menupanel")->Add("label", std::make_shared<UILabel>("menulabel.lua"));

	Locator::Get<World>()->GetSystem<UISystem>()->GetPanel("menupanel")->SetComponentOffsets();

	Locator::Get<World>()->GetEntity("person")->Get<AnimatedSpriteComponent>()->Play();
}

Menu::~Menu()
{
}

bool Menu::Event(sf::Event & e)
{
	Locator::Get<World>()->GetSystem<UISystem>()->Event();

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape) || e.type == sf::Event::Closed)
	{
		Locator::Get<Window>()->close();
	}

	return true;
}

bool Menu::Update(sf::Time dt)
{
	Locator::Get<World>()->GetSystem<RenderSystem>()->Update(dt);

	Locator::Get<World>()->GetSystem<UISystem>()->Update();

	return true;
}

void Menu::Render()
{
	Locator::Get<Window>()->clear(sf::Color::White);

	Locator::Get<World>()->GetSystem<RenderSystem>()->Render();
	Locator::Get<World>()->GetSystem<UISystem>()->Render();

	Locator::Get<Window>()->display();
}