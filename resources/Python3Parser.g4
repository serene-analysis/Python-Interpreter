parser grammar Python3Parser;

options {
  tokenVocab = Python3Lexer;
}

file_input: (NEWLINE | stmt)* EOF;// return EOF
funcdef: 'def' NAME parameters ':' suite;// I have a thought! Let's use a map to remember every (std::string->int).
  //When defined just remember it, activate only if called.
parameters: '(' typedargslist? ')';// Do nothing, will be done in functionWork()
typedargslist: (tfpdef ('=' test)? (',' tfpdef ('=' test)?)*);// Do nothing, will be done in functionWork()
tfpdef: NAME ;// return std::string

stmt: simple_stmt | compound_stmt;// return simple or compound
simple_stmt: small_stmt  NEWLINE;// return small
small_stmt: expr_stmt | flow_stmt;// return expr or flow
expr_stmt: testlist ( (augassign testlist) |
                     ('=' testlist)*);//连等 加等/减等/... // return std::vector<std::pair<std::any,int>>
augassign: ('+=' | '-=' | '*=' | '/=' | '//=' | '%=' );// return "+=" or "-=" or "*=" or "/=" or "//=" or "%="
flow_stmt: break_stmt | continue_stmt | return_stmt;// return brk/cont/ret
break_stmt: 'break';// return "break"
continue_stmt: 'continue';// return "continue"
return_stmt: 'return' (testlist)?;// return testlist or "return"
compound_stmt: if_stmt | while_stmt | funcdef ;// return gif or gwhile or gdef
if_stmt: 'if' test ':' suite ('elif' test ':' suite)* ('else' ':' suite)?;// return ctx->suite(x) or "if nothing satisfied"
while_stmt: 'while' test ':' suite;// return "while condition unsatisfied" or "return" or "while end"
suite: simple_stmt | NEWLINE INDENT stmt+ DEDENT;// INDENT and DEDENT are "suo jin" in Chinese! return simple or flow_result or "suite unflowed end"
test: or_test ;// return visit(or_test())
or_test: and_test ('or' and_test)*;// return true or false
and_test: not_test ('and' not_test)*;// return true or false
not_test: 'not' not_test | comparison;// return true or false
comparison: arith_expr (comp_op arith_expr)*; // return true or false
comp_op: '<'|'>'|'=='|'>='|'<=' | '!=';// return (self)
arith_expr: term (addorsub_op term)*;// return (result)
addorsub_op: '+'|'-';// return "+" or "-"
term: factor (muldivmod_op factor)*;// return (result)
muldivmod_op: '*'|'/'|'//'|'%';// return (self)
factor: ('+'|'-') factor | atom_expr; // return (+/-)visit(factor) or visit(atom_expr)
atom_expr: atom trailer?;// return (result of function) 
trailer: '(' (arglist)? ')' ;
  //Do nothing, because functionWork will do it.
atom: (NAME | NUMBER | STRING+| 'None' | 'True' | 'False' | ('(' test ')') | format_string); // return (result)
format_string: FORMAT_QUOTATION (FORMAT_STRING_LITERAL | '{' testlist '}')* QUOTATION;
  //We need to check bool first!
  //Nope, value can only be themselves, just make sure to implicitly transform to bool first.
testlist: test (',' test)* (',')?;//算式  eg： a,b   a   a+b
  //
arglist: argument (',' argument)* (',')?;// Same to trailer
argument: ( test |
            test '=' test );// Will be considered above.
//I need to make every value a pair {val, memory_position}!
//Implicit transformation in +-*/% is not dealt!
//How on earth does these explicit transformation work?
//format_string's {{ is not dealt!