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

#include <map>
#include <string>

#include <clang/AST/Decl.h>
#include <clang/ASTMatchers/ASTMatchFinder.h>
#include <clang/ASTMatchers/ASTMatchers.h>
#include <clang/Basic/SourceManager.h>

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
  Funcs::iterator registerFunc(const FunctionDecl &func,
                               const SourceManager &sm) const;
  void registerRef(const DeclRefExpr &ref,
                   const SourceManager &sm) const;

  Funcs &funcs;
};

MatchHelper::MatchHelper(Funcs &funcs)
    : funcs(funcs) {}

void MatchHelper::run(const Result &result) {
  using Func = FunctionDecl;
  using Ref = DeclRefExpr;

  if (const auto func = result.Nodes.getNodeAs<Func>("func")) {
    static_cast<void>(registerFunc(*func, *result.SourceManager));
  } else if (const auto ref = result.Nodes.getNodeAs<Ref>("ref")) {
    registerRef(*ref, *result.SourceManager);
  }
}

Funcs::iterator MatchHelper::registerFunc(const FunctionDecl &func,
                                          const SourceManager &sm) const {
  if (!func.isExternallyVisible() || func.isMain()) {
    return {};
  }

  const auto name = func.getNameAsString();
  const auto it = funcs.find(name);
  if (it == funcs.end()) {
    FuncInfo info(func, sm);
    return funcs.emplace(name, info).first;
  }
  it->second.processDeclaration(func, sm);
  return it;
}

void MatchHelper::registerRef(const DeclRefExpr &ref,
                              const SourceManager &sm) const {
  if (const auto func = ref.getDecl()->getAsFunction()) {
    const auto it = registerFunc(*func, sm);
    if (it != Funcs::iterator()) {
      it->second.registerRef(ref, sm);
    }
  }
}

}  // namespace

class Finder::Impl {
public:
  Impl();
  ~Impl();

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
        llvm::outs() << funcInfo << ": unused\n";
      } else if (funcInfo.canBeMadeStatic()) {
        llvm::outs() << funcInfo << ": can be made static\n";
      }
    }
  }
}

MatchFinder &Finder::Impl::getMatchFinder() { return matchFinder; }

Finder::Finder() : impl(llvm::make_unique<Impl>()) {}
Finder::~Finder() = default;

MatchFinder &Finder::getMatchFinder() { return impl->getMatchFinder(); }
