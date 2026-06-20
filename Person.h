#pragma once
// ============================================================
//  Person.h
//  Abstract base class: Person  (Abstraction)
//  Derived class:       Customer (Inheritance + Polymorphism)
// ============================================================
#include "Order.h"

// ============================================================
//  ABSTRACT CLASS: Person
//  → Abstraction: defines a shared interface for any person
//    in the system via a pure virtual function.
// ============================================================
class Person {
protected:
    string name;
    string contact;

public:
    Person() : name(""), contact("") {}
    Person(string n, string c) : name(n), contact(c) {}
    virtual ~Person() {}

    virtual void showInfo() const = 0;   // pure virtual → Person is abstract

    // Encapsulated getters
    string getName()    const { return name;    }
    string getContact() const { return contact; }
};

// ============================================================
//  CLASS: Customer  (Inherits Person)
//  → Inheritance + Polymorphism: overrides showInfo()
//  → Encapsulation: credentials and history are private
// ============================================================
class Customer : public Person {
private:
    string username;
    string password;
    string address;           // delivery address
    Order  history[MAX_ORDERS];
    int    orderCount;

public:
    Customer() : Person(), username(""), password(""), address(""), orderCount(0) {}
    // 5-arg constructor: username, password, name, contact, address
    Customer(string u, string p, string n, string c, string addr)
        : Person(n, c), username(u), password(p), address(addr), orderCount(0) {}

    // Polymorphic override
    void showInfo() const override {
        printf("Customer: %s | Contact: %s | Address: %s | Orders: %d\n",
               name.c_str(), contact.c_str(), address.c_str(), orderCount);
    }

    // ── Getters (Encapsulation) ──
    string getUsername()              const { return username;       }
    string getPassword()              const { return password;       }
    string getAddress()               const { return address;        }
    bool   checkPwd(const string& p)  const { return password == p;  }
    int    getNextOrderId()           const { return orderCount + 1; }
    int    getOrderCount()            const { return orderCount;     }

    void saveOrder(const Order& o) {
        if (orderCount < MAX_ORDERS) history[orderCount++] = o;
    }
    const Order& getOrder(int i) const { return history[i]; }
};