#pragma once
// ============================================================
//  GUIApp.h  –  Raylib GUI
//  Features:
//   • Address collected at registration, shown on profile
//   • Order confirmation / receipt screen after placing order
//   • Scrollable cart panel
//   • Rose + plum + charcoal colour theme, white text
// ============================================================
#include "FoodOrderingSystem.h"
#include <cstring>
#include <cstdio>

enum Screen {
    SCR_MAIN, SCR_LOGIN, SCR_REGISTER,
    SCR_DASHBOARD, SCR_MENU, SCR_CART,
    SCR_CONFIRM,   // ← new: order receipt/confirmation
    SCR_HISTORY
};

class GUIApp {
private:
    FoodOrderingSystem sys;
    Screen screen  = SCR_MAIN;
    Order  cart;
    Order  lastOrder;          // stored so confirm screen can display it
    bool   running = true;

    Font  fnt;
    float fSz = 18.f;

    // ── Input buffers ─────────────────────────────────────
    char ibUser[32]    = ""; int ibUserLen    = 0; bool ibUserAct    = false;
    char ibPass[32]    = ""; int ibPassLen    = 0; bool ibPassAct    = false;
    char ibName[48]    = ""; int ibNameLen    = 0; bool ibNameAct    = false;
    char ibContact[32] = ""; int ibContactLen = 0; bool ibContactAct = false;
    char ibAddr[64]    = ""; int ibAddrLen    = 0; bool ibAddrAct    = false;
    char ibPromo[24]   = ""; int ibPromoLen   = 0; bool ibPromoAct   = false;

    string statusMsg = ""; Color statusCol = C_SUCCESS; float statusT = 0;
    string activeCat = "All";
    float  menuScroll = 0, histScroll = 0, cartScroll = 0;

    // ════════════════════════════════════════════════════════
    //  Low-level draw helpers
    // ════════════════════════════════════════════════════════

    void txt(const char* s, float x, float y, float sz, Color c, bool ctr = false) const {
        Vector2 ms = MeasureTextEx(fnt, s, sz, 1);
        if (ctr) x -= ms.x / 2;
        DrawTextEx(fnt, s, {x, y}, sz, 1, c);
    }
    void txt(const string& s, float x, float y, float sz, Color c, bool ctr = false) const {
        txt(s.c_str(), x, y, sz, c, ctr);
    }

    void fillR(Rectangle r, float rnd, Color c) const {
        DrawRectangleRounded(r, rnd, 8, c);
    }

    // Subtle glow rectangle (used behind accent buttons)
    void glow(Rectangle r, Color col, unsigned char alpha = 40) const {
        Color g = col; g.a = alpha;
        DrawRectangleRounded({r.x-4, r.y-4, r.width+8, r.height+8}, 0.4f, 8, g);
    }

    // Standard button
    bool btn(Rectangle r, const char* label,
             Color bg = C_CARD2, Color hov = {70, 70, 70, 255}, float sz = 18.f) {
        Vector2 m = GetMousePosition();
        bool ov = CheckCollisionPointRec(m, r);
        if (ov) glow(r, C_ACCENT, 30);
        fillR(r, 0.3f, ov ? hov : bg);
        DrawRectangleRoundedLines(r, 0.3f, 8, ov ? C_ACCENT : C_CARD2);
        Vector2 ts = MeasureTextEx(fnt, label, sz, 1);
        DrawTextEx(fnt, label, {r.x+(r.width-ts.x)/2, r.y+(r.height-ts.y)/2},
                   sz, 1, ov ? C_ACCENT : C_TEXT);
        return ov && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
    }

    // Accent (filled) button
    bool abtn(Rectangle r, const char* label, float sz = 18.f) {
        Vector2 m = GetMousePosition();
        bool ov = CheckCollisionPointRec(m, r);
        if (ov) glow(r, C_ACCENT, 50);
        fillR(r, 0.3f, ov ? C_ACCENT2 : C_ACCENT);
        Vector2 ts = MeasureTextEx(fnt, label, sz, 1);
        // Dark text on bright accent
        DrawTextEx(fnt, label, {r.x+(r.width-ts.x)/2, r.y+(r.height-ts.y)/2},
                   sz, 1, C_BG);
        return ov && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
    }

    // Danger (red) filled button
    bool dbtn(Rectangle r, const char* label, float sz = 18.f) {
        Vector2 m = GetMousePosition();
        bool ov = CheckCollisionPointRec(m, r);
        Color fill = ov ? Color{220, 90, 75, 255} : C_DANGER;
        if (ov) glow(r, C_DANGER, 40);
        fillR(r, 0.3f, fill);
        Vector2 ts = MeasureTextEx(fnt, label, sz, 1);
        DrawTextEx(fnt, label, {r.x+(r.width-ts.x)/2, r.y+(r.height-ts.y)/2},
                   sz, 1, C_BG);
        return ov && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
    }

    void inputBox(Rectangle r, const char* label,
                  char* buf, int& len, bool& act,
                  bool pw = false, int maxLen = 31) {
        Vector2 m = GetMousePosition();
        bool ov = CheckCollisionPointRec(m, r);
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) act = ov;
        if (*label) txt(label, r.x, r.y - 22, fSz - 1, C_MUTED);
        // Box
        Color border = act ? C_ACCENT : (ov ? C_MUTED : C_CARD2);
        Color fill   = act ? C_CARD2  : C_CARD;
        fillR(r, 0.15f, fill);
        DrawRectangleRoundedLines(r, 0.15f, 8, border);
        // Accent left bar when active
        if (act) DrawRectangle((int)r.x, (int)r.y+4, 3, (int)r.height-8, C_ACCENT);
        // Text
        string disp = pw ? string(len, '*') : string(buf, len);
        DrawTextEx(fnt, disp.c_str(), {r.x+14, r.y+(r.height-fSz)/2}, fSz, 1, C_TEXT);
        // Blinking cursor
        if (act && (int)(GetTime()*2) % 2 == 0) {
            Vector2 ts = MeasureTextEx(fnt, disp.c_str(), fSz, 1);
            DrawRectangle((int)(r.x+14+ts.x+2), (int)(r.y+8), 2, (int)(r.height-16), C_ACCENT);
        }
        if (act) {
            int k = GetCharPressed();
            while (k > 0) {
                if (k >= 32 && len < maxLen) { buf[len++] = (char)k; buf[len] = '\0'; }
                k = GetCharPressed();
            }
            if (IsKeyPressed(KEY_BACKSPACE) && len > 0) buf[--len] = '\0';
        }
    }

    void setStatus(const string& msg, Color col = C_SUCCESS) {
        statusMsg = msg; statusCol = col; statusT = 3.0f;
    }
    void drawStatus() {
        if (statusT <= 0) return;
        statusT -= GetFrameTime();
        float a  = statusT > 1.f ? 1.f : statusT;
        Color c  = statusCol; c.a  = (unsigned char)(a * 220);
        Color bg = C_CARD;   bg.a = (unsigned char)(a * 240);
        Rectangle r = {(float)WIN_W/2-220.f, (float)WIN_H-58.f, 440.f, 38.f};
        fillR(r, 0.4f, bg);
        DrawRectangleRoundedLines(r, 0.4f, 8, c);
        Vector2 ts = MeasureTextEx(fnt, statusMsg.c_str(), fSz, 1);
        DrawTextEx(fnt, statusMsg.c_str(),
                   {r.x+(r.width-ts.x)/2, r.y+(r.height-fSz)/2}, fSz, 1, c);
    }

    void div(float y, float x1 = 40, float x2 = WIN_W-40) {
        DrawLineEx({x1, y}, {x2, y}, 1, {70, 65, 70, 220});
    }

    void badge(float x, float y, const char* t, Color bg, Color fg, float sz = 13.f) {
        Vector2 ts = MeasureTextEx(fnt, t, sz, 1);
        Rectangle r = {x, y, ts.x+14, sz+8};
        fillR(r, 0.5f, bg);
        DrawTextEx(fnt, t, {r.x+7, r.y+4}, sz, 1, fg);
    }

    // Section header label with left accent bar
    void sectionLabel(float x, float y, const char* t) {
        DrawRectangle((int)x, (int)y, 3, 20, C_ACCENT);
        txt(t, x+10, y, 17, C_ACCENT);
    }

    void clearLogin() {
        memset(ibUser, 0, sizeof ibUser); ibUserLen = 0; ibUserAct = false;
        memset(ibPass, 0, sizeof ibPass); ibPassLen = 0; ibPassAct = false;
    }
    void clearAll() {
        clearLogin();
        memset(ibName,    0, sizeof ibName);    ibNameLen    = 0; ibNameAct    = false;
        memset(ibContact, 0, sizeof ibContact); ibContactLen = 0; ibContactAct = false;
        memset(ibAddr,    0, sizeof ibAddr);    ibAddrLen    = 0; ibAddrAct    = false;
        memset(ibPromo,   0, sizeof ibPromo);   ibPromoLen   = 0; ibPromoAct   = false;
    }

    // Top navigation bar
    void nav(const char* title) {
        // Gradient-ish nav bar using two rects
        DrawRectangle(0, 0, WIN_W, 56, C_CARD);
        DrawRectangle(0, 54, WIN_W, 2, C_ACCENT);    // accent underline
        // Logo dot
        DrawCircle(30, 28, 10, C_ACCENT);
        DrawCircle(30, 28,  6, C_BG);
        DrawCircle(30, 28,  3, C_ACCENT);
        txt(title, 52, 15, 22, C_TEXT);
        Customer* u = sys.getLoggedIn();
        if (u) {
            string s = u->getName();
            Vector2 ts = MeasureTextEx(fnt, s.c_str(), fSz, 1);
            // Avatar circle
            float ax = WIN_W - ts.x - 52.f;
            DrawCircle((int)(ax - 16), 28, 14, C_ACCENT);
            txt(string(1, (char)toupper(u->getName()[0])).c_str(),
                ax-22, 17, 17, C_BG);
            txt(s, ax, 18, fSz, C_MUTED);
        }
    }

    // Bottom navigation bar (shared by most screens)
    void bottomBar(const char* leftLabel, Screen leftDest,
                   const char* rightLabel = nullptr, Screen rightDest = SCR_MAIN) {
        DrawRectangle(0, WIN_H-58, WIN_W, 58, C_CARD);
        DrawRectangle(0, WIN_H-58, WIN_W, 2, C_CARD2);
        if (btn({20.f, (float)WIN_H-46.f, 130.f, 36.f}, leftLabel))
            screen = leftDest;
        if (rightLabel)
            if (abtn({(float)WIN_W-158.f, (float)WIN_H-46.f, 138.f, 36.f}, rightLabel))
                screen = rightDest;
    }

    // ════════════════════════════════════════════════════════
    //  SCREEN: MAIN
    // ════════════════════════════════════════════════════════
    void drawMain() {
        // Background decorative elements
        DrawCircleGradient(WIN_W-80, 60, 280,
            {C_ACCENT.r, C_ACCENT.g, C_ACCENT.b, 18}, C_BG);
        DrawCircleGradient(80, WIN_H-80, 220,
            {C_ACCENT.r, C_ACCENT.g, C_ACCENT.b, 12}, C_BG);
        // Subtle grid lines (barely visible on charcoal)
        for (int i = 0; i < WIN_W; i += 60)
            DrawLineEx({(float)i, 0}, {(float)i, (float)WIN_H}, 1, {55, 55, 55, 180});
        for (int i = 0; i < WIN_H; i += 60)
            DrawLineEx({0, (float)i}, {(float)WIN_W, (float)i}, 1, {55, 55, 55, 180});

        // Logo
        DrawCircle(WIN_W/2, 80, 36, C_ACCENT);
        DrawCircle(WIN_W/2, 80, 24, C_BG);
        DrawCircle(WIN_W/2, 80, 10, C_ACCENT);

        txt("Order Food.",         WIN_W/2, 136, 54, C_TEXT,  true);
        txt("Fast. Simple. Delicious.", WIN_W/2, 202, 21, C_MUTED, true);

        float bw = 230, bh = 52, bx = (WIN_W-bw)/2.f;
        if (abtn({bx, 280, bw, bh}, "Login"))    { clearLogin(); screen = SCR_LOGIN;    }
        if (btn( {bx, 348, bw, bh}, "Register")) { clearAll();   screen = SCR_REGISTER; }
        if (dbtn({bx, 416, bw, bh}, "Exit"))      running = false;

        txt("v1.0  |  OOP C++ Project", WIN_W/2, WIN_H-32, 13, C_MUTED, true);
    }

    // ════════════════════════════════════════════════════════
    //  SCREEN: LOGIN
    // ════════════════════════════════════════════════════════
    void drawLogin() {
        nav("Login");
        Rectangle card = {WIN_W/2-190.f, 78.f, 380.f, 390.f};
        fillR(card, 0.06f, C_CARD);
        DrawRectangleRoundedLines(card, 0.06f, 8, C_CARD2);
        // Accent top strip
        DrawRectangle((int)card.x+2, (int)card.y+2, (int)card.width-4, 4, C_ACCENT);

        txt("Welcome Back",       WIN_W/2, 104, 28, C_TEXT,  true);
        txt("Sign in to continue", WIN_W/2, 140, fSz, C_MUTED, true);

        inputBox({WIN_W/2-148.f, 194.f, 296.f, 46.f}, "Username",
                 ibUser, ibUserLen, ibUserAct);
        inputBox({WIN_W/2-148.f, 278.f, 296.f, 46.f}, "Password",
                 ibPass, ibPassLen, ibPassAct, true);

        if (abtn({WIN_W/2-110.f, 356.f, 220.f, 48.f}, "Login")) {
            if (sys.login(ibUser, ibPass)) {
                cart = Order(sys.getLoggedIn()->getNextOrderId());
                screen = SCR_DASHBOARD; clearLogin();
                setStatus("Welcome back, " + sys.getLoggedIn()->getName() + "!");
            } else setStatus("Invalid username or password.", C_DANGER);
        }
        if (btn({WIN_W/2-80.f, 420.f, 160.f, 36.f}, "Back"))
            { clearLogin(); screen = SCR_MAIN; }
        drawStatus();
    }

    // ════════════════════════════════════════════════════════
    //  SCREEN: REGISTER
    // ════════════════════════════════════════════════════════
    void drawRegister() {
        nav("Create Account");
        Rectangle card = {WIN_W/2-200.f, 66.f, 400.f, 560.f};
        fillR(card, 0.06f, C_CARD);
        DrawRectangleRoundedLines(card, 0.06f, 8, C_CARD2);
        DrawRectangle((int)card.x+2, (int)card.y+2, (int)card.width-4, 4, C_ACCENT);

        txt("Create Account", WIN_W/2, 90, 26, C_TEXT, true);
        txt("Fill in your details below", WIN_W/2, 122, fSz-1, C_MUTED, true);

        inputBox({WIN_W/2-158.f, 162.f, 316.f, 44.f}, "Username",
                 ibUser, ibUserLen, ibUserAct);
        inputBox({WIN_W/2-158.f, 238.f, 316.f, 44.f}, "Password",
                 ibPass, ibPassLen, ibPassAct, true);
        inputBox({WIN_W/2-158.f, 314.f, 316.f, 44.f}, "Full Name",
                 ibName, ibNameLen, ibNameAct);
        inputBox({WIN_W/2-158.f, 390.f, 316.f, 44.f}, "Contact Number",
                 ibContact, ibContactLen, ibContactAct);
        inputBox({WIN_W/2-158.f, 466.f, 316.f, 44.f}, "Delivery Address",
                 ibAddr, ibAddrLen, ibAddrAct, false, 63);

        if (abtn({WIN_W/2-110.f, 526.f, 220.f, 48.f}, "Register")) {
            if (!ibUserLen || !ibPassLen || !ibNameLen || !ibContactLen || !ibAddrLen)
                setStatus("Please fill in all fields.", C_DANGER);
            else if (sys.registerUser(ibUser, ibPass, ibName, ibContact, ibAddr)) {
                setStatus("Account created! Please log in.");
                clearAll(); screen = SCR_MAIN;
            } else setStatus("Username already taken.", C_DANGER);
        }
        if (btn({WIN_W/2-70.f, 580.f, 140.f, 34.f}, "Back"))
            { clearAll(); screen = SCR_MAIN; }
        drawStatus();
    }

    // ════════════════════════════════════════════════════════
    //  SCREEN: DASHBOARD
    // ════════════════════════════════════════════════════════
    void drawDashboard() {
        nav("Dashboard");

        Customer* u = sys.getLoggedIn();

        // Greeting banner
        fillR({24.f, 62.f, (float)WIN_W-48.f, 88.f}, 0.05f, C_CARD);
        DrawRectangle(24, 62, 4, 88, C_ACCENT);
        txt("Hello, " + u->getName() + "!", 40, 76, 26, C_TEXT);
        char sub[64]; sprintf(sub, "%d order(s) placed so far.", u->getOrderCount());
        txt(sub, 40, 112, fSz-1, C_MUTED);
        if (!cart.isEmpty()) {
            char cb[32]; sprintf(cb, "%d item(s) in cart", cart.getItemCount());
            badge(WIN_W-210, 88, cb, C_ACCENT, C_BG, 14.f);
        }

        // Navigation tiles
        struct Tile { const char* label; const char* sub; Screen go; bool acc; };
        Tile tiles[] = {
            {"Browse Menu",   "View all categories",  SCR_MENU,    true},
            {"View Cart",     "Manage your order",    SCR_CART,    false},
            {"Order History", "Past orders & totals", SCR_HISTORY, false},
        };
        float tw = 274.f, th = 108.f, tx = 24.f, ty = 164.f, gap = 24.f;
        for (int i = 0; i < 3; i++) {
            Rectangle r = {tx+(tw+gap)*i, ty, tw, th};
            Vector2 m = GetMousePosition();
            bool ov = CheckCollisionPointRec(m, r);
            Color base = tiles[i].acc ? C_ACCENT : C_CARD;
            Color hov2 = tiles[i].acc ? C_ACCENT2 : C_CARD2;
            if (ov && !tiles[i].acc) glow(r, C_ACCENT, 20);
            fillR(r, 0.08f, ov ? hov2 : base);
            if (!tiles[i].acc) DrawRectangleRoundedLines(r, 0.08f, 8, ov ? C_ACCENT : C_CARD2);
            // Left accent bar on non-accent tiles
            if (!tiles[i].acc && ov) DrawRectangle((int)r.x, (int)r.y+8, 3, (int)r.height-16, C_ACCENT);
            txt(tiles[i].label, r.x+16, r.y+24, 18, tiles[i].acc ? C_BG : C_TEXT);
            txt(tiles[i].sub,   r.x+16, r.y+54, 13, tiles[i].acc ? C_BG : C_MUTED);
            if (ov && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) screen = tiles[i].go;
        }

        // Account info card
        fillR({24.f, 292.f, (float)WIN_W-48.f, 188.f}, 0.05f, C_CARD);
        DrawRectangleRoundedLines({24.f, 292.f, (float)WIN_W-48.f, 188.f}, 0.05f, 8, C_CARD2);
        sectionLabel(40, 308, "Account Info");
        div(332, 40, WIN_W-40);

        char li[128];
        float col1 = 40, col2 = 180;
        float ry = 344;
        txt("Username",  col1, ry,    fSz-1, C_MUTED);
        txt(u->getUsername(), col2, ry, fSz-1, C_TEXT);
        txt("Contact",   col1, ry+30, fSz-1, C_MUTED);
        txt(u->getContact(), col2, ry+30, fSz-1, C_TEXT);
        txt("Address",   col1, ry+60, fSz-1, C_MUTED);
        txt(u->getAddress().empty() ? "N/A" : u->getAddress(),
            col2, ry+60, fSz-1, C_TEXT);
        txt("Orders",    col1, ry+90, fSz-1, C_MUTED);
        sprintf(li, "%d", u->getOrderCount());
        txt(li, col2, ry+90, fSz-1, C_ACCENT);

        txt("Data saved to: ../data/users.txt", 40, 494, 12, C_MUTED);

        if (dbtn({(float)WIN_W-154.f, (float)WIN_H-68.f, 130.f, 40.f}, "Logout", 16)) {
            sys.logout(); cart = Order(); screen = SCR_MAIN;
        }
        drawStatus();
    }

    // ════════════════════════════════════════════════════════
    //  SCREEN: MENU
    // ════════════════════════════════════════════════════════
    void drawMenu() {
        nav("Browse Menu");
        Menu& m = sys.getMenu();
        string cats[MAX_CATEGORIES]; cats[0] = "All";
        int tc = m.getCategories(cats+1, MAX_CATEGORIES-1) + 1;

        // Category tab bar
        float tx = 16, ty = 62, th = 30;
        for (int i = 0; i < tc; i++) {
            Vector2 ts = MeasureTextEx(fnt, cats[i].c_str(), fSz-1, 1);
            float tw2 = ts.x + 24;
            Rectangle r = {tx, ty, tw2, th};
            bool sel = (cats[i] == activeCat);
            fillR(r, 0.5f, sel ? C_ACCENT : C_CARD);
            if (!sel) DrawRectangleRoundedLines(r, 0.5f, 8, C_CARD2);
            DrawTextEx(fnt, cats[i].c_str(), {tx+12, ty+6}, fSz-1, 1,
                       sel ? C_BG : C_MUTED);
            if (!sel && CheckCollisionPointRec(GetMousePosition(), r)
                && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                { activeCat = cats[i]; menuScroll = 0; }
            tx += tw2 + 6;
        }

        // Filter items
        FoodItem* filtered[MAX_MENU_ITEMS]; int fc = 0;
        for (int i = 0; i < m.getCount(); i++) {
            FoodItem* f = m.get(i);
            if (activeCat == "All" || f->getCategory() == activeCat)
                filtered[fc++] = f;
        }

        float viewH  = WIN_H - 100.f - 58.f;
        float scrollH = fc * 72.f;
        menuScroll -= GetMouseWheelMove() * 40.f;
        if (menuScroll < 0) menuScroll = 0;
        if (scrollH > viewH && menuScroll > scrollH-viewH) menuScroll = scrollH-viewH;

        BeginScissorMode(14, 100, WIN_W-28, (int)viewH);
        float iy = 100 - menuScroll;
        for (int i = 0; i < fc; i++) {
            FoodItem* f = filtered[i];
            Rectangle row = {14.f, iy, (float)WIN_W-28.f, 62.f};
            if (iy+62 > 100 && iy < WIN_H-58) {
                Vector2 mp = GetMousePosition();
                bool ov = CheckCollisionPointRec(mp, row) && mp.y > 100 && mp.y < WIN_H-58;
                fillR(row, 0.05f, ov ? C_CARD2 : C_CARD);
                Color border = f->isSpecial()
                    ? Color{C_GOLD.r, C_GOLD.g, C_GOLD.b, 90}
                    : (ov ? Color{C_ACCENT.r, C_ACCENT.g, C_ACCENT.b, 60} : C_CARD2);
                DrawRectangleRoundedLines(row, 0.05f, 8, border);
                if (ov) DrawRectangle((int)row.x, (int)row.y+6, 3, (int)row.height-12, C_ACCENT);

                float nx = row.x + 16;
                if (f->isSpecial()) {
                    badge(row.x+12, row.y+8, "SPECIAL", C_GOLD, C_BG);
                    nx = row.x + 96;
                }
                txt(f->getName(), nx, row.y+9, 18, C_TEXT);
                txt(f->getCategory(), nx, row.y+34, 13, C_MUTED);

                char ps[32]; sprintf(ps, "Rs. %.0f", f->getPrice());
                if (f->isSpecial()) {
                    SpecialItem* si = dynamic_cast<SpecialItem*>(f);
                    if (si) {
                        char eb[32]; sprintf(eb, "+Rs.%.0f premium", si->getExtra());
                        Vector2 ets = MeasureTextEx(fnt, eb, 13, 1);
                        txt(eb, WIN_W-ets.x-148, row.y+37, 13, C_MUTED);
                    }
                }
                Vector2 pts = MeasureTextEx(fnt, ps, 20, 1);
                txt(ps, WIN_W-pts.x-148, row.y+16, 20,
                    f->isSpecial() ? C_GOLD : C_ACCENT);

                if (abtn({(float)WIN_W-128.f, row.y+12.f, 106.f, 36.f}, "+ Add")) {
                    cart.addItem(f, 1);
                    setStatus("Added: " + f->getName());
                }
            }
            iy += 72.f;
        }
        EndScissorMode();

        // Scroll indicator
        if (scrollH > viewH) {
            float th2 = viewH * (viewH/scrollH);
            float ty2 = 100 + (menuScroll/scrollH) * viewH;
            DrawRectangle(WIN_W-5, (int)ty2, 3, (int)th2, C_MUTED);
        }

        DrawRectangle(0, WIN_H-58, WIN_W, 58, C_CARD);
        DrawRectangle(0, WIN_H-58, WIN_W, 2, C_CARD2);
        if (btn({18.f, (float)WIN_H-46.f, 110.f, 36.f}, "Back")) screen = SCR_DASHBOARD;
        char ctxt[32]; sprintf(ctxt, "Cart: %d item(s)", cart.getItemCount());
        txt(ctxt, WIN_W/2, WIN_H-38, fSz-1, C_MUTED, true);
        if (abtn({(float)WIN_W-150.f, (float)WIN_H-46.f, 130.f, 36.f}, "View Cart"))
            screen = SCR_CART;
        drawStatus();
    }

    // ════════════════════════════════════════════════════════
    //  SCREEN: CART  (scrollable item list)
    // ════════════════════════════════════════════════════════
    void drawCart() {
        nav("Your Cart");

        if (cart.isEmpty()) {
            txt("Your cart is empty.", WIN_W/2, WIN_H/2-30, 22, C_MUTED, true);
            txt("Go to the menu to add items.", WIN_W/2, WIN_H/2+10, fSz-1, C_MUTED, true);
            if (abtn({(float)WIN_W/2-100, (float)WIN_H/2+55, 200, 46}, "Browse Menu"))
                screen = SCR_MENU;
            drawStatus(); return;
        }

        // ── Left panel: scrollable item list ──────────────
        float panelW  = WIN_W - 318.f;
        float panelH  = WIN_H - 124.f;
        float itemH   = 64.f;
        float contentH = cart.getItemCount() * itemH;
        float viewH    = panelH - 54.f;   // leave room for header

        fillR({14.f, 62.f, panelW, panelH}, 0.05f, C_CARD);
        DrawRectangleRoundedLines({14.f, 62.f, panelW, panelH}, 0.05f, 8, C_CARD2);
        sectionLabel(30, 76, "Cart Items");
        div(100, 30, 30+panelW-24);

        // Scroll
        cartScroll -= GetMouseWheelMove() * 40.f;
        if (cartScroll < 0) cartScroll = 0;
        if (contentH > viewH && cartScroll > contentH-viewH) cartScroll = contentH-viewH;

        BeginScissorMode(14, 102, (int)panelW, (int)viewH);
        float iy = 106 - cartScroll;
        for (int i = 0; i < cart.getItemCount(); i++) {
            const OrderItem& oi = cart.getItem(i);
            if (!oi.food) continue;
            Rectangle row = {22.f, iy, panelW-16.f, 54.f};
            if (iy+54 > 102 && iy < 102+viewH) {
                Vector2 mp = GetMousePosition();
                bool ov = CheckCollisionPointRec(mp, row) && mp.y > 102;
                fillR(row, 0.07f, ov ? C_CARD2 : C_BG);
                if (ov) DrawRectangle((int)row.x, (int)row.y+6, 3, (int)row.height-12, C_ACCENT);

                float nx = row.x + 14;
                if (oi.food->isSpecial()) {
                    badge(row.x+8, row.y+6, "*", C_GOLD, C_BG);
                    nx = row.x + 42;
                }
                txt(oi.food->getName(), nx, row.y+7, 17, C_TEXT);
                char info[48]; sprintf(info, "Rs. %.0f  ×%d", oi.food->getPrice(), oi.qty);
                txt(info, nx, row.y+30, 13, C_MUTED);
                char tot[24]; sprintf(tot, "Rs. %.0f", oi.subtotal());
                Vector2 ts = MeasureTextEx(fnt, tot, 17, 1);
                txt(tot, row.x+row.width-ts.x-50, row.y+17, 17, C_ACCENT);

                if (dbtn({row.x+row.width-40.f, row.y+12.f, 30.f, 28.f}, "x", 14)) {
                    cart.removeItem(i); cartScroll = 0;
                    setStatus("Item removed."); break;
                }
            }
            iy += itemH;
        }
        EndScissorMode();

        // Scroll indicator for cart
        if (contentH > viewH) {
            float th = viewH * (viewH/contentH);
            float ty = 102 + (cartScroll/contentH) * viewH;
            DrawRectangle((int)(14+panelW-5), (int)ty, 3, (int)th, C_MUTED);
        }

        // ── Right panel: summary + promo ─────────────────
        float rx = WIN_W - 298.f;
        fillR({rx, 62.f, 284.f, panelH}, 0.05f, C_CARD);
        DrawRectangleRoundedLines({rx, 62.f, 284.f, panelH}, 0.05f, 8, C_CARD2);
        sectionLabel(rx+14, 76, "Order Summary");
        div(100, rx+14, rx+270);

        char buf[64];
        sprintf(buf, "Subtotal", 0); // suppress unused warning
        txt("Subtotal", rx+14, 112, fSz-1, C_MUTED);
        sprintf(buf, "Rs. %.2f", cart.getSubtotal());
        Vector2 bts = MeasureTextEx(fnt, buf, fSz-1, 1);
        txt(buf, rx+270-bts.x, 112, fSz-1, C_TEXT);

        if (cart.getDiscountPct() > 0) {
            char pl[40]; sprintf(pl, "Discount (%s)", cart.getPromo().c_str());
            txt(pl, rx+14, 140, fSz-1, C_SUCCESS);
            sprintf(buf, "-Rs. %.2f", cart.getDiscountAmt());
            bts = MeasureTextEx(fnt, buf, fSz-1, 1);
            txt(buf, rx+270-bts.x, 140, fSz-1, C_SUCCESS);
        }

        div(168, rx+14, rx+270);
        txt("Total", rx+14, 178, 19, C_TEXT);
        sprintf(buf, "Rs. %.2f", cart.getTotal());
        bts = MeasureTextEx(fnt, buf, 20, 1);
        txt(buf, rx+270-bts.x, 176, 20, C_ACCENT);

        // Promo code
        txt("Promo Code", rx+14, 216, fSz-1, C_MUTED);
        inputBox({rx+14, 238, 188, 42}, "", ibPromo, ibPromoLen, ibPromoAct);
        if (abtn({rx+208, 238, 60, 42}, "Go")) {
            double d = sys.getPromo().validate(ibPromo);
            if (d > 0) {
                cart.applyDiscount(d, ibPromo);
                char sm[48]; sprintf(sm, "%s applied! %.0f%% off", ibPromo, d);
                setStatus(sm);
            } else setStatus("Invalid promo code.", C_DANGER);
        }

        // Hint codes
        txt("SAVE10  SAVE20  WELCOME", rx+14, 290, 12, C_MUTED);
        txt("BIRYANI  FLAT30",         rx+14, 308, 12, C_MUTED);

        if (cart.getDiscountPct() > 0) {
            if (btn({rx+14, 332, 140, 28}, "Remove Promo",
                    C_CARD, {C_DANGER.r, C_DANGER.g, C_DANGER.b, 200}, 13)) {
                cart.clearDiscount();
                memset(ibPromo, 0, sizeof ibPromo); ibPromoLen = 0;
                setStatus("Promo removed.");
            }
        }

        // Delivery address preview
        Customer* u = sys.getLoggedIn();
        if (u) {
            div(370, rx+14, rx+270);
            txt("Deliver to:", rx+14, 380, 13, C_MUTED);
            string addr = u->getAddress().empty() ? "N/A" : u->getAddress();
            txt(addr, rx+14, 398, fSz-1, C_TEXT);
        }

        // Place order button
        if (abtn({rx+14, (float)panelH-88, 256, 52}, "Place Order")) {
            if (u) {
                lastOrder = cart;                // save for confirm screen
                u->saveOrder(cart);
                sys.onOrderPlaced();
                cart = Order(u->getNextOrderId());
                memset(ibPromo, 0, sizeof ibPromo); ibPromoLen = 0;
                cartScroll = 0;
                screen = SCR_CONFIRM;            // go to confirmation screen
            }
        }

        // Bottom bar
        DrawRectangle(0, WIN_H-58, WIN_W, 58, C_CARD);
        DrawRectangle(0, WIN_H-58, WIN_W, 2, C_CARD2);
        if (btn({18.f, (float)WIN_H-46.f, 110.f, 36.f}, "Menu")) screen = SCR_MENU;
        if (btn({(float)WIN_W-158.f, (float)WIN_H-46.f, 138.f, 36.f}, "Dashboard"))
            screen = SCR_DASHBOARD;
        drawStatus();
    }

    // ════════════════════════════════════════════════════════
    //  SCREEN: ORDER CONFIRMATION / RECEIPT
    // ════════════════════════════════════════════════════════
    void drawConfirm() {
        nav("Order Confirmed");
        Customer* u = sys.getLoggedIn();

        // Success banner
        DrawCircle(WIN_W/2, 108, 38, C_ACCENT);
        // Checkmark drawn with lines
        DrawLineEx({(float)WIN_W/2-14, 108}, {(float)WIN_W/2-2, 122}, 4, C_BG);
        DrawLineEx({(float)WIN_W/2-2,  122}, {(float)WIN_W/2+18, 94}, 4, C_BG);

        txt("Order Placed!", WIN_W/2, 158, 30, C_TEXT, true);
        char hdr[48]; sprintf(hdr, "Order #%d  confirmed", lastOrder.getOrderId());
        txt(hdr, WIN_W/2, 196, fSz-1, C_MUTED, true);

        // Receipt card
        float cw = 560.f, cx = (WIN_W-cw)/2.f, cy = 226.f;
        float ch  = WIN_H - cy - 90.f;
        fillR({cx, cy, cw, ch}, 0.05f, C_CARD);
        DrawRectangleRoundedLines({cx, cy, cw, ch}, 0.05f, 8, C_CARD2);
        DrawRectangle((int)cx+2, (int)cy+2, (int)cw-4, 4, C_ACCENT);

        sectionLabel(cx+16, cy+16, "Receipt");
        div(cy+42, cx+16, cx+cw-16);

        // Column headers
        txt("Item",     cx+16,      cy+52, 14, C_MUTED);
        txt("Qty",      cx+cw-180,  cy+52, 14, C_MUTED);
        txt("Subtotal", cx+cw-110,  cy+52, 14, C_MUTED);
        div(cy+70, cx+16, cx+cw-16);

        float ry = cy + 78;
        for (int i = 0; i < lastOrder.getItemCount(); i++) {
            const OrderItem& oi = lastOrder.getItem(i);
            if (!oi.food) continue;

            Color nameCol = oi.food->isSpecial() ? C_GOLD : C_TEXT;
            txt(oi.food->getName(), cx+16, ry, fSz-1, nameCol);

            char qty[8];   sprintf(qty,   "%d",        oi.qty);
            char sub2[24]; sprintf(sub2,  "Rs. %.0f",  oi.subtotal());
            txt(qty,  cx+cw-175, ry, fSz-1, C_TEXT);
            txt(sub2, cx+cw-110, ry, fSz-1, C_ACCENT);
            ry += 26.f;
        }

        div(ry+4, cx+16, cx+cw-16);
        ry += 14;

        char buf[64];
        txt("Subtotal", cx+16, ry, fSz-1, C_MUTED);
        sprintf(buf, "Rs. %.2f", lastOrder.getSubtotal());
        Vector2 bts = MeasureTextEx(fnt, buf, fSz-1, 1);
        txt(buf, cx+cw-16-bts.x, ry, fSz-1, C_TEXT);
        ry += 26;

        if (lastOrder.getDiscountPct() > 0) {
            char dl[40]; sprintf(dl, "Discount (%s)", lastOrder.getPromo().c_str());
            txt(dl, cx+16, ry, fSz-1, C_SUCCESS);
            sprintf(buf, "-Rs. %.2f", lastOrder.getDiscountAmt());
            bts = MeasureTextEx(fnt, buf, fSz-1, 1);
            txt(buf, cx+cw-16-bts.x, ry, fSz-1, C_SUCCESS);
            ry += 26;
        }

        div(ry+4, cx+16, cx+cw-16);
        ry += 14;

        txt("TOTAL", cx+16, ry, 20, C_TEXT);
        sprintf(buf, "Rs. %.2f", lastOrder.getTotal());
        bts = MeasureTextEx(fnt, buf, 20, 1);
        txt(buf, cx+cw-16-bts.x, ry, 20, C_ACCENT);
        ry += 34;

        // Delivery info
        if (u && !u->getAddress().empty()) {
            div(ry, cx+16, cx+cw-16); ry += 12;
            txt("Deliver to:", cx+16, ry, 13, C_MUTED);
            txt(u->getAddress(), cx+16, ry+18, fSz-1, C_TEXT);
        }

        // Buttons
        DrawRectangle(0, WIN_H-58, WIN_W, 58, C_CARD);
        DrawRectangle(0, WIN_H-58, WIN_W, 2, C_CARD2);
        if (btn({18.f, (float)WIN_H-46.f, 150.f, 36.f}, "Order History"))
            screen = SCR_HISTORY;
        if (abtn({(float)WIN_W-180.f, (float)WIN_H-46.f, 160.f, 36.f}, "Back to Dashboard"))
            screen = SCR_DASHBOARD;
    }

    // ════════════════════════════════════════════════════════
    //  SCREEN: ORDER HISTORY
    // ════════════════════════════════════════════════════════
    void drawHistory() {
        nav("Order History");
        Customer* u = sys.getLoggedIn();
        if (!u || u->getOrderCount() == 0) {
            txt("No past orders yet.", WIN_W/2, WIN_H/2-16, 22, C_MUTED, true);
            txt("Place your first order from the menu!", WIN_W/2, WIN_H/2+18, fSz-1, C_MUTED, true);
            if (abtn({(float)WIN_W/2-90, (float)WIN_H/2+58, 180, 46}, "Browse Menu"))
                screen = SCR_MENU;
            drawStatus(); return;
        }

        int   n     = u->getOrderCount();
        float rowH  = 134.f;
        float cH    = n * rowH + 20.f;
        float viewH = WIN_H - 120.f;
        histScroll -= GetMouseWheelMove() * 40.f;
        if (histScroll < 0) histScroll = 0;
        if (cH > viewH && histScroll > cH-viewH) histScroll = cH-viewH;

        BeginScissorMode(16, 62, WIN_W-32, (int)viewH);
        float oy = 68.f - histScroll;
        for (int i = n-1; i >= 0; i--) {
            const Order& o = u->getOrder(i);
            if (oy+rowH > 62 && oy < WIN_H-54) {
                Rectangle card = {20.f, oy, (float)WIN_W-40.f, rowH-10.f};
                fillR(card, 0.06f, C_CARD);
                DrawRectangleRoundedLines(card, 0.06f, 8, C_CARD2);
                DrawRectangle((int)card.x, (int)card.y+6, 4, (int)card.height-12, C_ACCENT);

                char hdr[32]; sprintf(hdr, "Order #%d", o.getOrderId());
                txt(hdr, card.x+16, card.y+10, 18, C_ACCENT);
                if (o.getDiscountPct() > 0) {
                    char pb[32]; sprintf(pb, "%s  %.0f%% off",
                        o.getPromo().c_str(), o.getDiscountPct());
                    badge(card.x+146, card.y+12, pb, C_SUCCESS, C_BG);
                }

                float ix = card.x+16, itmY = card.y+40;
                for (int j = 0; j < o.getItemCount(); j++) {
                    const OrderItem& oi = o.getItem(j);
                    if (!oi.food) continue;
                    char line[64];
                    sprintf(line, "• %s  ×%d  Rs.%.0f",
                        oi.food->getName().c_str(), oi.qty, oi.subtotal());
                    Vector2 ts = MeasureTextEx(fnt, line, 14, 1);
                    Color lc = oi.food->isSpecial() ? C_GOLD : C_MUTED;
                    txt(line, ix, itmY, 14, lc);
                    ix += ts.x + 18;
                    if (ix > WIN_W-220) { ix = card.x+16; itmY += 20; }
                }

                char tot[48]; sprintf(tot, "Total:  Rs. %.2f", o.getTotal());
                Vector2 ts = MeasureTextEx(fnt, tot, 17, 1);
                txt(tot, card.x+card.width-ts.x-16, card.y+card.height-28, 17, C_TEXT);
            }
            oy += rowH;
        }
        EndScissorMode();

        if (cH > viewH) {
            float th = viewH*(viewH/cH);
            float ty = 62 + (histScroll/cH)*viewH;
            DrawRectangle(WIN_W-5, (int)ty, 3, (int)th, C_MUTED);
        }

        DrawRectangle(0, WIN_H-58, WIN_W, 58, C_CARD);
        DrawRectangle(0, WIN_H-58, WIN_W, 2, C_CARD2);
        if (btn({18.f, (float)WIN_H-46.f, 140.f, 36.f}, "Dashboard"))
            screen = SCR_DASHBOARD;
        drawStatus();
    }

    // ════════════════════════════════════════════════════════
    //  Public entry point
    // ════════════════════════════════════════════════════════
public:
    void run() {
        SetConfigFlags(FLAG_MSAA_4X_HINT);
        InitWindow(WIN_W, WIN_H, "Online Food Ordering System");
        SetTargetFPS(60);
        fnt = LoadFont("../data/Lexend-VariableFont_wght.ttf");
        if (fnt.baseSize == 0) fnt = GetFontDefault();

        while (running && !WindowShouldClose()) {
            BeginDrawing();
            ClearBackground(C_BG);
            switch (screen) {
                case SCR_MAIN:      drawMain();      break;
                case SCR_LOGIN:     drawLogin();     break;
                case SCR_REGISTER:  drawRegister();  break;
                case SCR_DASHBOARD: drawDashboard(); break;
                case SCR_MENU:      drawMenu();      break;
                case SCR_CART:      drawCart();      break;
                case SCR_CONFIRM:   drawConfirm();   break;
                case SCR_HISTORY:   drawHistory();   break;
            }
            EndDrawing();
        }
        UnloadFont(fnt);
        CloseWindow();
    }
};