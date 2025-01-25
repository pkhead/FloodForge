#pragma once
#include <iostream>
#include <cmath>
#include <string>

template <typename T>
struct BasicVector2 {
	T x;
	T y;

	BasicVector2() : x(0.0), y(0.0) {}
	BasicVector2(const T x, const T y) : x(x), y(y) {}
	BasicVector2(const BasicVector2<T> &vector) : x(vector.x), y(vector.y) {}
	BasicVector2(const BasicVector2<T> *vector) : x(vector->x), y(vector->y) {}

	void add(const T x, const T y) {
		this->x += x;
		this->y += y;
	}

	void add(const BasicVector2<T> &other) {
		this->x += other.x;
		this->y += other.y;
	}

	BasicVector2<T> &round() {
		x = ::round(x);
		y = ::round(y);
		return *this;
	}

	double distanceTo(const BasicVector2<T> &other) {
		return std::sqrt(pow((float)(x - other.x), 2) + pow((float)(y - other.y), 2));
	}

	BasicVector2<T> operator+(const BasicVector2<T> &other) const {
		return BasicVector2(this->x + other.x, this->y + other.y);
	}

	BasicVector2<T> operator-(const BasicVector2<T> &other) const {
		return BasicVector2(this->x - other.x, this->y - other.y);
	}

	void X(const T x) {
		this->x = x;
	}

	void Y(const T y) {
		this->y = y;
	}

	static BasicVector2 min(const BasicVector2<T> &a, const BasicVector2<T> &b) {
		return BasicVector2<T>(
			(a.x < b.x) ? a.x : b.x,
			(a.y < b.y) ? a.y : b.y
		);
	}

	static BasicVector2 max(const BasicVector2<T> &a, const BasicVector2<T> &b) {
		return BasicVector2<T>(
			(a.x > b.x) ? a.x : b.x,
			(a.y > b.y) ? a.y : b.y
		);
	}

	inline operator BasicVector2<float>() const noexcept {
		return BasicVector2<float>((float)x, (float)y);
	}

	inline operator BasicVector2<double>() const noexcept {
		return BasicVector2<double>((double)x, (double)y);
	}

	inline explicit operator BasicVector2<int>() const noexcept {
		return BasicVector2<int>((int)x, (int)y);
	}
};

using Vector2 = BasicVector2<double>;
using Vector2f = BasicVector2<float>;
using Vector2i = BasicVector2<int>;

struct Vector3f {
    float x, y, z;

    inline constexpr Vector3f() noexcept : x(0.0f), y(0.0f), z(0.0f) {}
    inline constexpr Vector3f(float x, float y, float z) noexcept : x(x), y(y), z(z) {}

    inline constexpr float lengthSq() const { return x * x + y * y + z * z; }
    inline float length() const { return sqrtf(lengthSq()); }

    inline Vector3f normalized() const {
        float len = length();
        return Vector3f(x / len, y / len, z / len);
    }

    inline constexpr float dot(const Vector3f &other) const {
        return x * other.x + y * other.y + z * other.z;
    }

    inline constexpr Vector3f cross(const Vector3f &other) const {
        return Vector3f(
            y * other.z - z * other.y,
            z * other.x - x * other.z,
            x * other.y - y * other.x
        );
    }
};

// Vector2
std::ostream &operator<<(std::ostream &stream, Vector2 &obj);

template <typename T>
bool operator==(const BasicVector2<T> &lhs, const BasicVector2<T> &rhs) {
	return (lhs.x == rhs.x) && (lhs.y == rhs.y);
}

template <typename Tv, typename Ts>
Vector2 operator*(const BasicVector2<Tv> &lhs, const Ts &rhs) {
	return Vector2(lhs.x * rhs, lhs.y * rhs);
}

template <typename Ts, typename Tv>
Vector2 operator*(const Ts &lhs, const BasicVector2<Tv> &rhs) {
	return Vector2(lhs * rhs.x, lhs * rhs.y);
}

template <typename T>
std::string to_string(const BasicVector2<T> &vector) {
	return "(" + std::to_string(vector.x) + ", " + std::to_string(vector.y) + ")";
}

template <typename T>
std::ostream &operator<<(std::ostream &stream, BasicVector2<T> &obj) {
	return stream << "(" << obj.x << ", " << obj.y << ")";
}

template <typename T>
void operator+=(BasicVector2<T> &lhs, const BasicVector2<T> &rhs) {
	lhs.x += rhs.x;
	lhs.y += rhs.y;
}

// Vector3f
std::ostream &operator<<(std::ostream &stream, Vector3f &vec);

inline constexpr bool operator==(const Vector3f &a, const Vector3f &b) {
    return a.x == b.x && a.y == b.y && a.z == b.z;
}

inline constexpr Vector3f operator+(const Vector3f &a, const Vector3f &b) {
    return Vector3f(a.x + b.x, a.y + b.y, a.z + b.z);
}

inline constexpr Vector3f operator-(const Vector3f &a, const Vector3f &b) {
    return Vector3f(a.x - b.x, a.y - b.y, a.z - b.z);
}

template <typename T>
inline constexpr Vector3f operator*(const Vector3f &v, const T s) {
    return Vector3f(v.x * s, v.y * s, v.z * s);
}

template <typename T>
inline constexpr Vector3f operator*(const T s, const Vector3f &v) {
    return Vector3f(v.x * s, v.y * s, v.z * s);
}

template <typename T>
inline constexpr Vector3f operator/(const Vector3f &v, const T s) {
    return Vector3f(v.x / s, v.y / s, v.z / s);
}

template <typename T>
inline constexpr Vector3f operator/(const T s, const Vector3f &v) {
    return Vector3f(v.x / s, v.y / s, v.z / s);
}