#!/usr/bin/env python
# _*_ coding: utf8 _*_

import fmttrans_utils

# class FileList(object):
# 返回合法的目标文件

#class StmtList(object):
# 读入文件内容。返回解析并将文件内容转化后的dict。

class FormatTransform(object):
    def files_transform(self):
        with self.FileList() as flist:
            for fname in flist.files():
                if fmttrans_utils.is_continue():
                    self.file_transform(fname)

    def file_transform(self, fname):
        with self.StmtList(fname) as slist:
            fname_dict = {}
            stmt_parse = self.StmtParse(fname)
            for stmt in slist.stmts():
                if fmttrans_utils.is_continue():
                    fname = stmt_parse.make_db_file_name(stmt)
                    data = stmt_parse.make_db_file_data(stmt)
                    if not fname_dict.has_key(fname):
                        fname_dict[fname] = []
                    fname_dict[fname].append(data)

            if fmttrans_utils.is_continue():
                #print("fname_dict = [%s]" % fname_dict)
                for (fname, data_list) in fname_dict.items():
                    self.FileSave(fname, data_list)

class StmtParse(object):
    def __init__(self):
        self.make_db_file_name = self.make_db_file_name_func()
        self.make_db_file_data = self.make_db_file_data_func()

    def make_db_file_name_func(self):
        args_func_list = []
        for field_name in self.db_file_name_desc:
            args_func = self.make_db_field_value_func(field_name)
            args_func_list.append(args_func)

        tmpfunc = lambda args_func_list: lambda field_dict: self.db_file_name(*[f(field_dict) for f in args_func_list])
        return tmpfunc(args_func_list)

    def make_db_file_data_func(self):
        self.db_field_func_list = self.make_db_field_func_list()
        return lambda field_dict: self.combine_db_field_list(self.generate_db_field_list(field_dict))

    # 将字段列表组合为目标行
    def combine_db_field_list(self, db_field_list):
        retstring = '|'.join(db_field_list) + '\n'
        return retstring.encode('utf-8')

    def generate_db_field_list(self, field_dict):
        retlist = []
        for db_field_func in self.db_field_func_list:
            db_field = db_field_func(field_dict)
            retlist.append(db_field)
        return retlist

    def make_db_field_func_list(self):
        db_field_func_list = []
        for db_field_name, stmt_field_name_list in self.db_field_name_desc:
            db_field_func = getattr(self, "db_field_" + db_field_name)
            #print("db field func:", db_field_func)

            args_func_list = []
            for stmt_field_name in stmt_field_name_list:
                #print("smt field name = %s" % stmt_field_name)
                args_func = self.make_db_field_value_func(stmt_field_name)
                args_func_list.append(args_func)

            tmpfunc = lambda field_func, arg_func_list: lambda field_list: field_func(*[f(field_list) for f in arg_func_list])
            db_field_func = tmpfunc(db_field_func, args_func_list)
            db_field_func_list.append(db_field_func)

        #print db_field_func_list
        return db_field_func_list

    def make_db_field_value_func(self, field_name):
        #print("field_name = %s" % field_name)
        def get_db_field_value(field_dict):
            if field_dict.has_key(field_name):
                return field_dict[field_name]
            else:
                return None
        return get_db_field_value

