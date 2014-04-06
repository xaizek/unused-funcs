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

#include "Finder.hpp"

#include <iostream>
#include <map>
#include <string>

#include <clang/AST/Decl.h>

#include <clang/ASTMatchers/ASTMatchFinder.h>

#include "FuncInfo.hpp"

using namespace clang;
using namespace clang::ast_matchers;

typedef std::map<std::string, FuncInfo> Funcs;

static DeclarationMatcher funcDecl = functionDecl().bind("func");
static StatementMatcher funcRef =
    declRefExpr(              // referencing a variable/declaration
        to(                   // something that is ...
            functionDecl(     // ... a function

            )
        )
    ).bind("ref");            // bind matched function ref to "ref" name

namespace
{

class MatchHelper : public MatchFinder::MatchCallback
{
    typedef MatchFinder::MatchResult Result;

public:
    MatchHelper(std::map<std::string, FuncInfo> &funcs);

public:
    virtual void run(const Result &result);

private:
    Funcs::iterator registerFunc(const Result &result,
                                 const FunctionDecl *func) const;

    void registerRef(const Result &result, const DeclRefExpr *ref) const;

private:
    Funcs &funcs;
};

MatchHelper::MatchHelper(std::map<std::string, FuncInfo> &funcs)
    :funcs(funcs)
{
}

void
MatchHelper::run(const Result &result)
{
    typedef FunctionDecl Func;
    typedef DeclRefExpr Ref;

    if (const Func *func = result.Nodes.getNodeAs<Func>("func")) {
        static_cast<void>(registerFunc(result, func));
    } else if (const Ref *ref = result.Nodes.getNodeAs<Ref>("ref")) {
        registerRef(result, ref);
    }
}

Funcs::iterator
MatchHelper::registerFunc(const Result &result, const FunctionDecl *func) const
{
    if (!func->isExternallyVisible() || func->isMain()) {
        return Funcs::iterator();
    }

    const Funcs::iterator it = funcs.find(func->getNameAsString());
    if (it == funcs.end()) {
        const std::string &name = func->getNameAsString();
        FuncInfo info(func, result.SourceManager);
        return funcs.insert(std::make_pair(name, info)).first;
    } else {
        it->second.processDeclaration(func, result.SourceManager);
        return it;
    }
}

void
MatchHelper::registerRef(const Result &result, const DeclRefExpr *ref) const
{
    if (const FunctionDecl *func = ref->getDecl()->getAsFunction()) {
        const Funcs::iterator it = registerFunc(result, func);
        if (it != Funcs::iterator()) {
            it->second.registerRef(ref, result.SourceManager);
        }
    }
}

}

class Finder::Impl
{
public:
    Impl();
    ~Impl();

public:
    MatchFinder & getMatchFinder();

private:
    Funcs funcs;
    MatchHelper helper;
    MatchFinder matchFinder;
};

Finder::Impl::Impl()
    :helper(funcs)
{
    matchFinder.addMatcher(funcDecl, &helper);
    matchFinder.addMatcher(funcRef, &helper);
}

Finder::Impl::~Impl()
{
    for (Funcs::const_iterator cit = funcs.begin(); cit != funcs.end(); ++cit) {
        const FuncInfo &funcInfo = cit->second;

        if (funcInfo.isFullyDeclared()) {
            if (funcInfo.isUnused()) {
                std::cout << funcInfo << ":unused\n";
            } else if (funcInfo.canBeMadeStatic()) {
                std::cout << funcInfo << ":can be made static\n";
            }
        }
    }
}

Finder::MatchFinder &
Finder::Impl::getMatchFinder()
{
    return matchFinder;
}

Finder::Finder()
    :impl(new Impl())
{
}

Finder::~Finder()
{
}

Finder::MatchFinder &
Finder::getMatchFinder()
{
    return impl->getMatchFinder();
}
