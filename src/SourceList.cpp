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

#include <glm/gtx/vector_angle.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include "Source.h"
#include "Session.h"

#include "SourceList.h"

// utility to sort Sources by depth
bool compare_depth (Source * first, Source * second)
{
  return ( first->depth() < second->depth() );
}


bool notplayable (const Source *s) { return !s->playable(); }

SourceList playable_only (const SourceList &list)
{
    SourceList pl = list;

    pl.remove_if(notplayable);

    return pl;
}


bool isfailed (const Source *s) { return s->failed() != Source::FAIL_NONE; }

SourceList valid_only (const SourceList &list)
{
    SourceList pl = list;

    pl.remove_if(isfailed);

    return pl;
}

bool notactive (const Source *s) { return !s->active(); }

SourceList active_only (const SourceList &list)
{
    SourceList pl = list;

    pl.remove_if(notactive);

    return pl;
}

SourceList depth_sorted(const SourceList &list)
{
    SourceList sl = list;
    sl.sort(compare_depth);

    return sl;
}

// utility to sort Sources in MixingView in a clockwise order
// in reference to a center point
struct clockwise_centered {
    explicit clockwise_centered(glm::vec2 c) : center(c) { }
    bool operator() (Source * first, Source * second) {
        glm::vec2 pos_first = glm::vec2(first->group(View::MIXING)->translation_)-center;
        float angle_first = glm::orientedAngle( glm::normalize(pos_first), glm::vec2(1.f, 0.f) );
        glm::vec2 pos_second = glm::vec2(second->group(View::MIXING)->translation_)-center;
        float angle_second = glm::orientedAngle( glm::normalize(pos_second), glm::vec2(1.f, 0.f) );
        return (angle_first < angle_second);
    }
    glm::vec2 center;
};

SourceList mixing_sorted(const SourceList &list, glm::vec2 center)
{
    SourceList sl = list;
    sl.sort(clockwise_centered(center));

    return sl;
}


SourceIdList ids (const SourceList &list)
{
    SourceIdList idlist;

    for( auto sit = list.begin(); sit != list.end(); ++sit)
        idlist.push_back( (*sit)->id() );

    // make sure no duplicate
    idlist.unique();

    return idlist;
}


SourceListCompare compare (const SourceList &first, const SourceList &second)
{
    SourceListCompare ret = SOURCELIST_DISTINCT;
    if (first.empty() || second.empty())
        return ret;

    // a new test list: start with the second list and remove all commons with first list
    SourceList test = second;
    for (auto it = first.begin(); it != first.end(); ++it){
        test.remove(*it);
    }

    // all sources of the second list were in the first list
    if (test.empty()) {
        // same size, therefore they are the same!
        if (first.size() == second.size())
            ret = SOURCELIST_EQUAL;
        // otherwise, first list contains all sources of the second list.
        else
            ret = SOURCELIST_SECOND_IN_FIRST;
    }
    // some sources of the second list were in the first
    else if ( second.size() != test.size() ){
        // if the number of sources removed from second is the number of sources in the first
        if (second.size() - test.size() == first.size())
            ret = SOURCELIST_FIRST_IN_SECOND;
        // else, there is a patrial intersection
        else
            ret = SOURCELIST_INTERSECT;
    }
    // else no intersection, lists are distinct (return detault)

    return ret;
}


SourceList intersect (const SourceList &first, const SourceList &second)
{
    // take second list and remove all elements also in first list
    // -> builds the list of what remains in second list
    SourceList l1 = second;
    for (auto it = first.begin(); it != first.end(); ++it)
        l1.remove(*it);
    // take second list and remove all elements in the remainer list
    // -> builds the list of what is in second list and was part of the first list
    SourceList l2 = second;
    for (auto it = l1.begin(); it != l1.end(); ++it)
        l2.remove(*it);
    return l2;
}


SourceList join (const SourceList &first, const SourceList &second)
{
    SourceList l = second;
    for (auto it = first.begin(); it != first.end(); ++it)
        l.push_back(*it);
    l.unique();
    return l;
}

SourceLink::SourceLink(uint64_t id, Session *se): host_(nullptr), target_(nullptr), id_(0)
{
    connect(id, se);
}

SourceLink::SourceLink(Source *s): host_(nullptr), target_(nullptr), id_(0)
{
    connect(s);
}

SourceLink::SourceLink(const SourceLink &l)
{
    if (l.target_ != nullptr)
        connect(l.target_);
    else
        host_ = l.host_;
    id_ = l.id_;
}

void SourceLink::connect(uint64_t id, Session *se)
{
    if (connected())
        disconnect();

    if (se != nullptr && id > 0) {
        id_ = id;
        host_ = se;
    }
}

void SourceLink::connect(Source *s)
{
    if (connected())
        disconnect();

    if (s != nullptr) {
        target_ = s;
        target_->links_.push_back(this);

        id_ = s->id();
        // TODO veryfy circular dependency recursively ?
    }
}

void SourceLink::disconnect()
{
    if (target_)
        target_->links_.remove(this);
    target_ = nullptr;

    id_ = 0;
    host_ = nullptr;
}

SourceLink::~SourceLink()
{
    disconnect();
}

Source *SourceLink::source()
{
    // no link to pointer yet?
    if ( target_ == nullptr ) {
        // to find a source, we need a host and an id
        if ( id_ > 0 && host_ != nullptr) {
            // find target in session
            SourceList::iterator it = host_->find(id_);
            // found: keep pointer
            if (it != host_->end()) {
                target_ = *it;
                target_->links_.push_back(this);
            }
        }
        // no host: invalidate link
        else
            disconnect();
    }

    return target_;
}


SourceList validateLinkList (const SourceLinkList &list)
{
    SourceList sourcelist;

    for( auto sit = list.begin(); sit != list.end(); ++sit) {

        Source *s = (*sit)->source();
        if (s)
            sourcelist.push_back( s );

    }

    // make sure no duplicate
    sourcelist.unique();

    return sourcelist;
}


SourceLinkList getLinkList (const SourceList &list)
{
    SourceLinkList linklist;

    for( auto sit = list.begin(); sit != list.end(); ++sit)
        linklist.push_back( new SourceLink(*sit) );

    return linklist;
}


void clearLinkList (SourceLinkList list)
{
    for( auto sit = list.begin(); sit != list.end(); ) {
        delete (*sit);
        sit = list.erase(sit);
    }
}
