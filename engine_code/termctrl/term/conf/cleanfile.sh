#!/bin/sh
src_xmlfile=/gms/termctrl/conf/gmsconfig.xml
net_path=/etc/sysconfig/network-scripts/ifcfg-
dst_path=/gms/conf/
Ip_addr=192.168.0.171
Mask=255.255.255.0
Gw=192.168.0.1
Dns=202.106.0.20
init_key=123456
Mbuf=`cat /cfcard/chanct_conf/machine_type.conf`
Mtype=`echo -n ${Mbuf:0}`
init_crypt=`echo -n $init_key|md5sum|cut -f 1 -d ' '`
delete_conf(){
su - postgres <<EOF
/db/postgresql/bin/psql gms_db <<E2
delete from t_conf_customflow; 
delete from t_conf_device;     
delete from t_conf_importip;   
delete from t_conf_localdev;   
delete from t_conf_user where uname != 'admin';
update t_conf_user set passwd='$init_crypt';
delete from t_conf_whitelist;  
E2
EOF
}

delete_3rdquery()
{
su - postgres <<EOF
/db/postgresql/bin/psql gms_db <<E2
delete from t_event_3rdquery_m1      ;
delete from t_event_3rdquery_m10     ;
delete from t_event_3rdquery_m10_d1  ;
delete from t_event_3rdquery_m10_d10 ;
delete from t_event_3rdquery_m10_d11 ;
delete from t_event_3rdquery_m10_d12 ;
delete from t_event_3rdquery_m10_d13 ;
delete from t_event_3rdquery_m10_d14 ;
delete from t_event_3rdquery_m10_d15 ;
delete from t_event_3rdquery_m10_d16 ;
delete from t_event_3rdquery_m10_d17 ;
delete from t_event_3rdquery_m10_d18 ;
delete from t_event_3rdquery_m10_d19 ;
delete from t_event_3rdquery_m10_d2  ;
delete from t_event_3rdquery_m10_d20 ;
delete from t_event_3rdquery_m10_d21 ;
delete from t_event_3rdquery_m10_d22 ;
delete from t_event_3rdquery_m10_d23 ;
delete from t_event_3rdquery_m10_d24 ;
delete from t_event_3rdquery_m10_d25 ;
delete from t_event_3rdquery_m10_d26 ;
delete from t_event_3rdquery_m10_d27 ;
delete from t_event_3rdquery_m10_d28 ;
delete from t_event_3rdquery_m10_d29 ;
delete from t_event_3rdquery_m10_d3  ;
delete from t_event_3rdquery_m10_d30 ;
delete from t_event_3rdquery_m10_d31 ;
delete from t_event_3rdquery_m10_d4  ;
delete from t_event_3rdquery_m10_d5  ;
delete from t_event_3rdquery_m10_d6  ;
delete from t_event_3rdquery_m10_d7  ;
delete from t_event_3rdquery_m10_d8  ;
delete from t_event_3rdquery_m10_d9  ;
delete from t_event_3rdquery_m11     ;
delete from t_event_3rdquery_m11_d1  ;
delete from t_event_3rdquery_m11_d10 ;
delete from t_event_3rdquery_m11_d11 ;
delete from t_event_3rdquery_m11_d12 ;
delete from t_event_3rdquery_m11_d13 ;
delete from t_event_3rdquery_m11_d14 ;
delete from t_event_3rdquery_m11_d15 ;
delete from t_event_3rdquery_m11_d16 ;
delete from t_event_3rdquery_m11_d17 ;
delete from t_event_3rdquery_m11_d18 ;
delete from t_event_3rdquery_m11_d19 ;
delete from t_event_3rdquery_m11_d2  ;
delete from t_event_3rdquery_m11_d20 ;
delete from t_event_3rdquery_m11_d21 ;
delete from t_event_3rdquery_m11_d22 ;
delete from t_event_3rdquery_m11_d23 ;
delete from t_event_3rdquery_m11_d24 ;
delete from t_event_3rdquery_m11_d25 ;
delete from t_event_3rdquery_m11_d26 ;
delete from t_event_3rdquery_m11_d27 ;
delete from t_event_3rdquery_m11_d28 ;
delete from t_event_3rdquery_m11_d29 ;
delete from t_event_3rdquery_m11_d3  ;
delete from t_event_3rdquery_m11_d30 ;
delete from t_event_3rdquery_m11_d4  ;
delete from t_event_3rdquery_m11_d5  ;
delete from t_event_3rdquery_m11_d6  ;
delete from t_event_3rdquery_m11_d7  ;
delete from t_event_3rdquery_m11_d8  ;
delete from t_event_3rdquery_m11_d9  ;
delete from t_event_3rdquery_m12     ;
delete from t_event_3rdquery_m12_d1  ;
delete from t_event_3rdquery_m12_d10 ;
delete from t_event_3rdquery_m12_d11 ;
delete from t_event_3rdquery_m12_d12 ;
delete from t_event_3rdquery_m12_d13 ;
delete from t_event_3rdquery_m12_d14 ;
delete from t_event_3rdquery_m12_d15 ;
delete from t_event_3rdquery_m12_d16 ;
delete from t_event_3rdquery_m12_d17 ;
delete from t_event_3rdquery_m12_d18 ;
delete from t_event_3rdquery_m12_d19 ;
delete from t_event_3rdquery_m12_d2  ;
delete from t_event_3rdquery_m12_d20 ;
delete from t_event_3rdquery_m12_d21 ;
delete from t_event_3rdquery_m12_d22 ;
delete from t_event_3rdquery_m12_d23 ;
delete from t_event_3rdquery_m12_d24 ;
delete from t_event_3rdquery_m12_d25 ;
delete from t_event_3rdquery_m12_d26 ;
delete from t_event_3rdquery_m12_d27 ;
delete from t_event_3rdquery_m12_d28 ;
delete from t_event_3rdquery_m12_d29 ;
delete from t_event_3rdquery_m12_d3  ;
delete from t_event_3rdquery_m12_d30 ;
delete from t_event_3rdquery_m12_d31 ;
delete from t_event_3rdquery_m12_d4  ;
delete from t_event_3rdquery_m12_d5  ;
delete from t_event_3rdquery_m12_d6  ;
delete from t_event_3rdquery_m12_d7  ;
delete from t_event_3rdquery_m12_d8  ;
delete from t_event_3rdquery_m12_d9  ;
delete from t_event_3rdquery_m1_d1   ;
delete from t_event_3rdquery_m1_d10  ;
delete from t_event_3rdquery_m1_d11  ;
delete from t_event_3rdquery_m1_d12  ;
delete from t_event_3rdquery_m1_d13  ;
delete from t_event_3rdquery_m1_d14  ;
delete from t_event_3rdquery_m1_d15  ;
delete from t_event_3rdquery_m1_d16  ;
delete from t_event_3rdquery_m1_d17  ;
delete from t_event_3rdquery_m1_d18  ;
delete from t_event_3rdquery_m1_d19  ;
delete from t_event_3rdquery_m1_d2   ;
delete from t_event_3rdquery_m1_d20  ;
delete from t_event_3rdquery_m1_d21  ;
delete from t_event_3rdquery_m1_d22  ;
delete from t_event_3rdquery_m1_d23  ;
delete from t_event_3rdquery_m1_d24  ;
delete from t_event_3rdquery_m1_d25  ;
delete from t_event_3rdquery_m1_d26  ;
delete from t_event_3rdquery_m1_d27  ;
delete from t_event_3rdquery_m1_d28  ;
delete from t_event_3rdquery_m1_d29  ;
delete from t_event_3rdquery_m1_d3   ;
delete from t_event_3rdquery_m1_d30  ;
delete from t_event_3rdquery_m1_d31  ;
delete from t_event_3rdquery_m1_d4   ;
delete from t_event_3rdquery_m1_d5   ;
delete from t_event_3rdquery_m1_d6   ;
delete from t_event_3rdquery_m1_d7   ;
delete from t_event_3rdquery_m1_d8   ;
delete from t_event_3rdquery_m1_d9   ;
delete from t_event_3rdquery_m2      ;
delete from t_event_3rdquery_m2_d1   ;
delete from t_event_3rdquery_m2_d10  ;
delete from t_event_3rdquery_m2_d11  ;
delete from t_event_3rdquery_m2_d12  ;
delete from t_event_3rdquery_m2_d13  ;
delete from t_event_3rdquery_m2_d14  ;
delete from t_event_3rdquery_m2_d15  ;
delete from t_event_3rdquery_m2_d16  ;
delete from t_event_3rdquery_m2_d17  ;
delete from t_event_3rdquery_m2_d18  ;
delete from t_event_3rdquery_m2_d19  ;
delete from t_event_3rdquery_m2_d2   ;
delete from t_event_3rdquery_m2_d20  ;
delete from t_event_3rdquery_m2_d21  ;
delete from t_event_3rdquery_m2_d22  ;
delete from t_event_3rdquery_m2_d23  ;
delete from t_event_3rdquery_m2_d24  ;
delete from t_event_3rdquery_m2_d25  ;
delete from t_event_3rdquery_m2_d26  ;
delete from t_event_3rdquery_m2_d27  ;
delete from t_event_3rdquery_m2_d28  ;
delete from t_event_3rdquery_m2_d29  ;
delete from t_event_3rdquery_m2_d3   ;
delete from t_event_3rdquery_m2_d4   ;
delete from t_event_3rdquery_m2_d5   ;
delete from t_event_3rdquery_m2_d6   ;
delete from t_event_3rdquery_m2_d7   ;
delete from t_event_3rdquery_m2_d8   ;
delete from t_event_3rdquery_m2_d9   ;
delete from t_event_3rdquery_m3      ;
delete from t_event_3rdquery_m3_d1   ;
delete from t_event_3rdquery_m3_d10  ;
delete from t_event_3rdquery_m3_d11  ;
delete from t_event_3rdquery_m3_d12  ;
delete from t_event_3rdquery_m3_d13  ;
delete from t_event_3rdquery_m3_d14  ;
delete from t_event_3rdquery_m3_d15  ;
delete from t_event_3rdquery_m3_d16  ;
delete from t_event_3rdquery_m3_d17  ;
delete from t_event_3rdquery_m3_d18  ;
delete from t_event_3rdquery_m3_d19  ;
delete from t_event_3rdquery_m3_d2   ;
delete from t_event_3rdquery_m3_d20  ;
delete from t_event_3rdquery_m3_d21  ;
delete from t_event_3rdquery_m3_d22  ;
delete from t_event_3rdquery_m3_d23  ;
delete from t_event_3rdquery_m3_d24  ;
delete from t_event_3rdquery_m3_d25  ;
delete from t_event_3rdquery_m3_d26  ;
delete from t_event_3rdquery_m3_d27  ;
delete from t_event_3rdquery_m3_d28  ;
delete from t_event_3rdquery_m3_d29  ;
delete from t_event_3rdquery_m3_d3   ;
delete from t_event_3rdquery_m3_d30  ;
delete from t_event_3rdquery_m3_d31  ;
delete from t_event_3rdquery_m3_d4   ;
delete from t_event_3rdquery_m3_d5   ;
delete from t_event_3rdquery_m3_d6   ;
delete from t_event_3rdquery_m3_d7   ;
delete from t_event_3rdquery_m3_d8   ;
delete from t_event_3rdquery_m3_d9   ;
delete from t_event_3rdquery_m4      ;
delete from t_event_3rdquery_m4_d1   ;
delete from t_event_3rdquery_m4_d10  ;
delete from t_event_3rdquery_m4_d11  ;
delete from t_event_3rdquery_m4_d12  ;
delete from t_event_3rdquery_m4_d13  ;
delete from t_event_3rdquery_m4_d14  ;
delete from t_event_3rdquery_m4_d15  ;
delete from t_event_3rdquery_m4_d16  ;
delete from t_event_3rdquery_m4_d17  ;
delete from t_event_3rdquery_m4_d18  ;
delete from t_event_3rdquery_m4_d19  ;
delete from t_event_3rdquery_m4_d2   ;
delete from t_event_3rdquery_m4_d20  ;
delete from t_event_3rdquery_m4_d21  ;
delete from t_event_3rdquery_m4_d22  ;
delete from t_event_3rdquery_m4_d23  ;
delete from t_event_3rdquery_m4_d24  ;
delete from t_event_3rdquery_m4_d25  ;
delete from t_event_3rdquery_m4_d26  ;
delete from t_event_3rdquery_m4_d27  ;
delete from t_event_3rdquery_m4_d28  ;
delete from t_event_3rdquery_m4_d29  ;
delete from t_event_3rdquery_m4_d3   ;
delete from t_event_3rdquery_m4_d30  ;
delete from t_event_3rdquery_m4_d4   ;
delete from t_event_3rdquery_m4_d5   ;
delete from t_event_3rdquery_m4_d6   ;
delete from t_event_3rdquery_m4_d7   ;
delete from t_event_3rdquery_m4_d8   ;
delete from t_event_3rdquery_m4_d9   ;
delete from t_event_3rdquery_m5      ;
delete from t_event_3rdquery_m5_d1   ;
delete from t_event_3rdquery_m5_d10  ;
delete from t_event_3rdquery_m5_d11  ;
delete from t_event_3rdquery_m5_d12  ;
delete from t_event_3rdquery_m5_d13  ;
delete from t_event_3rdquery_m5_d14  ;
delete from t_event_3rdquery_m5_d15  ;
delete from t_event_3rdquery_m5_d16  ;
delete from t_event_3rdquery_m5_d17  ;
delete from t_event_3rdquery_m5_d18  ;
delete from t_event_3rdquery_m5_d19  ;
delete from t_event_3rdquery_m5_d2   ;
delete from t_event_3rdquery_m5_d20  ;
delete from t_event_3rdquery_m5_d21  ;
delete from t_event_3rdquery_m5_d22  ;
delete from t_event_3rdquery_m5_d23  ;
delete from t_event_3rdquery_m5_d24  ;
delete from t_event_3rdquery_m5_d25  ;
delete from t_event_3rdquery_m5_d26  ;
delete from t_event_3rdquery_m5_d27  ;
delete from t_event_3rdquery_m5_d28  ;
delete from t_event_3rdquery_m5_d29  ;
delete from t_event_3rdquery_m5_d3   ;
delete from t_event_3rdquery_m5_d30  ;
delete from t_event_3rdquery_m5_d31  ;
delete from t_event_3rdquery_m5_d4   ;
delete from t_event_3rdquery_m5_d5   ;
delete from t_event_3rdquery_m5_d6   ;
delete from t_event_3rdquery_m5_d7   ;
delete from t_event_3rdquery_m5_d8   ;
delete from t_event_3rdquery_m5_d9   ;
delete from t_event_3rdquery_m6      ;
delete from t_event_3rdquery_m6_d1   ;
delete from t_event_3rdquery_m6_d10  ;
delete from t_event_3rdquery_m6_d11  ;
delete from t_event_3rdquery_m6_d12  ;
delete from t_event_3rdquery_m6_d13  ;
delete from t_event_3rdquery_m6_d14  ;
delete from t_event_3rdquery_m6_d15  ;
delete from t_event_3rdquery_m6_d16  ;
delete from t_event_3rdquery_m6_d17  ;
delete from t_event_3rdquery_m6_d18  ;
delete from t_event_3rdquery_m6_d19  ;
delete from t_event_3rdquery_m6_d2   ;
delete from t_event_3rdquery_m6_d20  ;
delete from t_event_3rdquery_m6_d21  ;
delete from t_event_3rdquery_m6_d22  ;
delete from t_event_3rdquery_m6_d23  ;
delete from t_event_3rdquery_m6_d24  ;
delete from t_event_3rdquery_m6_d25  ;
delete from t_event_3rdquery_m6_d26  ;
delete from t_event_3rdquery_m6_d27  ;
delete from t_event_3rdquery_m6_d28  ;
delete from t_event_3rdquery_m6_d29  ;
delete from t_event_3rdquery_m6_d3   ;
delete from t_event_3rdquery_m6_d30  ;
delete from t_event_3rdquery_m6_d4   ;
delete from t_event_3rdquery_m6_d5   ;
delete from t_event_3rdquery_m6_d6   ;
delete from t_event_3rdquery_m6_d7   ;
delete from t_event_3rdquery_m6_d8   ;
delete from t_event_3rdquery_m6_d9   ;
delete from t_event_3rdquery_m7      ;
delete from t_event_3rdquery_m7_d1   ;
delete from t_event_3rdquery_m7_d10  ;
delete from t_event_3rdquery_m7_d11  ;
delete from t_event_3rdquery_m7_d12  ;
delete from t_event_3rdquery_m7_d13  ;
delete from t_event_3rdquery_m7_d14  ;
delete from t_event_3rdquery_m7_d15  ;
delete from t_event_3rdquery_m7_d16  ;
delete from t_event_3rdquery_m7_d17  ;
delete from t_event_3rdquery_m7_d18  ;
delete from t_event_3rdquery_m7_d19  ;
delete from t_event_3rdquery_m7_d2   ;
delete from t_event_3rdquery_m7_d20  ;
delete from t_event_3rdquery_m7_d21  ;
delete from t_event_3rdquery_m7_d22  ;
delete from t_event_3rdquery_m7_d23  ;
delete from t_event_3rdquery_m7_d24  ;
delete from t_event_3rdquery_m7_d25  ;
delete from t_event_3rdquery_m7_d26  ;
delete from t_event_3rdquery_m7_d27  ;
delete from t_event_3rdquery_m7_d28  ;
delete from t_event_3rdquery_m7_d29  ;
delete from t_event_3rdquery_m7_d3   ;
delete from t_event_3rdquery_m7_d30  ;
delete from t_event_3rdquery_m7_d31  ;
delete from t_event_3rdquery_m7_d4   ;
delete from t_event_3rdquery_m7_d5   ;
delete from t_event_3rdquery_m7_d6   ;
delete from t_event_3rdquery_m7_d7   ;
delete from t_event_3rdquery_m7_d8   ;
delete from t_event_3rdquery_m7_d9   ;
delete from t_event_3rdquery_m8      ;
delete from t_event_3rdquery_m8_d1   ;
delete from t_event_3rdquery_m8_d10  ;
delete from t_event_3rdquery_m8_d11  ;
delete from t_event_3rdquery_m8_d12  ;
delete from t_event_3rdquery_m8_d13  ;
delete from t_event_3rdquery_m8_d14  ;
delete from t_event_3rdquery_m8_d15  ;
delete from t_event_3rdquery_m8_d16  ;
delete from t_event_3rdquery_m8_d17  ;
delete from t_event_3rdquery_m8_d18  ;
delete from t_event_3rdquery_m8_d19  ;
delete from t_event_3rdquery_m8_d2   ;
delete from t_event_3rdquery_m8_d20  ;
delete from t_event_3rdquery_m8_d21  ;
delete from t_event_3rdquery_m8_d22  ;
delete from t_event_3rdquery_m8_d23  ;
delete from t_event_3rdquery_m8_d24  ;
delete from t_event_3rdquery_m8_d25  ;
delete from t_event_3rdquery_m8_d26  ;
delete from t_event_3rdquery_m8_d27  ;
delete from t_event_3rdquery_m8_d28  ;
delete from t_event_3rdquery_m8_d29  ;
delete from t_event_3rdquery_m8_d3   ;
delete from t_event_3rdquery_m8_d30  ;
delete from t_event_3rdquery_m8_d31  ;
delete from t_event_3rdquery_m8_d4   ;
delete from t_event_3rdquery_m8_d5   ;
delete from t_event_3rdquery_m8_d6   ;
delete from t_event_3rdquery_m8_d7   ;
delete from t_event_3rdquery_m8_d8   ;
delete from t_event_3rdquery_m8_d9   ;
delete from t_event_3rdquery_m9      ;
delete from t_event_3rdquery_m9_d1   ;
delete from t_event_3rdquery_m9_d10  ;
delete from t_event_3rdquery_m9_d11  ;
delete from t_event_3rdquery_m9_d12  ;
delete from t_event_3rdquery_m9_d13  ;
delete from t_event_3rdquery_m9_d14  ;
delete from t_event_3rdquery_m9_d15  ;
delete from t_event_3rdquery_m9_d16  ;
delete from t_event_3rdquery_m9_d17  ;
delete from t_event_3rdquery_m9_d18  ;
delete from t_event_3rdquery_m9_d19  ;
delete from t_event_3rdquery_m9_d2   ;
delete from t_event_3rdquery_m9_d20  ;
delete from t_event_3rdquery_m9_d21  ;
delete from t_event_3rdquery_m9_d22  ;
delete from t_event_3rdquery_m9_d23  ;
delete from t_event_3rdquery_m9_d24  ;
delete from t_event_3rdquery_m9_d25  ;
delete from t_event_3rdquery_m9_d26  ;
delete from t_event_3rdquery_m9_d27  ;
delete from t_event_3rdquery_m9_d28  ;
delete from t_event_3rdquery_m9_d29  ;
delete from t_event_3rdquery_m9_d3   ;
delete from t_event_3rdquery_m9_d30  ;
delete from t_event_3rdquery_m9_d4   ;
delete from t_event_3rdquery_m9_d5   ;
delete from t_event_3rdquery_m9_d6   ;
delete from t_event_3rdquery_m9_d7   ;
delete from t_event_3rdquery_m9_d8   ;
delete from t_event_3rdquery_m9_d9   ;
vacuum analyze t_event_3rdquery;
E2
EOF
}

delete_3rd(){
su - postgres <<EOF
/db/postgresql/bin/psql gms_db <<E2
delete from t_event_3rdtmp;      
delete from t_event_3rdhis_p1;   
delete from t_event_3rdhis_p10;  
delete from t_event_3rdhis_p11;  
delete from t_event_3rdhis_p12;  
delete from t_event_3rdhis_p2;   
delete from t_event_3rdhis_p3;   
delete from t_event_3rdhis_p4;   
delete from t_event_3rdhis_p5;   
delete from t_event_3rdhis_p6;   
delete from t_event_3rdhis_p7;   
delete from t_event_3rdhis_p8;   
delete from t_event_3rdhis_p9;   
delete from t_event_3rdhis;      
delete from t_event_3rdhour;     
delete from t_event_3rdmin;      
delete from t_event_3rdday;      
delete from t_event_3rdweek;      
delete from t_event_3rdmonth;
delete from t_event_3rdyear;      
vacuum analyze t_event_3rdhis;      
vacuum analyze t_event_3rdhour;     
vacuum analyze t_event_3rdmin;      
vacuum analyze t_event_3rdday;      
vacuum analyze t_event_3rdweek;      
vacuum analyze t_event_3rdmonth;
vacuum analyze t_event_3rdyear;      
vacuum analyze t_event_3rdtmp;      
E2
EOF
}

delete_abb(){
su - postgres <<EOF
/db/postgresql/bin/psql gms_db <<E2
delete from t_event_abb;
vacuum analyze t_event_abb;
E2
EOF
}
delete_attack(){
su - postgres <<EOF
/db/postgresql/bin/psql gms_db <<E2
delete from t_event_attacktmp    ;
delete from t_event_attackhis    ;
delete from t_event_attackhis_p1 ;
delete from t_event_attackhis_p10;
delete from t_event_attackhis_p11;
delete from t_event_attackhis_p12;
delete from t_event_attackhis_p2 ;
delete from t_event_attackhis_p3 ;
delete from t_event_attackhis_p4 ;
delete from t_event_attackhis_p5 ;
delete from t_event_attackhis_p6 ;
delete from t_event_attackhis_p7 ;
delete from t_event_attackhis_p8 ;
delete from t_event_attackhis_p9 ;
delete from t_event_attackmin    ;
delete from t_event_attackhour   ;
delete from t_event_attackday    ;
delete from t_event_attackweek   ;
delete from t_event_attackmonth  ;
delete from t_event_attackyear   ;
E2
EOF
}

delete_attackquery(){
su - postgres <<EOF
/db/postgresql/bin/psql gms_db <<E2
delete from t_event_attackquery_m1     ;
delete from t_event_attackquery_m10    ;
delete from t_event_attackquery_m10_d1 ;
delete from t_event_attackquery_m10_d10;
delete from t_event_attackquery_m10_d11;
delete from t_event_attackquery_m10_d12;
delete from t_event_attackquery_m10_d13;
delete from t_event_attackquery_m10_d14;
delete from t_event_attackquery_m10_d15;
delete from t_event_attackquery_m10_d16;
delete from t_event_attackquery_m10_d17;
delete from t_event_attackquery_m10_d18;
delete from t_event_attackquery_m10_d19;
delete from t_event_attackquery_m10_d2 ;
delete from t_event_attackquery_m10_d20;
delete from t_event_attackquery_m10_d21;
delete from t_event_attackquery_m10_d22;
delete from t_event_attackquery_m10_d23;
delete from t_event_attackquery_m10_d24;
delete from t_event_attackquery_m10_d25;
delete from t_event_attackquery_m10_d26;
delete from t_event_attackquery_m10_d27;
delete from t_event_attackquery_m10_d28;
delete from t_event_attackquery_m10_d29;
delete from t_event_attackquery_m10_d3 ;
delete from t_event_attackquery_m10_d30;
delete from t_event_attackquery_m10_d31;
delete from t_event_attackquery_m10_d4 ;
delete from t_event_attackquery_m10_d5 ;
delete from t_event_attackquery_m10_d6 ;
delete from t_event_attackquery_m10_d7 ;
delete from t_event_attackquery_m10_d8 ;
delete from t_event_attackquery_m10_d9 ;
delete from t_event_attackquery_m11    ;
delete from t_event_attackquery_m11_d1 ;
delete from t_event_attackquery_m11_d10;
delete from t_event_attackquery_m11_d11;
delete from t_event_attackquery_m11_d12;
delete from t_event_attackquery_m11_d13;
delete from t_event_attackquery_m11_d14;
delete from t_event_attackquery_m11_d15;
delete from t_event_attackquery_m11_d16;
delete from t_event_attackquery_m11_d17;
delete from t_event_attackquery_m11_d18;
delete from t_event_attackquery_m11_d19;
delete from t_event_attackquery_m11_d2 ;
delete from t_event_attackquery_m11_d20;
delete from t_event_attackquery_m11_d21;
delete from t_event_attackquery_m11_d22;
delete from t_event_attackquery_m11_d23;
delete from t_event_attackquery_m11_d24;
delete from t_event_attackquery_m11_d25;
delete from t_event_attackquery_m11_d26;
delete from t_event_attackquery_m11_d27;
delete from t_event_attackquery_m11_d28;
delete from t_event_attackquery_m11_d29;
delete from t_event_attackquery_m11_d3 ;
delete from t_event_attackquery_m11_d30;
delete from t_event_attackquery_m11_d4 ;
delete from t_event_attackquery_m11_d5 ;
delete from t_event_attackquery_m11_d6 ;
delete from t_event_attackquery_m11_d7 ;
delete from t_event_attackquery_m11_d8 ;
delete from t_event_attackquery_m11_d9 ;
delete from t_event_attackquery_m12    ;
delete from t_event_attackquery_m12_d1 ;
delete from t_event_attackquery_m12_d10;
delete from t_event_attackquery_m12_d11;
delete from t_event_attackquery_m12_d12;
delete from t_event_attackquery_m12_d13;
delete from t_event_attackquery_m12_d14;
delete from t_event_attackquery_m12_d15;
delete from t_event_attackquery_m12_d16;
delete from t_event_attackquery_m12_d17;
delete from t_event_attackquery_m12_d18;
delete from t_event_attackquery_m12_d19;
delete from t_event_attackquery_m12_d2 ;
delete from t_event_attackquery_m12_d20;
delete from t_event_attackquery_m12_d21;
delete from t_event_attackquery_m12_d22;
delete from t_event_attackquery_m12_d23;
delete from t_event_attackquery_m12_d24;
delete from t_event_attackquery_m12_d25;
delete from t_event_attackquery_m12_d26;
delete from t_event_attackquery_m12_d27;
delete from t_event_attackquery_m12_d28;
delete from t_event_attackquery_m12_d29;
delete from t_event_attackquery_m12_d3 ;
delete from t_event_attackquery_m12_d30;
delete from t_event_attackquery_m12_d31;
delete from t_event_attackquery_m12_d4 ;
delete from t_event_attackquery_m12_d5 ;
delete from t_event_attackquery_m12_d6 ;
delete from t_event_attackquery_m12_d7 ;
delete from t_event_attackquery_m12_d8 ;
delete from t_event_attackquery_m12_d9 ;
delete from t_event_attackquery_m1_d1  ;
delete from t_event_attackquery_m1_d10 ;
delete from t_event_attackquery_m1_d11 ;
delete from t_event_attackquery_m1_d12 ;
delete from t_event_attackquery_m1_d13 ;
delete from t_event_attackquery_m1_d14 ;
delete from t_event_attackquery_m1_d15 ;
delete from t_event_attackquery_m1_d16 ;
delete from t_event_attackquery_m1_d17 ;
delete from t_event_attackquery_m1_d18 ;
delete from t_event_attackquery_m1_d19 ;
delete from t_event_attackquery_m1_d2  ;
delete from t_event_attackquery_m1_d20 ;
delete from t_event_attackquery_m1_d21 ;
delete from t_event_attackquery_m1_d22 ;
delete from t_event_attackquery_m1_d23 ;
delete from t_event_attackquery_m1_d24 ;
delete from t_event_attackquery_m1_d25 ;
delete from t_event_attackquery_m1_d26 ;
delete from t_event_attackquery_m1_d27 ;
delete from t_event_attackquery_m1_d28 ;
delete from t_event_attackquery_m1_d29 ;
delete from t_event_attackquery_m1_d3  ;
delete from t_event_attackquery_m1_d30 ;
delete from t_event_attackquery_m1_d31 ;
delete from t_event_attackquery_m1_d4  ;
delete from t_event_attackquery_m1_d5  ;
delete from t_event_attackquery_m1_d6  ;
delete from t_event_attackquery_m1_d7  ;
delete from t_event_attackquery_m1_d8  ;
delete from t_event_attackquery_m1_d9  ;
delete from t_event_attackquery_m2     ;
delete from t_event_attackquery_m2_d1  ;
delete from t_event_attackquery_m2_d10 ;
delete from t_event_attackquery_m2_d11 ;
delete from t_event_attackquery_m2_d12 ;
delete from t_event_attackquery_m2_d13 ;
delete from t_event_attackquery_m2_d14 ;
delete from t_event_attackquery_m2_d15 ;
delete from t_event_attackquery_m2_d16 ;
delete from t_event_attackquery_m2_d17 ;
delete from t_event_attackquery_m2_d18 ;
delete from t_event_attackquery_m2_d19 ;
delete from t_event_attackquery_m2_d2  ;
delete from t_event_attackquery_m2_d20 ;
delete from t_event_attackquery_m2_d21 ;
delete from t_event_attackquery_m2_d22 ;
delete from t_event_attackquery_m2_d23 ;
delete from t_event_attackquery_m2_d24 ;
delete from t_event_attackquery_m2_d25 ;
delete from t_event_attackquery_m2_d26 ;
delete from t_event_attackquery_m2_d27 ;
delete from t_event_attackquery_m2_d28 ;
delete from t_event_attackquery_m2_d29 ;
delete from t_event_attackquery_m2_d3  ;
delete from t_event_attackquery_m2_d4  ;
delete from t_event_attackquery_m2_d5  ;
delete from t_event_attackquery_m2_d6  ;
delete from t_event_attackquery_m2_d7  ;
delete from t_event_attackquery_m2_d8  ;
delete from t_event_attackquery_m2_d9  ;
delete from t_event_attackquery_m3     ;
delete from t_event_attackquery_m3_d1  ;
delete from t_event_attackquery_m3_d10 ;
delete from t_event_attackquery_m3_d11 ;
delete from t_event_attackquery_m3_d12 ;
delete from t_event_attackquery_m3_d13 ;
delete from t_event_attackquery_m3_d14 ;
delete from t_event_attackquery_m3_d15 ;
delete from t_event_attackquery_m3_d16 ;
delete from t_event_attackquery_m3_d17 ;
delete from t_event_attackquery_m3_d18 ;
delete from t_event_attackquery_m3_d19 ;
delete from t_event_attackquery_m3_d2  ;
delete from t_event_attackquery_m3_d20 ;
delete from t_event_attackquery_m3_d21 ;
delete from t_event_attackquery_m3_d22 ;
delete from t_event_attackquery_m3_d23 ;
delete from t_event_attackquery_m3_d24 ;
delete from t_event_attackquery_m3_d25 ;
delete from t_event_attackquery_m3_d26 ;
delete from t_event_attackquery_m3_d27 ;
delete from t_event_attackquery_m3_d28 ;
delete from t_event_attackquery_m3_d29 ;
delete from t_event_attackquery_m3_d3  ;
delete from t_event_attackquery_m3_d30 ;
delete from t_event_attackquery_m3_d31 ;
delete from t_event_attackquery_m3_d4  ;
delete from t_event_attackquery_m3_d5  ;
delete from t_event_attackquery_m3_d6  ;
delete from t_event_attackquery_m3_d7  ;
delete from t_event_attackquery_m3_d8  ;
delete from t_event_attackquery_m3_d9  ;
delete from t_event_attackquery_m4     ;
delete from t_event_attackquery_m4_d1  ;
delete from t_event_attackquery_m4_d10 ;
delete from t_event_attackquery_m4_d11 ;
delete from t_event_attackquery_m4_d12 ;
delete from t_event_attackquery_m4_d13 ;
delete from t_event_attackquery_m4_d14 ;
delete from t_event_attackquery_m4_d15 ;
delete from t_event_attackquery_m4_d16 ;
delete from t_event_attackquery_m4_d17 ;
delete from t_event_attackquery_m4_d18 ;
delete from t_event_attackquery_m4_d19 ;
delete from t_event_attackquery_m4_d2  ;
delete from t_event_attackquery_m4_d20 ;
delete from t_event_attackquery_m4_d21 ;
delete from t_event_attackquery_m4_d22 ;
delete from t_event_attackquery_m4_d23 ;
delete from t_event_attackquery_m4_d24 ;
delete from t_event_attackquery_m4_d25 ;
delete from t_event_attackquery_m4_d26 ;
delete from t_event_attackquery_m4_d27 ;
delete from t_event_attackquery_m4_d28 ;
delete from t_event_attackquery_m4_d29 ;
delete from t_event_attackquery_m4_d3  ;
delete from t_event_attackquery_m4_d30 ;
delete from t_event_attackquery_m4_d4  ;
delete from t_event_attackquery_m4_d5  ;
delete from t_event_attackquery_m4_d6  ;
delete from t_event_attackquery_m4_d7  ;
delete from t_event_attackquery_m4_d8  ;
delete from t_event_attackquery_m4_d9  ;
delete from t_event_attackquery_m5     ;
delete from t_event_attackquery_m5_d1  ;
delete from t_event_attackquery_m5_d10 ;
delete from t_event_attackquery_m5_d11 ;
delete from t_event_attackquery_m5_d12 ;
delete from t_event_attackquery_m5_d13 ;
delete from t_event_attackquery_m5_d14 ;
delete from t_event_attackquery_m5_d15 ;
delete from t_event_attackquery_m5_d16 ;
delete from t_event_attackquery_m5_d17 ;
delete from t_event_attackquery_m5_d18 ;
delete from t_event_attackquery_m5_d19 ;
delete from t_event_attackquery_m5_d2  ;
delete from t_event_attackquery_m5_d20 ;
delete from t_event_attackquery_m5_d21 ;
delete from t_event_attackquery_m5_d22 ;
delete from t_event_attackquery_m5_d23 ;
delete from t_event_attackquery_m5_d24 ;
delete from t_event_attackquery_m5_d25 ;
delete from t_event_attackquery_m5_d26 ;
delete from t_event_attackquery_m5_d27 ;
delete from t_event_attackquery_m5_d28 ;
delete from t_event_attackquery_m5_d29 ;
delete from t_event_attackquery_m5_d3  ;
delete from t_event_attackquery_m5_d30 ;
delete from t_event_attackquery_m5_d31 ;
delete from t_event_attackquery_m5_d4  ;
delete from t_event_attackquery_m5_d5  ;
delete from t_event_attackquery_m5_d6  ;
delete from t_event_attackquery_m5_d7  ;
delete from t_event_attackquery_m5_d8  ;
delete from t_event_attackquery_m5_d9  ;
delete from t_event_attackquery_m6     ;
delete from t_event_attackquery_m6_d1  ;
delete from t_event_attackquery_m6_d10 ;
delete from t_event_attackquery_m6_d11 ;
delete from t_event_attackquery_m6_d12 ;
delete from t_event_attackquery_m6_d13 ;
delete from t_event_attackquery_m6_d14 ;
delete from t_event_attackquery_m6_d15 ;
delete from t_event_attackquery_m6_d16 ;
delete from t_event_attackquery_m6_d17 ;
delete from t_event_attackquery_m6_d18 ;
delete from t_event_attackquery_m6_d19 ;
delete from t_event_attackquery_m6_d2  ;
delete from t_event_attackquery_m6_d20 ;
delete from t_event_attackquery_m6_d21 ;
delete from t_event_attackquery_m6_d22 ;
delete from t_event_attackquery_m6_d23 ;
delete from t_event_attackquery_m6_d24 ;
delete from t_event_attackquery_m6_d25 ;
delete from t_event_attackquery_m6_d26 ;
delete from t_event_attackquery_m6_d27 ;
delete from t_event_attackquery_m6_d28 ;
delete from t_event_attackquery_m6_d29 ;
delete from t_event_attackquery_m6_d3  ;
delete from t_event_attackquery_m6_d30 ;
delete from t_event_attackquery_m6_d4  ;
delete from t_event_attackquery_m6_d5  ;
delete from t_event_attackquery_m6_d6  ;
delete from t_event_attackquery_m6_d7  ;
delete from t_event_attackquery_m6_d8  ;
delete from t_event_attackquery_m6_d9  ;
delete from t_event_attackquery_m7     ;
delete from t_event_attackquery_m7_d1  ;
delete from t_event_attackquery_m7_d10 ;
delete from t_event_attackquery_m7_d11 ;
delete from t_event_attackquery_m7_d12 ;
delete from t_event_attackquery_m7_d13 ;
delete from t_event_attackquery_m7_d14 ;
delete from t_event_attackquery_m7_d15 ;
delete from t_event_attackquery_m7_d16 ;
delete from t_event_attackquery_m7_d17 ;
delete from t_event_attackquery_m7_d18 ;
delete from t_event_attackquery_m7_d19 ;
delete from t_event_attackquery_m7_d2  ;
delete from t_event_attackquery_m7_d20 ;
delete from t_event_attackquery_m7_d21 ;
delete from t_event_attackquery_m7_d22 ;
delete from t_event_attackquery_m7_d23 ;
delete from t_event_attackquery_m7_d24 ;
delete from t_event_attackquery_m7_d25 ;
delete from t_event_attackquery_m7_d26 ;
delete from t_event_attackquery_m7_d27 ;
delete from t_event_attackquery_m7_d28 ;
delete from t_event_attackquery_m7_d29 ;
delete from t_event_attackquery_m7_d3  ;
delete from t_event_attackquery_m7_d30 ;
delete from t_event_attackquery_m7_d31 ;
delete from t_event_attackquery_m7_d4  ;
delete from t_event_attackquery_m7_d5  ;
delete from t_event_attackquery_m7_d6  ;
delete from t_event_attackquery_m7_d7  ;
delete from t_event_attackquery_m7_d8  ;
delete from t_event_attackquery_m7_d9  ;
delete from t_event_attackquery_m8     ;
delete from t_event_attackquery_m8_d1  ;
delete from t_event_attackquery_m8_d10 ;
delete from t_event_attackquery_m8_d11 ;
delete from t_event_attackquery_m8_d12 ;
delete from t_event_attackquery_m8_d13 ;
delete from t_event_attackquery_m8_d14 ;
delete from t_event_attackquery_m8_d15 ;
delete from t_event_attackquery_m8_d16 ;
delete from t_event_attackquery_m8_d17 ;
delete from t_event_attackquery_m8_d18 ;
delete from t_event_attackquery_m8_d19 ;
delete from t_event_attackquery_m8_d2  ;
delete from t_event_attackquery_m8_d20 ;
delete from t_event_attackquery_m8_d21 ;
delete from t_event_attackquery_m8_d22 ;
delete from t_event_attackquery_m8_d23 ;
delete from t_event_attackquery_m8_d24 ;
delete from t_event_attackquery_m8_d25 ;
delete from t_event_attackquery_m8_d26 ;
delete from t_event_attackquery_m8_d27 ;
delete from t_event_attackquery_m8_d28 ;
delete from t_event_attackquery_m8_d29 ;
delete from t_event_attackquery_m8_d3  ;
delete from t_event_attackquery_m8_d30 ;
delete from t_event_attackquery_m8_d31 ;
delete from t_event_attackquery_m8_d4  ;
delete from t_event_attackquery_m8_d5  ;
delete from t_event_attackquery_m8_d6  ;
delete from t_event_attackquery_m8_d7  ;
delete from t_event_attackquery_m8_d8  ;
delete from t_event_attackquery_m8_d9  ;
delete from t_event_attackquery_m9     ;
delete from t_event_attackquery_m9_d1  ;
delete from t_event_attackquery_m9_d10 ;
delete from t_event_attackquery_m9_d11 ;
delete from t_event_attackquery_m9_d12 ;
delete from t_event_attackquery_m9_d13 ;
delete from t_event_attackquery_m9_d14 ;
delete from t_event_attackquery_m9_d15 ;
delete from t_event_attackquery_m9_d16 ;
delete from t_event_attackquery_m9_d17 ;
delete from t_event_attackquery_m9_d18 ;
delete from t_event_attackquery_m9_d19 ;
delete from t_event_attackquery_m9_d2  ;
delete from t_event_attackquery_m9_d20 ;
delete from t_event_attackquery_m9_d21 ;
delete from t_event_attackquery_m9_d22 ;
delete from t_event_attackquery_m9_d23 ;
delete from t_event_attackquery_m9_d24 ;
delete from t_event_attackquery_m9_d25 ;
delete from t_event_attackquery_m9_d26 ;
delete from t_event_attackquery_m9_d27 ;
delete from t_event_attackquery_m9_d28 ;
delete from t_event_attackquery_m9_d29 ;
delete from t_event_attackquery_m9_d3  ;
delete from t_event_attackquery_m9_d30 ;
delete from t_event_attackquery_m9_d4  ;
delete from t_event_attackquery_m9_d5  ;
delete from t_event_attackquery_m9_d6  ;
delete from t_event_attackquery_m9_d7  ;
delete from t_event_attackquery_m9_d8  ;
delete from t_event_attackquery_m9_d9  ;
delete from t_event_attackquery;
vacuum analyze t_event_attackquery;
E2
EOF
}

delete_av_url_etc(){
su - postgres <<EOF
/db/postgresql/bin/psql gms_db <<E2
delete from t_event_av ; 
delete from t_event_bdns;
delete from t_event_burl;
vacuum analyze t_event_av ; 
vacuum analyze t_event_bdns;
vacuum analyze t_event_burl;
E2
EOF
}
delete_flow(){
su - postgres <<EOF
/db/postgresql/bin/psql gms_db <<E2
delete from t_event_flowminu_p1 ;
delete from t_event_flowminu_p10;
delete from t_event_flowminu_p11;
delete from t_event_flowminu_p12;
delete from t_event_flowminu_p2 ;
delete from t_event_flowminu_p3 ;
delete from t_event_flowminu_p4 ;
delete from t_event_flowminu_p5 ;
delete from t_event_flowminu_p6 ;
delete from t_event_flowminu_p7 ;
delete from t_event_flowminu_p8 ;
delete from t_event_flowminu_p9 ;
delete from t_event_flowday     ;
delete from t_event_flowhour    ;
delete from t_event_flowminu    ;
vacuum analyze t_event_flowday;
vacuum analyze t_event_flowhour;
vacuum analyze t_event_flowmin;
E2
EOF
}
delete_stat(){
su - postgres <<EOF
/db/postgresql/bin/psql gms_db <<E2
delete from t_event_sysstatus_p1 ; 
delete from t_event_sysstatus_p10; 
delete from t_event_sysstatus_p11; 
delete from t_event_sysstatus_p12; 
delete from t_event_sysstatus_p2 ; 
delete from t_event_sysstatus_p3 ; 
delete from t_event_sysstatus_p4 ; 
delete from t_event_sysstatus_p5 ; 
delete from t_event_sysstatus_p6 ; 
delete from t_event_sysstatus_p7 ; 
delete from t_event_sysstatus_p8 ; 
delete from t_event_sysstatus_p9 ; 
delete from t_event_sysstatus    ; 
delete from t_log_report ;
vacuum analyze t_event_sysstatus;
E2
EOF
}


delete_log(){
killall -9 java
killall -9 crond
killall -9 python
killall -9 store_proc
killall -9 flow_proc
killall -9 merge_proc
killall -9 sys_guard
killall -9 mtx
killall -9 apt
killall -9 dd
killall -9 apc
killall -9 dm
find /data/log/ -name *.log|xargs rm -rf 
find /data/ -name *.ok|xargs rm -rf 
rm -rf /gms/conf/*.gau
rm -rf /gms/conf/reg.dat
}

init_conf(){
	cp -f $src_xmlfile $dst_path
	if [ "$Mtype" == "1" ];then
		dev=eth0
	else
		dev=eth1
	fi	
	sed -i '/^IPADDR/d' $net_path$dev;sed -i "1a IPADDR=$Ip_addr" $net_path$dev
	sed -i '/^NETMASK/d' $net_path$dev;sed -i "1a NETMASK=$Mask" $net_path$dev
	sed -i '/^GATEWAY/d' $net_path$dev;sed -i "1a GATEWAY=$Gw" $net_path$dev
	sed -i '/^DNS/d' $net_path$dev;sed -i "1a DNS=$Dns" $net_path$dev
	ifconfig $dev $Ip_addr netmask $Mask
	route add default gw $Gw
		
}

delete_log
delete_3rdquery
delete_attackquery
delete_stat
delete_conf
delete_3rd
delete_attack
delete_flow
delete_abb
delete_av_url_etc
init_conf
sync
sync
sync
reboot
