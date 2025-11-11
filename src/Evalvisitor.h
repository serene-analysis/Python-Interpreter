#pragma once
#ifndef PYTHON_INTERPRETER_EVALVISITOR_H
#define PYTHON_INTERPRETER_EVALVISITOR_H


#include "Python3ParserBaseVisitor.h"
#include "int2048.h"
#include <iomanip>

//#define DEBUG
//#define DEBUG_comparison


using sjtu::int2048;
class EvalVisitor : public Python3ParserBaseVisitor {
	const static int kMaxn = 1e4+5;
	std::unordered_map<std::string,int>funcId;
	std::unordered_map<std::string,int>variableId;
	std::unordered_map<int,std::any>memory[kMaxn];
	std::unordered_map<int,bool>covered[kMaxn];
	std::unordered_map<int,Python3Parser::FuncdefContext*>function;
	int depth;
	int func_id = -7, variable_id = 1;
	// TODO: override all methods of Python3ParserBaseVisitor
	virtual std::any visitFile_input(Python3Parser::File_inputContext *ctx) override {
//#ifdef DEBUG
//std::cerr << "hello!" << '\n';
//#endif
		funcId["print"] = -2;
		funcId["int"] = -3;
		funcId["float"] = -4;
		funcId["str"] = -5;
		funcId["bool"] = -6;
		std::vector<Python3Parser::StmtContext *> son = ctx->stmt();
		auto newline = ctx->NEWLINE();
//#ifdef DEBUG
//printf("stmt.size() = %d, newline.size() = %d\n", (int)son.size(), (int)newline.size());
//if(ctx->EOF()){
//	printf("have EOF\n");
//}
//#endif
		for(auto now : son){
			visit(now);
		}
		return ctx->EOF();
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
#ifdef DEBUG
printf("visitStmt\n");
#endif
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

	std::any abstractize(std::any gave){
		std::pair<int2048,int> *got = std::any_cast<std::pair<int2048,int>>(&gave);
		std::pair<double,int> *got2 = std::any_cast<std::pair<double,int>>(&gave);
		std::pair<std::string,int> *got3 = std::any_cast<std::pair<std::string,int>>(&gave);
		std::pair<bool,int> *got4 = std::any_cast<std::pair<bool,int>>(&gave);
		std::pair<std::any,int> *got5 = std::any_cast<std::pair<std::any,int>>(&gave);
		if(got){
			std::any nv = got->first;
			return std::make_pair(nv,got->second);
		}
		if(got2){
			std::any nv = got2->first;
			return std::make_pair(nv,got2->second);
		}
		if(got3){
			std::any nv = got3->first;
			return std::make_pair(nv,got3->second);
		}
		if(got4){
			std::any nv = got4->first;
			return std::make_pair(nv,got4->second);
		}
		if(got5){
			return *got5;
		}
		assert(false);
		return false;
	}

	std::any concretize(std::any gave){
		std::pair<std::any,int> *must = std::any_cast<std::pair<std::any,int>>(&gave);
		if(!must){
			abstractize(gave);//Just to check the state
			return gave;
		}
#ifdef DEBUG
abstractize(gave);
#endif
		int2048 *got = std::any_cast<int2048>(&must->first);
		double *got2 = std::any_cast<double>(&must->first);
		std::string *got3 = std::any_cast<std::string>(&must->first);
		bool *got4 = std::any_cast<bool>(&must->first);
		if(got){
			return std::make_pair(*got,must->second);
		}
		if(got2){
			return std::make_pair(*got2,must->second);
		}
		if(got3){
			return std::make_pair(*got3,must->second);
		}
		if(got4){
			return std::make_pair(*got4,must->second);
		}
		assert(false);
		return false;
	}
	
	std::any unTie(std::any gave){
		gave = concretize(gave);
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
		if(variableId.count(name)){
			return variableId[name];
		}
		return 0;
	}

	void assignValue(std::pair<std::any,int> fir, std::pair<std::any,int> sec){
#ifdef DEBUG
printf("assignValue\n");
#endif
		int id = fir.second;
		for(int i=0;i<=depth;i++){
			if(!covered[i][id]){
				/*std::any gave = concretize(sec);
				std::pair<int2048,int> *got = std::any_cast<std::pair<int2048,int>>(&gave);
				std::pair<double,int> *got2 = std::any_cast<std::pair<double,int>>(&gave);
				std::pair<std::string,int> *got3 = std::any_cast<std::pair<std::string,int>>(&gave);
				std::pair<bool,int> *got4 = std::any_cast<std::pair<bool,int>>(&gave);
				if(got){
					memory[i][id] = got->first;
				}
				else if(got2){
					memory[i][id] = got2->first;
				}
				else if(got3){
					memory[i][id] = got3->first;
				}
				else if(got4){
					memory[i][id] = got4->first;
				}
				else{
					assert(false);
				}*/
				std::pair<std::any,int> gave = std::any_cast<std::pair<std::any,int>>(abstractize(sec));
				memory[i][id] = gave.first;
			}
		}
		return;
	}
	
	std::any addOrSub(std::any fir, std::any sec, std::string op){
#ifdef DEBUG
std::cout << "op = " << op << '\n';
#endif
		fir = unTie(fir), sec = unTie(sec);
		int2048 *gint = std::any_cast<int2048>(&fir), *sint = std::any_cast<int2048>(&sec);
		auto *gdouble = std::any_cast<double>(&fir), *sdouble = std::any_cast<double>(&sec);
		auto *gbool = std::any_cast<bool>(&fir), *sbool = std::any_cast<bool>(&sec);
		auto *fstring = std::any_cast<std::string>(&fir), *sstring = std::any_cast<std::string>(&sec);
		if(gint && sint){
			int2048 fi = *gint, si = *sint;
			if(op == "+"){
				return std::make_pair(fi + si,-1);
			}
			if(op == "-"){
				return std::make_pair(fi - si,-1);
			}
		}
		if(gdouble && sdouble){
			double fi = *gdouble, si = *sdouble;
			if(op == "+"){
				return std::make_pair(fi + si,-1);
			}
			if(op == "-"){
				return std::make_pair(fi - si,-1);
			}
		}
		if(gbool && sbool){
			bool fi = *gbool, si = *sbool;
			if(op == "+"){
				return std::make_pair(fi + si,-1);
			}
			if(op == "-"){
				return std::make_pair(fi - si,-1);
			}
		}
		if(fstring && sstring){
			assert(op == "+");
			return std::make_pair(*fstring + *sstring,-1);
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
				return std::make_pair(fi * si,-1);
			}
			if(op == "/" || op == "//"){
				return std::make_pair(fi / si,-1);
			}
			if(op == "%"){
				return std::make_pair(fi % si,-1);
			}
			assert(false);
			return false;
		}
		if(gdouble && sdouble){
			double fi = *gdouble, si = *sdouble;
			if(op == "*"){
				return std::make_pair(fi * si,-1);
			}
			if(op == "/"){
				return std::make_pair(fi / si,-1);
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
			return std::make_pair(ret,-1);
		}
		assert(false);
		return false;
	}

	virtual std::any visitExpr_stmt(Python3Parser::Expr_stmtContext *ctx) override {
#ifdef DEBUG
printf("visitExprstmt\n");
#endif
		std::vector<std::pair<std::any,int>>ret,lmem;
		if(!ctx->ASSIGN().empty()){
			auto testlist = ctx->testlist();
			int nsi = testlist.size();
			for(int i=nsi-1;i;i--){
				auto fir = testlist[i-1]->test(), sec = testlist[i]->test();
				int osi = fir.size();
				for(int j=0;j<osi;j++){
#ifdef DEBUG
printf("testlist:%d,%d\ntest:%d\n",i-1,i,j);
#endif
					std::any lef = abstractize(visit(fir[j])), rig = abstractize(visit(sec[j]));
					std::pair<std::any,int> *lefv = std::any_cast<std::pair<std::any,int>>(&lef),
											*rigv = std::any_cast<std::pair<std::any,int>>(&rig);
					if(!lefv || !rigv){
						assert(false);
					}
					lmem.push_back(*lefv);
					ret.push_back(*rigv);
				}
				for(int j=0;j<osi;j++){
					assignValue(lmem[j],ret[j]);
				}
			}
		}
		else if(!(!ctx->augassign())){
			auto testlist = ctx->testlist();
			auto aug = ctx->augassign();
			int nsi = testlist.size();
			auto fir = testlist[0]->test(), sec = testlist[1]->test();
			int osi = fir.size();
			std::any lef = visit(fir[0]), rig = visit(sec[0]);
			std::string op = std::any_cast<std::pair<std::string,int>>(visit(aug)).first;
			op.pop_back();
			if(op == "+" || op == "-"){
				assignValue(std::any_cast<std::pair<std::any,int>>(abstractize(lef)),
							std::any_cast<std::pair<std::any,int>>(abstractize(addOrSub(lef, rig, op))));
				int id = std::any_cast<std::pair<std::any,int>>(abstractize(lef)).second;
				ret.push_back(std::make_pair(memory[depth][id],id));
			}
			else{
				assignValue(std::any_cast<std::pair<std::any,int>>(abstractize(lef)),
							std::any_cast<std::pair<std::any,int>>(abstractize(mulDivMod(lef, rig, op))));
				int id = std::any_cast<std::pair<std::any,int>>(abstractize(lef)).second;
				ret.push_back(std::make_pair(memory[depth][id],id));
			}
		}
		else{
			return visit(ctx->testlist(0));
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
			return std::vector<std::pair<std::any,int>>{std::make_pair("return", -1), std::any_cast<std::pair<std::any,int>>(abstractize(visit(ret)))};
		}
		else{
			return std::vector<std::pair<std::any,int>>{std::make_pair("return", -1), std::make_pair((std::any)(0), 0)};
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
#ifdef DEBUG
printf("visitTest\n");
#endif
		return visit(ctx->or_test());
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
		if(got.size() == 1){
			return visit(got[0]);
		}
		int nsi = got.size();
		for(int i=0;i<nsi;i++){
			if(isTrue(visit(got[i]))){
				return std::make_pair(true,-1);
			}
		}
		return std::make_pair(false,-1);
	}

	virtual std::any visitAnd_test(Python3Parser::And_testContext *ctx) override {
		std::vector<Python3Parser::Not_testContext*> got = ctx->not_test();
		if(got.size() == 1){
			return visit(got[0]);
		}
		int nsi = got.size();
		for(int i=0;i<nsi;i++){
			if(isFalse(visit(got[i]))){
				return std::make_pair(false,-1);
			}
		}
		return std::make_pair(true,-1);
	}

	virtual std::any visitNot_test(Python3Parser::Not_testContext *ctx) override {
		Python3Parser::Not_testContext *ntest = ctx->not_test();
		Python3Parser::ComparisonContext *comp = ctx->comparison();
//#ifdef DEBUG
//printf("visitNotTest\n");
//#endif
		if(ntest){
//#ifdef DEBUG
//printf("not_test\n");
//#endif
			std::any ret = visit(ntest);
			if(ctx->NOT()){
				return std::make_pair(isFalse(ret),-1);
			}
			else{
				return ret;
			}
		}
		else{
//#ifdef DEBUG
//printf("comparison\n");
//#endif
			return visit(comp);
		}
	}

	bool compare(std::any fir, std::any sec, std::string op){
		fir = unTie(fir), sec = unTie(sec);
		int2048 *gint = std::any_cast<int2048>(&fir), *sint = std::any_cast<int2048>(&sec);
		auto *gdouble = std::any_cast<double>(&fir), *sdouble = std::any_cast<double>(&sec);
		auto *gbool = std::any_cast<bool>(&fir), *sbool = std::any_cast<bool>(&sec);
		if(gint && sint){
			int2048 fi = *gint, si = *sint;
#ifdef DEBUG_comparison
std::cout << "fi = " << fi << "\nsi = " << si << '\n';
#endif
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

	void checkState(std::any gave, std::string name = "default"){
		std::pair<int2048,int> *got = std::any_cast<std::pair<int2048,int>>(&gave);
		std::pair<double,int> *got2 = std::any_cast<std::pair<double,int>>(&gave);
		std::pair<std::string,int> *got3 = std::any_cast<std::pair<std::string,int>>(&gave);
		std::pair<bool,int> *got4 = std::any_cast<std::pair<bool,int>>(&gave);
		std::pair<std::any,int> *got5 = std::any_cast<std::pair<std::any,int>>(&gave);
		if(!got && !got2 && !got3 && !got4 && !got5){
			std::cout << "checkState error,name = " << name << '\n';
			assert(false);
		}
		return;
	}

	virtual std::any visitComparison(Python3Parser::ComparisonContext *ctx) override {
		std::vector<Python3Parser::Arith_exprContext*> ari = ctx->arith_expr();
		std::vector<Python3Parser::Comp_opContext*> comp = ctx->comp_op();
#ifdef DEBUG_comparison
printf("visitComparison\n");
#endif
		std::any fir = visit(ari[0]);
		if(comp.empty()){
//#ifdef DEBUG
//printf("no comparison\n");
//#endif
			//checkState(fir, "visitComparison");
			return fir;
		}
		int nsi = comp.size();
#ifdef DEBUG_comparison
printf("nsi = %d\n",nsi);
#endif
		for(int i=0;i<nsi;i++){
			std::any nv = visit(ari[i+1]);
			if(!compare(fir, nv, std::any_cast<std::string>(unTie(visit(comp[i]))))){
				return std::make_pair(false, -1);
			}
			fir = nv;
		}
		return std::make_pair(true, -1);
	}

	virtual std::any visitComp_op(Python3Parser::Comp_opContext *ctx) override {
		std::vector<antlr4::tree::TerminalNode*> all = {ctx->LESS_THAN(),
														ctx->GREATER_THAN(),
														ctx->EQUALS(),
														ctx->GT_EQ(),
														ctx->LT_EQ(),
														ctx->NOT_EQ_2()};
		if(all[0]){
			return std::make_pair(std::string("<"),-1);
		}
		if(all[1]){
			return std::make_pair(std::string(">"),-1);
		}
		if(all[2]){
			return std::make_pair(std::string("=="),-1);
		}
		if(all[3]){
			return std::make_pair(std::string(">="),-1);
		}
		if(all[4]){
			return std::make_pair(std::string("<="),-1);
		}
		if(all[5]){
			return std::make_pair(std::string("!="),-1);
		}
		assert(false);
		return false;
	}

	virtual std::any visitArith_expr(Python3Parser::Arith_exprContext *ctx) override {
		std::vector<Python3Parser::TermContext*> term = ctx->term();
		std::vector<Python3Parser::Addorsub_opContext*> op = ctx->addorsub_op();
#ifdef DEBUG
printf("visitArith_expr\n");
#endif
		std::any fir = visit(term[0]);
		int nsi = op.size();
		for(int i=0;i<nsi;i++){
			std::any nv = visit(term[i+1]);
#ifdef DEBUG
printf("i=%d\n",i);
#endif
			fir = addOrSub(fir, nv, std::any_cast<std::pair<std::string,int>>(visit(op[i])).first);
		}
		if(nsi == 1){
			//checkState(fir, "visitArith_expr");
		}
#ifdef DEBUG
printf("end : visitArith_expr\n");
#endif
		return fir;
	}

	virtual std::any visitAddorsub_op(Python3Parser::Addorsub_opContext *ctx) override {
		if(ctx->ADD()){
			return std::make_pair(std::string("+"),-1);
		}
		else if(ctx->MINUS()){
			return std::make_pair(std::string("-"),-1);
		}
		assert(false);
		return visitChildren(ctx);
	}

	virtual std::any visitTerm(Python3Parser::TermContext *ctx) override {
		std::vector<Python3Parser::FactorContext*> factor = ctx->factor();
		std::vector<Python3Parser::Muldivmod_opContext*> op = ctx->muldivmod_op();
		std::any fir = visit(factor[0]);
		int nsi = op.size();
		for(int i=0;i<nsi;i++){
			std::any nv = visit(factor[i+1]);
			fir = mulDivMod(fir, nv, std::any_cast<std::pair<std::string,int>>(visit(op[i])).first);
		}
		return fir;
	}

	virtual std::any visitMuldivmod_op(Python3Parser::Muldivmod_opContext *ctx) override {
		if(ctx->STAR()){
			return std::make_pair(std::string("*"),-1);
		}
		else if(ctx->DIV()){
			return std::make_pair(std::string("/"),-1);
		}
		else if(ctx->IDIV()){
			return std::make_pair(std::string("//"),-1);
		}
		else if(ctx->MOD()){
			return std::make_pair(std::string("%"),-1);
		}
		assert(false);
		return visitChildren(ctx);
	}

	std::any getVal(std::any ret){
		ret = unTie(ret);
		int2048 *gint = std::any_cast<int2048>(&ret);
		auto *gdouble = std::any_cast<double>(&ret);
		if(gint){
			return std::make_pair(*gint,-1);
		}
		if(gdouble){
			return std::make_pair(*gdouble,-1);
		}
		assert(false);
		return false;
	}

	std::any getNegativeVal(std::any ret){
		ret = unTie(ret);
		int2048 *gint = std::any_cast<int2048>(&ret);
		auto *gdouble = std::any_cast<double>(&ret);
		if(gint){
			return std::make_pair(-*gint,-1);
		}
		if(gdouble){
			return std::make_pair(-*gdouble,-1);
		}
		assert(false);
		return false;
	}

	virtual std::any visitFactor(Python3Parser::FactorContext *ctx) override {
		if(ctx->ADD()){
			return getVal(visit(ctx->factor()));
		}
		if(ctx->MINUS()){
			return getNegativeVal(visit(ctx->factor()));
		}
		std::any ret = visit(ctx->atom_expr());
		//checkState(ret, "visitFactor");
		return ret;
	}

	std::any functionWork(Python3Parser::FuncdefContext *fun, Python3Parser::TrailerContext *tra){
		auto argls = tra->arglist();
		if(argls){
			std::vector<Python3Parser::ArgumentContext*> arg = argls->argument();
			for(auto now : arg){
				std::vector<Python3Parser::TestContext*> test = now->test();
				if(test.size() != 1){
					auto gfir = abstractize(visit(test[0])), gsec = abstractize(visit(test[1]));
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
							std::make_pair(int2048(0),0));
					variable_id++;
				}
				int pos = valuePosition(name);
				assignValue(std::make_pair(memory[depth][pos],pos), 
							std::make_pair(std::any_cast<std::pair<std::any,int>>
								(abstractize(visit(arg[i]->test()[0]))).first,-1));
				covered[depth][pos] = true;
			}
			assert(arg.size() + test.size() >= var.size());
			for(int i=arg.size();i<var.size();i++){
				std::string name = removeQuotes(var[i]->NAME()->getText());
				if(!valuePosition(name)){
					assignValue(std::make_pair(std::any(false),variable_id),
							std::make_pair(int2048(0),0));
					variable_id++;
				}
				int pos = valuePosition(name);
				assignValue(std::make_pair(memory[depth][pos],pos), 
							std::make_pair(std::any_cast<std::pair<std::any,int>>
								(abstractize(visit(test[test.size()-(var.size()-i)]))).first,-1));
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
#ifdef DEBUG
printf("arg.size() = %d\n", (int)arg.size());
#endif
			for(auto now : arg){
				std::vector<Python3Parser::TestContext*> test = now->test();
#ifdef DEBUG
printf("test.size() = %d\n", (int)test.size());
#endif
				if(test.size() != 1){
					auto gfir = abstractize(visit(test[0])), gsec = abstractize(visit(test[1]));
					std::pair<std::any,int> *fir = std::any_cast<std::pair<std::any,int>>(&gfir),
											*sec = std::any_cast<std::pair<std::any,int>>(&gsec);
					assignValue(*fir, *sec);
				}
			}//because these five function all should have arguments
			if(id == -2){
				for(auto now : arg){
					std::vector<Python3Parser::TestContext*> test = now->test();
					auto dw = visit(test[0]);
#ifdef DEBUG
printf("print:dw got\n");
#endif
					auto fir = concretize(dw);
					if(std::pair<int2048,int> *dt = std::any_cast<std::pair<int2048,int>>(&fir); dt){
#ifdef DEBUG
printf("print:int2048\n");
#endif
						if(dt->second == 0){
							std::cout << "None" << ' ';
						}
						else{
							std::cout << dt->first << ' ';
						}
					}
					if(std::pair<double,int> *dt = std::any_cast<std::pair<double,int>>(&fir); dt){
#ifdef DEBUG
printf("print:double\n");
#endif
						std::cout << std::fixed << std::setprecision(6) << dt->first << ' ';
					}
					if(std::pair<std::string,int> *dt = std::any_cast<std::pair<std::string,int>>(&fir); dt){
#ifdef DEBUG
printf("print:string\n");
#endif
						std::cout << dt->first << ' ';
					}
					if(std::pair<bool,int> *dt = std::any_cast<std::pair<bool,int>>(&fir); dt){
#ifdef DEBUG
printf("print:bool\n");
#endif
						if(dt->first){
							std::cout << "True" << ' ';
						}
						else{
							std::cout << "False" << ' ';
						}
					}
				}
				std::cout << '\n';
				return std::make_pair(int2048(0), 0);
			}
			else if(id == -3){
				auto fir = concretize(visit(arg[0]->test()[0]));
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
				auto fir = concretize(visit(arg[0]->test()[0]));
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
				auto fir = concretize(visit(arg[0]->test()[0]));
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
				auto fir = concretize(visit(arg[0]->test()[0]));
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
#ifdef DEBUG
printf("visitAtomexpr\n");
#endif
		std::any down = visit(ctx->atom());
		if(auto *val = std::any_cast<std::pair<std::string,int>>(&down); val){
			if(val->second <= -7){
#ifdef DEBUG
printf("visitOuterFunction\n");
#endif
				return functionWork(function[val->second], ctx->trailer());
			}
			if(val->second >= -6 && val->second <= -2){
#ifdef DEBUG
printf("visitInsideFunction\n");
#endif
				return insideFunction(val->second, ctx->trailer());
			}
#ifdef DEBUG
printf("std::pair<std::string,int>\n");
#endif
			return down;
		}
		assert(ctx->trailer() == nullptr);
		//checkState(down, "visitAtom_expr");
		return down;
	}

	virtual std::any visitTrailer(Python3Parser::TrailerContext *ctx) override {
		return visitChildren(ctx);// I left it blank, for I will deal with it in functionWork().
	}

	virtual std::any visitAtom(Python3Parser::AtomContext *ctx) override {
#ifdef DEBUG
printf("visitAtom\n");
#endif
		if(auto now = ctx->NAME(); now){
			std::string name = now->getText();
#ifdef DEBUG
printf("name:"), std::cout << name << '\n';
#endif
			if(funcId.count(name)){
#ifdef DEBUG
printf("funcId = %d\n", funcId[name]);
#endif
				return std::make_pair(name, funcId[name]);
			}
			if(!valuePosition(name)){
#ifdef DEBUG
printf("new variable\n");
#endif
				assignValue(std::make_pair(std::any(false),variable_id),
						std::make_pair(int2048(0),0));
				variableId[name] = variable_id;
				variable_id++;
			}
#ifdef DEBUG
printf("maybe old variable\n");
#endif
			int pos = valuePosition(name);
#ifdef DEBUG
printf("variable value can be concretized\n");
#endif
			return std::make_pair(memory[depth][pos],pos);
		}
		if(auto now = ctx->NUMBER(); now){
#ifdef DEBUG
printf("number\n");
#endif
			std::string val = now->getText();
#ifdef DEBUG
std::cout << "val = " << val << '\n';
#endif
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
#ifdef DEBUG
printf("string:"), std::cout << ret << '\n';
#endif
			return std::make_pair(ret,-1);
		}
		if(auto now = ctx->NONE(); now){
#ifdef DEBUG
printf("atom:none\n");
#endif
			return std::make_pair(int2048(0),0);
		}
		if(auto now = ctx->TRUE(); now){
			return std::make_pair(true,-1);
		}
		if(auto now = ctx->FALSE(); now){
			return std::make_pair(false,-1);
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
		std::vector<std::pair<std::any,int>> ret;
		auto test = ctx->test();
		int nsi = test.size();
		for(int i=0;i<nsi;i++){
			ret.push_back(std::any_cast<std::pair<std::any,int>>(abstractize(visit(test[i]))));
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
/*
cd /home/enovo/Python-Interpreter

cd /home/enovo/Python-Interpreter/resources

chmod +x visualize.bash

bash visualize.bash file_input -gui
*/