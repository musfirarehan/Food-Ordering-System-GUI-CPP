#pragma once
// ============================================================
//  FoodOrderingSystem.h
//  Main controller – wires together all data classes.
//  Loads data on construction; persists on every mutation.
// ============================================================
#include "Person.h"
#include "Menu.h"
#include "PromoManager.h"
#include "FileManager.h"

class FoodOrderingSystem {
private:
    Customer     customers[MAX_CUSTOMERS];
    int          customerCount;
    Menu         menu;
    PromoManager promo;
    Customer*    loggedIn;

    Customer* find(const string& u) {
        for (int i = 0; i < customerCount; i++)
            if (customers[i].getUsername() == u) return &customers[i];
        return nullptr;
    }

    // Write full state to disk immediately
    void persist() {
        FileManager::save(customers, customerCount, menu);
    }

public:
    FoodOrderingSystem() : customerCount(0), loggedIn(nullptr) {
        // Restore previously saved customers on startup
        customerCount = FileManager::load(customers, MAX_CUSTOMERS, menu);
    }

    // Returns false if username is taken or the roster is full
    bool registerUser(const string& u, const string& p,
                      const string& n, const string& c, const string& addr) {
        if (customerCount >= MAX_CUSTOMERS || find(u)) return false;
        customers[customerCount++] = Customer(u, p, n, c, addr);
        persist();
        return true;
    }

    bool login(const string& u, const string& p) {
        Customer* c = find(u);
        if (c && c->checkPwd(p)) { loggedIn = c; return true; }
        return false;
    }

    void logout() { loggedIn = nullptr; }

    // Call after Customer::saveOrder() so the new order is persisted
    void onOrderPlaced() { persist(); }

    Customer*     getLoggedIn() { return loggedIn; }
    Menu&         getMenu()     { return menu;     }
    PromoManager& getPromo()    { return promo;    }
};