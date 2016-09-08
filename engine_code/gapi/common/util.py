#!/usr/bin/env python
# _*_ coding: utf8 _*_

import os, sys
import ply.lex as lex

tokens = ['STRING1', 'STRING2', 'FIELD']

t_STRING1 = r"'.*'"
t_STRING2 = r'".*"'
t_FIELD = r'[^\'\" \t]+'
t_ignore_space = r'[ \t]'

def t_error(tok):
    pass

# 参数允许 "aaa \"ccc ddd\" 'eee fff' ggg" 的形式
def split_args(args):
    argv = []
    lexer = lex.lex()
    lexer.input(args)
    for tok in lexer:
        if tok.type in ['STRING1', 'STRING2']:
            # 去掉两边的引号
            tok.value = tok.value[1:-1]
        argv.append(tok.value)
    print argv
    return argv

if "__main__" == __name__:
    split_args(sys.argv[1])

