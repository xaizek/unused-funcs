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

#include <clang/ASTMatchers/ASTMatchFinder.h>
#include <clang/Basic/Diagnostic.h>
#include <clang/Frontend/FrontendActions.h>
#include <clang/Tooling/CommonOptionsParser.h>
#include <clang/Tooling/Tooling.h>
#include <llvm/Support/CommandLine.h>

#include "Finder.hpp"

namespace ct = clang::tooling;

namespace {
class CustomDiagnosticConsumer : public clang::DiagnosticConsumer {
public:
  bool IncludeInDiagnosticCounts() const override { return false; }
};
} // namespace

static llvm::cl::extrahelp commonHelp(ct::CommonOptionsParser::HelpMessage);

int main(int argc, const char *argv[]) {
  llvm::cl::OptionCategory toolCategory("unused-funcs options");
  ct::CommonOptionsParser optionsParser(argc, argv, toolCategory);
  ct::ClangTool tool(optionsParser.getCompilations(),
                     optionsParser.getSourcePathList());
  CustomDiagnosticConsumer diagConsumer;
  tool.setDiagnosticConsumer(&diagConsumer);

  Finder finder;
  return tool.run(ct::newFrontendActionFactory(&finder.getMatchFinder()).get());
}
