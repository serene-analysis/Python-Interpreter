#pragma once
#ifndef PYTHON_INTERPRETER_EVALVISITOR_H
#define PYTHON_INTERPRETER_EVALVISITOR_H


#include "Python3ParserBaseVisitor.h"
#include "int2048.h"
#include <iomanip>

//#define DEBUG
//#define DEBUG_name
//#define DEBUG_abstractize
//#define DEBUG_concretize
//#define DEBUG_assignValue
//#define DEBUG_function_name
//#define DEBUG_comparison
//#define DEBUG_untie
//#define DEBUG_format_string


using sjtu::int2048;
class EvalVisitor : public Python3ParserBaseVisitor {
	const static int kMaxn = 1e4+5;
	std::unordered_map<std::string,int>funcId;
	std::unordered_map<std::string,int>variableId;
	std::unordered_map<int,std::any>memory[kMaxn];
	std::unordered_map<int,bool>covered[kMaxn];
	std::unordered_map<int,Python3Parser::FuncdefContext*>function;
	int depth = 0;
	int func_id = -7, variable_id = 1;
	// TODO: override all methods of Python3ParserBaseVisitor
	virtual std::any visitFile_input(Python3Parser::File_inputContext *ctx) override {
//#ifdef DEBUG
//std::cerr << "hello!" << std::endl;
//#endif
		std::ios::sync_with_stdio(true);
		funcId["print"] = -2;
		funcId["int"] = -3;
		funcId["float"] = -4;
		funcId["str"] = -5;
		funcId["bool"] = -6;
		std::vector<Python3Parser::StmtContext *> son = ctx->stmt();
		auto newline = ctx->NEWLINE();
//#ifdef DEBUG
//std::cout << "stmt.size() = %d, newline.size() = %d" << std::endl, (int)son.size(), (int)newline.size());
//if(ctx->EOF()){
//	std::cout << "have EOF" << std::endl);
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

	
	std::any abstractize(std::any gave){
		std::pair<int2048,int> *got = std::any_cast<std::pair<int2048,int>>(&gave);
		std::pair<double,int> *got2 = std::any_cast<std::pair<double,int>>(&gave);
		std::pair<std::string,int> *got3 = std::any_cast<std::pair<std::string,int>>(&gave);
		std::pair<bool,int> *got4 = std::any_cast<std::pair<bool,int>>(&gave);
		std::pair<std::any,int> *got5 = std::any_cast<std::pair<std::any,int>>(&gave);
		std::vector<std::pair<std::any,int>> *gvector = std::any_cast<std::vector<std::pair<std::any,int>>>(&gave);
		if(got){
#ifdef DEBUG_abstractize
std::cout << "abstractize : int2048" << std::endl;
#endif
			std::any nv = got->first;
			return std::make_pair(nv,got->second);
		}
		if(got2){
#ifdef DEBUG_abstractize
std::cout << "abstractize : double" << std::endl;
#endif
			std::any nv = got2->first;
			return std::make_pair(nv,got2->second);
		}
		if(got3){
#ifdef DEBUG_abstractize
std::cout << "abstractize : std::string" << std::endl;
#endif
			std::any nv = got3->first;
			return std::make_pair(nv,got3->second);
		}
		if(got4){
#ifdef DEBUG_abstractize
std::cout << "abstractize : bool" << std::endl;
#endif
			std::any nv = got4->first;
			return std::make_pair(nv,got4->second);
		}
		if(got5){
#ifdef DEBUG_abstractize
std::cout << "abstractize : any" << std::endl;
#endif
			return *got5;
		}
		assert(gvector && gvector->size() >= 1);
#ifdef DEBUG_abstractize
std::cout << "abstractize : gvector, size() = " << gvector->size() << std::endl;
#endif
		return (*gvector)[0];
	}

	std::any concretize(std::any gave){
		gave = abstractize(gave);
		std::pair<std::any,int> *must = std::any_cast<std::pair<std::any,int>>(&gave);
		int2048 *got = std::any_cast<int2048>(&must->first);
		double *got2 = std::any_cast<double>(&must->first);
		std::string *got3 = std::any_cast<std::string>(&must->first);
		bool *got4 = std::any_cast<bool>(&must->first);
		if(got){
#ifdef DEBUG_concretize
std::cout << "concretize : int2048" << std::endl;
#endif
			return std::make_pair(*got,must->second);
		}
		if(got2){
#ifdef DEBUG_concretize
std::cout << "concretize : double" << std::endl;
#endif
			return std::make_pair(*got2,must->second);
		}
		if(got3){
#ifdef DEBUG_concretize
std::cout << "concretize : std::string" << std::endl;
#endif
			return std::make_pair(*got3,must->second);
		}
		if(got4){
#ifdef DEBUG_concretize
std::cout << "concretize : bool" << std::endl;
#endif
			return std::make_pair(*got4,must->second);
		}
		assert(false);
		return false;
	}
	
	std::any unTie(std::any gave, std::string pos){
#ifdef DEBUG
std::cout << "unTie! pos = " << pos << std::endl;
#endif
		gave = concretize(gave);
		auto *gint = std::any_cast<std::pair<int2048,int>>(&gave);
		auto *gdouble = std::any_cast<std::pair<double,int>>(&gave);
		auto *gbool = std::any_cast<std::pair<bool,int>>(&gave);
		auto *gstring = std::any_cast<std::pair<std::string,int>>(&gave);
		auto *gvector = std::any_cast<std::vector<std::pair<std::any,int>>>(&gave);
		if(gint){
#ifdef DEBUG_untie
std::cout << "unTie:int2048" << std::endl;
#endif
			return gint->first;
		}
		if(gdouble){
#ifdef DEBUG_untie
std::cout << "unTie:double" << std::endl;
#endif
			return gdouble->first;
		}
		if(gbool){
#ifdef DEBUG_untie
std::cout << "unTie:bool" << std::endl;
#endif
			return gbool->first;
		}
		if(gstring){
#ifdef DEBUG_untie
std::cout << "unTie:std::string" << std::endl;
#endif
			return gstring->first;
		}
		if(gvector){
#ifdef DEBUG_untie
std::cout << "unTie:vector" << std::endl;
#endif
			return (*gvector)[0].first;
		}
		assert(false);
		return gave;
	}

	virtual std::any visitFuncdef(Python3Parser::FuncdefContext *ctx) override {
		funcId[ctx->NAME()->getText()] = func_id;
		function[func_id] = ctx;
		func_id --;
		return std::make_pair(std::any((int2048)(0)),0);
	}

	virtual std::any visitParameters(Python3Parser::ParametersContext *ctx) override {
		return visitChildren(ctx);//Do nothing, will be done in functionWork()
	}

	virtual std::any visitTypedargslist(Python3Parser::TypedargslistContext *ctx) override {
		return visitChildren(ctx);//Do nothing, will be done in functionWork()
	}

	virtual std::any visitTfpdef(Python3Parser::TfpdefContext *ctx) override {
		antlr4::tree::TerminalNode *name = ctx->NAME();
		return name->getText();// Is this right?
	}

	virtual std::any visitStmt(Python3Parser::StmtContext *ctx) override {
#ifdef DEBUG
std::cout << "visitStmt" << std::endl;
#endif
		Python3Parser::Simple_stmtContext *simple = ctx->simple_stmt();
		Python3Parser::Compound_stmtContext *compound = ctx->compound_stmt();
		if(simple){
			std::any ret = visit(simple);
#ifdef DEBUG
std::cout << "visitStmt end" << std::endl;
#endif
			return ret;
		}
		else{
			std::any ret = visit(compound);
#ifdef DEBUG
std::cout << "visitStmt end" << std::endl;
#endif
			return ret;
		}
	}

	virtual std::any visitSimple_stmt(Python3Parser::Simple_stmtContext *ctx) override {
		Python3Parser::Small_stmtContext *small = ctx->small_stmt();
		std::any ret = visit(small);
#ifdef DEBUG
unTie(ret, "visitSimple_Stmt");
abstractize(ret);
std::cout << "visitSimpleStmt end" << std::endl;
#endif
		return ret;
	}

	virtual std::any visitSmall_stmt(Python3Parser::Small_stmtContext *ctx) override {
		Python3Parser::Expr_stmtContext *expr = ctx->expr_stmt();
		Python3Parser::Flow_stmtContext *flow = ctx->flow_stmt();
		if(expr){
			return visit(expr);
		}
		else{
			std::any ret = visit(flow);
#ifdef DEBUG
std::cout << "visitSmallStmt : flow,end" << std::endl;
#endif
			return ret;
		}
	}
	
	bool isString(std::any ret, const std::string &str){
		ret = concretize(ret);
		auto *dt = std::any_cast<std::pair<std::string,int>>(&ret);
		if(!dt){
#ifdef DEBUG
std::cout << "isString : not even a string" << std::endl;
#endif
		}
		return dt && dt->first == str && dt->second == 0; // Need it to be a flow manu
	}

	int valuePosition(std::string name){
		if(variableId.count(name)){
			return variableId[name];
		}
		return 0;
	}

	std::string findName(int id){
		for(auto now : variableId){
			if(now.second == id){
				return now.first;
			}
		}
		assert(false);
		return "";
	}

	void assignValue(std::pair<std::any,int> fir, std::pair<std::any,int> sec){
		//concretize(sec);
		assert(fir.second >= 1);
#ifdef DEBUG_assignValue
std::cout << "assignValue, variable_id = " << fir.second << std::endl;
std::cout << "depth = " << depth << std::endl;
std::cout << findName(fir.second) << std::endl;
#endif
		int id = fir.second;
		if(covered[depth][id]){
#ifdef DEBUG_assignValue
std::cout << "covered = true" << std::endl;
#endif
			std::any gave = concretize(sec);
			std::pair<int2048,int> *got = std::any_cast<std::pair<int2048,int>>(&gave);
			std::pair<double,int> *got2 = std::any_cast<std::pair<double,int>>(&gave);
			std::pair<std::string,int> *got3 = std::any_cast<std::pair<std::string,int>>(&gave);
			std::pair<bool,int> *got4 = std::any_cast<std::pair<bool,int>>(&gave);
			if(got){
#ifdef DEBUG_assignValue
std::cout << "assign int2048" << ", val = " << got->first << std::endl;
#endif
				memory[depth][id] = got->first;
			}
			else if(got2){
#ifdef DEBUG_assignValue
std::cout << "assign double" << ", val = " << got2->first << std::endl;
#endif
				memory[depth][id] = got2->first;
			}
			else if(got3){
#ifdef DEBUG_assignValue
std::cout << "assign std::string" << ", val = " << got3->first << std::endl;
#endif
				memory[depth][id] = got3->first;
			}
			else if(got4){
#ifdef DEBUG_assignValue
std::cout << "assign bool" << ", val = " << got4->first << std::endl;
#endif
				memory[depth][id] = got4->first;
			}
			else{
				assert(false);
			}
		}
		else{
#ifdef DEBUG_assignValue
std::cout << "covered = false" << std::endl;
#endif
			std::any gave = concretize(sec);
			for(int i=0;i<=depth;i++){
				if(i == 0 && depth != 0){
					assert(memory[0].count(id));
				}
				if(!covered[i][id]){
					std::pair<int2048,int> *got = std::any_cast<std::pair<int2048,int>>(&gave);
					std::pair<double,int> *got2 = std::any_cast<std::pair<double,int>>(&gave);
					std::pair<std::string,int> *got3 = std::any_cast<std::pair<std::string,int>>(&gave);
					std::pair<bool,int> *got4 = std::any_cast<std::pair<bool,int>>(&gave);
					if(got){
#ifdef DEBUG_assignValue
std::cout << "assign int2048" << ", val = " << got->first << std::endl;
#endif
						memory[i][id] = got->first;
					}
					else if(got2){
#ifdef DEBUG_assignValue
std::cout << "assign double" << ", val = " << got2->first << std::endl;
#endif
						memory[i][id] = got2->first;
					}
					else if(got3){
#ifdef DEBUG_assignValue
std::cout << "assign std::string" << ", val = " << got3->first << std::endl;
#endif
						memory[i][id] = got3->first;
					}
					else if(got4){
#ifdef DEBUG_assignValue
std::cout << "assign bool" << ", val = " << got4->first << std::endl;
#endif
						memory[i][id] = got4->first;
					}
					else{
						assert(false);
					}
					//std::pair<std::any,int> gave = std::any_cast<std::pair<std::any,int>>(abstractize(sec));
					//memory[i][id] = gave.first;
				}
			}
		}
		return;
	}
	
	std::any addOrSub(std::any fir, std::any sec, std::string op){
#ifdef DEBUG
std::cout << "op = " << op << std::endl;
#endif
		fir = unTie(fir, "addOrSub"), sec = unTie(sec, "addOrSub");
		int2048 *gint = std::any_cast<int2048>(&fir), *sint = std::any_cast<int2048>(&sec);
		auto *gdouble = std::any_cast<double>(&fir), *sdouble = std::any_cast<double>(&sec);
		auto *gbool = std::any_cast<bool>(&fir), *sbool = std::any_cast<bool>(&sec);
		auto *fstring = std::any_cast<std::string>(&fir), *sstring = std::any_cast<std::string>(&sec);
		if(gbool){
			int2048 nv(*gbool);
			gint = &nv;
		}
		if(sbool){
			int2048 nv(*sbool);
			sint = &nv;
		}
		if(gint && sint){
			int2048 fi = *gint, si = *sint;
			if(op == "+"){
#ifdef DEBUG_function_name
std::cout << "add, ans = " << fi + si << std::endl;
#endif
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
		if(gint){
			int2048 lv = *gint;
			if(sdouble){
				if(op == "+"){
					return std::make_pair(lv.toDouble() + *sdouble,-1);
				}
				else{
					return std::make_pair(lv.toDouble() - *sdouble,-1);
				}
			}
		}
		if(gdouble){
			double lv = *gdouble;
			if(sint){
				if(op == "+"){
					return std::make_pair(lv + sint->toDouble(),-1);
				}
				else{
					return std::make_pair(lv - sint->toDouble(),-1);
				}
			}
		}
		assert(false);
		return false;
	}
	
	std::any mulDivMod(std::any fir, std::any sec, std::string op){
#ifdef DEBUG
std::cout << "mulDivMod : op = " << op << std::endl;
#endif
		fir = unTie(fir, "mulDivMod"), sec = unTie(sec, "mulDivMod");
		int2048 *gint = std::any_cast<int2048>(&fir), *sint = std::any_cast<int2048>(&sec);
		auto *gdouble = std::any_cast<double>(&fir), *sdouble = std::any_cast<double>(&sec);
		auto *gbool = std::any_cast<bool>(&fir), *sbool = std::any_cast<bool>(&sec);
		auto *fstring = std::any_cast<std::string>(&fir), *sstring = std::any_cast<std::string>(&sec);
		if(gbool){
			int2048 nv(*gbool);
			gint = &nv;
		}
		if(sbool){
			int2048 nv(*sbool);
			sint = &nv;
		}
		if(gint && sint){
			int2048 fi = *gint, si = *sint;
			if(op == "*"){
				return std::make_pair(fi * si,-1);
			}
			if(op == "/"){
				return std::make_pair(double(fi.toDouble() / si.toDouble()),-1);
			}
			if(op == "//"){
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
			if(op == "//"){
				return std::make_pair(std::floor(fi / si),-1);
			}
			assert(false);
			return false;
		}
		if(fstring && sint){
#ifdef DEBUG
std::cout << "op = " << op << std::endl;
#endif
			assert(op == "*");
			std::string ret = *fstring;
			if(*sint == 0){
				ret = std::string();
			}
			for(int i=1;i<*sint;i++){
				ret += *fstring;
			}
			return std::make_pair(ret,-1);
		}
		if(gint){
			int2048 lv = *gint;
			if(sdouble){
				if(op == "*"){
					return std::make_pair(lv.toDouble() * *sdouble,-1);
				}
				else if(op == "/"){
					return std::make_pair(lv.toDouble() / *sdouble,-1);
				}
				else if(op == "//"){
					return std::make_pair(std::floor(lv.toDouble() / *sdouble),-1);
				}
			}
		}
		if(gdouble){
			double lv = *gdouble;
			if(sint){
				if(op == "+"){
					return std::make_pair(lv * sint->toDouble(),-1);
				}
				else if(op == "/"){
					return std::make_pair(lv / sint->toDouble(),-1);
				}
				else{
					return std::make_pair(std::floor(lv / sint->toDouble()),-1);
				}
			}
		}
		assert(false);
		return false;
	}

	virtual std::any visitExpr_stmt(Python3Parser::Expr_stmtContext *ctx) override {
#ifdef DEBUG_function_name
std::cout << "visitExprstmt" << std::endl;
#endif
		std::vector<std::pair<std::any,int>>ret,lmem;
		if(!ctx->ASSIGN().empty()){
#ifdef DEBUG
std::cout << "assign" << std::endl;
#endif
			auto testlist = ctx->testlist();
			int nsi = testlist.size();
			for(int i=nsi-1;i;i--){
				auto fir = testlist[i-1]->test(), sec = testlist[i]->test();
				int osi = fir.size();
				for(int j=0;j<osi;j++){
#ifdef DEBUG
std::cout << "testlist:" << i << "\ntest" << std::endl;
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
					//concretize(ret[j]);
					assignValue(lmem[j],ret[j]);
				}
#ifdef DEBUG
std::cout << "testlist:" << i << "\nvalue assign end" << std::endl;
#endif
			}
			return ret;
		}
		else if(!(!ctx->augassign())){
			auto testlist = ctx->testlist();
			auto aug = ctx->augassign();
			int nsi = testlist.size();
			auto fir = testlist[0]->test(), sec = testlist[1]->test();
			int osi = fir.size();
			std::any lef = visit(fir[0]), rig = visit(sec[0]);
			std::string op = std::any_cast<std::pair<std::string,int>>(visit(aug)).first;
#ifdef DEBUG_function_name
std::cout << "augassign = " << op[0] << std::endl;
#endif
			op.pop_back();
			if(op == "+" || op == "-"){
				std::pair<std::any,int> lv = std::any_cast<std::pair<std::any,int>>(abstractize(lef));
				assignValue(lv,std::any_cast<std::pair<std::any,int>>(abstractize(addOrSub(lef, rig, op))));
				int id = lv.second;
				ret.push_back(std::make_pair(memory[depth][id],id));
			}
			else{
				std::pair<std::any,int> lv = std::any_cast<std::pair<std::any,int>>(abstractize(lef));
				assignValue(lv,std::any_cast<std::pair<std::any,int>>(abstractize(mulDivMod(lef, rig, op))));
				int id = lv.second;
				ret.push_back(std::make_pair(memory[depth][id],id));
			}
			return ret;
		}
		else{
#ifdef DEBUG
std::cout << "expr_stmt, single element" << std::endl;
#endif
			std::any ret = visit(ctx->testlist(0));
#ifdef DEBUG
std::cout << "expr_stmt, single element end" << std::endl;
#endif
			return ret;
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
			return std::make_pair(std::string("+="),-1);
		}
		if(sub){
			return std::make_pair(std::string("-="),-1);
		}
		if(mult){
			return std::make_pair(std::string("*="),-1);
		}
		if(div){
			return std::make_pair(std::string("/="),-1);
		}
		if(idiv){
			return std::make_pair(std::string("//="),-1);
		}
		return std::make_pair(std::string("%="),-1);
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
		return std::make_pair(std::string("break"),-1);
	}

	virtual std::any visitContinue_stmt(Python3Parser::Continue_stmtContext *ctx) override {
		return std::make_pair(std::string("continue"),-1);
	}

	virtual std::any visitReturn_stmt(Python3Parser::Return_stmtContext *ctx) override {
		Python3Parser::TestlistContext *testlist = ctx->testlist();
		if(testlist){
			std::vector<std::pair<std::any,int>> ret, got = std::any_cast<std::vector<std::pair<std::any,int>>>(visit(testlist));
			ret.push_back(std::make_pair(std::any(std::string("return")), 0));
			for(auto now : got){
				ret.push_back(now);
			}
#ifdef DEBUG
std::cout << "ReturnStmt : ret.size () = " << ret.size() << std::endl;
std::cout << "ReturnStmt end" << std::endl;
#endif
			return ret;
		}
		else{
			return std::vector<std::pair<std::any,int>>{std::make_pair(std::any(std::string("return")), 0), std::make_pair((std::any)((int2048)(0)), 0)};
		}
	}

	virtual std::any visitCompound_stmt(Python3Parser::Compound_stmtContext *ctx) override {
#ifdef DEBUG
std::cout << "\nvisitCompoundStmt\n" << std::endl;
#endif
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
		ret = unTie(ret, "isTrue");
		int2048 *gint = std::any_cast<int2048>(&ret);
		auto *gdouble = std::any_cast<double>(&ret);
		auto *gbool = std::any_cast<bool>(&ret);
		assert(gint || gdouble || gbool);
		return (gint && *gint != 0) || (gdouble && *gdouble) || (gbool && *gbool);
	}
	
	virtual std::any visitIf_stmt(Python3Parser::If_stmtContext *ctx) override {
#ifdef DEBUG
std::cout << "visitIf_stmt" << std::endl;
#endif
		Python3Parser::TestContext *fir = ctx->test(0);
		if(isTrue(visit(fir))){
#ifdef DEBUG
std::cout << "\nvisitIf_stmt if condition satisfied\n" << std::endl;
#endif
			return visit(ctx->suite(0));
		}
		else{
#ifdef DEBUG
std::cout << "\nvisitIf_stmt if condition not satisfied\n" << std::endl;
#endif
			auto branch = ctx->test();
			branch.assign(std::next(branch.begin()),branch.end());
#ifdef DEBUG
std::cout << "branch.size() = " << branch.size() << std::endl;
#endif
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
#ifdef DEBUG
std::cout << "\nvisitIf_stmt going into else\n" << std::endl;
#endif
				return visit(ctx->suite().back());
			}
		}
		return std::make_pair(std::string("if nothing satisfied"),-1);
	}

	bool isReturn(std::any gave){
#ifdef DEBUG
std::cout << "isReturn?" << std::endl;
#endif
		std::vector<std::pair<std::any,int>> *got = std::any_cast<std::vector<std::pair<std::any,int>>>(&gave);
		if(got){
#ifdef DEBUG
std::cout << "isReturn, vector" << std::endl;
#endif
			auto ret = *got;
			return isString(ret[0], "return");
		}
		return false;
	}

	virtual std::any visitWhile_stmt(Python3Parser::While_stmtContext *ctx) override {
#ifdef DEBUG_function_name
std::cout << "visitWhile_stmt" << std::endl;
#endif
		while(true){
#ifdef DEBUG_function_name
std::cout << "While_stmt going on" << std::endl;
#endif
			Python3Parser::TestContext *con = ctx->test();
			std::any got = visit(con);
			if(!isTrue(got)){
				return std::make_pair(std::string("while condition unsatisfied"),-1);
			}
			std::any ret = visit(ctx->suite());
			if(isString(ret,"break")){
#ifdef DEBUG_function_name
std::cout << "while break" << std::endl;
#endif
				break;
			}
			if(isReturn(ret)){
#ifdef DEBUG_function_name
std::cout << "while return" << std::endl;
#endif
				return ret;
			}
		}
		return std::make_pair(std::string("while end"),-1);
	}

	virtual std::any visitSuite(Python3Parser::SuiteContext *ctx) override {
#ifdef DEBUG
std::cout << "visitSuite" << std::endl;
#endif
		Python3Parser::Simple_stmtContext *simple = ctx->simple_stmt();
		if(simple){
			std::any ret = visit(simple);
#ifdef DEBUG
std::cout << "visitSuite : simple, end" << std::endl;
#endif
			return ret;
		}
		else{
			auto stmt = ctx->stmt();
			for(Python3Parser::StmtContext *now : stmt){
				std::any ret = visit(now);
				if(isString(ret, "continue")){
					return std::make_pair(std::string("continue"),-1);
				}
				if(isString(ret, "break")){
					return std::make_pair(std::string("break"),-1);
				}
				if(isReturn(ret)){
					return ret;
				}
			}
#ifdef DEBUG
std::cout << "visitSuite : unflowed, end" << std::endl;
#endif
			return std::make_pair(std::string("suite unflowed end"),-1);
		}
	}

	virtual std::any visitTest(Python3Parser::TestContext *ctx) override {
#ifdef DEBUG
std::cout << "visitTest" << std::endl;
#endif
		std::any ret = visit(ctx->or_test());
#ifdef DEBUG
//unTie(ret, "visitTest");
std::cout << "visitTest end" << std::endl;
#endif
		return ret;
	}

	bool isFalse(std::any ret){
		ret = unTie(ret, "isFalse");
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
//std::cout << "visitNotTest" << std::endl);
//#endif
		if(ntest){
//#ifdef DEBUG
//std::cout << "not_test" << std::endl);
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
//std::cout << "comparison" << std::endl);
//#endif
			return visit(comp);
		}
	}

	
	std::any secondUnTie(std::any gave, std::string pos){
#ifdef DEBUG
std::cout << "unTie! pos = " << pos << std::endl;
#endif
		gave = concretize(gave);
		auto *gint = std::any_cast<std::pair<int2048,int>>(&gave);
		auto *gdouble = std::any_cast<std::pair<double,int>>(&gave);
		auto *gbool = std::any_cast<std::pair<bool,int>>(&gave);
		auto *gstring = std::any_cast<std::pair<std::string,int>>(&gave);
		auto *gvector = std::any_cast<std::vector<std::pair<std::any,int>>>(&gave);
		if(gint){
#ifdef DEBUG_untie
std::cout << "unTie:int2048" << std::endl;
#endif
			return gint->second;
		}
		if(gdouble){
#ifdef DEBUG_untie
std::cout << "unTie:double" << std::endl;
#endif
			return gdouble->second;
		}
		if(gbool){
#ifdef DEBUG_untie
std::cout << "unTie:bool" << std::endl;
#endif
			return gbool->second;
		}
		if(gstring){
#ifdef DEBUG_untie
std::cout << "unTie:std::string" << std::endl;
#endif
			return gstring->second;
		}
		if(gvector){
#ifdef DEBUG_untie
std::cout << "unTie:vector" << std::endl;
#endif
			return (*gvector)[0].second;
		}
		assert(false);
		return gave;
	}

	bool compare(std::any mfir, std::any msec, std::string op){
		std::any fir = unTie(mfir, "compare"), sec = unTie(msec, "compare");
		int2048 *gint = std::any_cast<int2048>(&fir), *sint = std::any_cast<int2048>(&sec);
		auto *gdouble = std::any_cast<double>(&fir), *sdouble = std::any_cast<double>(&sec);
		auto *gbool = std::any_cast<bool>(&fir), *sbool = std::any_cast<bool>(&sec);
		auto *gstring = std::any_cast<std::string>(&fir), *sstring = std::any_cast<std::string>(&sec);
		mfir = secondUnTie(mfir, "compare"), msec = secondUnTie(msec, "compare");
		auto firnone = std::any_cast<int>(&mfir), secnone = std::any_cast<int>(&msec);
		if(firnone && *firnone == 0){
			if(op == "=="){
				return secnone && *secnone == 0;
			}
			else{
				return !secnone || secnone != 0;
			}
		}
		if(secnone && *secnone == 0){
			if(op == "=="){
				return firnone && firnone == 0;
			}
			else{
				return !firnone || firnone != 0;
			}
		}
		if(gbool){
			int2048 nv(*gbool);
			gint = &nv;
		}
		if(sbool){
			int2048 nv(*sbool);
			sint = &nv;
		}
		if(gint && sint){
			int2048 fi = *gint, si = *sint;
//#ifdef DEBUG_comparison
//std::cout << "int2048,fi = " << fi << "\nsi = " << si << std::endl;
//#endif
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
#ifdef DEBUG_comparison
std::cout << "double,fi = " << fi << "\nsi = " << si << std::endl;
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
		if(gstring && sstring){
			std::string fi = *gstring, si = *sstring;
#ifdef DEBUG_comparison
std::cout << "std::string,fi = " << fi << "\nsi = " << si << std::endl;
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
		if(gint){
			int2048 lv = *gint;
			if(sdouble){
				if(op == "<"){
					return (lv < (long long)(std::ceil(*sdouble)) - 1) || (lv.toDouble() < *sdouble);
				}
				if(op == "<="){
					return (lv < (long long)(std::ceil(*sdouble)) - 1) || (lv.toDouble() <= *sdouble);
				}
				if(op == ">"){
					return (lv > (long long)(std::ceil(*sdouble))) || (lv.toDouble() > *sdouble);
				}
				if(op == "<="){
					return (lv > (long long)(std::ceil(*sdouble))) || (lv.toDouble() >= *sdouble);
				}
				if(op == "=="){
					return (lv.toDouble() == *sdouble);
				}
				return (lv.toDouble() != *sdouble);
			}
		}
		if(gdouble){
			double lv = *gdouble;
			if(sint){
				std::swap(gint, sint), std::swap(gdouble, sdouble);
				int2048 lv = *gint;
				if(sdouble){
					if(op == "<"){
						return (lv < (long long)(std::ceil(*sdouble)) - 1) || (lv.toDouble() < *sdouble);
					}
					if(op == "<="){
						return (lv < (long long)(std::ceil(*sdouble)) - 1) || (lv.toDouble() <= *sdouble);
					}
					if(op == ">"){
						return (lv > (long long)(std::ceil(*sdouble))) || (lv.toDouble() > *sdouble);
					}
					if(op == "<="){
						return (lv > (long long)(std::ceil(*sdouble))) || (lv.toDouble() >= *sdouble);
					}
					if(op == "=="){
						return (lv.toDouble() == *sdouble);
					}
					return (lv.toDouble() != *sdouble);
				}
			}
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
			std::cout << "checkState error,name = " << name << std::endl;
			assert(false);
		}
		return;
	}

	virtual std::any visitComparison(Python3Parser::ComparisonContext *ctx) override {
		std::vector<Python3Parser::Arith_exprContext*> ari = ctx->arith_expr();
		std::vector<Python3Parser::Comp_opContext*> comp = ctx->comp_op();
#ifdef DEBUG_comparison
std::cout << "visitComparison" << std::endl;
#endif
		std::any fir = visit(ari[0]);
		if(comp.empty()){
#ifdef DEBUG
std::cout << "comparison end, single element" << std::endl;
#endif
			//checkState(fir, "visitComparison");
			return fir;
		}
		int nsi = comp.size();
#ifdef DEBUG_comparison
std::cout << "nsi = " << nsi << std::endl;
#endif
		for(int i=0;i<nsi;i++){
			std::any nv = visit(ari[i+1]);
			std::string ncomp = std::any_cast<std::string>(unTie(visit(comp[i]), "visitComparison"));
#ifdef DEBUG_comparison
std::cout << "ncomp = " << ncomp << std::endl;
abstractize(fir),abstractize(nv);
#endif
			if(!compare(fir, nv, ncomp)){
#ifdef DEBUG_comparison
std::cout << "comparison end,false" << std::endl;
#endif
				return std::make_pair(false, -1);
			}
			fir = nv;
		}
#ifdef DEBUG_comparison
std::cout << "comparison end,true" << std::endl;
#endif
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
std::cout << "visitArith_expr" << std::endl;
#endif
		std::any fir = visit(term[0]);
		int nsi = op.size();
		for(int i=0;i<nsi;i++){
			std::any nv = visit(term[i+1]);
#ifdef DEBUG
std::cout << "i = " << i << std::endl;
#endif
			fir = addOrSub(fir, nv, std::any_cast<std::pair<std::string,int>>(visit(op[i])).first);
		}
		if(nsi == 1){
			//checkState(fir, "visitArith_expr");
		}
#ifdef DEBUG
std::cout << "end : visitArith_expr" << std::endl;
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
#ifdef DEBUG
std::cout << "visiting term at the first time" << std::endl;
//unTie(fir, "visitTerm");
#endif
		int nsi = op.size();
		for(int i=0;i<nsi;i++){
			std::any nv = visit(factor[i+1]);
#ifdef DEBUG
std::cout << "visiting term, i = " << i << std::endl;
unTie(nv, "visitTerm");
#endif
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
		ret = unTie(ret, "getVal");
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
		ret = unTie(ret, "getNegativeVal");
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
		std::vector<std::pair<std::any,int>>mem;
		if(argls){
			std::vector<Python3Parser::ArgumentContext*> arg = argls->argument();
			for(auto now : arg){
				std::vector<Python3Parser::TestContext*> test = now->test();
				if(test.size() != 1){
					auto gfir = abstractize(visit(test[0])), gsec = abstractize(visit(test[1]));
					std::pair<std::any,int> *fir = std::any_cast<std::pair<std::any,int>>(&gfir),
											*sec = std::any_cast<std::pair<std::any,int>>(&gsec);
					assignValue(*fir, *sec);
					mem.push_back(*sec);
				}
				else{
					mem.push_back(std::any_cast<std::pair<std::any,int>>(abstractize(visit(test[0]))));
				}
			}
		}
		depth++;
		memory[depth] = memory[0], covered[depth].clear();
		auto targls = fun->parameters()->typedargslist();
		if(targls){
			auto var = targls->tfpdef();
			auto test = targls->test();
			auto argls = tra->arglist();
			std::vector<Python3Parser::ArgumentContext*> arg;
			if(argls){
				arg = argls->argument();
			}
//#ifdef DEBUG_assignValue
//std::cout << "arg.size() = " << arg.size() << std::endl;
//#endif
			for(int i=0;i<arg.size();i++){
				std::string name = var[i]->NAME()->getText();
				if(!valuePosition(name)){
					variableId[name] = variable_id;
					covered[depth][variable_id] = true;
					assignValue(std::make_pair(std::any(false),variable_id),
							std::make_pair(int2048(0),0));
					variable_id++;
				}
				int pos = valuePosition(name);
				covered[depth][pos] = true;
				assignValue(std::make_pair(memory[depth][pos],pos), mem[i]);
			}
			assert(arg.size() + test.size() >= var.size());
			for(int i=arg.size();i<var.size();i++){
				std::string name = var[i]->NAME()->getText();
				if(!valuePosition(name)){
					variableId[name] = variable_id;
					covered[depth][variable_id] = true;
					assignValue(std::make_pair(std::any(false),variable_id),
							std::make_pair(int2048(0),0));
					variable_id++;
				}
				int pos = valuePosition(name);
				covered[depth][pos] = true;
				assignValue(std::make_pair(memory[depth][pos],pos), 
							std::make_pair(std::any_cast<std::pair<std::any,int>>
								(abstractize(visit(test[test.size()-(var.size()-i)]))).first,-1));
			}
		}
		std::any ret = visit(fun->suite());
		if(isReturn(ret)){
			std::vector<std::pair<std::any,int>> *got = std::any_cast<std::vector<std::pair<std::any,int>>>(&ret);
			if(got){
				auto now = *got;
				now.assign(std::next(now.begin()),now.end());
				ret = now;
			}
			else{
				assert(false);
			}
		}
#ifdef DEBUG_name
std::cout << "\nfunctionWork end\n" << std::endl;
#endif
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
#ifdef DEBUG_format_string
std::cout << "v = " << v << std::endl;
#endif
			int nv = v / down;
			ret += nv + '0';
			v -= down * nv, v *= 10;
		}
		return ret;
	}

	std::any insideFunction(int id, Python3Parser::TrailerContext *tra){
		auto argls = tra->arglist();
		if(!argls){
			assert(id == -2);//only print can have no args?
			std::cout << std::endl;
			return std::make_pair(std::string("insideFunction:Printed an empty arglist"),-1);
		}
		else{
			std::vector<Python3Parser::ArgumentContext*> arg = argls->argument();
#ifdef DEBUG
std::cout << "arg.size() = " << (int)arg.size() << std::endl;
#endif
			for(auto now : arg){
				std::vector<Python3Parser::TestContext*> test = now->test();
#ifdef DEBUG
std::cout << "test.size() = " << (int)test.size() << std::endl;
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
std::cout << "print:dw got" << std::endl;
#endif
					auto fir = concretize(dw);
					if(std::pair<int2048,int> *dt = std::any_cast<std::pair<int2048,int>>(&fir); dt){
//#ifdef DEBUG
//std::cout << "print:int2048" << std::endl;
//#endif
						if(dt->second == 0){
#ifdef DEBUG
std::cout << "print:None" << std::endl;
#endif
							std::cout << "None" << ' ';
						}
						else{
//#ifdef DEBUG
//std::cout << "print:int2048" << std::endl;
//#endif
							std::cout << dt->first << ' ';
//#ifdef DEBUG
//std::cout << "print:int2048 end" << std::endl;
//#endif
						}
					}
					else if(std::pair<double,int> *dt = std::any_cast<std::pair<double,int>>(&fir); dt){
#ifdef DEBUG
std::cout << "print:double" << std::endl;
#endif
						std::cout << std::fixed << std::setprecision(6) << dt->first << ' ';
					}
					else if(std::pair<std::string,int> *dt = std::any_cast<std::pair<std::string,int>>(&fir); dt){
#ifdef DEBUG
std::cout << "print:string" << std::endl;
#endif
						std::cout << dt->first << ' ';
					}
					else if(std::pair<bool,int> *dt = std::any_cast<std::pair<bool,int>>(&fir); dt){
#ifdef DEBUG
std::cout << "print:bool" << std::endl;
#endif
						if(dt->first){
							std::cout << "True" << ' ';
						}
						else{
							std::cout << "False" << ' ';
						}
					}
					else{
						assert(false);
					}
				}
				std::cout << std::endl;
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
std::cout << "visitAtomexpr" << std::endl;
#endif
		std::any down = visit(ctx->atom());
		if(auto *val = std::any_cast<std::pair<std::string,int>>(&down); val){
			if(val->second <= -7){
#ifdef DEBUG
std::cout << "visitOuterFunction" << std::endl;
#endif
				return functionWork(function[val->second], ctx->trailer());
			}
			if(val->second >= -6 && val->second <= -2){
#ifdef DEBUG
std::cout << "visitInsideFunction" << std::endl;
#endif
				return insideFunction(val->second, ctx->trailer());
			}
#ifdef DEBUG
std::cout << "std::pair<std::string,int>" << std::endl;
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
#ifdef DEBUG_name
std::cout << "visitAtom" << std::endl;
#endif
		if(auto now = ctx->NAME(); now){
			std::string name = now->getText();
#ifdef DEBUG_name
std::cout << "\nname:", std::cout << name << '\n' << std::endl;
#endif
			if(funcId.count(name)){
#ifdef DEBUG_name
std::cout << "func = " << name << std::endl;
std::cout << "funcId = " << funcId[name] << std::endl;
#endif
				return std::make_pair(name, funcId[name]);
			}
			if(!valuePosition(name)){
#ifdef DEBUG_name
std::cout << "new variable" << std::endl;
#endif
				variableId[name] = variable_id;
				if(depth){
					covered[depth][variable_id] = true;
				}
				assignValue(std::make_pair(std::any(false),variable_id),
						std::make_pair(int2048(0),0));
				variable_id++;
			}
#ifdef DEBUG_name
//std::cout << "maybe old variable" << std::endl;
#endif
			int pos = valuePosition(name);
			bool check = true;
			if(depth && !memory[0].count(pos)){
#ifdef DEBUG_name
//std::cout << "variable announced before, but not globally, then covered = true" << std::endl;
#endif
				covered[depth][pos] = true;
				check = false;
			}
#ifdef DEBUG_name
std::cout << "variable value can be concretized" << std::endl;
#endif
			//if(check){
			//	unTie(std::make_pair(memory[depth][pos],pos), "visitAtom");
			//}
			return std::make_pair(memory[depth][pos],pos);
		}
		if(auto now = ctx->NUMBER(); now){
#ifdef DEBUG
std::cout << "number" << std::endl;
#endif
			std::string val = now->getText();
#ifdef DEBUG
std::cout << "val = " << val << std::endl;
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
std::cout << "string:", std::cout << ret << std::endl;
#endif
			return std::make_pair(ret,-1);
		}
		if(auto now = ctx->NONE(); now){
#ifdef DEBUG
std::cout << "atom:none" << std::endl;
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
#ifdef DEBUG_format_string
std::cout << "testlist.size() = " << tl.size() << std::endl;
#endif
		std::string ret;
		for(int i=0;i<nsi;i++){
			while(np != osi && fsl[i]->getSymbol()->getTokenIndex() > ob[np]->getSymbol()->getTokenIndex()){
#ifdef DEBUG_format_string
std::cout << "add brace content" << std::endl;
#endif
				std::any val = concretize(visit(tl[np]));
#ifdef DEBUG_format_string
std::cout << "brace content gotten" << std::endl;
#endif
				if(auto now = std::any_cast<std::pair<std::string,int>>(&val); now){
					ret += now->first;
				}
				else if(auto now = std::any_cast<std::pair<int2048,int>>(&val); now){
					ret += now->first.toString();
				}
				else if(auto now = std::any_cast<std::pair<double,int>>(&val); now){
					ret += doubleToString(now->first);
				}
				else if(auto now = std::any_cast<std::pair<bool,int>>(&val); now){
					ret += (now->first ? "True" : "False");
				}
				else{
					assert(false);
				}
				np++;
			}
#ifdef DEBUG_format_string
std::cout << "add string literal" << std::endl;
#endif
			ret += fsl[i]->getText();
		}
		while(np != osi){
#ifdef DEBUG_format_string
std::cout << "add brace content" << std::endl;
#endif
			std::any val = concretize(visit(tl[np]));
#ifdef DEBUG_format_string
std::cout << "brace content gotten" << std::endl;
std::cout << "np = " << np << std::endl;
#endif
			if(auto now = std::any_cast<std::pair<std::string,int>>(&val); now){
				ret += now->first;
			}
			else if(auto now = std::any_cast<std::pair<int2048,int>>(&val); now){
				ret += now->first.toString();
			}
			else if(auto now = std::any_cast<std::pair<double,int>>(&val); now){
				ret += doubleToString(now->first);
			}
			else if(auto now = std::any_cast<std::pair<bool,int>>(&val); now){
				ret += (now->first ? "True" : "False");
			}
			else{
				assert(false);
			}
			np++;
		}
#ifdef DEBUG_format_string
std::cout << "visiting format_string end" << std::endl;
#endif
		return std::make_pair(ret,-1);
	}

	virtual std::any visitTestlist(Python3Parser::TestlistContext *ctx) override {
		std::vector<std::pair<std::any,int>> ret;
		auto test = ctx->test();
		int nsi = test.size();
		for(int i=0;i<nsi;i++){
			std::pair<std::any,int> got = std::any_cast<std::pair<std::any,int>>(abstractize(visit(test[i])));
#ifdef DEBUG
unTie(got, "visitTestlist");
#endif
			ret.push_back(got);
		}
#ifdef DEBUG_format_string
std::cout << "visitTestlist : ret.size() = " << ret.size() << std::endl;
#endif
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
cmake -B build

cmake --build build

./build/code
*/

/*
cd /home/enovo/Python-Interpreter

cd /home/enovo/Python-Interpreter/resources

chmod +x visualize.bash

bash visualize.bash file_input -gui
*/