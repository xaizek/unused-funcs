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
#include <clang/ASTMatchers/ASTMatchers.h>

#include "FuncInfo.hpp"

namespace {

using namespace clang;
using namespace clang::ast_matchers;

using Funcs = std::map<std::string, FuncInfo>;

class MatchHelper : public MatchFinder::MatchCallback {
  using Result = MatchFinder::MatchResult;

public:
  MatchHelper(Funcs &funcs);

  void run(const Result &result) override;

private:
  Funcs::iterator registerFunc(const Result &result,
                               const FunctionDecl *func) const;

  void registerRef(const Result &result, const DeclRefExpr *ref) const;

  Funcs &funcs;
};

MatchHelper::MatchHelper(Funcs &funcs)
    : funcs(funcs) {}

void MatchHelper::run(const Result &result) {
  using Func = FunctionDecl;
  using Ref = DeclRefExpr;

  if (const auto func = result.Nodes.getNodeAs<Func>("func")) {
    static_cast<void>(registerFunc(result, func));
  } else if (const auto ref = result.Nodes.getNodeAs<Ref>("ref")) {
    registerRef(result, ref);
  }
}

Funcs::iterator MatchHelper::registerFunc(const Result &result,
                                          const FunctionDecl *func) const {
  if (!func->isExternallyVisible() || func->isMain()) {
    return {};
  }

  const auto it = funcs.find(func->getNameAsString());
  if (it == funcs.end()) {
    const auto name = func->getNameAsString();
    FuncInfo info(func, result.SourceManager);
    return funcs.insert(std::make_pair(name, info)).first;
  }
  it->second.processDeclaration(func, result.SourceManager);
  return it;
}

void MatchHelper::registerRef(const Result &result,
                              const DeclRefExpr *ref) const {
  if (const auto func = ref->getDecl()->getAsFunction()) {
    const auto it = registerFunc(result, func);
    if (it != Funcs::iterator()) {
      it->second.registerRef(ref, result.SourceManager);
    }
  }
}

} // namespace

class Finder::Impl {
public:
  Impl();
  ~Impl();

public:
  MatchFinder &getMatchFinder();

private:
  Funcs funcs;
  MatchHelper helper;
  MatchFinder matchFinder;
};

Finder::Impl::Impl() : helper(funcs) {
  const auto funcDecl = functionDecl().bind("func");
  matchFinder.addMatcher(funcDecl, &helper);
  const auto funcRef = declRefExpr(to(functionDecl())).bind("ref");
  matchFinder.addMatcher(funcRef, &helper);
}

Finder::Impl::~Impl() {
  for (auto & func : funcs) {
    const auto &funcInfo = func.second;

    if (funcInfo.isFullyDeclared()) {
      if (funcInfo.isUnused()) {
        std::cout << funcInfo << ": unused\n";
      } else if (funcInfo.canBeMadeStatic()) {
        std::cout << funcInfo << ": can be made static\n";
      }
    }
  }
}

MatchFinder &Finder::Impl::getMatchFinder() { return matchFinder; }

Finder::Finder() : impl(llvm::make_unique<Impl>()) {}
Finder::~Finder() = default;

MatchFinder &Finder::getMatchFinder() { return impl->getMatchFinder(); }
