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

tgui::Button::Ptr linking = nullptr;
std::vector<std::string> ElementListID = std::vector<std::string>(); // identifier
std::vector<nlohmann::json> ElementListVal = std::vector<nlohmann::json>(); // value
std::vector<size_t> BoxListCount = std::vector<size_t>();
std::vector<std::string> BoxListID = std::vector<std::string>(); // save box identifiers
std::vector<tgui::ChildWindow::Ptr> BoxList = std::vector<tgui::ChildWindow::Ptr>(); // box
std::vector<tgui::ChildWindow::Ptr> BoxConnections = std::vector<tgui::ChildWindow::Ptr>();
size_t count = 0;
size_t amount = 0;

void hide() {
    blueprintSearcher->setVisible(false);
    searchBar->setText("");
}

void newBlueprint(std::string name, byte checkCount, byte editCount) {
    auto blueprint = tgui::ChildWindow::create();
    //auto value = tgui::EditBox::create();
    //auto check = tgui::CheckBox::create();
    tgui::Button::Ptr linkToButton = tgui::Button::create();
    tgui::Button::Ptr linkFromButton = tgui::Button::create();

    blueprint->setClientSize({ 250, 120 });
    blueprint->setPosition(sf::Mouse::getPosition(window).x, sf::Mouse::getPosition(window).y);
    blueprint->setTitle(/*"Empty Blueprint"*/name);
    blueprint->setWidgetName("BP");
    gui.add(blueprint);

    for (byte i = 0; i < editCount; i++) {
        tgui::EditBox::Ptr value = tgui::EditBox::create();
        value->setPosition(75, 70);
        value->setText("\"value\"");
        value->setSize(100, 30);
        blueprint->add(value);
    }
    for (byte i = 0; i < checkCount; i++) {
        tgui::CheckBox::Ptr check = tgui::CheckBox::create();
        check->setPosition(75, 35);
        check->setText("check");
        check->setSize(25, 25);
        blueprint->add(check);
    }
    linkToButton->setWidgetName("lTB");
    linkToButton->setPosition(230, 45);
    linkToButton->setText(">");
    linkToButton->setSize(30, 30);
    linkToButton->onPress([=] {
        linking = linkToButton;
    });
    blueprint->add(linkToButton, "lTB");

    linkFromButton->setWidgetName("lFB");
    linkFromButton->setPosition(-10, 45);
    linkFromButton->setText("<");
    linkFromButton->setSize(30, 30);
    size_t ggCount = amount;
    linkFromButton->onPress([=] { 
        if (linking != nullptr) { // if set
            size_t gg = -1;
            tgui::ChildWindow::Ptr bP = linking->getParent()->cast<tgui::ChildWindow>(); // get selected elements parent element (window)
            for (size_t i = 0; i < BoxList.size(); i++)if (BoxList.at(i) == bP) { // find the selected element in the list
                gg = i; break;
            }
            if(gg!=-1){ // if found
                linking->setText(std::to_string(ggCount)); // set text on the previously selected element to this elements ID
                linkFromButton->setText(std::to_string(BoxListCount.at(gg))); // find the ID of the previous element and set the text of this element
                BoxConnections.insert(BoxConnections.begin()+gg,blueprint); // put the connection in the list
            }
            //linkFromButton->setText(std::to_string(count));
            linking = nullptr;
        }
    });
    blueprint->add(linkFromButton, "lFB");
    blueprint->onClose([=] { // when closed
        size_t gg = -1;
    for (size_t i = 0; i < BoxList.size(); i++)if (BoxList.at(i) == blueprint) { // find the index in list for THIS element
        gg = i; break;
    }
    if (gg != -1) { // if element was found in list, erase the data, and update connected buttons
        size_t connWin = -1;
        for(size_t d=0;d<BoxConnections.size();d++){
            if(BoxConnections.at(d)==blueprint){connWin=d;break;} // find the parent window index
        }
        tgui::ChildWindow::Ptr CWP = BoxConnections.at(gg);
        if (CWP != nullptr) {
            std::vector<tgui::Widget::Ptr> L = CWP->getWidgets(); // List of widgets in connected window
            for (size_t d = 0; d < L.size(); d++) {
                if (tgui::Button::Ptr btn = L.at(d)->cast<tgui::Button>()){tgui::String widname=btn->getWidgetName();if(widname == "lFB"){ btn->setText("X");break; } } // update left button in the connected window
            }
        }
        if (connWin != -1) {
            tgui::ChildWindow::Ptr CW = BoxConnections.at(connWin);
            std::vector<tgui::Widget::Ptr> L = CW->getWidgets(); // List of widgets in parent window
            std::cout<<std::to_string(L.size())<<" widgets long\n";
            for (size_t d = 0; d < L.size(); d++) {
                if (tgui::Button::Ptr btn=L.at(d)->cast<tgui::Button>()){tgui::String widname=btn->getWidgetName();if(widname == "lTB") { btn->setText("Y"); }} // update right button in the parent window
            }
        }
        BoxListID.erase(BoxListID.begin() + gg);
        BoxList.erase(BoxList.begin() + gg);
        BoxConnections.erase(BoxConnections.begin() + gg);
        BoxListCount.erase(BoxListCount.begin()+gg);
    }
    });
    //::memcpy(BoxListCount.data(), &ggCount, sizeof(ggCount));
    BoxListCount.push_back(ggCount);
    BoxListID.push_back(name);
    BoxList.push_back(blueprint);
    BoxConnections.push_back(nullptr); // add the element to the lists
    hide();
    amount++;
    count += 1;
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

/* detta testade vi bara lite, fungerade halvt 
void getDirection(sf::Event& event)
{
    if (event.type == sf::Event::MouseWheelScrolled) {
        if (event.mouseWheelScroll.delta > 0)
        {
            for (const tgui::Widget::Ptr& widget : gui.getWidgets())
                if (widget->getWidgetType() == "ChildWindow" && widget->getWidgetName() != "BP") {
                    std::cout << widget->getWidgetType();
                    if (widget->getSize().x > 100 && widget->getSize().y > 100) {
                        widget->setSize(widget->getSize().x - 5, widget->getSize().y - 5);

                    }

                }
            event.mouseWheelScroll.delta = 0;
        }
        else if (event.mouseWheelScroll.delta < 0)
        {
            for (const tgui::Widget::Ptr& widget : gui.getWidgets()) {
                if (widget->getWidgetType() == "ChildWindow") {
                    std::cout << widget->getWidgetType();
                    widget->setSize(widget->getSize().x + 5, widget->getSize().y + 5);
                }
            }
            event.mouseWheelScroll.delta = 0;
        }
    }
}*/


int main()
{
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
        byte checkBoxCount = data["amount_of_checkboxes"];
        byte editBoxCount = data["amount_of_editboxes"];
        // won't need more than 255 options per blueprint of these, optimize usage by using unsigned byte type (0-255)
        ElementListID.push_back(name);
        ElementListVal.push_back(data);
        // create button and load in blueprint
        auto button = tgui::Button::create();
        button->setSize(250, 30);
        button->getRenderer()->setBackgroundColor(tgui::Color(color));
        button->setText(name);
        button->onPress(&newBlueprint, button->getText().toStdString(), checkBoxCount, editBoxCount);
        blueprintSearcherScroll->add(button);
    }
    auto compile = tgui::Button::create();
    compile->setSize(250, 30);
    compile->setText("compile");
    compile->onClick([=] { 
        compile->setText("compiling");
        std::vector<tgui::Widget::Ptr> list = blueprintSearcherScroll->getWidgets();
        std::string compileStr = "";
        std::string headerList = "";
        for (size_t a = 0; a < BoxListID.size();a++) {
        //bool GG = false; // stop loop
        for (size_t i = 0; i < ElementListID.size(); i++) {
            if (ElementListVal.at(i)["name"] == BoxListID.at(a)) { // get widget information from ElementList, find by widget name
                if (ElementListVal.at(i)["priority"] == 1) {
                    if(compileStr == "")compileStr = ElementListVal.at(i)["code"];
                    size_t D = a;
                    while(D != -1 && BoxConnections.at(D) != nullptr) {
                        std::cout << "\nChecking>> " << std::to_string(D);
                        size_t nD=-1;
                        for(size_t b=0;b<BoxList.size();b++)if(BoxList.at(b)==BoxConnections.at(D)){nD=b;break;}
                        if(D == nD)break;
                        D = nD;
                        size_t t = -1;
                        for (size_t c = 0; c < ElementListID.size(); c++) {
                            if (ElementListVal.at(c)["name"] == BoxListID.at(D)) {
                                t = c;
                                break;
                            }
                        }
                        std::cout << "t> " << std::to_string(t);
                        if (t != -1) {
                            std::string topcode = "";
                            if ((topcode = ElementListVal.at(t)["top_code"]) != ""){size_t c = headerList.find(topcode);if(c==-1)headerList = headerList + "\n" + topcode;} // if #include is not included in the headerList it gets added
                            std::string code = ElementListVal.at(t)["code"]; // fetch template code
                            std::vector<tgui::Widget::Ptr> list = BoxList.at(D)->getWidgets();
                            //std::vector<tgui::EditBox::Ptr> edits = std::vector<tgui::EditBox::Ptr>();
                            //std::vector<tgui::CheckBox::Ptr> checks = std::vector<tgui::CheckBox::Ptr>();
                            size_t c = code.find("{v}"); // find {v} from the template code
                            if(c!=-1)
                            for (byte E = 0; E < list.size(); E++) { // loop through widgets, find correct elements and replace with the data, and find the next {v}
                                if(c!=-1)if (tgui::EditBox::Ptr gg = list.at(E)->cast<tgui::EditBox>()) { code.replace(c, 3, gg->getText().toStdString()); c = code.find("{v}"); }
                                else if (tgui::CheckBox::Ptr gg = list.at(E)->cast<tgui::CheckBox>()) { code.replace(c, 3, gg->isChecked() ? "true" : "false"); c = code.find("{v}"); }
                            }
                            compileStr = compileStr + code; // add the new code snippet to the compile string
                        } 

                        std::cout << "New D>" << std::to_string(D) << "\n";
                    }
                    break;
                }
            }
        }
        //if (GG)break;
    }
        compileStr = headerList + "\n" + compileStr + '}'; //combine headers and code
    printf(compileStr.c_str());
    compile->setText("compiled");
    });
    gui.add(compile);
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
        // getDirection(event);

        window.clear(sf::Color(40, 40, 40));
        gui.draw();
        window.display();
    }
} 