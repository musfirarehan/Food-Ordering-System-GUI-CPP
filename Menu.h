#pragma once
// ============================================================
//  Menu.h
//  Holds a polymorphic array of FoodItem* objects.
//  → Runtime Polymorphism: correct display()/getPrice()
//    called per subtype through base class pointer.
//  → Compile-time Polymorphism: logItem() is overloaded.
// ============================================================
#include "FoodItem.h"

class Menu {
private:
    FoodItem* items[MAX_MENU_ITEMS];
    int       count;

    // ── Compile-time polymorphism: function overloading ──
    void logItem(const FoodItem* f) const                { f->display(); }
    void logItem(const FoodItem* f, const char* t) const { printf("%s ", t); f->display(); }

public:
    Menu() : count(0) {
        // Regular menu items (MenuItem)
        items[count++] = new MenuItem(1,  "Classic Beef Burger", "Burgers",  350);
        items[count++] = new MenuItem(2,  "Zinger Burger",       "Burgers",  420);
        items[count++] = new MenuItem(3,  "Margherita Pizza",    "Pizza",    700);
        items[count++] = new MenuItem(4,  "BBQ Chicken Pizza",   "Pizza",    850);
        items[count++] = new MenuItem(5,  "Soft Drink 330ml",    "Drinks",    80);
        items[count++] = new MenuItem(6,  "Fresh Juice",         "Drinks",   150);
        items[count++] = new MenuItem(7,  "Mineral Water",       "Drinks",    50);
        items[count++] = new MenuItem(8,  "French Fries",        "Sides",    180);
        items[count++] = new MenuItem(9,  "Garlic Bread",        "Sides",    160);
        items[count++] = new MenuItem(10, "Chocolate Brownie",   "Desserts", 220);
        items[count++] = new MenuItem(11, "Ice Cream 2 scoops",  "Desserts", 180);
        // Chef's specials (SpecialItem) – different subtype, different behaviour
        items[count++] = new SpecialItem(12, "Double Smash Burger", "Burgers",  500, 50);
        items[count++] = new SpecialItem(13, "Seafood Pizza",       "Pizza",    900, 75);
        items[count++] = new SpecialItem(14, "Signature Milkshake", "Drinks",   250, 30);
        items[count++] = new SpecialItem(15, "Lava Cake",           "Desserts", 300, 40);
    }

    ~Menu() {
        for (int i = 0; i < count; i++) delete items[i];
    }

    int       getCount() const { return count; }
    FoodItem* get(int i) const { return items[i]; }

    FoodItem* findById(int id) const {
        for (int i = 0; i < count; i++)
            if (items[i]->getId() == id) return items[i];
        return nullptr;
    }

    // Returns unique category names; writes into caller-supplied array
    int getCategories(string cats[], int max) const {
        int n = 0;
        for (int i = 0; i < count; i++) {
            string c = items[i]->getCategory();
            bool exists = false;
            for (int j = 0; j < n; j++) if (cats[j] == c) { exists = true; break; }
            if (!exists && n < max) cats[n++] = c;
        }
        return n;
    }

    // Runtime polymorphism: correct display() called per subtype
    void dumpAll() const {
        for (int i = 0; i < count; i++) {
            if (items[i]->isSpecial()) logItem(items[i], "[SPECIAL]");
            else                       logItem(items[i]);
        }
    }
};
