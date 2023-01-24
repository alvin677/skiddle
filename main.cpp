#include <iostream>
#include <string>
#include <TGUI/TGUI.hpp>
#include <vector>
#include <string>
#include <Windows.h>
#include <nlohmann/json.hpp>
#include <fstream>

sf::RenderWindow window{ {1920, 1080}, "Skiddle" };
tgui::GuiSFML gui{ window };
auto blueprintSearcher = tgui::ChildWindow::create();
auto searchBar = tgui::EditBox::create();

void hide() {
    blueprintSearcher->setVisible(false);
    searchBar->setText("");
}

void newBlueprint(std::string name) {
    auto blueprint = tgui::ChildWindow::create();
    auto value = tgui::EditBox::create();
    auto check = tgui::CheckBox::create();
    auto linkToButton = tgui::Button::create();
    auto linkFromButton = tgui::Button::create();

    blueprint->setClientSize({ 250, 120 });
    blueprint->setPosition(sf::Mouse::getPosition().x, sf::Mouse::getPosition().y);
    blueprint->setTitle(/*"Empty Blueprint"*/name);
    blueprint->setWidgetName("BP");
    gui.add(blueprint);

    value->setPosition(75, 70);
    value->setText("value");
    value->setSize(100, 30);
    blueprint->add(value);

    check->setPosition(75, 35);
    check->setText("check");
    check->setSize(25, 25);
    blueprint->add(check);

    linkToButton->setPosition(230, 45);
    linkToButton->setText(">");
    linkToButton->setSize(30, 30);
    blueprint->add(linkToButton);

    linkFromButton->setPosition(-10, 45);
    linkFromButton->setText("<");
    linkFromButton->setSize(30, 30);
    blueprint->add(linkFromButton);

    hide();
}

bool RunGUI(tgui::GuiBase& gui)
{
    try
    {
        return true;
    }
    catch (const tgui::Exception& e)
    {
        std::cerr << "Failed to load TGUI widgets: " << e.what() << std::endl;
        return false;
    }
}

/*int getDirection(sf::Event& event)
{
    if (event.type == sf::Event::MouseWheelScrolled) {
        if (event.mouseWheelScroll.delta > 0)
        {
            for (const tgui::Widget::Ptr& widget : gui.getWidgets())
                if (widget->getWidgetType() == "ChildWindow") {
                    std::cout << widget->getWidgetType();
                    if (widget->getSize().x > 100 && widget->getSize().y > 100) {
                        widget->setSize(widget->getSize().x - 1, widget->getSize().y - 1);
                    }

                }
            return 1;
        }
        else if (event.mouseWheelScroll.delta < 0)
        {
            for (const tgui::Widget::Ptr& widget : gui.getWidgets())
                if (widget->getWidgetType() == "ChildWindow") {
                    std::cout << widget->getWidgetType();
                    widget->setSize(widget->getSize().x + 1, widget->getSize().y + 1);
                }
            return -1;
        }
    }
}*/


int main()
{
    int count;
    RunGUI(gui);
    blueprintSearcher->setClientSize({ 250, 500 });
    blueprintSearcher->setPosition(420, 80);
    blueprintSearcher->setTitle("Add new blueprint");
    blueprintSearcher->setVisible(false);
    blueprintSearcher->setTitleButtons(tgui::ChildWindow::TitleButton::Minimize);
    blueprintSearcher->onMinimize(hide);
    gui.add(blueprintSearcher);

    searchBar->setPosition(0, 0);
    searchBar->setSize(250, 30);
    blueprintSearcher->add(searchBar);

    auto blueprintSearcherScroll = tgui::ScrollablePanel::create();
    blueprintSearcherScroll->setSize(250, 500);
    blueprintSearcherScroll->setPosition(0, 30);
    blueprintSearcher->add(blueprintSearcherScroll);

    // load in all json blueprint files into blueprintSearcherScroll
    std::string path = "./Blueprints/";
    for (const auto& entry : std::filesystem::directory_iterator(path)) {
        //std::cout << "Reading file: " << entry.path() << std::endl;
        std::ifstream f(entry.path());
        nlohmann::json data = nlohmann::json::parse(f);

        std::string name = data["name"];
        std::string color = data["color"];
        int checkBoxCount = data["amount_of_checkboxes"];
        int editBoxCount = data["amount_of_editboxes"];

        // create button and load in blueprint
        auto button = tgui::Button::create();
        button->setSize(250, 30);
        button->getRenderer()->setBackgroundColor(tgui::Color(color));
        button->setText(name);
        button->onPress(&newBlueprint, button->getText().toStdString());
        blueprintSearcherScroll->add(button);

        auto compile = tgui::Button::create();
        compile->setSize(250, 30);
        compile->setText("compile");
        gui.add(compile);
    }

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            gui.handleEvent(event);

            if (event.type == sf::Event::Closed)
                window.close();
        }
        if (sf::Mouse::isButtonPressed(sf::Mouse::Right) /* && blueprintSearcher->isVisible() == false*/) {
            blueprintSearcher->setVisible(true);
            blueprintSearcher->setPosition(sf::Mouse::getPosition(window).x, sf::Mouse::getPosition(window).y);
            blueprintSearcher->setFocused(true);
            Sleep(100);
        }

        if (blueprintSearcher->isVisible() == true) {
            if (searchBar->getText() != "") { //typing started
                count = -1;
                for (const tgui::Widget::Ptr& widget : blueprintSearcherScroll->getWidgets()) {
                    //std::cout << blueprintSearcherScroll->getWidgets().size();
                    auto b = widget->cast<tgui::Button>();
                    if (b->getText().toLower().find(searchBar->getText().toLower()) != std::string::npos) {
                        b->setVisible(true);
                        count += 1;
                    }
                    else {
                        b->setVisible(false);
                    }

                    if (b->isVisible() == true) { b->setPosition(0, count * 30); }
                }
                Sleep(50);
            }
            else {
                count = 0;
                for (const tgui::Widget::Ptr& widget : blueprintSearcherScroll->getWidgets()) {
                    auto b = widget->cast<tgui::Button>();
                    b->setVisible(true);
                    b->setPosition(0, count * 30);
                    count += 1;
                }
                Sleep(10);
            }
            Sleep(5);
        }
        //getDirection(event);

        window.clear(sf::Color(40, 40, 40));
        gui.draw();
        window.display();
    }
}
