#!/usr/bin/python
rule_path="/gms/mtx/conf/rule.bak"
import os
sing='`'
def read_rule():
	fp=open(rule_path,"r")
	while True:
		buf=""
		tmp_line=fp.readline()
		if tmp_line == "":
			break
		if tmp_line[0] != '#':
			if tmp_line[0] == '[':
				while True:
					tmp_line=fp.readline()
					if tmp_line[0] != '\n':
						if tmp_line.split('=')[0].find("reportloc") != -1:
							for line in tmp_line.split('=')[1:]:
								if line.strip() == '':
									continue

								buf+=line.strip()
							print buf.strip()+sing+"1"
							break
						if tmp_line.split('=')[0].find("response") != -1:
							buf+="null"
						for line in tmp_line.split('=')[1:]:
							if line.strip() == '':
								continue
							buf+=line.strip()
						buf=buf.strip()
						buf+=sing

if __name__ == '__main__':
	read_rule()	
