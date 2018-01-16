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

#include "FuncInfo.hpp"

#include <clang/AST/Decl.h>
#include <clang/AST/Expr.h>
#include <clang/Basic/SourceLocation.h>
#include <clang/Basic/SourceManager.h>

FuncInfo::FuncInfo(const clang::FunctionDecl &func,
                   const clang::SourceManager &sm)
    : name(func.getNameAsString()) {
  processDeclaration(func, sm);
}

void FuncInfo::processDeclaration(const clang::FunctionDecl &func,
                                  const clang::SourceManager &sm) {
  if (isFullyDeclared() || !func.isThisDeclarationADefinition()) {
    return;
  }

  clang::FullSourceLoc fullLoc(func.getNameInfo().getBeginLoc(), sm);
  fileName = sm.getFilename(fullLoc);
  lineNum = fullLoc.getSpellingLineNumber();
}

bool FuncInfo::isFullyDeclared() const { return lineNum != 0U; }

void FuncInfo::registerRef(const clang::DeclRefExpr &ref,
                           const clang::SourceManager &sm) {
  calls.emplace_back(ref, sm);
}

bool FuncInfo::isUnused() const { return calls.empty(); }

bool FuncInfo::canBeMadeStatic() const {
  for (const auto &call : calls) {
    if (!call.isInThisUnit(fileName)) {
      return false;
    }
  }
  return true;
}

llvm::raw_ostream &operator<<(llvm::raw_ostream &os, const FuncInfo &fi) {
  return os << fi.fileName << ':' << fi.lineNum << ':' << fi.name;
}
