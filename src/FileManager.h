#pragma once
// ============================================================
//  FileManager.h
//  Encapsulates ALL file I/O in one place.
//
//  FILE FORMAT  (../data/users.txt)
//  ─────────────────────────────────────────────────────────
//  # comment lines and blank lines are ignored
//
//  USER
//  username|password|name|contact|address
//  ORDERS <n>
//    ORDER <orderId> <discountPct> <promoCode>
//    ITEMS <k>
//      <itemId> <qty>
//      ...
//  ENDUSER
//
//  Orders store only item IDs + quantities; on load they are
//  re-linked to live Menu pointers so no data is duplicated.
// ============================================================
#include "Person.h"   // pulls in Customer → Order → FoodItem
#include "Menu.h"
#include <fstream>
#include <sstream>
#include <cstdio>

class FileManager {
public:
    // ── Save ALL customers to file ────────────────────────
    static void save(const Customer customers[], int count, const Menu& menu) {
        ofstream f("users.txt");
        if (!f.is_open()) return;

        f << "# Online Food Ordering System - user data\n";
        f << "# DO NOT EDIT MANUALLY\n\n";

        for (int c = 0; c < count; c++) {
            const Customer& cu = customers[c];
            f << "USER\n";
            f << cu.getUsername() << "|"
              << cu.getPassword() << "|"
              << cu.getName()     << "|"
              << cu.getContact()  << "|"
              << cu.getAddress()  << "\n";

            f << "ORDERS " << cu.getOrderCount() << "\n";

            for (int o = 0; o < cu.getOrderCount(); o++) {
                const Order& ord = cu.getOrder(o);
                // Use "-" as placeholder when there is no promo code
                string promo = ord.getPromo().empty() ? "-" : ord.getPromo();

                f << "  ORDER " << ord.getOrderId()
                  << " "        << ord.getDiscountPct()
                  << " "        << promo << "\n";

                f << "  ITEMS " << ord.getItemCount() << "\n";
                for (int i = 0; i < ord.getItemCount(); i++) {
                    const OrderItem& oi = ord.getItem(i);
                    if (oi.food)
                        f << "    " << oi.food->getId()
                          << " "    << oi.qty << "\n";
                }
            }
            f << "ENDUSER\n\n";
        }
        f.close();
    }

    // ── Load customers from file ──────────────────────────
    // Returns the number of customers successfully loaded.
    // Orders are re-linked to live Menu pointers (no dangling ptrs).
    static int load(Customer customers[], int maxCustomers, const Menu& menu) {
        ifstream f("users.txt");
        if (!f.is_open()) return 0;   // file doesn't exist yet – fine on first run

        int    loaded = 0;
        string line;

        while (getline(f, line)) {
            if (line.empty() || line[0] == '#') continue;

            if (line == "USER") {
                if (loaded >= maxCustomers) break;

                // ── Parse credentials line (pipe-delimited) ──
                string credLine;
                if (!getline(f, credLine)) break;

                string parts[5];
                int    pi = 0;
                stringstream ss(credLine);
                string token;
                while (getline(ss, token, '|') && pi < 5)
                    parts[pi++] = token;
                if (pi < 4) continue;   // malformed line – skip this user
                string addr = (pi >= 5) ? parts[4] : "";  // backward compatible

                customers[loaded] = Customer(parts[0], parts[1], parts[2], parts[3], addr);

                // ── Parse ORDERS block ──
                string ordLine;
                if (!getline(f, ordLine)) { loaded++; continue; }

                int numOrders = 0;
                sscanf(ordLine.c_str(), "ORDERS %d", &numOrders);

                for (int o = 0; o < numOrders; o++) {
                    string oLine;
                    if (!getline(f, oLine)) break;

                    int    oid   = 0;
                    double disc  = 0;
                    char   promo[32] = "-";
                    sscanf(oLine.c_str(), "  ORDER %d %lf %31s", &oid, &disc, promo);

                    Order ord(oid);
                    if (disc > 0 && string(promo) != "-")
                        ord.applyDiscount(disc, promo);

                    string iLine;
                    if (!getline(f, iLine)) break;
                    int numItems = 0;
                    sscanf(iLine.c_str(), "  ITEMS %d", &numItems);

                    for (int i = 0; i < numItems; i++) {
                        string itLine;
                        if (!getline(f, itLine)) break;
                        int itemId = 0, qty = 0;
                        sscanf(itLine.c_str(), "    %d %d", &itemId, &qty);
                        FoodItem* fi = menu.findById(itemId);
                        if (fi && qty > 0) ord.addItem(fi, qty);
                    }
                    customers[loaded].saveOrder(ord);
                }

                // Consume the ENDUSER line
                string endLine;
                while (getline(f, endLine))
                    if (endLine == "ENDUSER") break;

                loaded++;
            }
        }
        f.close();
        return loaded;
    }
};