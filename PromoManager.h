#pragma once
// ============================================================
//  PromoManager.h
//  → Encapsulation: promo data is fully private;
//    the only way to interact with it is through
//    validate() and the read-only getters.
// ============================================================
#include "Constants.h"

class PromoManager {
private:
    string codes[MAX_PROMOS];
    double pcts[MAX_PROMOS];
    int    count;

public:
    PromoManager() : count(5) {
        codes[0] = "SAVE10";   pcts[0] = 10;
        codes[1] = "SAVE20";   pcts[1] = 20;
        codes[2] = "WELCOME";  pcts[2] = 15;
        codes[3] = "BIRYANI";  pcts[3] =  5;
        codes[4] = "FLAT30";   pcts[4] = 30;
    }

    // Returns discount percentage, or 0 if the code is invalid
    double validate(const string& code) const {
        for (int i = 0; i < count; i++)
            if (codes[i] == code) return pcts[i];
        return 0;
    }

    int    getCount()     const { return count;    }
    string getCode(int i) const { return codes[i]; }
    double getPct(int i)  const { return pcts[i];  }
};
