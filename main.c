#include <stdio.h>
#include "9cc.h"
/*
void print_tree(Node *node){
    if (node->kind == ND_NUM) {
        fprintf(stderr, "[num]");
    } else if (node->kind == ND_LVAR) {
        fprintf(stderr, "[lvar]");
    } else {
        fprintf(stderr, "[");
        print_tree(node->lhs);
        fprintf(stderr, "←%d→",node->kind);
        print_tree(node->rhs);
        fprintf(stderr, "]");
    }
}
*/
int main(int argc, char **argv){
    if(argc != 2)
        error("引数の個数が不正です\n");

    //user_inputに入力プログラムを保存
    user_input = argv[1];
//    fprintf(stderr, "start!\n");
    //トークナイズする
    token = tokenize(user_input);
    // fprintf(stderr, "tokenized!\n");
    // Token *t=token;
    // while(t->kind != TK_EOF) {
    //     fprintf(stderr, "%c\t%d\n",*t->str,t->len);
    //     t=t->next;
    // }
    program();
    
//    print_tree(code[0]);
//    fprintf(stderr,"\n");

    //アセンブリの前半部分を出力
    printf(".intel_syntax noprefix\n");
    printf(".global main\n");
    printf("main:\n");

    //プロローグ
    //変数26個分の領域を確保する
    printf("  push rbp\n");
    printf("  mov rbp, rsp\n");
    printf("  sub rsp, 208\n");

    //先頭の式から順にコード生成
    for (int i = 0; code[i]; i++) {
        gen(code[i]);
        //式の評価結果としてスタックに一つの値が残っているはずなので、
        //スタックが溢れないようにpopしておく
        printf("  pop rax\n");
    }

    //エピローグ
    //最後の式の値がRAXに残っているので返り値とする
    printf("  mov rsp, rbp\n");
    printf("  pop rbp\n");
    printf("  ret\n");
  
    return 0;
}
