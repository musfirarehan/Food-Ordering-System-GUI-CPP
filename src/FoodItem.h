#pragma once
// ============================================================
//  FoodItem.h
//  Abstract base class (Abstraction + Inheritance base)
//  Also contains: MenuItem, SpecialItem
// ============================================================
#include "Constants.h"
#include <cstdio>

// ============================================================
//  ABSTRACT BASE CLASS: FoodItem
//  → Abstraction : exposes interface via pure virtual functions
//  → Inheritance : base for MenuItem and SpecialItem
// ============================================================
class FoodItem {
protected:
    int    id;
    string name;
    double price;

public:
    FoodItem() : id(0), name(""), price(0.0) {}
    FoodItem(int i, string n, double p) : id(i), name(n), price(p) {}
    virtual ~FoodItem() {}

    // Pure virtual functions → FoodItem is abstract (cannot be instantiated)
    virtual double getPrice()    const = 0;
    virtual string getCategory() const = 0;
    virtual bool   isSpecial()   const = 0;
    virtual void   display()     const = 0;

    // Concrete encapsulated getters
    int    getId()   const { return id;   }
    string getName() const { return name; }
};

// ============================================================
//  CLASS: MenuItem  (Inherits FoodItem)
//  → Runtime Polymorphism: overrides all pure virtuals
// ============================================================
class MenuItem : public FoodItem {
private:
    string category;   // encapsulated

public:
    MenuItem() : FoodItem(), category("") {}
    MenuItem(int id, string n, string cat, double p)
        : FoodItem(id, n, p), category(cat) {}

    double getPrice()    const override { return price;    }
    string getCategory() const override { return category; }
    bool   isSpecial()   const override { return false;    }
    void   display()     const override {
        printf("[%2d] %-24s %-10s Rs.%.0f\n",
               id, name.c_str(), category.c_str(), price);
    }
};

// ============================================================
//  CLASS: SpecialItem  (Inherits FoodItem)
//  → Runtime Polymorphism: same interface, different behaviour
//  → Adds a premium charge on top of the base price
// ============================================================
class SpecialItem : public FoodItem {
private:
    string category;
    double extraCharge;   // encapsulated premium charge

public:
    SpecialItem() : FoodItem(), category(""), extraCharge(0) {}
    SpecialItem(int id, string n, string cat, double p, double extra)
        : FoodItem(id, n, p), category(cat), extraCharge(extra) {}

    double getPrice()    const override { return price + extraCharge; }
    string getCategory() const override { return category; }
    bool   isSpecial()   const override { return true; }
    double getExtra()    const          { return extraCharge; }
    void   display()     const override {
        printf("[%2d] %-24s %-10s Rs.%.0f [SPECIAL +%.0f]\n",
               id, name.c_str(), category.c_str(),
               price + extraCharge, extraCharge);
    }
};
