#!/bin/bash
try(){
	expected="$1"
	input="$2"

	./9cc "$input" > tmp.s
	gcc -o tmp tmp.s
	./tmp
	actual="$?"

	if [ "$actual" = "$expected" ]; then
		echo "$input -> $actual"
	else
		echo "$input -> $expected expected, but got $actual"
		exit 1
	fi
}

try 255 255
try 21 "5+20-4"
try 21 "5 + 20   -4"
try 15 "1+2+3*4"
try 13 "1+(5-3)*6"
try 13 "1+((5-3))*6"
try 1 " (  ( ( 1 )) ) "
try 9 "10+-1"
try 9 "10-+1"
try 10 "+5+5"
try 10 "-5*-2"
try 10 "- - 10"
try 10 "+-+-+10"
try 0 "2==3"
try 1 "2==2"
try 0 "2!=2"
try 1 "2!=3"
try 0 "2>3"
try 0 "3<2"
try 1 "2<3"
try 1 "3>2"
try 0 "2>=3"
try 0 "3<=2"
try 1 "2<=3"
try 1 "3>=2"
try 1 "2<=2"
try 1 "2>=2"

echo OK
