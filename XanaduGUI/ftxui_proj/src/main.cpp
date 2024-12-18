// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#include <functional>  // for function
#include <iostream>  // for basic_ostream::operator<<, operator<<, endl, basic_ostream, basic_ostream<>::__ostream_type, cout, ostream
#include <string>    // for string, basic_string, allocator
#include <chrono>
#include <vector>    // for vector
 
#include "ftxui/component/captured_mouse.hpp"      // for ftxui
#include "ftxui/component/component.hpp"           // for Menu
#include "ftxui/component/component_options.hpp"   // for MenuOption
#include "ftxui/component/screen_interactive.hpp"  // for ScreenInteractive
 
// Fuck it we ball
using namespace ftxui;
const char help[] = "XanaduGUI v1.0\nAn XVD experimentation playground tool\n\n";

///////////////////////////////////////////////////////////////
// FUNCTIONALITY MENU METHODS PREDEFINITIONS
///////////////////////////////////////////////////////////////
int HeaderDump();
int HeaderEditor();
int VerifySignature();
int RebuildSignature();
int ExtractEXVD();
int ExtractUserData();
int VerifyHashTree();
int RebuildHashTree();
int CryptoOptions();
int RemoveHashTree();
int RemoveMutableData();
int Settings();
int Help();

///////////////////////////////////////////////////////////////
// AUX METHODS
///////////////////////////////////////////////////////////////
// Display a component nicely with a title on the left.
Component Wrap(std::string name, Component component) {
  return Renderer(component, [name, component] {
    return hbox({
               text(name) | size(WIDTH, EQUAL, 8),
               separator(),
               component->Render() | xflex,
           }) |
           xflex;
  });
}
void ClearScreen(ScreenInteractive& screen)
{
    system("clear");

    auto reset_position = screen.ResetPosition();
    std::cout << reset_position;
}


///////////////////////////////////////////////////////////////
// MENU METHODS
///////////////////////////////////////////////////////////////
void VerifyHashTree(ScreenInteractive& screen)
{
      std::string reset_position;
        for (float percentage = 0.0f; percentage <= 1.0f; percentage += 0.002f) {
            std::string data_downloaded =
                std::to_string(int(percentage * 5000)) + "/5000";
            auto document = hbox({
                text("Verifying hash tree:"),
                gauge(percentage) | flex,
                text(" " + data_downloaded),
            });
    auto screen = Screen(100, 1);
    Render(screen, document);
    std::cout << reset_position;
    screen.Print();
    reset_position = screen.ResetPosition();
 
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
  std::cout << std::endl;
}

int InitMenu(ScreenInteractive& screen)
{
    std::vector<std::string> init_menu_entries = {
        "Open XVD File",
        "Settings",
        "Exit"
    };
    int selected = 0;

    MenuOption init_menu_settings;
    init_menu_settings.on_enter = screen.ExitLoopClosure();
    Component init_menu = Menu(&init_menu_entries, &selected, init_menu_settings);

    // Create an advanced menu where we replace the rendered
    Component init_menu_final = Renderer(init_menu, [&] {
                                return vbox({
                                        text(L"XanaduXVD GUI") | bold | center,
                                        separator(),
                                        gauge(0.75) | color(Color::Blue),
                                        separator(),
                                        vbox(init_menu->Render()) | center
                                    });
                            });


    // Post a custom event to force the screen to re-render after switching menus
    //screen.PostEvent(Event::Custom);

    screen.Loop(init_menu_final);
    
    // After the menu is done clear the screen
    ClearScreen(screen);

    return selected;
}

std::string OpenXVDMenu(ScreenInteractive& screen)
{
    // -- Input (Widget?) ------------------------------------------------------------------
    std::string filename;
    Component input = Input(&filename, " filename");
    //input = Wrap("Filename", input);

    // -- Button (Widget?) -----------------------------------------------------------------
    std::function<void()> on_button_clicked_ = [&screen](){screen.ExitLoopClosure(); return;};
    Component button = Button("Open File", /*on_button_clicked_*/screen.ExitLoopClosure(), ButtonOption::Simple());
    //button = Wrap("", button);

    // -- Layout (Layout) -----------------------------------------------------------------
    Component layout = Container::Vertical({
      input,
      button,
    });

    Component component = Renderer(layout, [&] {
        return vbox({
               text(L"XVD File Selection Dialogue") | bold | center,
               separator(),
               input->Render(),
               separator(),
               button->Render() | center,
           }) |
           xflex | size(WIDTH, GREATER_THAN, 40) | border;
    });

    screen.Loop(component);

    // After the menu is done clear the screen
    ClearScreen(screen);

    return filename;
}

int MainMenu(ScreenInteractive& screen)
{
    std::vector<std::string> entries = {
        "Header Dump",
        "Header Editor",
        "Verify Signature",
        "Rebuild Signature",
        "Extract eXVD",
        "Extract UserData",
        "Verify HashTree",
        "Rebuild HashTree",
        "Crypto Options",
        "Remove HashTree",
        "Remove Mutable Data",
        "Settings",
        "Help",
        "Exit"
    };
    int selected = 0;

    // Create a basic menu
    MenuOption init_menu_options;
    init_menu_options.on_enter = screen.ExitLoopClosure();
    Component init_menu = Menu(&entries, &selected, init_menu_options);

    // Create an advanced menu where we replace the rendered
    Component init_menu_final = Renderer(init_menu, [&] {
                                return vbox({
                                        text(L"Manipulation Options") | bold | center,
                                        separator(),
                                        // TODO: Top bar showing basic info (signature, xvd type, validity, sandbox, etc)
                                        // separator
                                        vbox(init_menu->Render()) | center
                                    });
                            });

    // Starts an interactive loop using the Renderer to display the menu. The menu is wrapped in a
    // vertical box (vbox) and the center modifier ensures everything is horizontally centered.
    screen.Loop(init_menu_final);

    // After the menu is done clear the screen
    ClearScreen(screen);

    return selected;
}

int ConfigureSettings(ScreenInteractive& screen)
{
    // Variables to track the state of checkboxes.
    bool checkbox_1_checked = false;  // First checkbox state
    bool checkbox_2_checked = false;  // Second checkbox state

    // Message to display the result after pressing the button.
    std::wstring result_message = L"";

    // Create the checkbox components.
    auto checkbox_1 = Checkbox(L"Checkbox 1", &checkbox_1_checked);  // First checkbox
    auto checkbox_2 = Checkbox(L"Checkbox 2", &checkbox_2_checked);  // Second checkbox

    // Action for the button when pressed.
    auto button = Button(L"Submit", [&] {
        // Read the checkbox values and perform some action.
        result_message = L"Checkbox 1 is " + std::wstring(checkbox_1_checked ? L"checked" : L"unchecked") + L", "
                        + L"Checkbox 2 is " + std::wstring(checkbox_2_checked ? L"checked" : L"unchecked");
    });

    // Layout combining checkboxes and the button.
    auto layout = Container::Vertical({
        checkbox_1,
        checkbox_2,
        button,
    });

    // Renderer for displaying the layout.
    auto renderer = Renderer(layout, [&] {
        return vbox({
                text(L"Checkbox Screen") | bold | center,
                separator(),
                checkbox_1->Render(),  // Render the first checkbox
                checkbox_2->Render(),  // Render the second checkbox
                button->Render(),      // Render the button
                separator(),
                text(result_message) | center,  // Display result message
            }) | center;
    });

    // Start the terminal and event loop.
    screen.Loop(renderer);

    // After the menu is done clear the screen
    ClearScreen(screen);

    return 0;
}

///////////////////////////////////////////////////////////////
// ENTRYPOINT
///////////////////////////////////////////////////////////////
int main()
{
    // Init the basic FTXUI ScreenInteractive object
    auto screen = ScreenInteractive::TerminalOutput();
    //std::cout << help;

    // Clean the screen
    ClearScreen(screen);

    // XanaduXVD follows (for now) a very basic menu flow.

    // Show the main menu
init_menu:
    int option = InitMenu(screen);
    
    // Check for settings
    if(option == 1)
    {
        ConfigureSettings(screen);
        goto init_menu;
    }

    // Check for exit
    if(option == 2) 
        return 0;

    // OpenXVD
    VerifyHashTree(screen);
    auto res = OpenXVDMenu(screen);
    if(res.empty())
        goto init_menu;

    // XVD Loaded, proceed to main menu
    option = MainMenu(screen);

    switch(option)
    {
        case 0:
            std::cout << "Dump Header\n"; break;
        case 1:
            std::cout << "Dump eXVD\n"; break;
        case 2:
            std::cout << "Dump UserData\n"; break;
        case 3:
            std::cout << "Exit\n"; break;
        default: break;
    }

    // Cleanup before leaving
    ClearScreen(screen);

    return 0;
}


///////////////////////////////////////////////////////////////
// FUNCTIONALITY MENU METHODS
///////////////////////////////////////////////////////////////

int HeaderDump()
{
    return 0;
}

int HeaderEditor()
{
    return 0;
}

int VerifySignature()
{
    return 0;
}

int RebuildSignature()
{
    return 0;
}

int ExtractEXVD()
{
    return 0;
}

int ExtractUserData()
{
    return 0;
}

int VerifyHashTree()
{
    return 0;
}

int RebuildHashTree()
{
    return 0;
}

int CryptoOptions()
{
    return 0;
}

int RemoveHashTree()
{
    return 0;
}

int RemoveMutableData()
{
    return 0;
}

int Settings()
{
    return 0;
}

int Help()
{
    return 0;
}