#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>
#include "9cc.h"

//エラー報告用関数
//printfと同じ引数を取る
void error(char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

void error_at(char *loc, char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    int pos = loc - user_input;
    fprintf(stderr, "%s\n",user_input);
    fprintf(stderr, "%*s", pos, " ");
    fprintf(stderr, "^ ");
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

//次のトークンが期待している記号の時はトークンを1つ読み進めてtrueを返す
//それ以外はfalseを返す
bool consume(char *op) {
    if (token->kind != TK_RESERVED ||
	    strlen(op) != token->len ||
	    memcmp(token->str, op, token->len))
	    return false;
    token = token->next;
    return true;
}

Token *consume_ident() {
    Token *ret;
    if (token->kind == TK_IDENT) {
        ret = token;
        token = token->next;
        return ret;
    }
    return NULL;
}

//次のトークンが期待している記号の時はトークンを1つ読み進める
//それ以外の場合にはエラーを報告する
void expect(char *op) {
    if (token->kind != TK_RESERVED ||
	    strlen(op) != token->len ||
	    memcmp(token->str, op, token->len))
	    error_at(token->str,"expected \"%s\"", op);
    token = token->next;
}

//次のトークンが数値の場合、トークンを1つ読み進めてその数値を返す
//それ以外の場合にはエラーを報告する
int expect_number() {
    if (token->kind != TK_NUM)
	    error_at(token->str, "expected a number");
    int val = token->val;
    token = token->next;
    return val;
}

bool at_eof() {
    return token->kind == TK_EOF;
}

//新しいトークンを作成してcurに繋げる
Token *new_token(TokenKind kind, Token *cur, char *str, int len) {
    Token *tok = calloc(1,sizeof(Token));
    tok->kind = kind;
    tok->str = str;
    tok->len = len;
    cur->next = tok;
    return tok;
}

//文字列pの開始が文字列qと一致するか
bool startswith(char *p, char *q) {
    return memcmp(p, q, strlen(q)) == 0;
}

//入力文字列pをトークナイズしてそれを返す
Token *tokenize(char *p) {
    Token head;
    head.next = NULL;
    Token *cur = &head;
    while(*p) {
	    //空白文字をスキップ
	    if(isspace(*p)) {
            p++;
	        continue;
	    }

        //識別子
        if ('a' <= *p && *p <= 'z') {
            cur = new_token(TK_IDENT, cur, p, 1);
            p++;
            continue;
        }

        //2文字の記号
	    if (startswith(p, "==") || startswith(p, "!=") ||
		    startswith(p, "<=") || startswith(p, ">=")) {
	        cur = new_token(TK_RESERVED, cur, p, 2);
    	    p += 2;
	        continue;
	    }
	
        //1文字の記号
	    if (*p == '+' || *p == '-' ||
		    *p == '*' || *p == '/' ||
    		*p == '(' || *p == ')' ||
	    	*p == '<' || *p == '>' ||
            *p == ';' || *p == '=') {
	        cur = new_token(TK_RESERVED , cur, p, 1);
            p++;
	        continue;
	    }

        //整数リテラル
	    if (isdigit(*p)) {
	        cur = new_token(TK_NUM, cur, p, 0);
	        char *q = p;
        	cur->val = strtol(p, &p, 10);
	        cur->len = p - q;
	        continue;
	    }
	
	    error_at(token->str, "トークナイズできません");
    }
  
    new_token(TK_EOF, cur, p, 0);
    return head.next;
}

Node *new_node(NodeKind kind) {
    Node *node = calloc(1, sizeof(Node));
    node->kind = kind;
    return node;
}

Node *new_binary(NodeKind kind, Node *lhs, Node *rhs) {
    Node *node = new_node(kind);
    node->lhs = lhs;
    node->rhs = rhs;
    return node;
}

Node *new_num(int val) {
    Node *node = calloc(1, sizeof(Node));
    node->kind = ND_NUM;
    node->val = val;
    return node;
}

//以下相互再帰関数群=============================
void program() {
    int i = 0;
    while (!at_eof())
        code[i++] = stmt();
    code[i] = NULL;
}

Node *stmt() {
    Node *node = expr();
    expect(";");
    return node;
}

Node *expr() {
    return assign();
}

Node *assign() {
    Node *node = equality();
    if (consume("="))
        node = new_binary(ND_ASSIGN, node, assign());
    return node;
}

Node *equality() {
    Node *node = relational();
    while(true) {
	    if (consume("=="))
	        node = new_binary(ND_EQ, node, relational());
	    else if (consume("!="))
	        node = new_binary(ND_NE, node, relational());
	    else
	        return node;
    }
}

Node *relational() {
    Node *node = add();
    while(true) {
    	if (consume("<"))
	        node = new_binary(ND_LT, node, add());
    	else if (consume("<="))
	        node = new_binary(ND_LE, node, add());
    	else if (consume(">"))
	        node = new_binary(ND_LT, add(), node);
	    else if (consume(">="))
	        node = new_binary(ND_LE, add(), node);
	    else
	    return node;
    }
}

Node *add() {
    Node *node = mul();
    while(true) {
	    if (consume("+"))
	        node = new_binary(ND_ADD, node, mul());
	    else if (consume("-"))
	        node = new_binary(ND_SUB, node, mul());
	    else
	        return node;
    }
}

Node *mul() {
    Node *node = unary();
    while(true) {
	    if (consume("*"))
	        node = new_binary(ND_MUL, node, unary());
	    else if (consume("/"))
	        node = new_binary(ND_DIV, node, unary());
	    else
	        return node;
    }
}

Node *unary() {
    if (consume("+"))
	    return unary();
    if (consume("-"))
	    return new_binary(ND_SUB, new_num(0), unary());
    return primary();
}

Node *primary(){
    Token *tok = consume_ident();
    if (tok) {
        Node *node = calloc(1, sizeof(Node));
        node->kind = ND_LVAR;
        node->offset = (tok->str[0] - 'a' + 1) * 8;
        return node;
    }
    if (consume("(")) {
	    Node *node = expr();
	    expect(")");
	    return node;
	}
    return new_num(expect_number());
}
