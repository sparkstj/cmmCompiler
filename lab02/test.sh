#!/bin/sh
flex lexical.l
bison -d -v syntax.y 
gcc main.c symtable.c syntax.tab.c check.c -ll -ly -o parser
echo **Test Begin**
echo Test 1
./parser Tests/test1.cmm
echo Test 2
./parser Tests/test2.cmm
echo Test 3
./parser Tests/test3.cmm
echo Test 4
./parser Tests/test4.cmm
echo Test 5
./parser Tests/test5.cmm
echo Test 6
./parser Tests/test6.cmm
echo Test 7
./parser Tests/test7.cmm
echo Test 8
./parser Tests/test8.cmm
echo Test 9
./parser Tests/test9.cmm
echo Test 10
./parser Tests/test10.cmm
echo Test 11
./parser Tests/test11.cmm
echo Test 12
./parser Tests/test12.cmm
echo Test 13
./parser Tests/test13.cmm
echo Test 14
./parser Tests/test14.cmm
echo Test 15
./parser Tests/test15.cmm
echo Test 16
./parser Tests/test16.cmm
echo Test 17
./parser Tests/test17.cmm
