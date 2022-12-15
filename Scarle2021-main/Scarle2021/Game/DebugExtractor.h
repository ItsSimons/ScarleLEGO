#pragma once

#include "q3.h"
#include <SimpleMath.h>

//This is an adapation of a OpenGL debug render library into DirectX
//Takes vertices of where the hitbox of the elements should be and 
//draws a dube on top of them 
class DebugExtractor : public q3Render
{
public:
    void SetPenColor(f32 r, f32 g, f32 b, f32 a = 1.0f) override
    {
        Q3_UNUSED(a);

        color = Color(r, g, b);
    }

    void SetPenPosition(f32 x, f32 y, f32 z) override
    {
        x_ = x, y_ = y, z_ = z;
    }

    void SetScale(f32 sx, f32 sy, f32 sz) override
    {
        sx_ = sx, sy_ = sy, sz_ = sz;
    }

    void Line(f32 x, f32 y, f32 z) override
    {
        //Gets the position for extents on a line
        debug_cube_locations.emplace_back(Vector3((float)x_, (float)y_, (float)z_));
        debug_cube_locations.emplace_back(Vector3((float)x, (float)y, (float)z));
        SetPenPosition(x, y, z);
    }

    void Triangle(
        f32 x1, f32 y1, f32 z1,
        f32 x2, f32 y2, f32 z2,
        f32 x3, f32 y3, f32 z3
    ) override
    {
        //Gets the position extents on a triangle
        debug_cube_locations.emplace_back(Vector3((float)x1, (float)y1, (float)z1));
        debug_cube_locations.emplace_back(Vector3((float)x2, (float)y2, (float)z2));
        debug_cube_locations.emplace_back(Vector3((float)x3, (float)y3, (float)z3));
    }

    void SetTriNormal(f32 x, f32 y, f32 z) override
    {
        nx_ = x;
        ny_ = y;
        nz_ = z;
    }

    void Point() override
    {
        //Gets the position extents on a single line
        debug_cube_locations.emplace_back(Vector3((float)x_, (float)y_, (float)z_));
    };

    std::vector<Vector3>& GetDebugCubes() 
    {
        return debug_cube_locations;
    }

private:
    Color color;
    f32 x_, y_, z_;
    f32 sx_, sy_, sz_;
    f32 nx_, ny_, nz_;

    //Position to move cubes
    std::vector<Vector3> debug_cube_locations;
};