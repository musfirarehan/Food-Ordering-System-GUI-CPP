#pragma once
// ============================================================
//  CONSTANTS & COLOUR PALETTE
//  Shared across all headers – include this first.
// ============================================================
#include "raylib.h"
#include <string>
using namespace std;

// ── Limits ────────────────────────────────────────────────
const int MAX_MENU_ITEMS  = 20;
const int MAX_ORDER_ITEMS = 20;
const int MAX_ORDERS      = 50;
const int MAX_CUSTOMERS   = 20;
const int MAX_PROMOS      = 5;
const int MAX_CATEGORIES  = 10;
const int WIN_W           = 1000;
const int WIN_H           = 680;

// ── Data file path ─────────────────────────────────────────
// src\ is one level below the project root; data\ is a sibling of src\
static const char* DATA_FILE = "../data/users.txt";

// ── Colour palette  (rose + plum + charcoal + light grey) ─
//   #F3F4F4  light grey     → background
//   #853953  deep rose      → primary accent
//   #612D53  dark plum      → secondary / hover
//   #2C2C2C  near-black     → cards, nav bar
static const Color C_BG      = { 44,  44,  44, 255};  // #2C2C2C  charcoal – main bg
static const Color C_CARD    = { 36,  36,  36, 255};  // slightly darker card surface
static const Color C_CARD2   = { 55,  55,  55, 255};  // lighter card / hover surface
static const Color C_ACCENT  = {133,  57,  83, 255};  // #853953  deep rose
static const Color C_ACCENT2 = { 97,  45,  83, 255};  // #612D53  dark plum (hover)
static const Color C_TEXT    = {243, 244, 244, 255};  // #F3F4F4  near-white text
static const Color C_MUTED   = {160, 155, 160, 255};  // soft grey muted text
static const Color C_SUCCESS = {133,  57,  83, 255};  // rose = success (consistent)
static const Color C_DANGER  = {190,  70,  60, 255};  // warm red for destructive actions
static const Color C_GOLD    = {210, 160,  50, 255};  // amber for special items