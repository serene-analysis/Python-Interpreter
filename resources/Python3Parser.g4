parser grammar Python3Parser;

options {
  tokenVocab = Python3Lexer;
}

file_input: (NEWLINE | stmt)* EOF;// return EOF
funcdef: 'def' NAME parameters ':' suite;// I have a thought! Let's use a map to remember every (std::string->int)
parameters: '(' typedargslist? ')';
typedargslist: (tfpdef ('=' test)? (',' tfpdef ('=' test)?)*);
tfpdef: NAME ;// return std::string

stmt: simple_stmt | compound_stmt;// return simple or compound
simple_stmt: small_stmt  NEWLINE;// return small
small_stmt: expr_stmt | flow_stmt;// return expr or flow
expr_stmt: testlist ( (augassign testlist) |
                     ('=' testlist)*);//连等 加等/减等/...
augassign: ('+=' | '-=' | '*=' | '/=' | '//=' | '%=' );// return "add" or "sub" or "mult" or "div" or "idiv" or "mod"
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
arith_expr: term (addorsub_op term)*;
addorsub_op: '+'|'-';
term: factor (muldivmod_op factor)*;
muldivmod_op: '*'|'/'|'//'|'%';
factor: ('+'|'-') factor | atom_expr;
atom_expr: atom trailer?;
trailer: '(' (arglist)? ')' ;
atom: (NAME | NUMBER | STRING+| 'None' | 'True' | 'False' | ('(' test ')') | format_string);
format_string: FORMAT_QUOTATION (FORMAT_STRING_LITERAL | '{' testlist '}')* QUOTATION;
testlist: test (',' test)* (',')?;//算式  eg： a,b   a   a+b
arglist: argument (',' argument)*  (',')?;
argument: ( test |
            test '=' test );
