#pragma once

#include <string>
#include <sstream>
#include <assert.h>
#include <exception>

/*
Grammar :
Production rules :
EXP    -> EXP + TERM |
          EXP - TERM |
          TERM
TERM   -> TERM * FACTOR |
          TERM / FACTOR |
          FACTOR
FACTOR -> ( EXP ) | - EXP | number


EXP, TERM, FACTOR are called non-terminal symbols
+, -, /, *, (, ) number are called terminal symbols
EXT is the start symbol

The above grammar is left recursive.
If we start with EXP and go into EXP we will loop indefintely !
We rewrite it elimanting the recursivity as follows :
EXP    -> TERM EXP1
EXP1   -> + TERM EXP1 |
          - TERM EXP1 |
          null
TERM   -> FACTOR TERM1
TERM1  -> * FACTOR TERM1 |
          / FACTOR TERM1 |
          null
FACTOR -> ( EXP ) | - EXP | number

null here means nothing or empty



----------------------------------------------------------------------------------
| Production rule            | Semantic rule
----------------------------------------------------------------------------------
| EXP     -> TERM EXP1       | EXP.node = mk_node('+', TERM.node, EXP1.node)
| EXP1    -> + TERM EXP1     | EXP1.node = mk_node('+', EXP1.node, TERM.node)
| EXP1    -> - TERM EXP1     | EXP1.node = mk_node('-', EXP1.node, TERM.node)
| EXP1    -> empty           | EXP1.node = mk_node(number, value)         
| TERM    -> FACTOR TERM1    | TERM.node = mk_node('*', FACTOR.node, TERM1.node)
| TERM1   -> * FACTOR TERM1  | TERM1.node = mk_node('*', TERM1.node, FACTOR.node)
| TERM1   -> / FACTOR TERM1  | TERM1.node = mk_node('/', TERM1.node, FACTOR.node)
| TERM1   -> empty           | TERM1.node = mk_node(number, value)
| FACTOR  -> ( EXP )         | FACTOR.node = EXP.node
| FACTOR  -> - EXP           | FACTOR.node = mk_node(negate, EXP.node)
| FACTOR  -> number          | FACTOR.node = mk_node (number, value)
----------------------------------------------------------------------------------

Now our parser can be written as (pseudo code) :

let tok be the current input token

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
*/


#include "AST.h"

namespace ast {
class ParserException : public std::exception
{
   size_t m_Pos;

public:
   ParserException(const std::string& message, size_t pos):
      std::exception(message.c_str()),
      m_Pos(pos)
   {
   }
};


class Lexer {
   const char* m_input;
   size_t m_pos;
   void skip_white_spaces()
   {
      while (::isspace(m_input[m_pos])) m_pos++;
   }
public:
   void init(const char* text) { m_input = text; m_pos = 0; }
   size_t get_position() { return m_pos; }

   // Extrat a token from the input text
   void next_token(Token& tok)
   { 
      tok.m_value = 0;
      tok.m_symbol = 0;
      skip_white_spaces();

      if (m_input[m_pos] == '\0') {
         tok.m_tag = TextEnd;
         return;
      }

      if (::isdigit(m_input[m_pos])) {
         tok.m_tag = Number;
         tok.m_value = consume_number();
         return;
      }

      tok.m_tag = Error;

      switch (m_input[m_pos]) {
      case '+': tok.m_tag = Plus; break;
      case '-': tok.m_tag = Minus; break;
      case '*': tok.m_tag = Mul; break;
      case '/': tok.m_tag = Div; break;
      case '(': tok.m_tag = OpenParenthesis; break;
      case ')': tok.m_tag = ClosedParenthesis; break;
      }

      if (tok.m_tag != Error) {
         tok.m_symbol = m_input[m_pos];
         m_pos++;
      } else {
         std::ostringstream os;
         os << "Unexpected token '" << m_input[m_pos] << "' at position " << m_pos;
         throw ParserException(os.str(), m_pos);
      }
      return;
   }

   // Extrat a number from the input text
   double consume_number()
   {
      skip_white_spaces();

      int start = m_pos;
      while (::isdigit(m_input[m_pos])) m_pos++;
      if (m_input[m_pos] == '.') m_pos++;
      while (::isdigit(m_input[m_pos])) m_pos++;

      if (m_pos == start) {
         throw ParserException("Number expected but not found!", m_pos);
      }
      if (m_pos - start > 32) {
         throw ParserException("Unsupported number length(limit is 32 caracters)!", m_pos);
      }

      char buffer[32] = { 0 };
      memcpy(buffer, &m_input[start], m_pos - start);
      return ::atof(buffer);
   }
   void expect_symbol(char expected)
   {
      if (m_input[m_pos - 1] == expected) {
         
      } else {
         std::ostringstream os;
         os << "Expected token '" << expected << "' at position " << m_pos << " not found";
         throw ParserException(os.str(), m_pos);
      }
   }
};


class Parser
{
private:
   Lexer lexer;
   Token m_current_tok;
   Node* parse_expression()
   {      
      Node* tnode = parse_term();
      Node* e1node = parse_expression1();
      return make_node(OP_PLUS, tnode, e1node);
   }

   Node* parse_expression1()
   {
      Node* tnode;
      Node* e1node;

      switch(m_current_tok.m_tag) {
      case Plus:
         lexer.next_token(m_current_tok);
         tnode = parse_term();
         e1node = parse_expression1();

         return make_node(OP_PLUS, e1node, tnode);

      case Minus:
         lexer.next_token(m_current_tok);
         tnode = parse_term();
         e1node = parse_expression1();

         return make_node(OP_MINUS, e1node, tnode);
      }

      return make_number_node(0);
   }

   Node* parse_term()
   {
      Node* fnode = parse_factor();
      Node* t1node = parse_term1();

      return make_node(ast::OP_MUL, fnode, t1node);
   }

   Node* parse_term1()
   {
      Node* fnode;
      Node* t1node;

      switch(m_current_tok.m_tag)
      {
      case Mul: 
         lexer.next_token(m_current_tok);
         fnode = parse_factor();
         t1node = parse_term1();
         return make_node(ast::OP_MUL, t1node, fnode);

      case Div:
         lexer.next_token(m_current_tok);
         fnode = parse_factor();
         t1node = parse_term1();
         return make_node(ast::OP_DIV, t1node, fnode);
      }

      return make_number_node(1);
   }

   Node* parse_factor()
   {
      Node* node;
      switch(m_current_tok.m_tag) {
      case OpenParenthesis:
         lexer.next_token(m_current_tok);
         node = parse_expression();
         lexer.expect_symbol(')');
         lexer.next_token(m_current_tok);
         return node;

      case Minus:
         lexer.next_token(m_current_tok);
		   node = parse_factor();
         return make_unary_node(node);

      case Number: {
            double value = m_current_tok.m_value;
            lexer.next_token(m_current_tok);
            return make_number_node(value);
      }
      default: {
         int pos = lexer.get_position();
         std::ostringstream os; 
         os << "Unexpected token '" << m_current_tok.m_symbol << "' at position " << pos;
         throw ParserException(os.str(), pos);
      }
      }
   }

   static
   Node* make_node(NodeType type, Node* left, Node* right)
   {
      return new Node(type, left, right);
   }

   static
   Node* make_unary_node(Node* left)
   {
      return new Node(MINUS_SIGN, left, nullptr);
   }

   static
   Node* make_number_node(double value)
   {
      return new Node(value);
   }

public:   
   Node* parse(const char* text)
   {
      lexer.init(text);     
      lexer.next_token(m_current_tok);
      return parse_expression();
   }
};

} // ns