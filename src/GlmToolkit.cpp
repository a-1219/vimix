/*
 * This file is part of vimix - video live mixer
 *
 * **Copyright** (C) 2019-2023 Bruno Herbelin <bruno.herbelin@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
**/

#include <ctime>

#include <glm/gtc/matrix_access.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/random.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/vector_angle.hpp>

#include "GlmToolkit.h"


glm::mat4 GlmToolkit::transform(glm::vec3 translation, glm::vec3 rotation, glm::vec3 scale)
{
    glm::mat4 View = glm::translate(glm::identity<glm::mat4>(), translation);
    View = glm::rotate(View, rotation.x, glm::vec3(1.f, 0.f, 0.f));
    View = glm::rotate(View, rotation.y, glm::vec3(0.f, 1.f, 0.f));
    View = glm::rotate(View, rotation.z, glm::vec3(0.f, 0.f, 1.f));
    glm::mat4 Model = glm::scale(glm::identity<glm::mat4>(), scale);
    return View * Model;
}

void GlmToolkit::inverse_transform(glm::mat4 M, glm::vec3 &translation, glm::vec3 &rotation, glm::vec3 &scale)
{
    // extract rotation from modelview
    glm::mat4 ctm;
    glm::vec3 rot(0.f);
    glm::vec4 vec = M * glm::vec4(1.f, 0.f, 0.f, 0.f);
    rot.z = glm::orientedAngle( glm::vec3(1.f, 0.f, 0.f), glm::normalize(glm::vec3(vec)), glm::vec3(0.f, 0.f, 1.f) );
    rotation = rot;

    // extract scaling
    ctm = glm::rotate(glm::identity<glm::mat4>(), -rot.z, glm::vec3(0.f, 0.f, 1.f)) * M ;
    vec = ctm *  glm::vec4(1.f, 1.f, 0.f, 0.f);
    scale = glm::vec3(vec.x, vec.y, 1.f);

    // extract translation
    vec = M * glm::vec4(0.f, 0.f, 0.f, 1.f);
    translation = glm::vec3(vec);
}

//float rewrapAngleRestricted(float angle)
//// This function takes an angle in the range [-3*pi, 3*pi] and
//// wraps it to the range [-pi, pi].
//{
//    if (angle > glm::pi<float>() )
//        return angle - glm::two_pi<float>();
//    else if (angle < - glm::pi<float>())
//        return angle + glm::two_pi<float>();
//    else
//        return angle;
//}

// Freely inspired from https://github.com/alter-rokuz/glm-aabb.git

GlmToolkit::AxisAlignedBoundingBox::AxisAlignedBoundingBox() :
    mMin(glm::vec3(1.f)), mMax(glm::vec3(-1.f))
{
}

GlmToolkit::AxisAlignedBoundingBox::AxisAlignedBoundingBox(const GlmToolkit::AxisAlignedBoundingBox &D) :
    mMin(D.mMin), mMax(D.mMax)
{
}

void GlmToolkit::AxisAlignedBoundingBox::extend(const glm::vec3& point)
{
    if (isNull()) {
        mMin = point;
        mMax = point;
    }
    // general case
    else  {
        mMin = glm::min(point, mMin);
        mMax = glm::max(point, mMax);
    }
}

void GlmToolkit::AxisAlignedBoundingBox::extend(std::vector<glm::vec3> points)
{
    for (auto p = points.begin(); p != points.end(); ++p)
        extend(*p);
}


void GlmToolkit::AxisAlignedBoundingBox::extend(const AxisAlignedBoundingBox& bb)
{
    if (bb.isNull())
        return;

    if (isNull()) {
        mMin = bb.mMin;
        mMax = bb.mMax;
    }
    // general case
    else {
        mMin = glm::min(bb.mMin, mMin);
        mMax = glm::max(bb.mMax, mMax);
    }
}

glm::vec3 GlmToolkit::AxisAlignedBoundingBox::center(bool ignore_z) const
{
    glm::vec3 ret = glm::vec3(0.f);

    if (!isNull())
    {
      glm::vec3 d = mMax - mMin;
      ret = mMin + (d * 0.5f);
    }

    if (ignore_z)
        ret.z = 0.f;

    return ret;
}

glm::vec3 GlmToolkit::AxisAlignedBoundingBox::scale(bool ignore_z) const
{
    glm::vec3 ret = glm::vec3(1.f);

    if (!isNull())
    {
      glm::vec3 d = mMax - mMin;
      ret = d * 0.5f;
    }

    if (ignore_z)
        ret.z = 1.f;

    return ret;
}

bool GlmToolkit::AxisAlignedBoundingBox::intersect(const AxisAlignedBoundingBox& bb, bool ignore_z) const
{
    if (isNull() || bb.isNull())
        return false;

    if (    (mMax.x < bb.mMin.x) || (mMin.x > bb.mMax.x) ||
            (mMax.y < bb.mMin.y) || (mMin.y > bb.mMax.y) ||
            ( !ignore_z && ((mMax.z < bb.mMin.z) || (mMin.z > bb.mMax.z)) ) )
    {
        return false;
    }

    return true;
}

bool GlmToolkit::AxisAlignedBoundingBox::contains(const AxisAlignedBoundingBox& bb, bool ignore_z) const
{
    if ( !intersect(bb, ignore_z))
        return false;

    if (    (mMin.x < bb.mMin.x) && (mMax.x > bb.mMax.x) &&
            (mMin.y < bb.mMin.y) && (mMax.y > bb.mMax.y)
            && ( ignore_z || ((mMin.z < bb.mMin.z) && (mMax.z > bb.mMax.z)) )
            )
    {
        return true;
    }

    return false;
}

bool GlmToolkit::AxisAlignedBoundingBox::contains(glm::vec3 point, bool ignore_z) const
{
    if (    (mMax.x < point.x) || (mMin.x > point.x) ||
            (mMax.y < point.y) || (mMin.y > point.y) ||
            ( !ignore_z && ((mMax.z < point.z) || (mMin.z > point.z)) ) )
        return false;

    return true;
}


GlmToolkit::AxisAlignedBoundingBox GlmToolkit::AxisAlignedBoundingBox::translated(glm::vec3 t) const
{
    GlmToolkit::AxisAlignedBoundingBox bb;
    bb = *this;

    bb.mMin += t;
    bb.mMax += t;

    return bb;
}

GlmToolkit::AxisAlignedBoundingBox GlmToolkit::AxisAlignedBoundingBox::scaled(glm::vec3 s) const
{
    GlmToolkit::AxisAlignedBoundingBox bb;    
    glm::vec3 vec;

    // Apply scaling to min & max corners (can be inverted) and update bbox accordingly
    vec = mMin * s;
    bb.extend(vec);

    vec = mMax * s;
    bb.extend(vec);

    return bb;
}

GlmToolkit::AxisAlignedBoundingBox GlmToolkit::AxisAlignedBoundingBox::transformed(glm::mat4 m) const
{
    GlmToolkit::AxisAlignedBoundingBox bb;
    glm::vec4 vec;

    // Apply transform to all four corners (can be rotated) and update bbox accordingly
    vec = m * glm::vec4(mMin.x, mMin.y, 0.f, 1.f);
    bb.extend(glm::vec3(vec));

    vec = m * glm::vec4(mMax.x, mMax.y, 0.f, 1.f);
    bb.extend(glm::vec3(vec));

    vec = m * glm::vec4(mMin.x, mMax.y, 0.f, 1.f);
    bb.extend(glm::vec3(vec));

    vec = m * glm::vec4(mMax.x, mMin.y, 0.f, 1.f);
    bb.extend(glm::vec3(vec));

    return bb;
}

float GlmToolkit::AxisAlignedBoundingBox::area() const
{
    if (isNull())
        return 0.f;

    return (mMax.x - mMin.x) * (mMax.y - mMin.y) ;
}

bool GlmToolkit::operator< (const GlmToolkit::AxisAlignedBoundingBox& A, const GlmToolkit::AxisAlignedBoundingBox& B )
{
    if (A.isNull())
        return true;
    if (B.isNull())
        return false;
    return ( glm::length2(A.mMax-A.mMin) < glm::length2(B.mMax-B.mMin) );
}

GlmToolkit::AxisAlignedBoundingBox& GlmToolkit::AxisAlignedBoundingBox::operator = (const GlmToolkit::AxisAlignedBoundingBox &D )
{
    if (this != &D) {
        this->mMin = D.mMin;
        this->mMax = D.mMax;
    }
    return *this;
}


glm::ivec2 GlmToolkit::resolutionFromDescription(int aspectratio, int height)
{
    int ar = glm::clamp(aspectratio, 0, 5);
    int h  = glm::clamp(height, 0, 8);

    static glm::vec2 aspect_ratio_size[6] = { glm::vec2(1.f,1.f), glm::vec2(4.f,3.f), glm::vec2(3.f,2.f), glm::vec2(16.f,10.f), glm::vec2(16.f,9.f), glm::vec2(21.f,9.f) };
    static float resolution_height[10] = { 16.f, 64.f, 200.f, 320.f, 480.f, 576.f, 720.f, 1080.f, 1440.f, 2160.f };

    float width = aspect_ratio_size[ar].x * resolution_height[h] / aspect_ratio_size[ar].y;
    glm::ivec2 res = glm::ivec2( width, resolution_height[h]);

    return res;
}
