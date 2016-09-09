import subprocess
import re
import platform

# 本机所有ip信息的获取 
def find_all_ip(platform):
    ipstr = '([0-9]{1,3}\.){3}[0-9]{1,3}'
    if platform == "Darwin" or platform == "Linux":
        ipconfig_process = subprocess.Popen("ifconfig", stdout=subprocess.PIPE)
        output = ipconfig_process.stdout.read()
        ip_pattern = re.compile('(inet %s)' % ipstr)
        if platform == "Linux":
            ip_pattern = re.compile('(inet addr:%s)' % ipstr)
        pattern = re.compile(ipstr)
        iplist = []
        for ipaddr in re.finditer(ip_pattern, str(output)):
            ip = pattern.search(ipaddr.group())
            if ip.group() != "127.0.0.1":
                iplist.append(ip.group())
        return iplist
    elif platform == "Windows":
        ipconfig_process = subprocess.Popen("ipconfig", stdout=subprocess.PIPE)
        output = ipconfig_process.stdout.read()
        ip_pattern = re.compile("IPv4 Address(\. )*: %s" % ipstr)
        pattern = re.compile(ipstr)
        iplist = []
        for ipaddr in re.finditer(ip_pattern, str(output)):
            ip = pattern.search(ipaddr.group())
            if ip.group() != "127.0.0.1":
                iplist.append(ip.group())
        return iplist

# 本机所有子网掩码信息的获取
def find_all_mask(platform):
    ipstr = '([0-9]{1,3}\.){3}[0-9]{1,3}'
    maskstr = '0x([0-9a-f]{8})'
    if platform == "Darwin" or platform == "Linux":
        ipconfig_process = subprocess.Popen("ifconfig", stdout=subprocess.PIPE)
        output = ipconfig_process.stdout.read()
        mask_pattern = re.compile('(netmask %s)' % maskstr)
        pattern = re.compile(maskstr)
        if platform == "Linux":
            mask_pattern = re.compile(r'Mask:%s' % ipstr)
            pattern = re.compile(ipstr)
        masklist = []
        for maskaddr in mask_pattern.finditer(str(output)):
            mask = pattern.search(maskaddr.group())
            if mask.group() != '0xff000000' and mask.group() != '255.0.0.0':
                masklist.append(mask.group())
        return masklist
    elif platform == "Windows":
        ipconfig_process = subprocess.Popen("ipconfig", stdout=subprocess.PIPE)
        output = ipconfig_process.stdout.read()
        mask_pattern = re.compile(r"Subnet Mask (\. )*: %s" % ipstr)
        pattern = re.compile(ipstr)
        masklist = []
        for maskaddr in mask_pattern.finditer(str(output)):
            mask = pattern.search(maskaddr.group())
            if mask.group() != '255.0.0.0':
                masklist.append(mask.group())
        return masklist

system = platform.system()
# iptables规则的初始化
#subprocess.Popen("/root/init_iptables.sh", shell=True)
# 初始化本网段ip段为信任ip段
def main():
   exchange_mask =lambda mask: sum(bin(int(i)).count('1') \
                                 for i in mask.split('.'))
   #print(exchange_mask('255.255.0.0')) 
   local_ip_all = find_all_ip(system)
   local_mask_all = find_all_mask(system)
   ip_segment_all = [local_ip_all[i] + "/" + str(exchange_mask(local_mask_all[i])) for i in range(len(local_ip_all))]
   print ip_segment_all
   for i in ip_segment_all:
       subprocess.Popen(["iptables", "-A", "INPUT", "-s", i, "-j", "ACCEPT"], stdout=subprocess.PIPE) 
   subprocess.Popen(["/etc/rc.d/init.d/iptables", "save"], stdout=subprocess.PIPE) 
   subprocess.Popen(["service", "iptables", "restart"], stdout=subprocess.PIPE) 
   return subprocess.Popen(["iptables", "-L"], stdout=subprocess.PIPE)


if __name__ == "__main__":
    print main()
