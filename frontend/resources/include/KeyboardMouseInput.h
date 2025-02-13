/*
 * KeyboardMouseInput.h
 *
 * Copyright (C) 2018 by VISUS (Universitaet Stuttgart).
 * Alle Rechte vorbehalten.
 */

#pragma once

#include <algorithm>
#include <bitset>
#include <iterator>
#include <ostream>
#include <sstream> // stringstream
#include <type_traits>
#include <vector>

namespace megamol::frontend_resources {

// GLFW keyboard keys
enum class Key : int {
    KEY_UNKNOWN = -1,
    KEY_SPACE = 32,
    KEY_APOSTROPHE = 39, /* ' */
    KEY_COMMA = 44,      /* , */
    KEY_MINUS = 45,      /* - */
    KEY_PERIOD = 46,     /* . */
    KEY_SLASH = 47,      /* / */
    KEY_0 = 48,
    KEY_1 = 49,
    KEY_2 = 50,
    KEY_3 = 51,
    KEY_4 = 52,
    KEY_5 = 53,
    KEY_6 = 54,
    KEY_7 = 55,
    KEY_8 = 56,
    KEY_9 = 57,
    KEY_SEMICOLON = 59, /* ; */
    KEY_EQUAL = 61,     /* = */
    KEY_A = 65,
    KEY_B = 66,
    KEY_C = 67,
    KEY_D = 68,
    KEY_E = 69,
    KEY_F = 70,
    KEY_G = 71,
    KEY_H = 72,
    KEY_I = 73,
    KEY_J = 74,
    KEY_K = 75,
    KEY_L = 76,
    KEY_M = 77,
    KEY_N = 78,
    KEY_O = 79,
    KEY_P = 80,
    KEY_Q = 81,
    KEY_R = 82,
    KEY_S = 83,
    KEY_T = 84,
    KEY_U = 85,
    KEY_V = 86,
    KEY_W = 87,
    KEY_X = 88,
    KEY_Y = 89,
    KEY_Z = 90,
    KEY_LEFT_BRACKET = 91,  /* [ */
    KEY_BACKSLASH = 92,     /* \ */
    KEY_RIGHT_BRACKET = 93, /* ] */
    KEY_GRAVE_ACCENT = 96,  /* ` */
    //KEY_a = 97,
    //KEY_b = 98,
    //KEY_c = 99,
    //KEY_d = 100,
    //KEY_e = 101,
    //KEY_f = 102,
    //KEY_g = 103,
    //KEY_h = 104,
    //KEY_i = 105,
    //KEY_j = 106,
    //KEY_k = 107,
    //KEY_l = 108,
    //KEY_m = 109,
    //KEY_n = 110,
    //KEY_o = 111,
    //KEY_p = 112,
    //KEY_q = 113,
    //KEY_r = 114,
    //KEY_s = 115,
    //KEY_t = 116,
    //KEY_u = 117,
    //KEY_v = 118,
    //KEY_w = 119,
    //KEY_x = 120,
    //KEY_y = 121,
    //KEY_z = 122,
    KEY_WORLD_1 = 161, /* non-US #1 */
    KEY_WORLD_2 = 162, /* non-US #2 */
    KEY_ESCAPE = 256,
    KEY_ENTER = 257,
    KEY_TAB = 258,
    KEY_BACKSPACE = 259,
    KEY_INSERT = 260,
    KEY_DELETE = 261,
    KEY_RIGHT = 262,
    KEY_LEFT = 263,
    KEY_DOWN = 264,
    KEY_UP = 265,
    KEY_PAGE_UP = 266,
    KEY_PAGE_DOWN = 267,
    KEY_HOME = 268,
    KEY_END = 269,
    KEY_CAPS_LOCK = 280,
    KEY_SCROLL_LOCK = 281,
    KEY_NUM_LOCK = 282,
    KEY_PRINT_SCREEN = 283,
    KEY_PAUSE = 284,
    KEY_F1 = 290,
    KEY_F2 = 291,
    KEY_F3 = 292,
    KEY_F4 = 293,
    KEY_F5 = 294,
    KEY_F6 = 295,
    KEY_F7 = 296,
    KEY_F8 = 297,
    KEY_F9 = 298,
    KEY_F10 = 299,
    KEY_F11 = 300,
    KEY_F12 = 301,
    KEY_F13 = 302,
    KEY_F14 = 303,
    KEY_F15 = 304,
    KEY_F16 = 305,
    KEY_F17 = 306,
    KEY_F18 = 307,
    KEY_F19 = 308,
    KEY_F20 = 309,
    KEY_F21 = 310,
    KEY_F22 = 311,
    KEY_F23 = 312,
    KEY_F24 = 313,
    KEY_F25 = 314,
    KEY_KP_0 = 320,
    KEY_KP_1 = 321,
    KEY_KP_2 = 322,
    KEY_KP_3 = 323,
    KEY_KP_4 = 324,
    KEY_KP_5 = 325,
    KEY_KP_6 = 326,
    KEY_KP_7 = 327,
    KEY_KP_8 = 328,
    KEY_KP_9 = 329,
    KEY_KP_DECIMAL = 330,
    KEY_KP_DIVIDE = 331,
    KEY_KP_MULTIPLY = 332,
    KEY_KP_SUBTRACT = 333,
    KEY_KP_ADD = 334,
    KEY_KP_ENTER = 335,
    KEY_KP_EQUAL = 336,
    KEY_LEFT_SHIFT = 340,
    KEY_LEFT_CONTROL = 341,
    KEY_LEFT_ALT = 342,
    KEY_LEFT_SUPER = 343,
    KEY_RIGHT_SHIFT = 344,
    KEY_RIGHT_CONTROL = 345,
    KEY_RIGHT_ALT = 346,
    KEY_RIGHT_SUPER = 347,
    KEY_MENU = 348
};

enum class KeyAction : int { PRESS = 0, REPEAT, RELEASE };

enum class MouseButton : int {
    BUTTON_1 = 0,
    BUTTON_2 = 1,
    BUTTON_3 = 2,
    BUTTON_4 = 3,
    BUTTON_5 = 4,
    BUTTON_6 = 5,
    BUTTON_7 = 6,
    BUTTON_8 = 7,
    BUTTON_LEFT = BUTTON_1,
    BUTTON_MIDDLE = BUTTON_3,
    BUTTON_RIGHT = BUTTON_2,
};


enum class MouseButtonAction : int { PRESS = 0, RELEASE };

enum class Modifier : int { ALT = 4, CTRL = 2, SHIFT = 1, NONE = 0 };


class Modifiers {
    typedef std::bitset<3> Bits;
    Bits bits;

public:
    Modifiers() {}

    Modifiers(Modifier mod) : bits(static_cast<typename std::underlying_type<Modifier>::type>(mod)) {}

    explicit Modifiers(int bits) : bits(bits) {}

    inline int toInt() const {
        return static_cast<int>(bits.to_ulong());
    }

    inline std::string ToString() const {
        std::vector<std::string> range;
        const bool s = this->test(frontend_resources::Modifier::SHIFT);
        const bool c = this->test(frontend_resources::Modifier::CTRL);
        const bool a = this->test(frontend_resources::Modifier::ALT);
        if (s)
            range.emplace_back("SHIFT");
        if (c)
            range.emplace_back("CTRL");
        if (a)
            range.emplace_back("ALT");
        std::ostringstream out;
        auto b = range.begin();
        auto e = range.end();
        if (b != e) {
            std::copy(b, std::prev(e), std::ostream_iterator<std::string>(out, " + "));
            b = std::prev(e);
        }
        if (b != e) {
            out << *b;
        }
        return out.str();
    }

    inline void fromInt(int val) {
        bits = Bits(val);
    }

    inline bool none() const {
        return bits.none();
    }

    inline bool test(Modifier mod) const {
        return (bits & Modifiers(mod).bits).any();
    }

    inline bool test(Modifiers mods) const {
        return (bits & mods.bits).any();
    }

    inline bool equals(Modifier mod) const {
        return (bits == Modifiers(mod).bits);
    }

    inline bool equals(Modifiers mods) const {
        return (bits == mods.bits);
    }

    inline Modifiers& reset(Modifier mod) {
        Modifiers mask(mod);
        bits &= ~mask.bits;
        return *this;
    }

    inline Modifiers& set(Modifier mod) {
        Modifiers mask(mod);
        bits |= mask.bits;
        return *this;
    }

    inline Modifiers& operator|=(const Modifiers& other) {
        bits |= other.bits;
        return *this;
    }

    inline Modifiers& operator&=(const Modifiers& other) {
        bits &= other.bits;
        return *this;
    }

    inline Modifiers& operator^=(const Modifiers& other) {
        bits = other.bits;
        return *this;
    }

    inline bool operator==(const Modifiers& other) const {
        return other.equals(*this);
    }
};

inline Modifiers operator|(Modifiers lhs, Modifiers rhs) {
    Modifiers ans = lhs;
    return (ans |= rhs);
}

inline Modifiers operator&(Modifiers lhs, Modifiers rhs) {
    Modifiers ans = lhs;
    return (ans &= rhs);
}

inline Modifiers operator^(Modifiers lhs, Modifiers rhs) {
    Modifiers ans = lhs;
    return (ans ^= rhs);
}

inline Modifiers operator|(Modifier lhs, Modifier rhs) {
    Modifiers ansl(lhs);
    Modifiers ansr(rhs);
    return (ansl |= ansr);
}

inline Modifiers operator&(Modifier lhs, Modifier rhs) {
    Modifiers ansl(lhs);
    Modifiers ansr(rhs);
    return (ansl &= ansr);
}

inline Modifiers operator^(Modifier lhs, Modifier rhs) {
    Modifiers ansl(lhs);
    Modifiers ansr(rhs);
    return (ansl ^= ansr);
}


class KeyCode {
public:
    Modifiers mods;
    Key key;

    /** Ctor. */
    KeyCode() {
        this->key = frontend_resources::Key::KEY_UNKNOWN;
        this->mods = frontend_resources::Modifiers(frontend_resources::Modifier::NONE);
    }

    /** Ctor. */
    KeyCode(Key key) {
        this->key = key;
        this->mods = frontend_resources::Modifiers(frontend_resources::Modifier::NONE);
    }

    /** Ctor. */
    KeyCode(Key key, Modifiers mods) {
        this->mods = mods;
        this->key = key;
    }

    bool operator==(const KeyCode& other) const {
        return (mods.equals(other.mods) && key == other.key);
    }

    /**
     * Generates a human-readable ASCII String representing the key code.
     *
     * @return A human-readable ASCII String
     */
    std::string ToString() const {

        std::string msg;

        const auto mmsg = mods.ToString();
        if (!mmsg.empty()) {
            msg += mmsg + " + ";
        }

        switch (this->key) {
        case (frontend_resources::Key::KEY_UNKNOWN):
            msg += "";
            break; // 'Unknown Key'
        case (frontend_resources::Key::KEY_SPACE):
            msg += "Space";
            break;
        case (frontend_resources::Key::KEY_APOSTROPHE):
            msg += "'";
            break;
        case (frontend_resources::Key::KEY_COMMA):
            msg += ",";
            break;
        case (frontend_resources::Key::KEY_MINUS):
            msg += "-";
            break;
        case (frontend_resources::Key::KEY_PERIOD):
            msg += ".";
            break;
        case (frontend_resources::Key::KEY_SLASH):
            msg += "/";
            break;
        case (frontend_resources::Key::KEY_0):
            msg += "0";
            break;
        case (frontend_resources::Key::KEY_1):
            msg += "1";
            break;
        case (frontend_resources::Key::KEY_2):
            msg += "2";
            break;
        case (frontend_resources::Key::KEY_3):
            msg += "3";
            break;
        case (frontend_resources::Key::KEY_4):
            msg += "4";
            break;
        case (frontend_resources::Key::KEY_5):
            msg += "5";
            break;
        case (frontend_resources::Key::KEY_6):
            msg += "6";
            break;
        case (frontend_resources::Key::KEY_7):
            msg += "7";
            break;
        case (frontend_resources::Key::KEY_8):
            msg += "8";
            break;
        case (frontend_resources::Key::KEY_9):
            msg += "9";
            break;
        case (frontend_resources::Key::KEY_SEMICOLON):
            msg += ":";
            break;
        case (frontend_resources::Key::KEY_EQUAL):
            msg += "=";
            break;
        case (frontend_resources::Key::KEY_A):
            msg += "a";
            break;
        case (frontend_resources::Key::KEY_B):
            msg += "b";
            break;
        case (frontend_resources::Key::KEY_C):
            msg += "c";
            break;
        case (frontend_resources::Key::KEY_D):
            msg += "d";
            break;
        case (frontend_resources::Key::KEY_E):
            msg += "e";
            break;
        case (frontend_resources::Key::KEY_F):
            msg += "f";
            break;
        case (frontend_resources::Key::KEY_G):
            msg += "g";
            break;
        case (frontend_resources::Key::KEY_H):
            msg += "h";
            break;
        case (frontend_resources::Key::KEY_I):
            msg += "i";
            break;
        case (frontend_resources::Key::KEY_J):
            msg += "j";
            break;
        case (frontend_resources::Key::KEY_K):
            msg += "k";
            break;
        case (frontend_resources::Key::KEY_L):
            msg += "l";
            break;
        case (frontend_resources::Key::KEY_M):
            msg += "m";
            break;
        case (frontend_resources::Key::KEY_N):
            msg += "n";
            break;
        case (frontend_resources::Key::KEY_O):
            msg += "o";
            break;
        case (frontend_resources::Key::KEY_P):
            msg += "p";
            break;
        case (frontend_resources::Key::KEY_Q):
            msg += "q";
            break;
        case (frontend_resources::Key::KEY_R):
            msg += "r";
            break;
        case (frontend_resources::Key::KEY_S):
            msg += "s";
            break;
        case (frontend_resources::Key::KEY_T):
            msg += "t";
            break;
        case (frontend_resources::Key::KEY_U):
            msg += "u";
            break;
        case (frontend_resources::Key::KEY_V):
            msg += "v";
            break;
        case (frontend_resources::Key::KEY_W):
            msg += "w";
            break;
        case (frontend_resources::Key::KEY_X):
            msg += "x";
            break;
        case (frontend_resources::Key::KEY_Y):
            msg += "y";
            break;
        case (frontend_resources::Key::KEY_Z):
            msg += "z";
            break;
        case (frontend_resources::Key::KEY_LEFT_BRACKET):
            msg += "[";
            break;
        case (frontend_resources::Key::KEY_BACKSLASH):
            msg += "\\";
            break;
        case (frontend_resources::Key::KEY_RIGHT_BRACKET):
            msg += "]";
            break;
        case (frontend_resources::Key::KEY_GRAVE_ACCENT):
            msg += "`";
            break;
        case (frontend_resources::Key::KEY_WORLD_1):
            msg += "World 1";
            break;
        case (frontend_resources::Key::KEY_WORLD_2):
            msg += "Worls 2";
            break;
        case (frontend_resources::Key::KEY_ESCAPE):
            msg += "Esc";
            break;
        case (frontend_resources::Key::KEY_ENTER):
            msg += "Enter";
            break;
        case (frontend_resources::Key::KEY_TAB):
            msg += "Tab";
            break;
        case (frontend_resources::Key::KEY_BACKSPACE):
            msg += "Backspace";
            break;
        case (frontend_resources::Key::KEY_INSERT):
            msg += "Insert";
            break;
        case (frontend_resources::Key::KEY_DELETE):
            msg += "Delete";
            break;
        case (frontend_resources::Key::KEY_RIGHT):
            msg += "Right";
            break;
        case (frontend_resources::Key::KEY_LEFT):
            msg += "Left";
            break;
        case (frontend_resources::Key::KEY_DOWN):
            msg += "Down";
            break;
        case (frontend_resources::Key::KEY_UP):
            msg += "Up";
            break;
        case (frontend_resources::Key::KEY_PAGE_UP):
            msg += "Page Up";
            break;
        case (frontend_resources::Key::KEY_PAGE_DOWN):
            msg += "Page Down";
            break;
        case (frontend_resources::Key::KEY_HOME):
            msg += "Home";
            break;
        case (frontend_resources::Key::KEY_END):
            msg += "End";
            break;
        case (frontend_resources::Key::KEY_CAPS_LOCK):
            msg += "Caps Lock";
            break;
        case (frontend_resources::Key::KEY_SCROLL_LOCK):
            msg += "Scroll Lock";
            break;
        case (frontend_resources::Key::KEY_NUM_LOCK):
            msg += "Num Lock";
            break;
        case (frontend_resources::Key::KEY_PRINT_SCREEN):
            msg += "Print Screen";
            break;
        case (frontend_resources::Key::KEY_PAUSE):
            msg += "Pause";
            break;
        case (frontend_resources::Key::KEY_F1):
            msg += "F1";
            break;
        case (frontend_resources::Key::KEY_F2):
            msg += "F2";
            break;
        case (frontend_resources::Key::KEY_F3):
            msg += "F3";
            break;
        case (frontend_resources::Key::KEY_F4):
            msg += "F4";
            break;
        case (frontend_resources::Key::KEY_F5):
            msg += "F5";
            break;
        case (frontend_resources::Key::KEY_F6):
            msg += "F6";
            break;
        case (frontend_resources::Key::KEY_F7):
            msg += "F7";
            break;
        case (frontend_resources::Key::KEY_F8):
            msg += "F8";
            break;
        case (frontend_resources::Key::KEY_F9):
            msg += "F9";
            break;
        case (frontend_resources::Key::KEY_F10):
            msg += "F10";
            break;
        case (frontend_resources::Key::KEY_F11):
            msg += "F11";
            break;
        case (frontend_resources::Key::KEY_F12):
            msg += "F12";
            break;
        case (frontend_resources::Key::KEY_F13):
            msg += "F13";
            break;
        case (frontend_resources::Key::KEY_F14):
            msg += "F14";
            break;
        case (frontend_resources::Key::KEY_F15):
            msg += "F15";
            break;
        case (frontend_resources::Key::KEY_F16):
            msg += "F16";
            break;
        case (frontend_resources::Key::KEY_F17):
            msg += "F17";
            break;
        case (frontend_resources::Key::KEY_F18):
            msg += "F18";
            break;
        case (frontend_resources::Key::KEY_F19):
            msg += "F19";
            break;
        case (frontend_resources::Key::KEY_F20):
            msg += "F20";
            break;
        case (frontend_resources::Key::KEY_F21):
            msg += "F21";
            break;
        case (frontend_resources::Key::KEY_F22):
            msg += "F22";
            break;
        case (frontend_resources::Key::KEY_F23):
            msg += "F23";
            break;
        case (frontend_resources::Key::KEY_F24):
            msg += "F24";
            break;
        case (frontend_resources::Key::KEY_F25):
            msg += "F25";
            break;
        case (frontend_resources::Key::KEY_KP_0):
            msg += "Num 0";
            break;
        case (frontend_resources::Key::KEY_KP_1):
            msg += "Num 1";
            break;
        case (frontend_resources::Key::KEY_KP_2):
            msg += "Num 2";
            break;
        case (frontend_resources::Key::KEY_KP_3):
            msg += "Num 3";
            break;
        case (frontend_resources::Key::KEY_KP_4):
            msg += "Num 4";
            break;
        case (frontend_resources::Key::KEY_KP_5):
            msg += "Num 5";
            break;
        case (frontend_resources::Key::KEY_KP_6):
            msg += "Num 6";
            break;
        case (frontend_resources::Key::KEY_KP_7):
            msg += "Num 7";
            break;
        case (frontend_resources::Key::KEY_KP_8):
            msg += "Num 8";
            break;
        case (frontend_resources::Key::KEY_KP_9):
            msg += "Num 9";
            break;
        case (frontend_resources::Key::KEY_KP_DECIMAL):
            msg += "Num ,";
            break;
        case (frontend_resources::Key::KEY_KP_DIVIDE):
            msg += "Num /";
            break;
        case (frontend_resources::Key::KEY_KP_MULTIPLY):
            msg += "Num *";
            break;
        case (frontend_resources::Key::KEY_KP_SUBTRACT):
            msg += "Num -";
            break;
        case (frontend_resources::Key::KEY_KP_ADD):
            msg += "Num +";
            break;
        case (frontend_resources::Key::KEY_KP_ENTER):
            msg += "Num Enter";
            break;
        case (frontend_resources::Key::KEY_KP_EQUAL):
            msg += "Num Equal";
            break;
        case (frontend_resources::Key::KEY_LEFT_SHIFT):
            msg += "Left Shift";
            break;
        case (frontend_resources::Key::KEY_LEFT_CONTROL):
            msg += "Left Ctrl";
            break;
        case (frontend_resources::Key::KEY_LEFT_ALT):
            msg += "Left Alt";
            break;
        case (frontend_resources::Key::KEY_LEFT_SUPER):
            msg += "Left Super";
            break;
        case (frontend_resources::Key::KEY_RIGHT_SHIFT):
            msg += "Right Shift";
            break;
        case (frontend_resources::Key::KEY_RIGHT_CONTROL):
            msg += "Right Ctrl";
            break;
        case (frontend_resources::Key::KEY_RIGHT_ALT):
            msg += "Right Alt";
            break;
        case (frontend_resources::Key::KEY_RIGHT_SUPER):
            msg += "Right Super";
            break;
        case (frontend_resources::Key::KEY_MENU):
            msg += "Menu";
            break;
        default: {
            std::stringstream key_stream;
            key_stream << "[" << static_cast<int>(this->key) << "]";
            msg += key_stream.str();
        } break;
        }

        return msg;
    }
};

namespace input = frontend_resources;

} // namespace megamol::frontend_resources

namespace std {

template<>
struct hash<megamol::frontend_resources::KeyCode> {
    std::size_t operator()(const megamol::frontend_resources::KeyCode& k) const noexcept {
        using std::hash;
        using std::size_t;
        using std::string;

        return (static_cast<uint64_t>(k.key) << 32 | k.mods.toInt());
    }
};

template<>
struct hash<megamol::frontend_resources::Modifiers> {
    std::size_t operator()(const megamol::frontend_resources::Modifiers& m) const noexcept {
        return m.toInt();
    }
};

} // namespace std
