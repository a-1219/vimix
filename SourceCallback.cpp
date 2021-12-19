/*
 * This file is part of vimix - video live mixer
 *
 * **Copyright** (C) 2020-2021 Bruno Herbelin <bruno.herbelin@gmail.com>
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

#include "defines.h"
#include "Source.h"
#include "Log.h"

#include "SourceCallback.h"

SourceCallback::SourceCallback(): active_(true), finished_(false), initialized_(false)
{
}

SetAlpha::SetAlpha(float alpha) : SourceCallback(), alpha_(CLAMP(alpha, 0.f, 1.f))
{
    step_ = glm::normalize(glm::vec2(1.f, 1.f));   // step in diagonal by default
}

void SetAlpha::update(Source *s, float)
{
    if (s && !s->locked()) {
        // set start position on first run or upon call of reset()
        if (!initialized_){
            // initial position
            pos_ = glm::vec2(s->group(View::MIXING)->translation_);

            // step in direction of source translation if possible
            if ( glm::length(pos_) > DELTA_ALPHA)
                step_ = glm::normalize(pos_);

            initialized_ = true;
        }

        // perform operation
        float delta = SourceCore::alphaFromCordinates(pos_.x, pos_.y) - alpha_;

        // converge to reduce the difference of alpha using dichotomic algorithm
        if ( glm::abs(delta) > DELTA_ALPHA ){
            pos_ += step_ * (delta / 2.f);
            s->group(View::MIXING)->translation_ = glm::vec3(pos_, s->group(View::MIXING)->translation_.z);
        }
        // done
        else
            finished_ = true;
    }
    else
        finished_ = true;
}

SetDepth::SetDepth(float target, float duration) : SourceCallback(),
    duration_(duration), progress_(0.f), start_(0.f), target_(CLAMP(target, MIN_DEPTH, MAX_DEPTH))
{
}

void SetDepth::update(Source *s, float dt)
{
    if (s && !s->locked()) {
        // set start position on first run or upon call of reset()
        if (!initialized_){
            start_ = s->group(View::LAYER)->translation_.z;
            progress_ = 0.f;
            initialized_ = true;
        }

        // calculate amplitude of movement
        progress_ += dt / duration_;

        // perform movement
        s->group(View::LAYER)->translation_.z = start_ + progress_ * (target_ - start_);

        // end of movement
        if ( progress_ > 1.f ) {
            // apply depth to target
            s->group(View::LAYER)->translation_.z = target_;
            // ensure reordering of view
            ++View::need_deep_update_;
            // done
            finished_ = true;
        }
    }
    else
        finished_ = true;
}


SetPlay::SetPlay(bool on, float delay) : SourceCallback(), play_(on), delay_(delay), progress_(0.f)
{
}

void SetPlay::update(Source *s, float dt)
{
    if (s && s->playing() != play_) {
        // reset on first run or upon call of reset()
        if (!initialized_){
            progress_ = 0.f;
            initialized_ = true;
        }

        // increment time count
        progress_ += dt;

        // timeout
        if ( progress_ > delay_ ) {
            // call play function
            s->play(play_);
            // done
            finished_ = true;
        }
    }
    else
        finished_ = true;
}

Translation::Translation(float dx, float dy, float duration) : SourceCallback(), speed_(glm::vec2(dx,dy)),
    duration_(duration), progress_(0.f)
{
}

void Translation::update(Source *s, float dt)
{
    if (s && !s->locked()) {
        // reset on first run or upon call of reset()
        if (!initialized_){
            // start animation
            progress_ = 0.f;
            // initial position
            start_ = glm::vec2(s->group(View::GEOMETRY)->translation_);
            initialized_ = true;
        }

        // calculate amplitude of movement
        progress_ += dt / duration_;

        // perform movement
        glm::vec2 pos = start_ + speed_ * dt;
        s->group(View::GEOMETRY)->translation_ = glm::vec3(pos, s->group(View::GEOMETRY)->translation_.z);

        // timeout
        if ( progress_ > 1.f ) {
            // done
            finished_ = true;
        }
    }
    else
        finished_ = true;
}
