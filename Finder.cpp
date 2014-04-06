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

#include <clang/Basic/SourceLocation.h>
#include <clang/Basic/SourceManager.h>

#include <clang/ASTMatchers/ASTMatchFinder.h>

using namespace clang::ast_matchers;

static DeclarationMatcher funcDecl = functionDecl().bind("func");
static StatementMatcher invocation = callExpr().bind("call");

namespace
{

class MatchHelper : public MatchFinder::MatchCallback
{
    typedef MatchFinder::MatchResult Result;

public:
    virtual void run(const Result &result);

private:
    void printOut(const Result &result, const clang::CallExpr *call) const;

    void printOut(const Result &result, const clang::FunctionDecl *func) const;
};

void
MatchHelper::run(const Result &result)
{
    typedef clang::FunctionDecl Func;
    typedef clang::CallExpr Call;

    if (const Func *func = result.Nodes.getNodeAs<Func>("func")) {
        if (func->isExternallyVisible()) {
            printOut(result, func);
        }
    } else if (const Call *call = result.Nodes.getNodeAs<Call>("call")) {
        printOut(result, call);
    }
}

void
MatchHelper::printOut(const Result &result,
                      const clang::CallExpr *call) const
{
    clang::FullSourceLoc fullLoc(call->getLocStart(), *result.SourceManager);

    const std::string &fileName = result.SourceManager->getFilename(fullLoc);
    const unsigned int lineNum = fullLoc.getSpellingLineNumber();

    std::cout << fileName
                << ":"
                << lineNum
                << ":call of "
                << call->getDirectCallee()->getNameAsString()
                << '\n';
}

void
MatchHelper::printOut(const Result &result,
                      const clang::FunctionDecl *func) const
{
    clang::FullSourceLoc fullLoc(func->getLocStart(), *result.SourceManager);

    const std::string &fileName = result.SourceManager->getFilename(fullLoc);
    const unsigned int lineNum = fullLoc.getSpellingLineNumber();

    std::cout << fileName
                << ":"
                << lineNum
                << ":declaration of "
                << func->getNameAsString()
                << '\n';
}

}

class Finder::Impl
{
public:
    Impl();

public:
    MatchFinder & getMatchFinder();

private:
    MatchHelper helper;
    MatchFinder matchFinder;
};

Finder::Impl::Impl()
{
    matchFinder.addMatcher(funcDecl, &helper);
    matchFinder.addMatcher(invocation, &helper);
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
