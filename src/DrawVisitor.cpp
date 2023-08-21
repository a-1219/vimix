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

#include <algorithm>

#include <glm/gtc/matrix_transform.hpp>

#include "Decorations.h"
#include "defines.h"
#include "Scene.h"
#include "Primitives.h"

#include "DrawVisitor.h"


DrawVisitor::DrawVisitor(Node *nodetodraw, glm::mat4 projection, bool force): force_(force)
{
    targets_.push_back(nodetodraw);
    modelview_ = glm::identity<glm::mat4>();
    projection_ = projection;
    num_duplicat_ = 1;
    transform_duplicat_ = glm::identity<glm::mat4>();
}

DrawVisitor::DrawVisitor(const std::vector<Node *> &nodestodraw, glm::mat4 projection, bool force):
    modelview_(glm::identity<glm::mat4>()), projection_(projection), targets_(nodestodraw), force_(force),
    num_duplicat_(1), transform_duplicat_(glm::identity<glm::mat4>())
{

}

void DrawVisitor::loop(int num, glm::mat4 transform)
{
    num_duplicat_ = CLAMP(num, 1, 10000);
    transform_duplicat_ = transform;
}

void DrawVisitor::visit(Node &n)
{
    // force visible status if required
    bool v = n.visible_;
    if (force_)
        n.visible_ = true;

    // find the node with this id
    std::vector<Node *>::iterator it = std::find_if(targets_.begin(), targets_.end(), hasId(n.id()));

    // found this node in the list of targets: draw it
    if (it != targets_.end()) {

        targets_.erase(it);

        for (int i = 0; i < num_duplicat_; ++i) {
            // draw multiple copies if requested
            n.draw(modelview_, projection_);
            modelview_ *= transform_duplicat_;
        }
    }

    // restore visibility
    n.visible_ = v;

    if (targets_.empty()) return;

    // update transform
    modelview_ *= n.transform_;
}


void DrawVisitor::visit(Group &n)
{
    // no need to traverse deeper if this node was drawn already
    if (targets_.empty()) return;

    // traverse children
    glm::mat4 mv = modelview_;
    for (NodeSet::iterator node = n.begin(); !targets_.empty() && node != n.end(); ++node) {
        if ( (*node)->visible_ || force_)
            (*node)->accept(*this);
        modelview_ = mv;
    }
}

void DrawVisitor::visit(Scene &n)
{
    modelview_ = glm::identity<glm::mat4>();
    n.root()->accept(*this);
}

void DrawVisitor::visit(Switch &n)
{
    // no need to traverse deeper if this node was drawn already
    if (targets_.empty()) return;

    // traverse acive child
    glm::mat4 mv = modelview_;
    if ( n.activeChild()->visible_ || force_)
        n.activeChild()->accept(*this);
    modelview_ = mv;
}

void DrawVisitor::visit(Primitive &)
{
}



ColorVisitor::ColorVisitor(glm::vec4 color): color_(color)
{

}
void ColorVisitor::visit(Node &)
{
}

void ColorVisitor::visit(Group &n)
{
    // traverse children
    for (NodeSet::iterator node = n.begin(); node != n.end(); ++node) {
        (*node)->accept(*this);
    }
}

void ColorVisitor::visit(Scene &n)
{
    n.root()->accept(*this);
}

void ColorVisitor::visit(Switch &n)
{
    n.activeChild()->accept(*this);
}

void ColorVisitor::visit(Primitive &p)
{
    p.shader()->color = color_;
}

void ColorVisitor::visit(Frame &d)
{
    d.color = color_;
}

void ColorVisitor::visit(Handles &d)
{
    d.color = color_;
}

void ColorVisitor::visit(Symbol &d)
{
    d.color = color_;
}

void ColorVisitor::visit(Disk &d)
{
    d.color = color_;
}

void ColorVisitor::visit(Character &d)
{
    d.color = color_;
}




