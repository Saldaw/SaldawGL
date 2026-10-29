#pragma once
#include <cmath>
#include <vector>
#include <iostream>
#include <algorithm>

const float PI = 3.1415927f;

template <class t> struct Vec2 {
    t x, y;
    Vec2() : x(0), y(0) {}
    Vec2(t x, t y) : x(x), y(y) {}
    Vec2 operator+(const Vec2& v) const { return Vec2(x + v.x, y + v.y); }
    Vec2 operator-(const Vec2& v) const { return Vec2(x - v.x, y - v.y); }
    Vec2 operator*(float f) const { return Vec2(x * f, y * f); }
    t operator[](int i) const { return i == 0 ? x : y; }
    t& operator[](int i) { return i == 0 ? x : y; }
};

template <class t> struct Vec3 {
    t x, y, z;
    Vec3() : x(0), y(0), z(0) {}
    Vec3(t x, t y, t z) : x(x), y(y), z(z) {}
    Vec3 operator+(const Vec3& v) const { return Vec3(x + v.x, y + v.y, z + v.z); }
    Vec3 operator-(const Vec3& v) const { return Vec3(x - v.x, y - v.y, z - v.z); }
    Vec3 operator*(float f) const { return Vec3(x * f, y * f, z * f); }
    float operator*(const Vec3& v) const { return x * v.x + y * v.y + z * v.z; }
    float norm() const { return std::sqrt(x * x + y * y + z * z); }
    Vec3 normalized() const { float n = norm(); return Vec3(x / n, y / n, z / n); }
    t operator[](int i) const { return i == 0 ? x : (i == 1 ? y : z); }
    t& operator[](int i) { return i == 0 ? x : (i == 1 ? y : z); }
};

template <class t> struct Vec4 {
    t x, y, z, w;
    Vec4() : x(0), y(0), z(0), w(0) {}
    Vec4(t x, t y, t z, t w) : x(x), y(y), z(z), w(w) {}
    Vec4 operator+(const Vec4& v) const { return Vec4(x + v.x, y + v.y, z + v.z, w + v.w); }
    Vec4 operator-(const Vec4& v) const { return Vec4(x - v.x, y - v.y, z - v.z, w - v.w); }
    Vec4 operator*(float f) const { return Vec4(x * f, y * f, z * f, w * f); }
    t operator[](int i) const { return i == 0 ? x : (i == 1 ? y : (i == 2 ? z : w)); }
    t& operator[](int i) { return i == 0 ? x : (i == 1 ? y : (i == 2 ? z : w)); }
};

typedef Vec2<float> Vec2f;
typedef Vec2<int> Vec2i;
typedef Vec3<float> Vec3f;
typedef Vec3<int> Vec3i;
typedef Vec4<float> Vec4f;

struct Matrix {
    float m[4][4];
    Matrix() { for (int i = 0; i < 4; i++) for (int j = 0; j < 4; j++) m[i][j] = (i == j); }

    Matrix operator*(const Matrix& a) const {
        Matrix r;
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                r.m[i][j] = 0;
                for (int k = 0; k < 4; k++) r.m[i][j] += m[i][k] * a.m[k][j];
            }
        }
        return r;
    }

    Vec4f operator*(const Vec4f& v) const {
        return Vec4f(
            m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z + m[0][3] * v.w,
            m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z + m[1][3] * v.w,
            m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z + m[2][3] * v.w,
            m[3][0] * v.x + m[3][1] * v.y + m[3][2] * v.z + m[3][3] * v.w
        );
    }
};

inline Vec3f cross(const Vec3f& v1, const Vec3f& v2) {
    return Vec3f(v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x);
}

inline Vec3f barycentric(Vec2f A, Vec2f B, Vec2f C, Vec2f P) {
    Vec3f s[2];
    s[0].x = C.x - A.x; s[0].y = B.x - A.x; s[0].z = A.x - P.x;
    s[1].x = C.y - A.y; s[1].y = B.y - A.y; s[1].z = A.y - P.y;
    Vec3f u = cross(s[0], s[1]);
    if (std::abs(u.z) > 1e-2) return Vec3f(1.f - (u.x + u.y) / u.z, u.y / u.z, u.x / u.z);
    return Vec3f(-1, 1, 1);
}

inline Matrix lookat(Vec3f eye, Vec3f center, Vec3f up) {
    Vec3f f = (center - eye).normalized();
    Vec3f u = up.normalized();
    Vec3f s = cross(f, u).normalized();
    u = cross(s, f);
    Matrix M;
    M.m[0][0] = s.x; M.m[0][1] = s.y; M.m[0][2] = s.z; M.m[0][3] = -(s * eye);
    M.m[1][0] = u.x; M.m[1][1] = u.y; M.m[1][2] = u.z; M.m[1][3] = -(u * eye);
    M.m[2][0] = -f.x; M.m[2][1] = -f.y; M.m[2][2] = -f.z; M.m[2][3] = (f * eye);
    return M;
}

inline Matrix perspective(float fov, float aspect, float near, float far) {
    Matrix M;
    float f = 1.0f / std::tan(fov * PI / 360.0f);
    M.m[0][0] = f / aspect;
    M.m[1][1] = f;
    M.m[2][2] = (far + near) / (near - far);
    M.m[2][3] = (2 * far * near) / (near - far);
    M.m[3][2] = -1.0f;
    M.m[3][3] = 0.0f;
    return M;
}

inline Matrix viewport(int x, int y, int w, int h) {
    Matrix M;
    M.m[0][0] = w / 2.0f; M.m[0][3] = x + w / 2.0f;
    M.m[1][1] = h / 2.0f; M.m[1][3] = y + h / 2.0f;
    M.m[2][2] = 1.0f;     M.m[2][3] = 0.0f;
    return M;
}