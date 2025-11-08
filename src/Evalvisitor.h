#pragma once
#ifndef PYTHON_INTERPRETER_EVALVISITOR_H
#define PYTHON_INTERPRETER_EVALVISITOR_H


#include "Python3ParserBaseVisitor.h"
#include "int2048.h"


class EvalVisitor : public Python3ParserBaseVisitor {
	// TODO: override all methods of Python3ParserBaseVisitor
	virtual std::any visitFile_input(Python3Parser::File_inputContext *ctx) override {
		std::vector<Python3Parser::StmtContext *> son = ctx->stmt();
		for(auto now : son){
			visit(now);
		}
		return ctx->EOF();
	}

	virtual std::any visitFuncdef(Python3Parser::FuncdefContext *ctx) override {
		return visitChildren(ctx);
	}

	virtual std::any visitParameters(Python3Parser::ParametersContext *ctx) override {
		return visitChildren(ctx);
	}

	virtual std::any visitTypedargslist(Python3Parser::TypedargslistContext *ctx) override {
		return visitChildren(ctx);
	}

	virtual std::any visitTfpdef(Python3Parser::TfpdefContext *ctx) override {
		antlr4::tree::TerminalNode *name = ctx->NAME();
		return name->toString();// Is this right?
	}

	virtual std::any visitStmt(Python3Parser::StmtContext *ctx) override {
		Python3Parser::Simple_stmtContext *simple = ctx->simple_stmt();
		Python3Parser::Compound_stmtContext *compound = ctx->compound_stmt();
		if(simple){
			return visit(simple);
		}
		else{
			return visit(compound);
		}
	}

	virtual std::any visitSimple_stmt(Python3Parser::Simple_stmtContext *ctx) override {
		Python3Parser::Small_stmtContext *small = ctx->small_stmt();
		return visit(small);
	}

	virtual std::any visitSmall_stmt(Python3Parser::Small_stmtContext *ctx) override {
		Python3Parser::Expr_stmtContext *expr = ctx->expr_stmt();
		Python3Parser::Flow_stmtContext *flow = ctx->flow_stmt();
		if(expr){
			return visit(expr);
		}
		else{
			return visit(flow);
		}
	}

	virtual std::any visitExpr_stmt(Python3Parser::Expr_stmtContext *ctx) override {
		return visitChildren(ctx);
	}

	virtual std::any visitAugassign(Python3Parser::AugassignContext *ctx) override {
		antlr4::tree::TerminalNode  *add = ctx->ADD_ASSIGN(),
									*sub = ctx->SUB_ASSIGN(),
									*mult = ctx->MULT_ASSIGN(),
									*div = ctx->DIV_ASSIGN(),
									*idiv = ctx->IDIV_ASSIGN(),
									*mod = ctx->MOD_ASSIGN();
		if(add){
			return "add";
		}
		if(sub){
			return "sub";
		}
		if(mult){
			return "mult";
		}
		if(div){
			return "div";
		}
		if(idiv){
			return "idiv";
		}
		return "mod";
	}

	virtual std::any visitFlow_stmt(Python3Parser::Flow_stmtContext *ctx) override {
		Python3Parser::Break_stmtContext *brk = ctx->break_stmt();
		Python3Parser::Continue_stmtContext *cont = ctx->continue_stmt();
		Python3Parser::Return_stmtContext *ret = ctx->return_stmt();
		if(brk){
			return visit(brk);
		}
		else if(cont){
			return visit(cont);
		}
		else{
			return visit(ret);
		}
	}

	virtual std::any visitBreak_stmt(Python3Parser::Break_stmtContext *ctx) override {
		return "break";
	}

	virtual std::any visitContinue_stmt(Python3Parser::Continue_stmtContext *ctx) override {
		return "continue";
	}

	virtual std::any visitReturn_stmt(Python3Parser::Return_stmtContext *ctx) override {
		Python3Parser::TestlistContext *ret = ctx->testlist();
		if(ret){
			return std::vector<std::any>{"return", visit(ret)};
		}
		else{
			return "return";
		}
	}

	virtual std::any visitCompound_stmt(Python3Parser::Compound_stmtContext *ctx) override {
		Python3Parser::If_stmtContext *sif = ctx->if_stmt();
		Python3Parser::While_stmtContext *swhile = ctx->while_stmt();
		Python3Parser::FuncdefContext *def = ctx->funcdef();
		if(sif){
			return visit(sif);
		}
		if(swhile){
			return visit(swhile);
		}
		return visit(def);
	}

	bool isTrue(std::any ret){
		int2048 *gint = std::any_cast<int2048>(&ret);
		auto *gdouble = std::any_cast<double>(&ret);
		auto *gbool = std::any_cast<bool>(&ret);
		return (gint && *gint != 0) || (gdouble && *gdouble) || (gbool && *gbool);
	}
	virtual std::any visitIf_stmt(Python3Parser::If_stmtContext *ctx) override {
		Python3Parser::TestContext *fir = ctx->test(0);
		if(isTrue(visit(fir))){
			return visit(ctx->suite(0));
		}
		else{
			auto branch = ctx->test();
			branch.assign(std::next(branch.begin()),branch.end());
			if(ctx->ELSE()){
				branch.pop_back();
			}
			int np = 1;
			if(!branch.empty()){
				for(auto now : branch){
					if(isTrue(visit(now))){
						return visit(ctx->suite(np));
					}
					np++;
				}
			}
			if(ctx->ELSE()){
				return visit(ctx->suite().back());
			}
		}
		return "if nothing satisfied";
	}

	bool isString(std::any ret, const std::string &str){
		auto *dt = std::any_cast<std::string>(&ret);
		return dt && *dt == str;
	}

	bool isReturn(std::any gave){
		std::vector<std::any> *got = std::any_cast<std::vector<std::any>>(&gave);
		if(got){
			auto ret = *got;
			if(ret.empty()){
				return false;
			}
			auto *dt = std::any_cast<std::string>(&ret[0]);
			return dt && isString(*dt, "return");
		}
		auto *dt = std::any_cast<std::string>(&gave);
		return dt && *dt == "return";
	}

	virtual std::any visitWhile_stmt(Python3Parser::While_stmtContext *ctx) override {
		while(true){
			Python3Parser::TestContext *con = ctx->test();
			std::any got = visit(con);
			if(!isTrue(got)){
				return "while condition unsatisfied";
			}
			std::any ret = visit(ctx->suite());
			if(isString(ret,"break")){
				break;
			}
			if(isString(ret, "return")){
				return "return";
			}
		}
		return "while end";
	}

	virtual std::any visitSuite(Python3Parser::SuiteContext *ctx) override {
		Python3Parser::Simple_stmtContext *simple = ctx->simple_stmt();
		if(simple){
			return visit(simple);
		}
		else{
			auto stmt = ctx->stmt();
			for(Python3Parser::StmtContext *now : stmt){
				std::any ret = visit(now);
				if(isString(ret, "continue")){
					return "continue";
				}
				if(isString(ret, "break")){
					return "break";
				}
				if(isReturn(ret)){
					return ret;
				}
			}
			return "suite unflowed end";
		}
	}

	virtual std::any visitTest(Python3Parser::TestContext *ctx) override {
		return visit(ctx->or_test());
	}

	bool isTrue(std::any got){
		bool *ret = std::any_cast<bool>(&got);
		return ret && *ret;
	}

	bool isFalse(std::any ret){
		int2048 *gint = std::any_cast<int2048>(&ret);
		auto *gdouble = std::any_cast<double>(&ret);
		auto *gbool = std::any_cast<bool>(&ret);
		return (gint && *gint == 0) || (gdouble && !*gdouble) || (gbool && !*gbool);
	}

	virtual std::any visitOr_test(Python3Parser::Or_testContext *ctx) override {
		std::vector<Python3Parser::And_testContext*> got = ctx->and_test();
		int nsi = got.size();
		for(int i=0;i<nsi;i++){
			if(isTrue(visit(got[i]))){
				return true;
			}
		}
		return false;
	}

	virtual std::any visitAnd_test(Python3Parser::And_testContext *ctx) override {
		std::vector<Python3Parser::Not_testContext*> got = ctx->not_test();
		int nsi = got.size();
		for(int i=0;i<nsi;i++){
			if(isFalse(visit(got[i]))){
				return false;
			}
		}
		return true;
	}

	virtual std::any visitNot_test(Python3Parser::Not_testContext *ctx) override {
		Python3Parser::Not_testContext *ntest = ctx->not_test();
		Python3Parser::ComparisonContext *comp = ctx->comparison();
		if(ntest){
			std::any ret = visit(ntest);
			assert(isTrue(ret) || isFalse(ret));
			return isFalse(ret);
		}
		else{
			std::any ret = visit(comp);
			assert(isTrue(ret) || isFalse(ret));
			return isFalse(ret);
		}
	}

	bool compare(std::any fir, std::any sec, std::string op){
		int2048 *gint = std::any_cast<int2048>(&fir), *sint = std::any_cast<int2048>(&sec);
		auto *gdouble = std::any_cast<double>(&fir), *sdouble = std::any_cast<double>(&sec);
		auto *gbool = std::any_cast<bool>(&fir), *sbool = std::any_cast<bool>(&sec);
		if(gint && sint){
			int2048 fi = *gint, si = *sint;
			if(op=="<"){
				return fi < si;
			}
			if(op=="<="){
				return fi <= si;
			}
			if(op==">"){
				return fi > si;
			}
			if(op==">="){
				return fi >= si;
			}
			if(op=="=="){
				return fi == si;
			}
			return fi != si;
		}
		if(gdouble && sdouble){
			double fi = *gdouble, si = *sdouble;
			if(op=="<"){
				return fi < si;
			}
			if(op=="<="){
				return fi <= si;
			}
			if(op==">"){
				return fi > si;
			}
			if(op==">="){
				return fi >= si;
			}
			if(op=="=="){
				return fi == si;
			}
			return fi != si;
		}
		if(gbool && sbool){
			bool fi = *gbool, si = *sbool;
			if(op=="<"){
				return fi < si;
			}
			if(op=="<="){
				return fi <= si;
			}
			if(op==">"){
				return fi > si;
			}
			if(op==">="){
				return fi >= si;
			}
			if(op=="=="){
				return fi == si;
			}
			return fi != si;
		}
		assert(false);
		return false;
	}
	virtual std::any visitComparison(Python3Parser::ComparisonContext *ctx) override {
		std::vector<Python3Parser::Arith_exprContext*> ari = ctx->arith_expr();
		std::vector<Python3Parser::Comp_opContext*> comp = ctx->comp_op();
		std::any fir = visit(ari[0]);
		if(comp.empty()){
			return isTrue(fir);
		}
		int nsi = comp.size();
		for(int i=1;i<nsi;i++){
			std::any nv = visit(ari[i]);
			if(!compare(fir, nv, std::any_cast<std::string>(visit(comp[i-1])))){
				return false;
			}
			fir = nv;
		}
		return true;
	}

	virtual std::any visitComp_op(Python3Parser::Comp_opContext *ctx) override {
		std::vector<antlr4::tree::TerminalNode*> all = {ctx->LESS_THAN(),
														ctx->GREATER_THAN(),
														ctx->EQUALS(),
														ctx->GT_EQ(),
														ctx->LT_EQ(),
														ctx->NOT_EQ_2()};
		if(all[0]){
			return "<";
		}
		if(all[1]){
			return ">";
		}
		if(all[2]){
			return "==";
		}
		if(all[3]){
			return ">=";
		}
		if(all[4]){
			return "<=";
		}
		if(all[5]){
			return "!=";
		}
		assert(false);
		return false;
	}

	std::any addOrSub(std::any fir, std::any sec, std::string op){
		int2048 *gint = std::any_cast<int2048>(&fir), *sint = std::any_cast<int2048>(&sec);
		auto *gdouble = std::any_cast<double>(&fir), *sdouble = std::any_cast<double>(&sec);
		auto *gbool = std::any_cast<bool>(&fir), *sbool = std::any_cast<bool>(&sec);
		if(gint && sint){
			int2048 fi = *gint, si = *sint;
			if(op=="+"){
				return fi + si;
			}
			if(op=="-"){
				return fi - si;
			}
		}
		if(gdouble && sdouble){
			double fi = *gdouble, si = *sdouble;
			if(op=="+"){
				return fi + si;
			}
			if(op=="-"){
				return fi - si;
			}
		}
		if(gbool && sbool){
			bool fi = *gbool, si = *sbool;
			if(op=="+"){
				return fi + si;
			}
			if(op=="-"){
				return fi - si;
			}
		}
		assert(false);
		return false;
	}

	virtual std::any visitArith_expr(Python3Parser::Arith_exprContext *ctx) override {
		std::vector<Python3Parser::TermContext*> term = ctx->term();
		std::vector<Python3Parser::Addorsub_opContext*> op = ctx->addorsub_op();
		std::any fir = visit(term[0]);
		int nsi = term.size();
		for(int i=1;i<nsi;i++){
			std::any nv = visit(term[i]);
			fir = addOrSub(fir, nv, std::any_cast<std::string>(visit(op[i-1])));
		}
		return fir;
	}

	virtual std::any visitAddorsub_op(Python3Parser::Addorsub_opContext *ctx) override {
		return visitChildren(ctx);
	}

	virtual std::any visitTerm(Python3Parser::TermContext *ctx) override {
		return visitChildren(ctx);
	}

	virtual std::any visitMuldivmod_op(Python3Parser::Muldivmod_opContext *ctx) override {
		return visitChildren(ctx);
	}

	virtual std::any visitFactor(Python3Parser::FactorContext *ctx) override {
		return visitChildren(ctx);
	}

	virtual std::any visitAtom_expr(Python3Parser::Atom_exprContext *ctx) override {
		return visitChildren(ctx);
	}

	virtual std::any visitTrailer(Python3Parser::TrailerContext *ctx) override {
		return visitChildren(ctx);
	}

	virtual std::any visitAtom(Python3Parser::AtomContext *ctx) override {
		return visitChildren(ctx);
	}

	virtual std::any visitFormat_string(Python3Parser::Format_stringContext *ctx) override {
		return visitChildren(ctx);
	}

	virtual std::any visitTestlist(Python3Parser::TestlistContext *ctx) override {
		return visitChildren(ctx);
	}

	virtual std::any visitArglist(Python3Parser::ArglistContext *ctx) override {
		return visitChildren(ctx);
	}

	virtual std::any visitArgument(Python3Parser::ArgumentContext *ctx) override {
		return visitChildren(ctx);
	}
};

//hello!
#endif//PYTHON_INTERPRETER_EVALVISITOR_H
