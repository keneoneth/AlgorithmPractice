//===- FullVisitor.cpp ---------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// Example clang plugin which simply prints the names of all the top-level decls
// in the input file.
//
//===----------------------------------------------------------------------===//

#include <iostream>

#include "clang/Frontend/FrontendPluginRegistry.h"
#include "clang/AST/AST.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Sema/Sema.h"
#include "llvm/Support/raw_ostream.h"

// #include "clang/AST/ASTMatchers.…h"


using namespace clang;

namespace {


// class VecCallback : public clang::ast_matchers::MatchFinder::MatchCallback {
// public:
//   virtual void
//   run(const clang::ast_matchers::MatchFinder::MatchResult &Result) final {
//     llvm::errs() << ".";
//     if (const auto *F =
//             Result.Nodes.getDeclAs<clang::FunctionDecl>(FunctionID)) {
//       const auto& SM = *Result.SourceManager;
//       const auto& Loc = F->getLocation();
//       llvm::errs() << SM.getFilename(Loc) << ":"
//                    << SM.getSpellingLineNumber(Loc) << ":"
//                    << SM.getSpellingColumnNumber(Loc) << "\n";
//     }
//   }
// };

struct Visitor : public RecursiveASTVisitor<Visitor> {
  const std::set<std::string> &ParsedTemplates;
  CompilerInstance &Instance;
  Visitor(const std::set<std::string> &ParsedTemplates, CompilerInstance &Instance)
      : ParsedTemplates(ParsedTemplates), Instance(Instance) {}

  bool VisitDecl(Decl *Declaration) {
    // For debugging, dumping the AST nodes will show which nodes are already
    // being visited.
    // Declaration->dump();
    auto fname = Instance.getSourceManager().getFilename(Declaration->getLocation()).data();
    if (fname!=nullptr && strcmp(fname,"try_llvm.cc")==0) {
      printf("@@ Declaration->getQualifiedNameAsString() %s\n",Declaration->getDeclKindName () );
      
      printf("@@ filename %s\n",fname);
      printf("@@ src location %s\n",Declaration->getLocation().printToString(Instance.getSourceManager()).c_str());
      // The return value indicates whether we want the visitation to proceed.
      // Return false to stop the traversal of the AST.
    }
    return true;
  }

  bool VisitStmt(Stmt *Statement) {

    auto fname = Instance.getSourceManager().getFilename(Statement->getBeginLoc()).data();
    if (fname!=nullptr && strcmp(fname,"try_llvm.cc")==0) {
      printf("@@ getStmtClassName %s\n",Statement->getStmtClassName());

      printf("@@ filename %s\n",fname);
      printf("@@ src location %s\n",Statement->getBeginLoc().printToString(Instance.getSourceManager()).c_str());
      
      if (strcmp(Statement->getStmtClassName(),"DeclRefExpr")==0) {
        DeclRefExpr * DRExpr = static_cast<DeclRefExpr*>(Statement);
        // getLHS getRHS
        QualType qtype = DRExpr->getType();
        printf("@@@ DRExpr Q getType %s\n", qtype.getAsString().c_str());
        printf("@@@ DRExpr Q customQualifier %d\n",qtype.customQualifier);
        // float *
        const Type * type = qtype.getTypePtrOrNull();
        if (type!=nullptr){
          const auto *A = dyn_cast<AttributedType>(type);
          if (A!=nullptr){
            auto attrkind = A->getAttrKind() ;
            printf("@@@@@ type attrkind: %d",attrkind);
          }
        }

        QualType pt_qtype = type->getPointeeType();
        
        printf("@@@@ DRExpr getType %s\n", pt_qtype.getAsString().c_str());
        printf("@@@@ DRExpr customQualifier %d\n",pt_qtype.customQualifier);

        // const Type * pt_type = pt_qtype.getTypePtrOrNull();

        if (pt_type!=nullptr){
          // const auto *A = dyn_cast<AttributedType>(pt_type);
          const auto *A = pt_type->getAs<AttributedType>();
          if (A!=nullptr){
            auto attrkind = A->getAttrKind();
            printf("@@@@@ attrkind: %d\n",attrkind);
            printf("@@@@@ attrkind==: %d\n",attrkind==attr::MyShared);
            // const AnnotateTypeAttr * pt_type = pt_qtype.getTypePtrOrNull();
            const auto *Annotate = dyn_cast<AnnotateTypeAttr>(AttributedTL.getAttr())
            if (pt_type!=nullptr){

               printf("@@@@@ annotationtype==: %s\n",pt_type->getAnnotation().data());
            }

            // TypeSourceInfo *DI = nullptr;
            // if (const LocInfoType *LIT = dyn_cast<LocInfoType>(QT)) {
            //   QT = LIT->getType();
            //   DI = LIT->getTypeSourceInfo();
            // }
           
          }
        }

      }
      


    }
    return true;

  }

  // bool VisitFunctionDecl(FunctionDecl *FD) {
    // if (FD->isLateTemplateParsed() &&
    //     ParsedTemplates.count(FD->getNameAsString()))
    //   LateParsedDecls.insert(FD);
  //   return true;
  // }

  // VisitDeclRefExpr ?

  // std::set<FunctionDecl*> LateParsedDecls;
};

class FullVisitorConsumer : public ASTConsumer {
  CompilerInstance &Instance;
  std::set<std::string> ParsedTemplates;

public:
  FullVisitorConsumer(CompilerInstance &Instance,
                         std::set<std::string> ParsedTemplates)
      : Instance(Instance), ParsedTemplates(ParsedTemplates) {}


  // HandleTopLevelDecl - Handle the specified top-level declaration.
  bool HandleTopLevelDecl(DeclGroupRef DG) override {
    // for (DeclGroupRef::iterator i = DG.begin(), e = DG.end(); i != e; ++i) {
    //   const Decl *D = *i;
    //   if (const NamedDecl *ND = dyn_cast<NamedDecl>(D))
    //     llvm::errs() << "top-level-decl: \"" << ND->getNameAsString() << "\"\n";
    // }

    return true;
  }
  
  // HandleTranslationUnit - This method is called when the ASTs for entire translation unit have been parsed.
  void HandleTranslationUnit(ASTContext& context) override {
    
    // auto decl_vec = context.getTraversalScope();
    // for (Decl * decl :decl_vec) {
    //   printf("@@ decl kind name %s\n",decl->getDeclKindName() );
    // }


    // auto attrvec = context.getDeclAttrs();
    // for (auto& attr : attrvec) {
    //   llvm::errs() << "find attr:" << std::to_string(attr.getSpelling()) << "\n";
    // }
    
    
    
    // if (!Instance.getLangOpts().DelayedTemplateParsing)
    //   return;

    // This demonstrates how to force instantiation of some templates in
    // -fdelayed-template-parsing mode. (Note: Doing this unconditionally for
    // all templates is similar to not using -fdelayed-template-parsig in the
    // first place.)
    // The advantage of doing this in HandleTranslationUnit() is that all
    // codegen (when using -add-plugin) is completely finished and this can't
    // affect the compiler output.
    Visitor v(ParsedTemplates,Instance);
    
    v.TraverseDecl(context.getTranslationUnitDecl());



    // clang::Sema &sema = Instance.getSema();
    // for (const FunctionDecl *FD : v.LateParsedDecls) {
    //   clang::LateParsedTemplate &LPT =
    //       *sema.LateParsedTemplateMap.find(FD)->second;
    //   sema.LateTemplateParser(sema.OpaqueParser, LPT);
    //   llvm::errs() << "late-parsed-decl: \"" << FD->getNameAsString() << "\"\n";
    // }   
  }
};

class FullVisitorAction : public PluginASTAction {
  std::set<std::string> ParsedTemplates;
protected:
  std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI,
                                                 llvm::StringRef) override {
    return std::make_unique<FullVisitorConsumer>(CI, ParsedTemplates);
  }

  bool ParseArgs(const CompilerInstance &CI,
                 const std::vector<std::string> &args) override {
    for (unsigned i = 0, e = args.size(); i != e; ++i) {
      llvm::errs() << "FullVisitor arg = " << args[i] << "\n";

      // Example error handling.
      // DiagnosticsEngine &D = CI.getDiagnostics();
      // if (args[i] == "-an-error") {
      //   unsigned DiagID = D.getCustomDiagID(DiagnosticsEngine::Error,
      //                                       "invalid argument '%0'");
      //   D.Report(DiagID) << args[i];
      //   return false;
      // } else if (args[i] == "-parse-template") {
      //   if (i + 1 >= e) {
      //     D.Report(D.getCustomDiagID(DiagnosticsEngine::Error,
      //                                "missing -parse-template argument"));
      //     return false;
      //   }
      //   ++i;
      //   ParsedTemplates.insert(args[i]);
      // }
    }
    if (!args.empty() && args[0] == "help")
      PrintHelp(llvm::errs());

    return true;
  }
  void PrintHelp(llvm::raw_ostream& ros) {
    ros << "Help for FullVisitor plugin goes here\n";
  }

};

}

static FrontendPluginRegistry::Add<FullVisitorAction>
X("full-vst", "visit all parts of the program");
