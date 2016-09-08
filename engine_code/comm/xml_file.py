#!/usr/bin/python  
# -*- coding=utf-8 -*- 
from xml.etree.ElementTree import ElementTree,Element
def read_xml(in_path, tree):  
    return tree.parse(in_path)  

def write_xml(tree, out_path):  
    tree.write(out_path, encoding="utf-8")

def find_nodes(tree, path):  
    return tree.findall(path) 

def if_match(node, kv_map):  
    for key in kv_map:  
        if node.get(key) != kv_map.get(key):  
            return False  
    return True 

def get_node_by_keyvalue(nodelist, kv_map):  
    result_nodes = []  
    for node in nodelist:  
        if if_match(node, kv_map):  
            result_nodes.append(node)  
            return result_nodes  

def change_node_text(nodelist, text, is_add=False, is_delete=False):  
    #改变/增加/删除一个节点的文本 nodelist:节点列表 text : 更新后的文本
    for node in nodelist:  
        if is_add:  
            node.text += text  
        elif is_delete:  
            node.text = ""  
        else:  
            node.text = text

def create_node(tag, property_map, content):  
    #新造一个节点 tag:节点标签 property_map:属性及属性值map content: 节点闭合标签里的文本内容 
    element = Element(tag, property_map)  
    element.text = content  
    return element

def add_child_node(nodelist, element):  
    #给一个节点添加子节点 nodelist: 节点列表 element: 子节点 
    nodelist.append(element)
          
def del_node_by_tagkeyvalue(nodelist, tag, kv_map):  
    #同过属性及属性值定位一个节点，并删除之 
    #nodelist: 父节点列表 
    #tag:子节点标签 
    #kv_map: 属性及属性值列表" 
    for parent_node in nodelist:  
        children = parent_node.getchildren()  
        for child in children:  
            if child.tag == tag and if_match(child, kv_map):  
                parent_node.remove(child)  

