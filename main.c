#include <stdio.h>
#include "9cc.h"

int main(int argc, char **argv){
  if(argc != 2)
    error("引数の個数が不正です\n");

  //user_inputに入力プログラムを保存
  user_input = argv[1];
  
  //トークナイズする
  token = tokenize(user_input);
  Node *node = expr();
  
  //アセンブリの前半部分を出力
  printf(".intel_syntax noprefix\n");
  printf(".global main\n");
  printf("main:\n");

  //抽象構文木を下りながらコード生成
  gen(node);

  //スタックトップに式全体の値が残っているはずなので
  //それをRAXにロードしてから関数の返り値とする
  printf("  pop rax\n");
  printf("  ret\n");
  
  return 0;
}
