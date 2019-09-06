#include <stdarg.h>

//入力ソースコード
char* user_input;

//トークンの種類
typedef enum {
  TK_RESERVED,//予約語(記号)
  TK_IDENT,
  TK_NUM,     //整数
  TK_EOF,     //入力の終端
} TokenKind;

//トークン型
typedef struct Token Token;
struct Token {
    TokenKind kind; //トークンの型
    Token *next;    //次の入力トークン
    int val;        //kindがTK_NUMの場合、その数値
    char *str;      //トークン文字列
    int len;        //トークンの長さ
};

Token *token;

//抽象構文木のノードの型
typedef enum {
    ND_ADD,   // +
    ND_SUB,   // -
    ND_MUL,   // *
    ND_DIV,   // /
    ND_ASSIGN,// =
    ND_EQ,    // ==
    ND_NE,    // !=
    ND_LT,    // <
    ND_LE,    // <=
    ND_LVAR,  // ローカル変数
    ND_NUM,   // 整数
} NodeKind;

//ノード本体(連結リスト)
typedef struct Node Node;
struct Node{
    NodeKind kind;//ノードの型
    Node *lhs;    //左辺
    Node *rhs;    //右辺
    int val;      //kindがND_NUMの場合のみ使う
    int offset;   //kindがND_LVARの場合のみ使う
};

Node *code[100];

/*==codeden.cppで定義==*/
void gen(Node *);               //main関数から呼ばれる
/*=====================*/

/*===parse.cppで定義===*/
Token *tokenize(char *);         //main関数から呼ばれる
void error(char *, ...);             //拡張性のためヘッダファイルで宣言
void error_at(char *, char *, ...);  //拡張性のためヘッダファイルで宣言
//以下相互再帰用宣言
Node *primary();
Node *unary();
Node *mul();
Node *add();
Node *relational();
Node *equality();
Node *assign();
Node *expr();
Node *stmt();
void program();
/*=====================*/








