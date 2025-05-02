# EvalExpressionAST


Thanks to :
https://mariusbancila.ro/blog/2009/02/03/evaluating-expressions-part-1/
Parse an evaluate a simple mathematical expression 

Grammar :

|Production rule|        |                       |     
|  ---   | :---: | ---                           |
| EXP    | -> | EXP + TERM \|                    | 
|        |    | EXP - TERM \|                    |      
|        |    | TERM                             |
| TERM   | -> | TERM * FACTOR \|                 |   
|        |    | TERM / FACTOR \|                 | 
|        |    | FACTOR                           |        
| FACTOR | -> | ( EXP ) \| - EXP \| number       |

EXP, TERM, FACTOR are called non-terminal symbols
+, -, /, *, (, ) number are called terminal symbols
EXT is the start symbol

The above grammar is left recursive.
If we start with EXP and go into EXP we will loop indefintely !
We rewrite it elimanting the recursivity as follows :

|        |      |                          | 
| ---    | :---:| ---                      |
| EXP    | ->   | TERM EXP1                |
| EXP1   | ->   | + TERM EXP1 \|           |
|        |      | - TERM EXP1 \|           |
|        |      | null                     |
| TERM   | ->   | FACTOR TERM1             |
| TERM1  | ->   | * FACTOR TERM1 \|        |
|        |      | / FACTOR TERM1 \|        |
|        |      | null                     |
| FACTOR | ->   | ( EXP ) \| - EXP \| number |

null here means nothing or empty



| Production rule            | Semantic rule                                         |
| ---                        | :---                                                  |
| EXP     -> TERM EXP1       | EXP.node = mk_node('+', TERM.node, EXP1.node)         |
| EXP1    -> + TERM EXP1     | EXP1.node = mk_node('+', EXP1.node, TERM.node)         |
| EXP1    -> - TERM EXP1     | EXP1.node = mk_node('-', EXP1.node, TERM.node)         |
| EXP1    -> empty           | EXP1.node = mk_node(number, value)                     |
| TERM    -> FACTOR TERM1    | TERM.node = mk_node('*', FACTOR.node, TERM1.node)         |
| TERM1   -> * FACTOR TERM1  | TERM1.node = mk_node('*', TERM1.node, FACTOR.node)         |
| TERM1   -> / FACTOR TERM1  | TERM1.node = mk_node('/', TERM1.node, FACTOR.node)         |
| TERM1   -> empty           | TERM1.node = mk_node(number, value)         |
| FACTOR  -> ( EXP )         | FACTOR.node = EXP.node         |
| FACTOR  -> - EXP           | FACTOR.node = mk_node(negate, EXP.node)         |
| FACTOR  -> number          | FACTOR.node = mk_node (number, value)         |
----------------------------------------------------------------------------------

Now our parser can be written as (pseudo code) :


<pre><code>Let tok be the current input token

AST* parse() { tok = next_token(); return parse_expression();}

AST* parse_expression() { lhs = parse_term(); rhs = parse_expression1(); add_node('+', lhs, rhs); }
AST* parse_expression1() {
   if( tok == '+' || tok == '-' ) { tok = next_token(); lhs = parse_term(); rhs = parse_expression1(); add_node(tok, lhs, rhs); }
}
AST* parse_term() { lhs = parse_factor(); rhs = parse_term1(); add_node('*', lhs, rhs); }
AST* parse_term1() {
   if( tok == '*' || tok == '/' ) { tok = next_token(); lhs = parse_factor(); rhs = parse_term1(); add_node(tok, lhs, rhs); }
}
AST* parse_factor() {
   if (tok == '(') { tok = next_token(); node = parse_expression(); add_node(node); expect_closing_parenthesis();
   if(tok == '-') { tok = next_token();  node = parse_factor(); add_unary_node('-', node);}
   if( is_number(tok) { tok = next_token(); add_number_node(tok as number); }
}
</code></pre>



<table>
 <tr> Line 1 </tr>
 <tr> Line 2 </tr>
 <tr>Line 3 </tr>
 <tr> Line 4 </tr>
</table>

| header 1 | header 2| header 3|
| :--- | :---: | ---: |
| key 1 | Value 1 | | First value |
| key 2 | Value 2 | Second value \| |
| key 3 | Value 3 | Third value |
