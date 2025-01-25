#ifndef MATH_UTILS_HPP
#define MATH_UTILS_HPP

namespace MathUtils {
	constexpr double PI = 3.14159265358979323846;
    constexpr float PIf = 3.14159265358979323846;

	static double lerp(double a, double b, double t) {
		return (b - a) * t + a;
	}

	inline constexpr float deg2rad(float deg) {
        return deg / 180 * PIf;
    }

    inline constexpr double deg2rad(double deg) {
        return deg / 180 * PI;
    }

    inline constexpr float rad2deg(float rad) {
        return rad / PIf * 180;
    }

    inline constexpr double rad2deg(double rad) {
        return rad / PI * 180;
    }
};

#endif