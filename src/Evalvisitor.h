#pragma once
#ifndef PYTHON_INTERPRETER_EVALVISITOR_H
#define PYTHON_INTERPRETER_EVALVISITOR_H


#include "Python3ParserBaseVisitor.h"
#include "int2048.h"
#include <iomanip>


class EvalVisitor : public Python3ParserBaseVisitor {
	// TODO: override all methods of Python3ParserBaseVisitor
	virtual std::any visitFile_input(Python3Parser::File_inputContext *ctx) override {
		std::vector<Python3Parser::StmtContext *> son = ctx->stmt();
		for(auto now : son){
			visit(now);
		}
		return ctx->EOF();
	}

	const static int kMaxn = 1e4+5;
	std::unordered_map<std::string,int>funcId;
	std::unordered_map<int,std::any>memory[kMaxn];
	std::unordered_map<int,bool>covered[kMaxn];
	std::unordered_map<int,Python3Parser::FuncdefContext*>function;
	int depth;
	int func_id = -7, variable_id = 1;

	EvalVisitor(){
		funcId["print"] = -2;
		funcId["int"] = -3;
		funcId["float"] = -4;
		funcId["str"] = -5;
		funcId["bool"] = -6;
		return;
	}

	std::string removeQuotes(std::string str){
		assert(str[0] == '\'' || str[0] == '\"');
		return str.substr(1, str.length()-2);
	}

	virtual std::any visitFuncdef(Python3Parser::FuncdefContext *ctx) override {
		funcId[removeQuotes(ctx->NAME()->getText())] = func_id;
		function[func_id] = ctx;
		func_id --;
		return visitChildren(ctx);
	}

	virtual std::any visitParameters(Python3Parser::ParametersContext *ctx) override {
		return visitChildren(ctx);//Do nothing, will be done in functionWork()
	}

	virtual std::any visitTypedargslist(Python3Parser::TypedargslistContext *ctx) override {
		return visitChildren(ctx);//Do nothing, will be done in functionWork()
	}

	virtual std::any visitTfpdef(Python3Parser::TfpdefContext *ctx) override {
		antlr4::tree::TerminalNode *name = ctx->NAME();
		return removeQuotes(name->getText());// Is this right?
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

	
	std::any unTie(std::any gave){
		auto *gint = std::any_cast<std::pair<int2048,int>>(&gave);
		auto *gdouble = std::any_cast<std::pair<double,int>>(&gave);
		auto *gbool = std::any_cast<std::pair<bool,int>>(&gave);
		auto *gstring = std::any_cast<std::pair<std::string,int>>(&gave);
		auto *gvector = std::any_cast<std::vector<std::pair<std::any,int>>>(&gave);
		if(gint){
			return gint->first;
		}
		if(gdouble){
			return gdouble->first;
		}
		if(gbool){
			return gbool->first;
		}
		if(gstring){
			return gstring->first;
		}
		if(gvector){
			return gvector;
		}
		//assert(false);//maybe unavoidable
		return gave;
	}
	
	bool isString(std::any ret, const std::string &str){
		ret = unTie(ret);
		auto *dt = std::any_cast<std::string>(&ret);
		return dt && *dt == str;
	}

	int valuePosition(std::string name){
		for(auto now=memory[depth].begin();now!=memory[depth].end();now++){
			if(isString(now->second, name)){
				return now->first;
			}
		}
		return 0;
	}

	void assignValue(std::pair<std::any,int> fir, std::pair<std::any,int> sec){
		int id = fir.second;
		for(int i=0;i<=depth;i++){
			if(!covered[i][id]){
				memory[i][id] = sec;
			}
		}
		return;
	}
	
	std::any addOrSub(std::any fir, std::any sec, std::string op){
		fir = unTie(fir), sec = unTie(sec);
		int2048 *gint = std::any_cast<int2048>(&fir), *sint = std::any_cast<int2048>(&sec);
		auto *gdouble = std::any_cast<double>(&fir), *sdouble = std::any_cast<double>(&sec);
		auto *gbool = std::any_cast<bool>(&fir), *sbool = std::any_cast<bool>(&sec);
		auto *fstring = std::any_cast<std::string>(&fir), *sstring = std::any_cast<std::string>(&sec);
		if(gint && sint){
			int2048 fi = *gint, si = *sint;
			if(op == "+"){
				return fi + si;
			}
			if(op == "-"){
				return fi - si;
			}
		}
		if(gdouble && sdouble){
			double fi = *gdouble, si = *sdouble;
			if(op == "+"){
				return fi + si;
			}
			if(op == "-"){
				return fi - si;
			}
		}
		if(gbool && sbool){
			bool fi = *gbool, si = *sbool;
			if(op == "+"){
				return fi + si;
			}
			if(op == "-"){
				return fi - si;
			}
		}
		if(fstring && sstring){
			assert(op == "+");
			return *fstring + *sstring;
		}
		assert(false);
		return false;
	}
	
	std::any mulDivMod(std::any fir, std::any sec, std::string op){
		fir = unTie(fir), sec = unTie(sec);
		int2048 *gint = std::any_cast<int2048>(&fir), *sint = std::any_cast<int2048>(&sec);
		auto *gdouble = std::any_cast<double>(&fir), *sdouble = std::any_cast<double>(&sec);
		//auto *gbool = std::any_cast<bool>(&fir), *sbool = std::any_cast<bool>(&sec); // should not appear?
		auto *fstring = std::any_cast<std::string>(&fir), *sstring = std::any_cast<std::string>(&sec);
		if(gint && sint){
			int2048 fi = *gint, si = *sint;
			if(op == "*"){
				return fi * si;
			}
			if(op == "/" || op == "//"){
				return fi / si;
			}
			if(op == "%"){
				return fi % si;
			}
			assert(false);
			return false;
		}
		if(gdouble && sdouble){
			double fi = *gdouble, si = *sdouble;
			if(op == "*"){
				return fi * si;
			}
			if(op == "/"){
				return fi / si;
			}
			assert(false);
			return false;
		}
		if(fstring && sint){
			assert(op == "*");
			std::string ret = *fstring;
			for(int i=1;i<*sint;i++){
				ret += *fstring;
			}
			return ret;
		}
		assert(false);
		return false;
	}

	virtual std::any visitExpr_stmt(Python3Parser::Expr_stmtContext *ctx) override {
		std::vector<std::pair<std::any,int>>ret;
		if(!ctx->ASSIGN().empty()){
			auto testlist = ctx->testlist();
			int nsi = testlist.size();
			for(int i=nsi-1;i;i--){
				auto fir = testlist[i-1]->test(), sec = testlist[i]->test();
				int osi = fir.size();
				for(int j=0;j<osi;j++){
					std::any lef = visit(fir[j]), rig = visit(sec[j]);
					assignValue(std::any_cast<std::pair<std::any,int>>(lef),
								std::any_cast<std::pair<std::any,int>>(rig));
					ret.push_back(std::any_cast<std::pair<std::any,int>>(rig));
				}
			}
		}
		else{
			auto testlist = ctx->testlist();
			auto aug = ctx->augassign();
			int nsi = testlist.size();
			auto fir = testlist[0]->test(), sec = testlist[1]->test();
			int osi = fir.size();
			std::any lef = visit(fir[0]), rig = visit(sec[0]);
			std::string op = std::any_cast<std::pair<std::string,int>>(visit(aug)).first;
			op.pop_back();
			if(op == "+" || op == "-"){
				assignValue(std::any_cast<std::pair<std::any,int>>(lef),
							std::any_cast<std::pair<std::any,int>>(addOrSub(lef, rig, op)));
				int id = std::any_cast<std::pair<std::any,int>>(lef).second;
				ret.push_back(std::make_pair(memory[depth][id],id));
			}
			else{
				assignValue(std::any_cast<std::pair<std::any,int>>(lef),
							std::any_cast<std::pair<std::any,int>>(mulDivMod(lef, rig, op)));
				int id = std::any_cast<std::pair<std::any,int>>(lef).second;
				ret.push_back(std::make_pair(memory[depth][id],id));
			}
		}
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
			return std::make_pair("add",-1);
		}
		if(sub){
			return std::make_pair("sub",-1);
		}
		if(mult){
			return std::make_pair("mult",-1);
		}
		if(div){
			return std::make_pair("div",-1);
		}
		if(idiv){
			return std::make_pair("idiv",-1);
		}
		return std::make_pair("mod",-1);
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
		return std::make_pair("break",-1);
	}

	virtual std::any visitContinue_stmt(Python3Parser::Continue_stmtContext *ctx) override {
		return std::make_pair("continue",-1);
	}

	virtual std::any visitReturn_stmt(Python3Parser::Return_stmtContext *ctx) override {
		Python3Parser::TestlistContext *ret = ctx->testlist();
		if(ret){
			return std::vector<std::any>{std::make_pair("return", -1), visit(ret)};
		}
		else{
			return std::vector<std::any>{std::make_pair("return", -1)};
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
		ret = unTie(ret);
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
		return std::make_pair("if nothing satisfied",-1);
	}

	bool isReturn(std::any gave){
		std::any real = unTie(gave);
		std::vector<std::pair<std::any,int>> *got = std::any_cast<std::vector<std::pair<std::any,int>>>(&real);
		if(got){
			auto ret = *got;
			return isString(ret[0].first, "return");
		}
		return false;
	}

	virtual std::any visitWhile_stmt(Python3Parser::While_stmtContext *ctx) override {
		while(true){
			Python3Parser::TestContext *con = ctx->test();
			std::any got = visit(con);
			if(!isTrue(got)){
				return std::make_pair("while condition unsatisfied",-1);
			}
			std::any ret = visit(ctx->suite());
			if(isString(ret,"break")){
				break;
			}
			if(isReturn(ret)){
				return ret;
			}
		}
		return std::make_pair("while end",-1);
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
					return std::make_pair("continue",-1);
				}
				if(isString(ret, "break")){
					return std::make_pair("break",-1);
				}
				if(isReturn(ret)){
					return ret;
				}
			}
			return std::make_pair("suite unflowed end",-1);
		}
	}

	virtual std::any visitTest(Python3Parser::TestContext *ctx) override {
		return visit(ctx->or_test());
	}

	bool isTrue(std::any got){
		got = unTie(got);
		bool *ret = std::any_cast<bool>(&got);
		return ret && *ret;
	}

	bool isFalse(std::any ret){
		ret = unTie(ret);
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
		fir = unTie(fir), sec = unTie(sec);
		int2048 *gint = std::any_cast<int2048>(&fir), *sint = std::any_cast<int2048>(&sec);
		auto *gdouble = std::any_cast<double>(&fir), *sdouble = std::any_cast<double>(&sec);
		auto *gbool = std::any_cast<bool>(&fir), *sbool = std::any_cast<bool>(&sec);
		if(gint && sint){
			int2048 fi = *gint, si = *sint;
			if(op == "<"){
				return fi < si;
			}
			if(op == "<="){
				return fi <= si;
			}
			if(op == ">"){
				return fi > si;
			}
			if(op == ">="){
				return fi >= si;
			}
			if(op == "=="){
				return fi == si;
			}
			return fi != si;
		}
		if(gdouble && sdouble){
			double fi = *gdouble, si = *sdouble;
			if(op == "<"){
				return fi < si;
			}
			if(op == "<="){
				return fi <= si;
			}
			if(op == ">"){
				return fi > si;
			}
			if(op == ">="){
				return fi >= si;
			}
			if(op == "=="){
				return fi == si;
			}
			return fi != si;
		}
		if(gbool && sbool){
			bool fi = *gbool, si = *sbool;
			if(op == "<"){
				return fi < si;
			}
			if(op == "<="){
				return fi <= si;
			}
			if(op == ">"){
				return fi > si;
			}
			if(op == ">="){
				return fi >= si;
			}
			if(op == "=="){
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
			if(!compare(fir, nv, std::any_cast<std::string>(unTie(visit(comp[i-1]))))){
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
			return std::make_pair("<",-1);
		}
		if(all[1]){
			return std::make_pair(">",-1);
		}
		if(all[2]){
			return std::make_pair("==",-1);
		}
		if(all[3]){
			return std::make_pair(">=",-1);
		}
		if(all[4]){
			return std::make_pair("<=",-1);
		}
		if(all[5]){
			return std::make_pair("!=",-1);
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
			fir = addOrSub(fir, nv, std::any_cast<std::string>(unTie(visit(op[i-1]))));
		}
		return fir;
	}

	virtual std::any visitAddorsub_op(Python3Parser::Addorsub_opContext *ctx) override {
		if(ctx->ADD()){
			return std::make_pair("+",-1);
		}
		else if(ctx->MINUS()){
			return std::make_pair("-",-1);
		}
		assert(false);
		return visitChildren(ctx);
	}

	virtual std::any visitTerm(Python3Parser::TermContext *ctx) override {
		std::vector<Python3Parser::FactorContext*> factor = ctx->factor();
		std::vector<Python3Parser::Muldivmod_opContext*> op = ctx->muldivmod_op();
		std::any fir = visit(factor[0]);
		int nsi = factor.size();
		for(int i=1;i<nsi;i++){
			std::any nv = visit(factor[i]);
			fir = mulDivMod(fir, nv, std::any_cast<std::string>(visit(op[i-1])));
		}
		return fir;
	}

	virtual std::any visitMuldivmod_op(Python3Parser::Muldivmod_opContext *ctx) override {
		if(ctx->STAR()){
			return std::make_pair("*",-1);
		}
		else if(ctx->DIV()){
			return std::make_pair("/",-1);
		}
		else if(ctx->IDIV()){
			return std::make_pair("//",-1);
		}
		else if(ctx->MOD()){
			return std::make_pair("%",-1);
		}
		assert(false);
		return visitChildren(ctx);
	}

	std::any getNegativeVal(std::any ret){
		ret = unTie(ret);
		int2048 *gint = std::any_cast<int2048>(&ret);
		auto *gdouble = std::any_cast<double>(&ret);
		if(gint){
			return -*gint;
		}
		if(gdouble){
			return -*gdouble;
		}
		assert(false);
		return false;
	}

	virtual std::any visitFactor(Python3Parser::FactorContext *ctx) override {
		if(ctx->ADD()){
			return visit(ctx->factor());
		}
		if(ctx->MINUS()){
			return getNegativeVal(visit(ctx->factor()));
		}
		return visit(ctx->atom_expr());
	}

	std::any functionWork(Python3Parser::FuncdefContext *fun, Python3Parser::TrailerContext *tra){
		auto argls = tra->arglist();
		if(argls){
			std::vector<Python3Parser::ArgumentContext*> arg = argls->argument();
			for(auto now : arg){
				std::vector<Python3Parser::TestContext*> test = now->test();
				if(test.size() != 1){
					auto gfir = visit(test[0]), gsec = visit(test[1]);
					std::pair<std::any,int> *fir = std::any_cast<std::pair<std::any,int>>(&gfir),
											*sec = std::any_cast<std::pair<std::any,int>>(&gsec);
					assignValue(*fir, *sec);
				}
			}
		}
		depth++;
		memory[depth] = memory[depth-1], covered[depth].clear();
		auto targls = fun->parameters()->typedargslist();
		if(targls){
			auto var = targls->tfpdef();
			auto test = targls->test();
			auto argls = tra->arglist();
			std::vector<Python3Parser::ArgumentContext*> arg;
			if(argls){
				arg = argls->argument();
			}
			for(int i=0;i<arg.size();i++){
				std::string name = removeQuotes(var[0]->NAME()->getText());
				if(!valuePosition(name)){
					assignValue(std::make_pair(std::any(false),variable_id),
							std::make_pair(std::any(0),0));
					variable_id++;
				}
				int pos = valuePosition(name);
				assignValue(std::make_pair(memory[depth][pos],pos), std::make_pair(visit(arg[i]->test()[0]),-1));
				covered[depth][pos] = true;
			}
			assert(arg.size() + test.size() >= var.size());
			for(int i=arg.size();i<var.size();i++){
				std::string name = removeQuotes(var[i]->NAME()->getText());
				if(!valuePosition(name)){
					assignValue(std::make_pair(std::any(false),variable_id),
							std::make_pair(std::any(0),0));
					variable_id++;
				}
				int pos = valuePosition(name);
				assignValue(std::make_pair(memory[depth][pos],pos),
					std::make_pair(visit(test[test.size()-(var.size()-i)]),-1));
				covered[depth][pos] = true;
			}
		}
		std::any ret = visit(fun->suite());
		depth--;
		return ret;
	}

	std::string doubleToString(double v){
		double down = 1;
		int po = 0;
		while(down < v){
			down *= 10, po++;
		}
		while(down > v){
			down /= 10, po--;
		}
		std::string ret;
		if(po < 0){
			ret += "0.", po++;
			while(po < 0){
				ret += '0', po++;
			}
		}
		while(v){
			int nv = v / po;
			ret += nv + '0';
			v -= po * nv, v *= 10;
		}
		return ret;
	}

	std::any insideFunction(int id, Python3Parser::TrailerContext *tra){
		auto argls = tra->arglist();
		if(!argls){
			assert(id == -2);//only print can have no args?
			std::cout << '\n';
			return std::make_pair("insideFunction:Printed an empty arglist",-1);
		}
		else{
			std::vector<Python3Parser::ArgumentContext*> arg = argls->argument();
			for(auto now : arg){
				std::vector<Python3Parser::TestContext*> test = now->test();
				if(test.size() != 1){
					auto gfir = visit(test[0]), gsec = visit(test[1]);
					std::pair<std::any,int> *fir = std::any_cast<std::pair<std::any,int>>(&gfir),
											*sec = std::any_cast<std::pair<std::any,int>>(&gsec);
					assignValue(*fir, *sec);
				}
			}//because these five function all should have arguments
			if(id == -2){
				for(auto now : arg){
					std::vector<Python3Parser::TestContext*> test = now->test();
					auto gfir = visit(test[0]);
					std::pair<std::any,int> *fir = std::any_cast<std::pair<std::any,int>>(&gfir);
					if(std::pair<int2048,int> *dt = std::any_cast<std::pair<int2048,int>*>(fir); dt){
						if(dt->second == 0){
							std::cout << "None" << ' ';
						}
						else{
							std::cout << dt->first << ' ';
						}
					}
					if(std::pair<double,int> *dt = std::any_cast<std::pair<double,int>*>(fir); dt){
						std::cout << std::fixed << std::setprecision(6) << dt->first << ' ';
					}
					if(std::pair<std::string,int> *dt = std::any_cast<std::pair<std::string,int>*>(fir); dt){
						std::cout << dt->first << ' ';
					}
					if(std::pair<bool,int> *dt = std::any_cast<std::pair<bool,int>*>(fir); dt){
						if(dt->first){
							std::cout << "True" << ' ';
						}
						else{
							std::cout << "False" << ' ';
						}
					}
				}
				std::cout << '\n';
			}
			else if(id == -3){
				auto fir = visit(arg[0]->test()[0]);
				if(auto *dt = std::any_cast<std::pair<int2048,int>>(&fir); dt){
					if(dt->second == 0){
						return std::make_pair(dt->first,-1);
					}
					else{
						return *dt;
					}
				}
				if(auto *dt = std::any_cast<std::pair<double,int>>(&fir); dt){
					return std::make_pair((int2048)((long long)std::floor(dt->first)),-1);
				}
				if(auto *dt = std::any_cast<std::pair<std::string,int>>(&fir); dt){
					return std::make_pair((int2048)(dt->first),-1);
				}
				if(auto *dt = std::any_cast<std::pair<bool,int>>(&fir); dt){
					return std::make_pair((int2048)(dt->first),-1);
				}
				assert(false);
			}
			else if(id == -4){
				auto fir = visit(arg[0]->test()[0]);
				if(auto *dt = std::any_cast<std::pair<int2048,int>>(&fir); dt){
					return std::make_pair(dt->first.toDouble(),-1);
				}
				if(auto *dt = std::any_cast<std::pair<double,int>>(&fir); dt){
					return *dt;
				}
				if(auto *dt = std::any_cast<std::pair<std::string,int>>(&fir); dt){
					return std::make_pair(std::stod(dt->first),-1);
				}
				if(auto *dt = std::any_cast<std::pair<bool,int>>(&fir); dt){
					return std::make_pair((double)(dt->first ? 1 : 0),-1);
				}
				assert(false);
			}
			else if(id == -5){
				auto fir = visit(arg[0]->test()[0]);
				if(auto *dt = std::any_cast<std::pair<int2048,int>>(&fir); dt){
					return std::make_pair(dt->first.toString(),-1);
				}
				if(auto *dt = std::any_cast<std::pair<double,int>>(&fir); dt){
					return std::make_pair(doubleToString(dt->first),-1);
				}
				if(auto *dt = std::any_cast<std::pair<std::string,int>>(&fir); dt){
					return *dt;
				}
				if(auto *dt = std::any_cast<std::pair<bool,int>>(&fir); dt){
					return std::make_pair((std::string)(dt->first ? "True" : "False"),-1);//How?
				}
				assert(false);
			}
			else{
				auto fir = visit(arg[0]->test()[0]);
				if(auto *dt = std::any_cast<std::pair<int2048,int>>(&fir); dt){
					return std::make_pair(dt->first == 0 ? false : true,-1);
				}
				if(auto *dt = std::any_cast<std::pair<double,int>>(&fir); dt){
					return std::make_pair(dt->first == 0.0 ? false : true,-1);
				}
				if(auto *dt = std::any_cast<std::pair<std::string,int>>(&fir); dt){
					return std::make_pair(dt->first.empty() ? false : true,-1);
				}
				if(auto *dt = std::any_cast<std::pair<bool,int>>(&fir); dt){
					return *dt;
				}
				assert(false);
			}
		}
		assert(false);
		return false;
	}

	virtual std::any visitAtom_expr(Python3Parser::Atom_exprContext *ctx) override {
		std::any down = visit(ctx->atom());
		if(auto *val = std::any_cast<std::pair<std::string,int>>(&down); val){
			if(val->second <= -7){
				return functionWork(function[val->second], ctx->trailer());
			}
			if(val->second >= -6 && val->second <= -2){
				return insideFunction(val->second, ctx->trailer());
			}
			return down;
		}
		assert(ctx->trailer() == nullptr);
		return down;
	}

	virtual std::any visitTrailer(Python3Parser::TrailerContext *ctx) override {
		return visitChildren(ctx);// I left it blank, for I will deal with it in functionWork().
	}

	virtual std::any visitAtom(Python3Parser::AtomContext *ctx) override {
		if(auto now = ctx->NAME(); now){
			std::string name = now->getText();
			if(!valuePosition(name)){
				assignValue(std::make_pair(std::any(false),variable_id),
						std::make_pair(std::any(0),0));
				variable_id++;
			}
			int pos = valuePosition(name);
			return std::make_pair(memory[depth][pos],pos);
		}
		if(auto now = ctx->NUMBER(); now){
			std::string val = now->getText();
			if(val.find('.') != std::string::npos){
				return std::make_pair(std::stod(val),-1);
			}
			else{
				return std::make_pair(int2048(val),-1);
			}
		}
		if(auto now = ctx->STRING(); !now.empty()){
			std::string ret;
			for(auto val : now){
				ret += removeQuotes(val->getText());
			}
			return ret;
		}
		if(auto now = ctx->NONE(); now){
			return std::make_pair("None",0);
		}
		if(auto now = ctx->TRUE(); now){
			return std::make_pair(true,0);
		}
		if(auto now = ctx->FALSE(); now){
			return std::make_pair(false,0);
		}
		if(auto now = ctx->test(); now){
			return visit(now);
		}
		if(auto now = ctx->format_string(); now){
			return visit(now);
		}
		assert(false);
		return visitChildren(ctx);
	}

	virtual std::any visitFormat_string(Python3Parser::Format_stringContext *ctx) override {
		auto fsl = ctx->FORMAT_STRING_LITERAL();
		auto ob = ctx->OPEN_BRACE();
		auto tl = ctx->testlist();
		int nsi = fsl.size(), np = 0, osi = ob.size();
		std::string ret;
		for(int i=0;i<nsi;i++){
			if(np != osi && fsl[i]->getSymbol()->getTokenIndex() > ob[i]->getSymbol()->getTokenIndex()){
				std::any val = unTie(visit(tl[i]));
				if(auto now = std::any_cast<std::string>(&val); now){
					ret += *now;
				}
				if(auto now = std::any_cast<int2048>(&val); now){
					ret += now->toString();
				}
				if(auto now = std::any_cast<double>(&val); now){
					ret += doubleToString(*now);
				}
				if(auto now = std::any_cast<bool>(&val); now){
					ret += (*now ? "True" : "False");
				}
				np++;
			}
			ret += removeQuotes(fsl[i]->getText());
		}
		return visitChildren(ctx);
	}

	virtual std::any visitTestlist(Python3Parser::TestlistContext *ctx) override {
		std::vector<std::any> ret;
		auto test = ctx->test();
		int nsi = test.size();
		for(int i=0;i<nsi;i++){
			ret.push_back(visit(test[i]));
		}
		return ret;
	}

	virtual std::any visitArglist(Python3Parser::ArglistContext *ctx) override {
		return visitChildren(ctx);// Same to trailer.
	}

	virtual std::any visitArgument(Python3Parser::ArgumentContext *ctx) override {
		return visitChildren(ctx);// Will be considered above.
	}
};

//hello!
#endif//PYTHON_INTERPRETER_EVALVISITOR_H
