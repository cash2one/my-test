# _*_ coding: utf8 _*_

import sys
from transform.common import FormatTransform
from apt_file_list import AptFileList
from apt_file_save import AptFileSave
from apt_stmt_list import AptStmtList
from apt_stmt_parse import AptStmtParse


class AptFormatTransform(FormatTransform):
    def __init__(self):
        self.FileList = AptFileList
        self.StmtList = AptStmtList
        self.StmtParse = AptStmtParse
        self.FileSave = AptFileSave

