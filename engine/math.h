#pragma once
#include <iostream>
#include <random>
#include <sstream>
#include <algorithm>
#include <cmath>

class Vector2 {
    public:
    float x, y;
    constexpr Vector2():x(0),y(0){};
    constexpr Vector2(float x, float y=0):x(x),y(y){};

    std::string toString() {std::stringstream ss; ss<<"("<<this->x<<", "<<this->y<<")"; return ss.str();};
    constexpr Vector2 asInt() {return Vector2(static_cast<int>(this->x), static_cast<int>(this->y));};

    //Vector Maths
    constexpr friend Vector2 operator/(const Vector2 &v1, const float &num) { return Vector2(v1.x / num, v1.y / num);};
    constexpr friend Vector2 operator/(const float &num, const Vector2 &v1) { return Vector2(num / v1.x, num / v1.y);};
    constexpr friend Vector2 operator*(const Vector2 &v1, const float &num) { return Vector2(v1.x * num, v1.y * num);};
    constexpr friend Vector2 operator*(const float &num, const Vector2 &v1) { return Vector2(num * v1.x, num * v1.y);};
    
    constexpr friend Vector2 operator/(const Vector2 &v1, const Vector2 &v2) { return Vector2(v1.x / v2.x, v1.y / v1.y);};
    constexpr friend Vector2 operator*(const Vector2 &v1, const Vector2 &v2) { return Vector2(v1.x * v2.x, v1.y * v2.y);};
    constexpr friend Vector2 operator+(const Vector2 &v1, const Vector2 &v2) { return Vector2(v1.x + v2.x, v1.y + v2.y);};
    constexpr friend Vector2 operator-(const Vector2 &v1, const Vector2 &v2) { return Vector2(v1.x - v2.x, v1.y - v2.y);};

    constexpr Vector2 & operator/=(const float &num) { x /= num; y /= num; return *this;};
    constexpr Vector2 & operator*=(const float &num) { x *= num; y *= num; return *this;};
    constexpr Vector2 & operator/=(const Vector2 &v2) { x /= v2.x; y /= v2.y; return *this;};
    constexpr Vector2 & operator*=(const Vector2 &v2) { x *= v2.x; y *= v2.y; return *this;};
    constexpr Vector2 & operator+=(const Vector2 &v2) { x += v2.x; y += v2.y; return *this;};
    constexpr Vector2 & operator-=(const Vector2 &v2) { x -= v2.x; y -= v2.y; return *this;};

    constexpr friend bool operator==(const Vector2 &v1, const Vector2 &v2) { return (v1.x == v2.x && v1.y == v2.y);};
    constexpr friend bool operator!=(const Vector2 &v1, const Vector2 &v2) { return (v1.x != v2.x && v1.y != v2.y);};
    constexpr friend bool operator>(const Vector2 &v1, const Vector2 &v2) { return (v1.x > v2.x || v1.y > v2.y);};
    constexpr friend bool operator<(const Vector2 &v1, const Vector2 &v2) { return (v1.x < v2.x || v1.y < v2.y);};
};

namespace Math {
    constexpr double PI = 3.14159265358979323846;
    constexpr float clamp(float min, float val, float max) {
        if (min > max) std::swap(min, max); // Flip if negative
        if (val < min) {return min;}
        if (val > max) {return max;}
        return val;
    }
    constexpr Vector2 clampVector2(Vector2 min, Vector2 val, Vector2 max) {
        return Vector2(clamp(min.x, val.x, max.x), clamp(min.y, val.y, max.y));
    }
    constexpr float wrap(float min, float val, float max) {
        if (min > max) std::swap(min, max); // Flip if negative
        float range = max - min;
        while (val < min) val += range;
        while (val >= max) val -= range;
        return val;
    }
    constexpr Vector2 wrapVector2(Vector2 min, Vector2 val, Vector2 max) {
        return Vector2(wrap(min.x, val.x, max.x), wrap(min.y, val.y, max.y));
    }
    constexpr float round(float val, float nearest) {
        return (int)(val / nearest) * nearest;
    }
    static float getRandomFloat(float min, float max) {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        std::uniform_real_distribution<float> dist(min, max);

        return dist(gen);
    }
    inline float lerp(float finalVal, float initVal, float time) {
        return (finalVal - initVal) / time;
    }
    inline Vector2 lerp(Vector2 finalVal, Vector2 initVal, float time) {
        return (finalVal - initVal) / time;
    }
    template<typename num> inline num powLITE(num x, int pow) { // Power function LITE
        num result = x * (pow != 0) + (pow == 0);
        for (int i=1; i<pow; i++) {
            result *= x;
        }
        return result;
    }

    template<typename num> inline num floorDecimal(num x) {
        return x - std::floor(x);
    }

    template<typename num, typename num2> inline auto maxLITE(num a, num2 b) {
        return (((a) > (b)) ? (a) : (b));
    }
}