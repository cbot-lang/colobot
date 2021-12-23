/*
 * This file is part of the Colobot: Gold Edition source code
 * Copyright (C) 2001-2021, Daniel Roux, EPSITEC SA & TerranovaTeam
 * http://epsitec.ch; http://colobot.info; http://github.com/colobot
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see http://gnu.org/licenses
 */

/**
 * \file math/point.h
 * \brief Point struct and related functions
 */

#pragma once


#include "math/const.h"
#include "math/func.h"


#include <cmath>
#include <sstream>
#include <glm/glm.hpp>


// Math module namespace
namespace Math
{


// Temporary type alias
using Point = glm::vec2;


//! Returns a string "[x, y]"
inline std::string ToString(const Point& point)
{
    std::stringstream s;
    s.precision(3);
    s << "[" << point.x << ", " << point.y << "]";
    return s.str();
}

//! Checks if two vectors are equal within given \a tolerance
inline bool PointsEqual(const Point &a, const Point &b, float tolerance = TOLERANCE)
{
    return IsEqual(a.x, b.x, tolerance) && IsEqual(a.y, b.y, tolerance);
}

//! Permutes two points
inline void Swap(Point &a, Point &b)
{
    Point c;

    c = a;
    a = b;
    b = c;
}

//! Returns the distance between two points
inline float Distance(const Point &a, const Point &b)
{
    return sqrtf((a.x-b.x)*(a.x-b.x) + (a.y-b.y)*(a.y-b.y));
}


} // namespace Math

