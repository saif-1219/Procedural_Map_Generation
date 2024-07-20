#pragma once

#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <random>

// Class to represent 2D vectors
class Vector2 {
public:
    double x, y;

    Vector2(double x, double y) : x(x), y(y) {}

    double dot(const Vector2& other) const {
        return x * other.x + y * other.y;
    }
};

// Function to shuffle an array
void Shuffle(std::vector<int>& arrayToShuffle) {
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(arrayToShuffle.begin(), arrayToShuffle.end(), g);
}

// Function to create a permutation table
std::vector<int> MakePermutation() {
    std::vector<int> permutation(256);
    for (int i = 0; i < 256; ++i) {
        permutation[i] = i;
    }

    Shuffle(permutation);

    permutation.insert(permutation.end(), permutation.begin(), permutation.end());

    return permutation;
}

std::vector<int> Permutation = MakePermutation();

// Function to get a constant vector based on a value from the permutation table
Vector2 GetConstantVector(int v) {
    int h = v & 3;
    if (h == 0) {
        return Vector2(1.0, 1.0);
    }
    else if (h == 1) {
        return Vector2(-1.0, 1.0);
    }
    else if (h == 2) {
        return Vector2(-1.0, -1.0);
    }
    else {
        return Vector2(1.0, -1.0);
    }
}

// Function to perform the fade operation
double Fade(double t) {
    return ((6 * t - 15) * t + 10) * t * t * t;
}

// Function to linearly interpolate between two values
double Lerp(double t, double a1, double a2) {
    return a1 + t * (a2 - a1);
}

// Function to generate 2D noise
double Noise2D(double x, double y) {
    int X = static_cast<int>(std::floor(x)) & 255;
    int Y = static_cast<int>(std::floor(y)) & 255;

    double xf = x - std::floor(x);
    double yf = y - std::floor(y);

    Vector2 topRight(xf - 1.0, yf - 1.0);
    Vector2 topLeft(xf, yf - 1.0);
    Vector2 bottomRight(xf - 1.0, yf);
    Vector2 bottomLeft(xf, yf);

    int valueTopRight = Permutation[Permutation[X + 1] + Y + 1];
    int valueTopLeft = Permutation[Permutation[X] + Y + 1];
    int valueBottomRight = Permutation[Permutation[X + 1] + Y];
    int valueBottomLeft = Permutation[Permutation[X] + Y];

    double dotTopRight = topRight.dot(GetConstantVector(valueTopRight));
    double dotTopLeft = topLeft.dot(GetConstantVector(valueTopLeft));
    double dotBottomRight = bottomRight.dot(GetConstantVector(valueBottomRight));
    double dotBottomLeft = bottomLeft.dot(GetConstantVector(valueBottomLeft));

    double u = Fade(xf);
    double v = Fade(yf);

    return Lerp(u, Lerp(v, dotBottomLeft, dotTopLeft), Lerp(v, dotBottomRight, dotTopRight));
}


