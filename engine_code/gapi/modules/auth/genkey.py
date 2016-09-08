#!/usr/bin/python
import sys_auth
import sys
sys.path.append("/gms/gapi/modules/network")
import text_xml

def xml_dev(Idname):
    tree = text_xml.read_xml("/gms/conf/gmsconfig.xml")
    root = tree.getroot()
    nodes = text_xml.find_nodes(tree,"gms_id")
    if nodes == []:
        #auth_line=False
        a=text_xml.create_node("dev_id",{"Id":Idname}," ","\n\t")
        b=text_xml.create_node("gms_id",{},"\n\t","\n")
        root.append(b)
        child_nodes=text_xml.find_nodes(tree,"gms_id")
        text_xml.add_child_node(child_nodes,a)
    else:
        net_node=text_xml.find_nodes(tree,"gms_id/dev_id") 
        text_xml.change_node_properties(net_node,{"Id":Idname},"")
    text_xml.write_xml(tree, "/gms/conf/gmsconfig.xml")  
if __name__ =='__main__':
    dev_id=sys_auth.get_devid_id()
    dev_id=dev_id.strip().rstrip("\n")
    #xml_dev(dev_id)
    sys.stdout.write(dev_id)
    exit(0)
    
