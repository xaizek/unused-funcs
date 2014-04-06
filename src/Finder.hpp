/*
 * unused-funcs
 *
 * Copyright (C) 2014 xaizek.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA
 */

#ifndef UNUSED_FUNCS__FINDER_HPP__
#define UNUSED_FUNCS__FINDER_HPP__

#include <memory>

namespace clang
{
    namespace ast_matchers
    {
        class MatchFinder;
    }
}

class Finder
{
    typedef clang::ast_matchers::MatchFinder MatchFinder;

public:
    Finder();
    ~Finder();

public:
    MatchFinder & getMatchFinder();

private:
    // these operations are forbidden
    Finder(const Finder &rhs);
    Finder & operator=(const Finder &rhs);

private:
    class Impl;

    const std::auto_ptr<Impl> impl;
};

#endif // UNUSED_FUNCS__FINDER_HPP__
