#pragma once
// ============================================================
//  Order.h
//  Contains: OrderItem (struct), Order (class)
// ============================================================
#include "FoodItem.h"

// ============================================================
//  STRUCT: OrderItem
//  → Pairs a FoodItem pointer with a quantity.
//    Using FoodItem* enables runtime polymorphism:
//    the correct getPrice() is called regardless of subtype.
// ============================================================
struct OrderItem {
    FoodItem* food = nullptr;
    int       qty  = 0;
    double subtotal() const { return food ? food->getPrice() * qty : 0.0; }
};

// ============================================================
//  CLASS: Order
//  → Encapsulation: all order data is private;
//    modified only through well-defined methods.
// ============================================================
class Order {
private:
    int       orderId;
    OrderItem items[MAX_ORDER_ITEMS];
    int       itemCount;
    double    discountPct;
    string    promoUsed;

public:
    Order() : orderId(0), itemCount(0), discountPct(0), promoUsed("") {}
    Order(int id) : orderId(id), itemCount(0), discountPct(0), promoUsed("") {}

    // If the item already exists in the order, increase its quantity instead
    bool addItem(FoodItem* f, int q) {
        for (int i = 0; i < itemCount; i++) {
            if (items[i].food && items[i].food->getId() == f->getId()) {
                items[i].qty += q;
                return true;
            }
        }
        if (itemCount >= MAX_ORDER_ITEMS) return false;
        items[itemCount++] = {f, q};
        return true;
    }

    bool removeItem(int idx) {
        if (idx < 0 || idx >= itemCount) return false;
        for (int i = idx; i < itemCount - 1; i++) items[i] = items[i + 1];//removes the gap in array
        items[--itemCount] = {nullptr, 0};
        return true;
    }

    void applyDiscount(double pct, const string& code) {
        discountPct = pct;
        promoUsed   = code;
    }
    void clearDiscount() { discountPct = 0; promoUsed = ""; }

    // ── Getters (Encapsulation) ──
    double getSubtotal() const {
        double t = 0;
        for (int i = 0; i < itemCount; i++) t += items[i].subtotal();
        return t;
    }
    double getDiscountAmt() const { return getSubtotal() * discountPct / 100.0; }
    double getTotal()       const { return getSubtotal() - getDiscountAmt(); }
    int    getOrderId()     const { return orderId;    }
    int    getItemCount()   const { return itemCount;  }
    bool   isEmpty()        const { return itemCount == 0; }
    double getDiscountPct() const { return discountPct; }
    string getPromo()       const { return promoUsed;  }
    const  OrderItem& getItem(int i) const { return items[i]; }
};
