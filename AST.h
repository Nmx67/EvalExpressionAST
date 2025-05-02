#pragma once 


/*
 ----------------------------
|  +  |       | left  | right|
 ----------------------------
                  |      |
                  |      v
                  |      --------------------------
                  |      | NUM | 2   |  nil  | nil|
                  |       -------------------------
                  v
                  -------------------------
                 | NUM | 1     | nil  | nil|
                  -------------------------

*/
enum TokenType 
{
   Error,
   Plus,
   Minus,
   Mul,
   Div,
   OpenParenthesis,
   ClosedParenthesis,
   Number,
   TextEnd
};

struct Token 
{
	TokenType	m_tag;
	double		m_value;
	char		   m_symbol;

	Token() : m_tag(Error), m_value(0), m_symbol(0) {}
};


namespace ast {
enum NodeType 
{
   Undefined,
   OP_PLUS, OP_MINUS, OP_MUL, OP_DIV,
   MINUS_SIGN,
   NUMBER
};

class Node
{
public:
   NodeType m_type;
   double   m_value;
   Node*    m_left;
   Node*    m_right;

   Node() : m_type(Undefined), m_left(nullptr), m_right(nullptr), m_value(0) {}
   Node(NodeType t) : m_type(t), m_left(nullptr), m_right(nullptr), m_value(0) {}
   Node(NodeType t, Node* l, Node* r) : m_type(t), m_left(l), m_right(r), m_value(0) {}
   Node(double value) : m_type(NUMBER), m_left(nullptr), m_right(nullptr), m_value(value) {}
   ~Node()
   {
      if(m_left) delete m_left;
      if(m_right) delete m_right;
   }
};
} // ns