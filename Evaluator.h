#pragma once 

#include "AST.h"

namespace ast {

class EvaluatorException : public std::exception
{
public:
   EvaluatorException(const std::string& message):
      std::exception(message.c_str())
      {
      }
};

class Evaluator 
{
   // traverse the AST tree and evaluate 
   double evaluate_subtree(Node* ast)
   {
      if(ast == nullptr) 
         throw EvaluatorException("Empty tree!");

      if(ast->m_type == NUMBER) {
         return ast->m_value;
      } else if(ast->m_type == MINUS_SIGN) {
         return -evaluate_subtree(ast->m_left);
      } else {
         double v1 = evaluate_subtree(ast->m_left);
         double v2 = evaluate_subtree(ast->m_right);
         switch(ast->m_type) {
            case OP_PLUS:  return v1 + v2;
            case OP_MINUS: return v1 - v2;
            case OP_MUL:   return v1 * v2;
            case OP_DIV:   return v1 / v2;
         }
      }
      throw EvaluatorException("Incorrect syntax tree!");
   }

public:
   double evaluate(Node* ast)
   {
      if(ast == nullptr)
         throw EvaluatorException("Empty abstract syntax tree");
      // recursive travering
      return evaluate_subtree(ast);
   }
};

} // ns