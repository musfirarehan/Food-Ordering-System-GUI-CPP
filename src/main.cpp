// ============================================================
//  main.cpp
//  Entry point only – all logic lives in the header files.
//
//  Project structure:
//    src\
//      main.cpp              ← you are here
//      Constants.h           ← shared constants & colour palette
//      FoodItem.h            ← abstract FoodItem, MenuItem, SpecialItem
//      Order.h               ← OrderItem struct, Order class
//      Person.h              ← abstract Person, Customer
//      Menu.h                ← polymorphic menu (FoodItem* array)
//      PromoManager.h        ← promo code validation
//      FileManager.h         ← file I/O (save / load users.txt)
//      FoodOrderingSystem.h  ← controller: wires all classes together
//      GUIApp.h              ← Raylib GUI (all screens & rendering)
//    data\
//      users.txt             ← auto-created on first run
//      Lexend-VariableFont_wght.ttf
// ============================================================
#include "GUIApp.h"   // pulls in the entire dependency chain

int main() {
    GUIApp app;
    app.run();
    return 0;
}
