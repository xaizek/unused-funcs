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

#include <iostream>

#include <llvm/Support/CommandLine.h>

#include <clang/ASTMatchers/ASTMatchFinder.h>
#include <clang/ASTMatchers/ASTMatchers.h>

#include <clang/Tooling/CommonOptionsParser.h>
#include <clang/Tooling/Tooling.h>

#include <clang/Basic/SourceLocation.h>
#include <clang/Basic/SourceManager.h>

using namespace clang;
using namespace clang::ast_matchers;
using namespace clang::tooling;

static llvm::cl::OptionCategory toolCategory("unused-funcs options");

static llvm::cl::extrahelp commonHelp(CommonOptionsParser::HelpMessage);

static DeclarationMatcher funcDecl = functionDecl().bind("func");
static StatementMatcher invocation = callExpr().bind("call");

class MatchHelper : public MatchFinder::MatchCallback
{
public:
    virtual void run(const MatchFinder::MatchResult &result)
    {
        using namespace clang;

        typedef FunctionDecl Func;
        typedef CallExpr Call;

        if (const Func *func = result.Nodes.getNodeAs<Func>("func")) {
            if (func->isExternallyVisible()) {
                printOut(result, func);
            }
        } else if (const Call *call = result.Nodes.getNodeAs<Call>("call")) {
            printOut(result, call);
        }
    }

private:
    void printOut(const MatchFinder::MatchResult &result,
                  const CallExpr *call) const
    {
        FullSourceLoc fullLoc(call->getLocStart(), *result.SourceManager);

        const std::string &fileName = result.SourceManager->getFilename(fullLoc);
        const unsigned int lineNum = fullLoc.getSpellingLineNumber();

        std::cout << fileName
                  << ":"
                  << lineNum
                  << ":call of "
                  << call->getDirectCallee()->getNameAsString()
                  << '\n';
    }

    void printOut(const MatchFinder::MatchResult &result,
                  const FunctionDecl *func) const
    {
        FullSourceLoc fullLoc(func->getLocStart(), *result.SourceManager);

        const std::string &fileName = result.SourceManager->getFilename(fullLoc);
        const unsigned int lineNum = fullLoc.getSpellingLineNumber();

        std::cout << fileName
                  << ":"
                  << lineNum
                  << ":declaration of "
                  << func->getNameAsString()
                  << '\n';
    }
};

int
main(int argc, const char *argv[])
{
    CommonOptionsParser optionsParser(argc, argv, toolCategory);
    ClangTool tool(optionsParser.getCompilations(),
                   optionsParser.getSourcePathList());

    MatchHelper helper;

    MatchFinder finder;
    finder.addMatcher(funcDecl, &helper);
    finder.addMatcher(invocation, &helper);

    return tool.run(newFrontendActionFactory(&finder));
}
