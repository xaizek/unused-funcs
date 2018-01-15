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

#ifndef UNUSED_FUNCS__FUNCINFO_HPP__
#define UNUSED_FUNCS__FUNCINFO_HPP__

#include <llvm/Support/raw_ostream.h>

#include <string>
#include <vector>

#include "RefInfo.hpp"


namespace clang {
class FunctionDecl;
class DeclRefExpr;
class SourceManager;
} // namespace clang


class FuncInfo {
  friend llvm::raw_ostream &operator<<(llvm::raw_ostream &os, const FuncInfo &fi);

public:
  FuncInfo(const clang::FunctionDecl &func, const clang::SourceManager &sm);

  void processDeclaration(const clang::FunctionDecl &func,
                          const clang::SourceManager &sm);
  bool isFullyDeclared() const;
  void registerRef(const clang::DeclRefExpr &ref,
                   const clang::SourceManager &sm);
  bool isUnused() const;
  bool canBeMadeStatic() const;

private:
  const std::string name;
  std::string fileName;
  unsigned int lineNum {0};
  typedef std::vector<RefInfo> Refs;
  Refs calls;
};

llvm::raw_ostream &operator<<(llvm::raw_ostream &os, const FuncInfo &fi);

#endif // UNUSED_FUNCS__FUNCINFO_HPP__
