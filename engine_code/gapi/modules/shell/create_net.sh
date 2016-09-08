#!/bin/sh
/db/postgresql/bin/psql -U postgres -d gms_db <<E2
CREATE TABLE t_net_audit			/*用于应用类型详细查询*/
(
 id 			serial NOT NULL,
 dev_id 		character varying(64),			/*设备的devid,通过这个devid和t_conf_device表做关联查询可知道是哪台设备产生的*/
 ftime_start 	bigint,							/*开始时间*/
 ftime_end 	bigint,							/*结束时间*/
 if_mobite 	smallint,						/*区分pc和移动 0 为pc端，1 为移动端*/
 service_group integer,						/*应用类型所属组*/
 serivce_type 	integer,						/*应用类型*/
 protol_id		integer,						/*为协议id*/
 up_bytes 		bigint,							/*单位是字节*/
 down_bytes 	bigint,							/*单位是字节*/
 userip character varying(16),					/*用户ip*/
 user_id		character varying(32),							/*用户账号*/
 hash_id		integer,
 CONSTRAINT pk_t_net_audit PRIMARY KEY (id)
 )
WITH (OIDS=FALSE);
ALTER TABLE t_net_audit OWNER TO gms_user;
CREATE INDEX net_audit_st
ON t_net_audit
USING btree
(ftime_start);
CREATE INDEX net_audit_et
ON t_net_audit
USING btree
(ftime_end);
CREATE INDEX net_audit_up
ON t_net_audit
USING btree
(up_bytes);
CREATE INDEX net_audit_down
ON t_net_audit
USING btree
(down_bytes);
CREATE TABLE t_net_audit_0
(
-- 继承:   id integer NOT NULL DEFAULT,
-- 继承:   dev_id character varying(64),
-- 继承:   ftime_start bigint,
-- 继承:   ftime_end bigint,
-- 继承:   if_mobite smallint,
-- 继承:   service_group integer,
-- 继承:   serivce_type integer,
-- 继承:   protol_id integer,
-- 继承:   up_bytes bigint,
-- 继承:   down_bytes bigint,
-- 继承:   userip character varying(16),
-- 继承:   user_id character varying(32),
-- 继承:   hash_id integer,
  CONSTRAINT hash_net_id_0 CHECK (hash_id = 0)
)
INHERITS (t_net_audit)
WITH (OIDS=FALSE);
ALTER TABLE t_net_audit_0 OWNER TO gms_user;

CREATE TABLE t_net_audit_1
(
-- 继承:   id integer NOT NULL DEFAULT,
-- 继承:   dev_id character varying(64),
-- 继承:   ftime_start bigint,
-- 继承:   ftime_end bigint,
-- 继承:   if_mobite smallint,
-- 继承:   service_group integer,
-- 继承:   serivce_type integer,
-- 继承:   protol_id integer,
-- 继承:   up_bytes bigint,
-- 继承:   down_bytes bigint,
-- 继承:   userip character varying(16),
-- 继承:   user_id character varying(32),
-- 继承:   hash_id integer,
  CONSTRAINT hash_net_id_1 CHECK (hash_id = 1)
)
INHERITS (t_net_audit)
WITH (OIDS=FALSE);
ALTER TABLE t_net_audit_1 OWNER TO gms_user;

CREATE TABLE t_net_audit_2
(
-- 继承:   id integer NOT NULL DEFAULT,
-- 继承:   dev_id character varying(64),
-- 继承:   ftime_start bigint,
-- 继承:   ftime_end bigint,
-- 继承:   if_mobite smallint,
-- 继承:   service_group integer,
-- 继承:   serivce_type integer,
-- 继承:   protol_id integer,
-- 继承:   up_bytes bigint,
-- 继承:   down_bytes bigint,
-- 继承:   userip character varying(16),
-- 继承:   user_id character varying(32),
-- 继承:   hash_id integer,
  CONSTRAINT hash_net_id_2 CHECK (hash_id = 2)
)
INHERITS (t_net_audit)
WITH (OIDS=FALSE);
ALTER TABLE t_net_audit_2 OWNER TO gms_user;

CREATE TABLE t_net_audit_3
(
-- 继承:   id integer NOT NULL DEFAULT,
-- 继承:   dev_id character varying(64),
-- 继承:   ftime_start bigint,
-- 继承:   ftime_end bigint,
-- 继承:   if_mobite smallint,
-- 继承:   service_group integer,
-- 继承:   serivce_type integer,
-- 继承:   protol_id integer,
-- 继承:   up_bytes bigint,
-- 继承:   down_bytes bigint,
-- 继承:   userip character varying(16),
-- 继承:   user_id character varying(32),
-- 继承:   hash_id integer,
  CONSTRAINT hash_net_id_3 CHECK (hash_id = 3)
)
INHERITS (t_net_audit)
WITH (OIDS=FALSE);
ALTER TABLE t_net_audit_3 OWNER TO gms_user;

CREATE TABLE t_net_audit_4
(
-- 继承:   id integer NOT NULL DEFAULT,
-- 继承:   dev_id character varying(64),
-- 继承:   ftime_start bigint,
-- 继承:   ftime_end bigint,
-- 继承:   if_mobite smallint,
-- 继承:   service_group integer,
-- 继承:   serivce_type integer,
-- 继承:   protol_id integer,
-- 继承:   up_bytes bigint,
-- 继承:   down_bytes bigint,
-- 继承:   userip character varying(16),
-- 继承:   user_id character varying(32),
-- 继承:   hash_id integer,
  CONSTRAINT hash_net_id_4 CHECK (hash_id = 4)
)
INHERITS (t_net_audit)
WITH (OIDS=FALSE);
ALTER TABLE t_net_audit_4 OWNER TO gms_user;

CREATE TABLE t_net_audit_5
(
-- 继承:   id integer NOT NULL DEFAULT,
-- 继承:   dev_id character varying(64),
-- 继承:   ftime_start bigint,
-- 继承:   ftime_end bigint,
-- 继承:   if_mobite smallint,
-- 继承:   service_group integer,
-- 继承:   serivce_type integer,
-- 继承:   protol_id integer,
-- 继承:   up_bytes bigint,
-- 继承:   down_bytes bigint,
-- 继承:   userip character varying(16),
-- 继承:   user_id character varying(32),
-- 继承:   hash_id integer,
  CONSTRAINT hash_net_id_5 CHECK (hash_id = 5)
)
INHERITS (t_net_audit)
WITH (OIDS=FALSE);
ALTER TABLE t_net_audit_5 OWNER TO gms_user;

CREATE TABLE t_net_audit_6
(
-- 继承:   id integer NOT NULL DEFAULT,
-- 继承:   dev_id character varying(64),
-- 继承:   ftime_start bigint,
-- 继承:   ftime_end bigint,
-- 继承:   if_mobite smallint,
-- 继承:   service_group integer,
-- 继承:   serivce_type integer,
-- 继承:   protol_id integer,
-- 继承:   up_bytes bigint,
-- 继承:   down_bytes bigint,
-- 继承:   userip character varying(16),
-- 继承:   user_id character varying(32),
-- 继承:   hash_id integer,
  CONSTRAINT hash_net_id_6 CHECK (hash_id = 6)
)
INHERITS (t_net_audit)
WITH (OIDS=FALSE);
ALTER TABLE t_net_audit_6 OWNER TO gms_user;

CREATE TABLE t_net_audit_7
(
-- 继承:   id integer NOT NULL DEFAULT,
-- 继承:   dev_id character varying(64),
-- 继承:   ftime_start bigint,
-- 继承:   ftime_end bigint,
-- 继承:   if_mobite smallint,
-- 继承:   service_group integer,
-- 继承:   serivce_type integer,
-- 继承:   protol_id integer,
-- 继承:   up_bytes bigint,
-- 继承:   down_bytes bigint,
-- 继承:   userip character varying(16),
-- 继承:   user_id character varying(32),
-- 继承:   hash_id integer,
  CONSTRAINT hash_net_id_7 CHECK (hash_id = 7)
)
INHERITS (t_net_audit)
WITH (OIDS=FALSE);
ALTER TABLE t_net_audit_7 OWNER TO gms_user;

CREATE TABLE t_net_audit_8
(
-- 继承:   id integer NOT NULL DEFAULT,
-- 继承:   dev_id character varying(64),
-- 继承:   ftime_start bigint,
-- 继承:   ftime_end bigint,
-- 继承:   if_mobite smallint,
-- 继承:   service_group integer,
-- 继承:   serivce_type integer,
-- 继承:   protol_id integer,
-- 继承:   up_bytes bigint,
-- 继承:   down_bytes bigint,
-- 继承:   userip character varying(16),
-- 继承:   user_id character varying(32),
-- 继承:   hash_id integer,
  CONSTRAINT hash_net_id_8 CHECK (hash_id = 8)
)
INHERITS (t_net_audit)
WITH (OIDS=FALSE);
ALTER TABLE t_net_audit_8 OWNER TO gms_user;

CREATE TABLE t_net_audit_9
(
-- 继承:   id integer NOT NULL DEFAULT,
-- 继承:   dev_id character varying(64),
-- 继承:   ftime_start bigint,
-- 继承:   ftime_end bigint,
-- 继承:   if_mobite smallint,
-- 继承:   service_group integer,
-- 继承:   serivce_type integer,
-- 继承:   protol_id integer,
-- 继承:   up_bytes bigint,
-- 继承:   down_bytes bigint,
-- 继承:   userip character varying(16),
-- 继承:   user_id character varying(32),
-- 继承:   hash_id integer,
  CONSTRAINT hash_net_id_9 CHECK (hash_id = 9)
)
INHERITS (t_net_audit)
WITH (OIDS=FALSE);
ALTER TABLE t_net_audit_9 OWNER TO gms_user;


CREATE TABLE t_net_audit_10
(
-- 继承:   id integer NOT NULL DEFAULT,
-- 继承:   dev_id character varying(64),
-- 继承:   ftime_start bigint,
-- 继承:   ftime_end bigint,
-- 继承:   if_mobite smallint,
-- 继承:   service_group integer,
-- 继承:   serivce_type integer,
-- 继承:   protol_id integer,
-- 继承:   up_bytes bigint,
-- 继承:   down_bytes bigint,
-- 继承:   userip character varying(16),
-- 继承:   user_id character varying(32),
-- 继承:   hash_id integer,
  CONSTRAINT hash_net_id_10 CHECK (hash_id = 10)
)
INHERITS (t_net_audit)
WITH (OIDS=FALSE);
ALTER TABLE t_net_audit_10 OWNER TO gms_user;

CREATE TABLE t_net_audit_11
(
-- 继承:   id integer NOT NULL DEFAULT,
-- 继承:   dev_id character varying(64),
-- 继承:   ftime_start bigint,
-- 继承:   ftime_end bigint,
-- 继承:   if_mobite smallint,
-- 继承:   service_group integer,
-- 继承:   serivce_type integer,
-- 继承:   protol_id integer,
-- 继承:   up_bytes bigint,
-- 继承:   down_bytes bigint,
-- 继承:   userip character varying(16),
-- 继承:   user_id character varying(32),
-- 继承:   hash_id integer,
  CONSTRAINT hash_net_id_11 CHECK (hash_id = 11)
)
INHERITS (t_net_audit)
WITH (OIDS=FALSE);
ALTER TABLE t_net_audit_11 OWNER TO gms_user;

CREATE TABLE t_net_audit_12
(
-- 继承:   id integer NOT NULL DEFAULT,
-- 继承:   dev_id character varying(64),
-- 继承:   ftime_start bigint,
-- 继承:   ftime_end bigint,
-- 继承:   if_mobite smallint,
-- 继承:   service_group integer,
-- 继承:   serivce_type integer,
-- 继承:   protol_id integer,
-- 继承:   up_bytes bigint,
-- 继承:   down_bytes bigint,
-- 继承:   userip character varying(16),
-- 继承:   user_id character varying(32),
-- 继承:   hash_id integer,
  CONSTRAINT hash_net_id_12 CHECK (hash_id = 12)
)
INHERITS (t_net_audit)
WITH (OIDS=FALSE);
ALTER TABLE t_net_audit_12 OWNER TO gms_user;

CREATE TABLE t_net_audit_13
(
-- 继承:   id integer NOT NULL DEFAULT,
-- 继承:   dev_id character varying(64),
-- 继承:   ftime_start bigint,
-- 继承:   ftime_end bigint,
-- 继承:   if_mobite smallint,
-- 继承:   service_group integer,
-- 继承:   serivce_type integer,
-- 继承:   protol_id integer,
-- 继承:   up_bytes bigint,
-- 继承:   down_bytes bigint,
-- 继承:   userip character varying(16),
-- 继承:   user_id character varying(32),
-- 继承:   hash_id integer,
  CONSTRAINT hash_net_id_13 CHECK (hash_id = 13)
)
INHERITS (t_net_audit)
WITH (OIDS=FALSE);
ALTER TABLE t_net_audit_13 OWNER TO gms_user;

CREATE TABLE t_net_audit_14
(
-- 继承:   id integer NOT NULL DEFAULT,
-- 继承:   dev_id character varying(64),
-- 继承:   ftime_start bigint,
-- 继承:   ftime_end bigint,
-- 继承:   if_mobite smallint,
-- 继承:   service_group integer,
-- 继承:   serivce_type integer,
-- 继承:   protol_id integer,
-- 继承:   up_bytes bigint,
-- 继承:   down_bytes bigint,
-- 继承:   userip character varying(16),
-- 继承:   user_id character varying(32),
-- 继承:   hash_id integer,
  CONSTRAINT hash_net_id_14 CHECK (hash_id = 14)
)
INHERITS (t_net_audit)
WITH (OIDS=FALSE);
ALTER TABLE t_net_audit_14 OWNER TO gms_user;

CREATE TABLE t_net_audit_15
(
-- 继承:   id integer NOT NULL DEFAULT,
-- 继承:   dev_id character varying(64),
-- 继承:   ftime_start bigint,
-- 继承:   ftime_end bigint,
-- 继承:   if_mobite smallint,
-- 继承:   service_group integer,
-- 继承:   serivce_type integer,
-- 继承:   protol_id integer,
-- 继承:   up_bytes bigint,
-- 继承:   down_bytes bigint,
-- 继承:   userip character varying(16),
-- 继承:   user_id character varying(32),
-- 继承:   hash_id integer,
  CONSTRAINT hash_net_id_15 CHECK (hash_id = 15)
)
INHERITS (t_net_audit)
WITH (OIDS=FALSE);
ALTER TABLE t_net_audit_15 OWNER TO gms_user;

CREATE TABLE t_net_audit_16
(
-- 继承:   id integer NOT NULL DEFAULT,
-- 继承:   dev_id character varying(64),
-- 继承:   ftime_start bigint,
-- 继承:   ftime_end bigint,
-- 继承:   if_mobite smallint,
-- 继承:   service_group integer,
-- 继承:   serivce_type integer,
-- 继承:   protol_id integer,
-- 继承:   up_bytes bigint,
-- 继承:   down_bytes bigint,
-- 继承:   userip character varying(16),
-- 继承:   user_id character varying(32),
-- 继承:   hash_id integer,
  CONSTRAINT hash_net_id_16 CHECK (hash_id = 16)
)
INHERITS (t_net_audit)
WITH (OIDS=FALSE);
ALTER TABLE t_net_audit_16 OWNER TO gms_user;

CREATE TABLE t_net_audit_17
(
-- 继承:   id integer NOT NULL DEFAULT,
-- 继承:   dev_id character varying(64),
-- 继承:   ftime_start bigint,
-- 继承:   ftime_end bigint,
-- 继承:   if_mobite smallint,
-- 继承:   service_group integer,
-- 继承:   serivce_type integer,
-- 继承:   protol_id integer,
-- 继承:   up_bytes bigint,
-- 继承:   down_bytes bigint,
-- 继承:   userip character varying(16),
-- 继承:   user_id character varying(32),
-- 继承:   hash_id integer,
  CONSTRAINT hash_net_id_17 CHECK (hash_id = 17)
)
INHERITS (t_net_audit)
WITH (OIDS=FALSE);
ALTER TABLE t_net_audit_17 OWNER TO gms_user;

CREATE TABLE t_net_audit_18
(
-- 继承:   id integer NOT NULL DEFAULT,
-- 继承:   dev_id character varying(64),
-- 继承:   ftime_start bigint,
-- 继承:   ftime_end bigint,
-- 继承:   if_mobite smallint,
-- 继承:   service_group integer,
-- 继承:   serivce_type integer,
-- 继承:   protol_id integer,
-- 继承:   up_bytes bigint,
-- 继承:   down_bytes bigint,
-- 继承:   userip character varying(16),
-- 继承:   user_id character varying(32),
-- 继承:   hash_id integer,
  CONSTRAINT hash_net_id_18 CHECK (hash_id = 18)
)
INHERITS (t_net_audit)
WITH (OIDS=FALSE);
ALTER TABLE t_net_audit_18 OWNER TO gms_user;

CREATE TABLE t_net_audit_19
(
-- 继承:   id integer NOT NULL DEFAULT,
-- 继承:   dev_id character varying(64),
-- 继承:   ftime_start bigint,
-- 继承:   ftime_end bigint,
-- 继承:   if_mobite smallint,
-- 继承:   service_group integer,
-- 继承:   serivce_type integer,
-- 继承:   protol_id integer,
-- 继承:   up_bytes bigint,
-- 继承:   down_bytes bigint,
-- 继承:   userip character varying(16),
-- 继承:   user_id character varying(32),
-- 继承:   hash_id integer,
  CONSTRAINT hash_net_id_19 CHECK (hash_id = 19)
)
INHERITS (t_net_audit)
WITH (OIDS=FALSE);
ALTER TABLE t_net_audit_19 OWNER TO gms_user;

CREATE TABLE t_net_audit_20
(
-- 继承:   id integer NOT NULL DEFAULT,
-- 继承:   dev_id character varying(64),
-- 继承:   ftime_start bigint,
-- 继承:   ftime_end bigint,
-- 继承:   if_mobite smallint,
-- 继承:   service_group integer,
-- 继承:   serivce_type integer,
-- 继承:   protol_id integer,
-- 继承:   up_bytes bigint,
-- 继承:   down_bytes bigint,
-- 继承:   userip character varying(16),
-- 继承:   user_id character varying(32),
-- 继承:   hash_id integer,
  CONSTRAINT hash_net_id_20 CHECK (hash_id = 20)
)
INHERITS (t_net_audit)
WITH (OIDS=FALSE);
ALTER TABLE t_net_audit_20 OWNER TO gms_user;

CREATE TABLE t_net_audit_21
(
-- 继承:   id integer NOT NULL DEFAULT,
-- 继承:   dev_id character varying(64),
-- 继承:   ftime_start bigint,
-- 继承:   ftime_end bigint,
-- 继承:   if_mobite smallint,
-- 继承:   service_group integer,
-- 继承:   serivce_type integer,
-- 继承:   protol_id integer,
-- 继承:   up_bytes bigint,
-- 继承:   down_bytes bigint,
-- 继承:   userip character varying(16),
-- 继承:   user_id character varying(32),
-- 继承:   hash_id integer,
  CONSTRAINT hash_net_id_21 CHECK (hash_id = 21)
)
INHERITS (t_net_audit)
WITH (OIDS=FALSE);
ALTER TABLE t_net_audit_21 OWNER TO gms_user;

CREATE TABLE t_net_audit_22
(
-- 继承:   id integer NOT NULL DEFAULT,
-- 继承:   dev_id character varying(64),
-- 继承:   ftime_start bigint,
-- 继承:   ftime_end bigint,
-- 继承:   if_mobite smallint,
-- 继承:   service_group integer,
-- 继承:   serivce_type integer,
-- 继承:   protol_id integer,
-- 继承:   up_bytes bigint,
-- 继承:   down_bytes bigint,
-- 继承:   userip character varying(16),
-- 继承:   user_id character varying(32),
-- 继承:   hash_id integer,
  CONSTRAINT hash_net_id_22 CHECK (hash_id = 22)
)
INHERITS (t_net_audit)
WITH (OIDS=FALSE);
ALTER TABLE t_net_audit_22 OWNER TO gms_user;

CREATE TABLE t_net_audit_23
(
-- 继承:   id integer NOT NULL DEFAULT,
-- 继承:   dev_id character varying(64),
-- 继承:   ftime_start bigint,
-- 继承:   ftime_end bigint,
-- 继承:   if_mobite smallint,
-- 继承:   service_group integer,
-- 继承:   serivce_type integer,
-- 继承:   protol_id integer,
-- 继承:   up_bytes bigint,
-- 继承:   down_bytes bigint,
-- 继承:   userip character varying(16),
-- 继承:   user_id character varying(32),
-- 继承:   hash_id integer,
  CONSTRAINT hash_net_id_23 CHECK (hash_id = 23)
)
INHERITS (t_net_audit)
WITH (OIDS=FALSE);
ALTER TABLE t_net_audit_23 OWNER TO gms_user;

CREATE TABLE t_net_audit_24
(
-- 继承:   id integer NOT NULL DEFAULT,
-- 继承:   dev_id character varying(64),
-- 继承:   ftime_start bigint,
-- 继承:   ftime_end bigint,
-- 继承:   if_mobite smallint,
-- 继承:   service_group integer,
-- 继承:   serivce_type integer,
-- 继承:   protol_id integer,
-- 继承:   up_bytes bigint,
-- 继承:   down_bytes bigint,
-- 继承:   userip character varying(16),
-- 继承:   user_id character varying(32),
-- 继承:   hash_id integer,
  CONSTRAINT hash_net_id_24 CHECK (hash_id = 24)
)
INHERITS (t_net_audit)
WITH (OIDS=FALSE);
ALTER TABLE t_net_audit_24 OWNER TO gms_user;

CREATE TABLE t_net_audit_25
(
-- 继承:   id integer NOT NULL DEFAULT,
-- 继承:   dev_id character varying(64),
-- 继承:   ftime_start bigint,
-- 继承:   ftime_end bigint,
-- 继承:   if_mobite smallint,
-- 继承:   service_group integer,
-- 继承:   serivce_type integer,
-- 继承:   protol_id integer,
-- 继承:   up_bytes bigint,
-- 继承:   down_bytes bigint,
-- 继承:   userip character varying(16),
-- 继承:   user_id character varying(32),
-- 继承:   hash_id integer,
  CONSTRAINT hash_net_id_25 CHECK (hash_id = 25)
)
INHERITS (t_net_audit)
WITH (OIDS=FALSE);
ALTER TABLE t_net_audit_25 OWNER TO gms_user;

CREATE TABLE t_net_audit_26
(
-- 继承:   id integer NOT NULL DEFAULT,
-- 继承:   dev_id character varying(64),
-- 继承:   ftime_start bigint,
-- 继承:   ftime_end bigint,
-- 继承:   if_mobite smallint,
-- 继承:   service_group integer,
-- 继承:   serivce_type integer,
-- 继承:   protol_id integer,
-- 继承:   up_bytes bigint,
-- 继承:   down_bytes bigint,
-- 继承:   userip character varying(16),
-- 继承:   user_id character varying(32),
-- 继承:   hash_id integer,
  CONSTRAINT hash_net_id_26 CHECK (hash_id = 26)
)
INHERITS (t_net_audit)
WITH (OIDS=FALSE);
ALTER TABLE t_net_audit_26 OWNER TO gms_user;

CREATE TABLE t_net_audit_27
(
-- 继承:   id integer NOT NULL DEFAULT,
-- 继承:   dev_id character varying(64),
-- 继承:   ftime_start bigint,
-- 继承:   ftime_end bigint,
-- 继承:   if_mobite smallint,
-- 继承:   service_group integer,
-- 继承:   serivce_type integer,
-- 继承:   protol_id integer,
-- 继承:   up_bytes bigint,
-- 继承:   down_bytes bigint,
-- 继承:   userip character varying(16),
-- 继承:   user_id character varying(32),
-- 继承:   hash_id integer,
  CONSTRAINT hash_net_id_27 CHECK (hash_id = 27)
)
INHERITS (t_net_audit)
WITH (OIDS=FALSE);
ALTER TABLE t_net_audit_27 OWNER TO gms_user;

CREATE TABLE t_net_audit_28
(
-- 继承:   id integer NOT NULL DEFAULT,
-- 继承:   dev_id character varying(64),
-- 继承:   ftime_start bigint,
-- 继承:   ftime_end bigint,
-- 继承:   if_mobite smallint,
-- 继承:   service_group integer,
-- 继承:   serivce_type integer,
-- 继承:   protol_id integer,
-- 继承:   up_bytes bigint,
-- 继承:   down_bytes bigint,
-- 继承:   userip character varying(16),
-- 继承:   user_id character varying(32),
-- 继承:   hash_id integer,
  CONSTRAINT hash_net_id_28 CHECK (hash_id = 28)
)
INHERITS (t_net_audit)
WITH (OIDS=FALSE);
ALTER TABLE t_net_audit_28 OWNER TO gms_user;

CREATE TABLE t_net_audit_29
(
-- 继承:   id integer NOT NULL DEFAULT,
-- 继承:   dev_id character varying(64),
-- 继承:   ftime_start bigint,
-- 继承:   ftime_end bigint,
-- 继承:   if_mobite smallint,
-- 继承:   service_group integer,
-- 继承:   serivce_type integer,
-- 继承:   protol_id integer,
-- 继承:   up_bytes bigint,
-- 继承:   down_bytes bigint,
-- 继承:   userip character varying(16),
-- 继承:   user_id character varying(32),
-- 继承:   hash_id integer,
  CONSTRAINT hash_net_id_29 CHECK (hash_id = 29)
)
INHERITS (t_net_audit)
WITH (OIDS=FALSE);
ALTER TABLE t_net_audit_29 OWNER TO gms_user;

CREATE TABLE t_net_audit_30
(
-- 继承:   id integer NOT NULL DEFAULT,
-- 继承:   dev_id character varying(64),
-- 继承:   ftime_start bigint,
-- 继承:   ftime_end bigint,
-- 继承:   if_mobite smallint,
-- 继承:   service_group integer,
-- 继承:   serivce_type integer,
-- 继承:   protol_id integer,
-- 继承:   up_bytes bigint,
-- 继承:   down_bytes bigint,
-- 继承:   userip character varying(16),
-- 继承:   user_id character varying(32),
-- 继承:   hash_id integer,
  CONSTRAINT hash_net_id_30 CHECK (hash_id = 30)
)
INHERITS (t_net_audit)
WITH (OIDS=FALSE);
ALTER TABLE t_net_audit_30 OWNER TO gms_user;

CREATE TABLE t_net_audit_31
(
-- 继承:   id integer NOT NULL DEFAULT,
-- 继承:   dev_id character varying(64),
-- 继承:   ftime_start bigint,
-- 继承:   ftime_end bigint,
-- 继承:   if_mobite smallint,
-- 继承:   service_group integer,
-- 继承:   serivce_type integer,
-- 继承:   protol_id integer,
-- 继承:   up_bytes bigint,
-- 继承:   down_bytes bigint,
-- 继承:   userip character varying(16),
-- 继承:   user_id character varying(32),
-- 继承:   hash_id integer,
  CONSTRAINT hash_net_id_31 CHECK (hash_id = 31)
)
INHERITS (t_net_audit)
WITH (OIDS=FALSE);
ALTER TABLE t_net_audit_31 OWNER TO gms_user;

CREATE TABLE t_net_audit_32
(
-- 继承:   id integer NOT NULL DEFAULT,
-- 继承:   dev_id character varying(64),
-- 继承:   ftime_start bigint,
-- 继承:   ftime_end bigint,
-- 继承:   if_mobite smallint,
-- 继承:   service_group integer,
-- 继承:   serivce_type integer,
-- 继承:   protol_id integer,
-- 继承:   up_bytes bigint,
-- 继承:   down_bytes bigint,
-- 继承:   userip character varying(16),
-- 继承:   user_id character varying(32),
-- 继承:   hash_id integer,
  CONSTRAINT hash_net_id_32 CHECK (hash_id = 32)
)
INHERITS (t_net_audit)
WITH (OIDS=FALSE);
ALTER TABLE t_net_audit_32 OWNER TO gms_user;

CREATE TABLE t_net_audit_33
(
-- 继承:   id integer NOT NULL DEFAULT,
-- 继承:   dev_id character varying(64),
-- 继承:   ftime_start bigint,
-- 继承:   ftime_end bigint,
-- 继承:   if_mobite smallint,
-- 继承:   service_group integer,
-- 继承:   serivce_type integer,
-- 继承:   protol_id integer,
-- 继承:   up_bytes bigint,
-- 继承:   down_bytes bigint,
-- 继承:   userip character varying(16),
-- 继承:   user_id character varying(32),
-- 继承:   hash_id integer,
  CONSTRAINT hash_net_id_33 CHECK (hash_id = 33)
)
INHERITS (t_net_audit)
WITH (OIDS=FALSE);
ALTER TABLE t_net_audit_33 OWNER TO gms_user;

CREATE TABLE t_net_audit_34
(
-- 继承:   id integer NOT NULL DEFAULT,
-- 继承:   dev_id character varying(64),
-- 继承:   ftime_start bigint,
-- 继承:   ftime_end bigint,
-- 继承:   if_mobite smallint,
-- 继承:   service_group integer,
-- 继承:   serivce_type integer,
-- 继承:   protol_id integer,
-- 继承:   up_bytes bigint,
-- 继承:   down_bytes bigint,
-- 继承:   userip character varying(16),
-- 继承:   user_id character varying(32),
-- 继承:   hash_id integer,
  CONSTRAINT hash_net_id_34 CHECK (hash_id = 34)
)
INHERITS (t_net_audit)
WITH (OIDS=FALSE);
ALTER TABLE t_net_audit_34 OWNER TO gms_user;

CREATE TABLE t_net_audit_35
(
-- 继承:   id integer NOT NULL DEFAULT,
-- 继承:   dev_id character varying(64),
-- 继承:   ftime_start bigint,
-- 继承:   ftime_end bigint,
-- 继承:   if_mobite smallint,
-- 继承:   service_group integer,
-- 继承:   serivce_type integer,
-- 继承:   protol_id integer,
-- 继承:   up_bytes bigint,
-- 继承:   down_bytes bigint,
-- 继承:   userip character varying(16),
-- 继承:   user_id character varying(32),
-- 继承:   hash_id integer,
  CONSTRAINT hash_net_id_35 CHECK (hash_id = 35)
)
INHERITS (t_net_audit)
WITH (OIDS=FALSE);
ALTER TABLE t_net_audit_35 OWNER TO gms_user;

CREATE TABLE t_net_audit_36
(
-- 继承:   id integer NOT NULL DEFAULT,
-- 继承:   dev_id character varying(64),
-- 继承:   ftime_start bigint,
-- 继承:   ftime_end bigint,
-- 继承:   if_mobite smallint,
-- 继承:   service_group integer,
-- 继承:   serivce_type integer,
-- 继承:   protol_id integer,
-- 继承:   up_bytes bigint,
-- 继承:   down_bytes bigint,
-- 继承:   userip character varying(16),
-- 继承:   user_id character varying(32),
-- 继承:   hash_id integer,
  CONSTRAINT hash_net_id_36 CHECK (hash_id = 36)
)
INHERITS (t_net_audit)
WITH (OIDS=FALSE);
ALTER TABLE t_net_audit_36 OWNER TO gms_user;

CREATE TABLE t_net_audit_37
(
-- 继承:   id integer NOT NULL DEFAULT,
-- 继承:   dev_id character varying(64),
-- 继承:   ftime_start bigint,
-- 继承:   ftime_end bigint,
-- 继承:   if_mobite smallint,
-- 继承:   service_group integer,
-- 继承:   serivce_type integer,
-- 继承:   protol_id integer,
-- 继承:   up_bytes bigint,
-- 继承:   down_bytes bigint,
-- 继承:   userip character varying(16),
-- 继承:   user_id character varying(32),
-- 继承:   hash_id integer,
  CONSTRAINT hash_net_id_37 CHECK (hash_id = 37)
)
INHERITS (t_net_audit)
WITH (OIDS=FALSE);
ALTER TABLE t_net_audit_37 OWNER TO gms_user;

CREATE TABLE t_net_audit_38
(
-- 继承:   id integer NOT NULL DEFAULT,
-- 继承:   dev_id character varying(64),
-- 继承:   ftime_start bigint,
-- 继承:   ftime_end bigint,
-- 继承:   if_mobite smallint,
-- 继承:   service_group integer,
-- 继承:   serivce_type integer,
-- 继承:   protol_id integer,
-- 继承:   up_bytes bigint,
-- 继承:   down_bytes bigint,
-- 继承:   userip character varying(16),
-- 继承:   user_id character varying(32),
-- 继承:   hash_id integer,
  CONSTRAINT hash_net_id_38 CHECK (hash_id = 38)
)
INHERITS (t_net_audit)
WITH (OIDS=FALSE);
ALTER TABLE t_net_audit_38 OWNER TO gms_user;

CREATE TABLE t_net_audit_39
(
-- 继承:   id integer NOT NULL DEFAULT,
-- 继承:   dev_id character varying(64),
-- 继承:   ftime_start bigint,
-- 继承:   ftime_end bigint,
-- 继承:   if_mobite smallint,
-- 继承:   service_group integer,
-- 继承:   serivce_type integer,
-- 继承:   protol_id integer,
-- 继承:   up_bytes bigint,
-- 继承:   down_bytes bigint,
-- 继承:   userip character varying(16),
-- 继承:   user_id character varying(32),
-- 继承:   hash_id integer,
  CONSTRAINT hash_net_id_39 CHECK (hash_id = 39)
)
INHERITS (t_net_audit)
WITH (OIDS=FALSE);
ALTER TABLE t_net_audit_39 OWNER TO gms_user;

CREATE TABLE t_net_audit_40
(
-- 继承:   id integer NOT NULL DEFAULT,
-- 继承:   dev_id character varying(64),
-- 继承:   ftime_start bigint,
-- 继承:   ftime_end bigint,
-- 继承:   if_mobite smallint,
-- 继承:   service_group integer,
-- 继承:   serivce_type integer,
-- 继承:   protol_id integer,
-- 继承:   up_bytes bigint,
-- 继承:   down_bytes bigint,
-- 继承:   userip character varying(16),
-- 继承:   user_id character varying(32),
-- 继承:   hash_id integer,
  CONSTRAINT hash_net_id_40 CHECK (hash_id = 40)
)
INHERITS (t_net_audit)
WITH (OIDS=FALSE);
ALTER TABLE t_net_audit_40 OWNER TO gms_user;

CREATE TABLE t_net_audit_41
(
-- 继承:   id integer NOT NULL DEFAULT,
-- 继承:   dev_id character varying(64),
-- 继承:   ftime_start bigint,
-- 继承:   ftime_end bigint,
-- 继承:   if_mobite smallint,
-- 继承:   service_group integer,
-- 继承:   serivce_type integer,
-- 继承:   protol_id integer,
-- 继承:   up_bytes bigint,
-- 继承:   down_bytes bigint,
-- 继承:   userip character varying(16),
-- 继承:   user_id character varying(32),
-- 继承:   hash_id integer,
  CONSTRAINT hash_net_id_41 CHECK (hash_id = 41)
)
INHERITS (t_net_audit)
WITH (OIDS=FALSE);
ALTER TABLE t_net_audit_41 OWNER TO gms_user;

CREATE TABLE t_net_audit_42
(
-- 继承:   id integer NOT NULL DEFAULT,
-- 继承:   dev_id character varying(64),
-- 继承:   ftime_start bigint,
-- 继承:   ftime_end bigint,
-- 继承:   if_mobite smallint,
-- 继承:   service_group integer,
-- 继承:   serivce_type integer,
-- 继承:   protol_id integer,
-- 继承:   up_bytes bigint,
-- 继承:   down_bytes bigint,
-- 继承:   userip character varying(16),
-- 继承:   user_id character varying(32),
-- 继承:   hash_id integer,
  CONSTRAINT hash_net_id_42 CHECK (hash_id = 42)
)
INHERITS (t_net_audit)
WITH (OIDS=FALSE);
ALTER TABLE t_net_audit_42 OWNER TO gms_user;

CREATE TABLE t_net_audit_43
(
-- 继承:   id integer NOT NULL DEFAULT,
-- 继承:   dev_id character varying(64),
-- 继承:   ftime_start bigint,
-- 继承:   ftime_end bigint,
-- 继承:   if_mobite smallint,
-- 继承:   service_group integer,
-- 继承:   serivce_type integer,
-- 继承:   protol_id integer,
-- 继承:   up_bytes bigint,
-- 继承:   down_bytes bigint,
-- 继承:   userip character varying(16),
-- 继承:   user_id character varying(32),
-- 继承:   hash_id integer,
  CONSTRAINT hash_net_id_43 CHECK (hash_id = 43)
)
INHERITS (t_net_audit)
WITH (OIDS=FALSE);
ALTER TABLE t_net_audit_43 OWNER TO gms_user;

CREATE TABLE t_net_audit_44
(
-- 继承:   id integer NOT NULL DEFAULT,
-- 继承:   dev_id character varying(64),
-- 继承:   ftime_start bigint,
-- 继承:   ftime_end bigint,
-- 继承:   if_mobite smallint,
-- 继承:   service_group integer,
-- 继承:   serivce_type integer,
-- 继承:   protol_id integer,
-- 继承:   up_bytes bigint,
-- 继承:   down_bytes bigint,
-- 继承:   userip character varying(16),
-- 继承:   user_id character varying(32),
-- 继承:   hash_id integer,
  CONSTRAINT hash_net_id_44 CHECK (hash_id = 44)
)
INHERITS (t_net_audit)
WITH (OIDS=FALSE);
ALTER TABLE t_net_audit_44 OWNER TO gms_user;

CREATE TABLE t_net_audit_45
(
-- 继承:   id integer NOT NULL DEFAULT,
-- 继承:   dev_id character varying(64),
-- 继承:   ftime_start bigint,
-- 继承:   ftime_end bigint,
-- 继承:   if_mobite smallint,
-- 继承:   service_group integer,
-- 继承:   serivce_type integer,
-- 继承:   protol_id integer,
-- 继承:   up_bytes bigint,
-- 继承:   down_bytes bigint,
-- 继承:   userip character varying(16),
-- 继承:   user_id character varying(32),
-- 继承:   hash_id integer,
  CONSTRAINT hash_net_id_45 CHECK (hash_id = 45)
)
INHERITS (t_net_audit)
WITH (OIDS=FALSE);
ALTER TABLE t_net_audit_45 OWNER TO gms_user;

CREATE TABLE t_net_audit_46
(
-- 继承:   id integer NOT NULL DEFAULT,
-- 继承:   dev_id character varying(64),
-- 继承:   ftime_start bigint,
-- 继承:   ftime_end bigint,
-- 继承:   if_mobite smallint,
-- 继承:   service_group integer,
-- 继承:   serivce_type integer,
-- 继承:   protol_id integer,
-- 继承:   up_bytes bigint,
-- 继承:   down_bytes bigint,
-- 继承:   userip character varying(16),
-- 继承:   user_id character varying(32),
-- 继承:   hash_id integer,
  CONSTRAINT hash_net_id_46 CHECK (hash_id = 46)
)
INHERITS (t_net_audit)
WITH (OIDS=FALSE);
ALTER TABLE t_net_audit_46 OWNER TO gms_user;

CREATE TABLE t_net_audit_47
(
-- 继承:   id integer NOT NULL DEFAULT,
-- 继承:   dev_id character varying(64),
-- 继承:   ftime_start bigint,
-- 继承:   ftime_end bigint,
-- 继承:   if_mobite smallint,
-- 继承:   service_group integer,
-- 继承:   serivce_type integer,
-- 继承:   protol_id integer,
-- 继承:   up_bytes bigint,
-- 继承:   down_bytes bigint,
-- 继承:   userip character varying(16),
-- 继承:   user_id character varying(32),
-- 继承:   hash_id integer,
  CONSTRAINT hash_net_id_47 CHECK (hash_id = 47)
)
INHERITS (t_net_audit)
WITH (OIDS=FALSE);
ALTER TABLE t_net_audit_47 OWNER TO gms_user;

CREATE TABLE t_net_audit_48
(
-- 继承:   id integer NOT NULL DEFAULT,
-- 继承:   dev_id character varying(64),
-- 继承:   ftime_start bigint,
-- 继承:   ftime_end bigint,
-- 继承:   if_mobite smallint,
-- 继承:   service_group integer,
-- 继承:   serivce_type integer,
-- 继承:   protol_id integer,
-- 继承:   up_bytes bigint,
-- 继承:   down_bytes bigint,
-- 继承:   userip character varying(16),
-- 继承:   user_id character varying(32),
-- 继承:   hash_id integer,
  CONSTRAINT hash_net_id_48 CHECK (hash_id = 48)
)
INHERITS (t_net_audit)
WITH (OIDS=FALSE);
ALTER TABLE t_net_audit_48 OWNER TO gms_user;

CREATE TABLE t_net_audit_49
(
-- 继承:   id integer NOT NULL DEFAULT,
-- 继承:   dev_id character varying(64),
-- 继承:   ftime_start bigint,
-- 继承:   ftime_end bigint,
-- 继承:   if_mobite smallint,
-- 继承:   service_group integer,
-- 继承:   serivce_type integer,
-- 继承:   protol_id integer,
-- 继承:   up_bytes bigint,
-- 继承:   down_bytes bigint,
-- 继承:   userip character varying(16),
-- 继承:   user_id character varying(32),
-- 继承:   hash_id integer,
  CONSTRAINT hash_net_id_49 CHECK (hash_id = 49)
)
INHERITS (t_net_audit)
WITH (OIDS=FALSE);
ALTER TABLE t_net_audit_49 OWNER TO gms_user;


CREATE TABLE t_net_audit_50
(
-- 继承:   id integer NOT NULL DEFAULT,
-- 继承:   dev_id character varying(64),
-- 继承:   ftime_start bigint,
-- 继承:   ftime_end bigint,
-- 继承:   if_mobite smallint,
-- 继承:   service_group integer,
-- 继承:   serivce_type integer,
-- 继承:   protol_id integer,
-- 继承:   up_bytes bigint,
-- 继承:   down_bytes bigint,
-- 继承:   userip character varying(16),
-- 继承:   user_id character varying(32),
-- 继承:   hash_id integer,
  CONSTRAINT hash_net_id_50 CHECK (hash_id = 50)
)
INHERITS (t_net_audit)
WITH (OIDS=FALSE);
ALTER TABLE t_net_audit_50 OWNER TO gms_user;

CREATE TABLE t_net_audit_51
(
-- 继承:   id integer NOT NULL DEFAULT,
-- 继承:   dev_id character varying(64),
-- 继承:   ftime_start bigint,
-- 继承:   ftime_end bigint,
-- 继承:   if_mobite smallint,
-- 继承:   service_group integer,
-- 继承:   serivce_type integer,
-- 继承:   protol_id integer,
-- 继承:   up_bytes bigint,
-- 继承:   down_bytes bigint,
-- 继承:   userip character varying(16),
-- 继承:   user_id character varying(32),
-- 继承:   hash_id integer,
  CONSTRAINT hash_net_id_51 CHECK (hash_id = 51)
)
INHERITS (t_net_audit)
WITH (OIDS=FALSE);
ALTER TABLE t_net_audit_51 OWNER TO gms_user;

CREATE TABLE t_net_audit_52
(
-- 继承:   id integer NOT NULL DEFAULT,
-- 继承:   dev_id character varying(64),
-- 继承:   ftime_start bigint,
-- 继承:   ftime_end bigint,
-- 继承:   if_mobite smallint,
-- 继承:   service_group integer,
-- 继承:   serivce_type integer,
-- 继承:   protol_id integer,
-- 继承:   up_bytes bigint,
-- 继承:   down_bytes bigint,
-- 继承:   userip character varying(16),
-- 继承:   user_id character varying(32),
-- 继承:   hash_id integer,
  CONSTRAINT hash_net_id_52 CHECK (hash_id = 52)
)
INHERITS (t_net_audit)
WITH (OIDS=FALSE);
ALTER TABLE t_net_audit_52 OWNER TO gms_user;

CREATE TABLE t_net_audit_53
(
-- 继承:   id integer NOT NULL DEFAULT,
-- 继承:   dev_id character varying(64),
-- 继承:   ftime_start bigint,
-- 继承:   ftime_end bigint,
-- 继承:   if_mobite smallint,
-- 继承:   service_group integer,
-- 继承:   serivce_type integer,
-- 继承:   protol_id integer,
-- 继承:   up_bytes bigint,
-- 继承:   down_bytes bigint,
-- 继承:   userip character varying(16),
-- 继承:   user_id character varying(32),
-- 继承:   hash_id integer,
  CONSTRAINT hash_net_id_53 CHECK (hash_id = 53)
)
INHERITS (t_net_audit)
WITH (OIDS=FALSE);
ALTER TABLE t_net_audit_53 OWNER TO gms_user;

CREATE TABLE t_net_audit_54
(
-- 继承:   id integer NOT NULL DEFAULT,
-- 继承:   dev_id character varying(64),
-- 继承:   ftime_start bigint,
-- 继承:   ftime_end bigint,
-- 继承:   if_mobite smallint,
-- 继承:   service_group integer,
-- 继承:   serivce_type integer,
-- 继承:   protol_id integer,
-- 继承:   up_bytes bigint,
-- 继承:   down_bytes bigint,
-- 继承:   userip character varying(16),
-- 继承:   user_id character varying(32),
-- 继承:   hash_id integer,
  CONSTRAINT hash_net_id_54 CHECK (hash_id = 54)
)
INHERITS (t_net_audit)
WITH (OIDS=FALSE);
ALTER TABLE t_net_audit_54 OWNER TO gms_user;

CREATE TABLE t_net_audit_55
(
-- 继承:   id integer NOT NULL DEFAULT,
-- 继承:   dev_id character varying(64),
-- 继承:   ftime_start bigint,
-- 继承:   ftime_end bigint,
-- 继承:   if_mobite smallint,
-- 继承:   service_group integer,
-- 继承:   serivce_type integer,
-- 继承:   protol_id integer,
-- 继承:   up_bytes bigint,
-- 继承:   down_bytes bigint,
-- 继承:   userip character varying(16),
-- 继承:   user_id character varying(32),
-- 继承:   hash_id integer,
  CONSTRAINT hash_net_id_55 CHECK (hash_id = 55)
)
INHERITS (t_net_audit)
WITH (OIDS=FALSE);
ALTER TABLE t_net_audit_55 OWNER TO gms_user;

CREATE TABLE t_net_audit_56
(
-- 继承:   id integer NOT NULL DEFAULT,
-- 继承:   dev_id character varying(64),
-- 继承:   ftime_start bigint,
-- 继承:   ftime_end bigint,
-- 继承:   if_mobite smallint,
-- 继承:   service_group integer,
-- 继承:   serivce_type integer,
-- 继承:   protol_id integer,
-- 继承:   up_bytes bigint,
-- 继承:   down_bytes bigint,
-- 继承:   userip character varying(16),
-- 继承:   user_id character varying(32),
-- 继承:   hash_id integer,
  CONSTRAINT hash_net_id_56 CHECK (hash_id = 56)
)
INHERITS (t_net_audit)
WITH (OIDS=FALSE);
ALTER TABLE t_net_audit_56 OWNER TO gms_user;

CREATE TABLE t_net_audit_57
(
-- 继承:   id integer NOT NULL DEFAULT,
-- 继承:   dev_id character varying(64),
-- 继承:   ftime_start bigint,
-- 继承:   ftime_end bigint,
-- 继承:   if_mobite smallint,
-- 继承:   service_group integer,
-- 继承:   serivce_type integer,
-- 继承:   protol_id integer,
-- 继承:   up_bytes bigint,
-- 继承:   down_bytes bigint,
-- 继承:   userip character varying(16),
-- 继承:   user_id character varying(32),
-- 继承:   hash_id integer,
  CONSTRAINT hash_net_id_57 CHECK (hash_id = 57)
)
INHERITS (t_net_audit)
WITH (OIDS=FALSE);
ALTER TABLE t_net_audit_57 OWNER TO gms_user;

CREATE TABLE t_net_audit_58
(
-- 继承:   id integer NOT NULL DEFAULT,
-- 继承:   dev_id character varying(64),
-- 继承:   ftime_start bigint,
-- 继承:   ftime_end bigint,
-- 继承:   if_mobite smallint,
-- 继承:   service_group integer,
-- 继承:   serivce_type integer,
-- 继承:   protol_id integer,
-- 继承:   up_bytes bigint,
-- 继承:   down_bytes bigint,
-- 继承:   userip character varying(16),
-- 继承:   user_id character varying(32),
-- 继承:   hash_id integer,
  CONSTRAINT hash_net_id_58 CHECK (hash_id = 58)
)
INHERITS (t_net_audit)
WITH (OIDS=FALSE);
ALTER TABLE t_net_audit_58 OWNER TO gms_user;

CREATE TABLE t_net_audit_59
(
-- 继承:   id integer NOT NULL DEFAULT,
-- 继承:   dev_id character varying(64),
-- 继承:   ftime_start bigint,
-- 继承:   ftime_end bigint,
-- 继承:   if_mobite smallint,
-- 继承:   service_group integer,
-- 继承:   serivce_type integer,
-- 继承:   protol_id integer,
-- 继承:   up_bytes bigint,
-- 继承:   down_bytes bigint,
-- 继承:   userip character varying(16),
-- 继承:   user_id character varying(32),
-- 继承:   hash_id integer,
  CONSTRAINT hash_net_id_59 CHECK (hash_id = 59)
)
INHERITS (t_net_audit)
WITH (OIDS=FALSE);
ALTER TABLE t_net_audit_59 OWNER TO gms_user;

CREATE TABLE t_net_audit_60
(
-- 继承:   id integer NOT NULL DEFAULT,
-- 继承:   dev_id character varying(64),
-- 继承:   ftime_start bigint,
-- 继承:   ftime_end bigint,
-- 继承:   if_mobite smallint,
-- 继承:   service_group integer,
-- 继承:   serivce_type integer,
-- 继承:   protol_id integer,
-- 继承:   up_bytes bigint,
-- 继承:   down_bytes bigint,
-- 继承:   userip character varying(16),
-- 继承:   user_id character varying(32),
-- 继承:   hash_id integer,
  CONSTRAINT hash_net_id_60 CHECK (hash_id = 60)
)
INHERITS (t_net_audit)
WITH (OIDS=FALSE);
ALTER TABLE t_net_audit_60 OWNER TO gms_user;

CREATE TABLE t_net_audit_61
(
-- 继承:   id integer NOT NULL DEFAULT,
-- 继承:   dev_id character varying(64),
-- 继承:   ftime_start bigint,
-- 继承:   ftime_end bigint,
-- 继承:   if_mobite smallint,
-- 继承:   service_group integer,
-- 继承:   serivce_type integer,
-- 继承:   protol_id integer,
-- 继承:   up_bytes bigint,
-- 继承:   down_bytes bigint,
-- 继承:   userip character varying(16),
-- 继承:   user_id character varying(32),
-- 继承:   hash_id integer,
  CONSTRAINT hash_net_id_61 CHECK (hash_id = 61)
)
INHERITS (t_net_audit)
WITH (OIDS=FALSE);
ALTER TABLE t_net_audit_61 OWNER TO gms_user;

CREATE TABLE t_net_audit_62
(
-- 继承:   id integer NOT NULL DEFAULT,
-- 继承:   dev_id character varying(64),
-- 继承:   ftime_start bigint,
-- 继承:   ftime_end bigint,
-- 继承:   if_mobite smallint,
-- 继承:   service_group integer,
-- 继承:   serivce_type integer,
-- 继承:   protol_id integer,
-- 继承:   up_bytes bigint,
-- 继承:   down_bytes bigint,
-- 继承:   userip character varying(16),
-- 继承:   user_id character varying(32),
-- 继承:   hash_id integer,
  CONSTRAINT hash_net_id_62 CHECK (hash_id = 62)
)
INHERITS (t_net_audit)
WITH (OIDS=FALSE);
ALTER TABLE t_net_audit_62 OWNER TO gms_user;

CREATE TABLE t_net_audit_63
(
-- 继承:   id integer NOT NULL DEFAULT,
-- 继承:   dev_id character varying(64),
-- 继承:   ftime_start bigint,
-- 继承:   ftime_end bigint,
-- 继承:   if_mobite smallint,
-- 继承:   service_group integer,
-- 继承:   serivce_type integer,
-- 继承:   protol_id integer,
-- 继承:   up_bytes bigint,
-- 继承:   down_bytes bigint,
-- 继承:   userip character varying(16),
-- 继承:   user_id character varying(32),
-- 继承:   hash_id integer,
  CONSTRAINT hash_net_id_63 CHECK (hash_id = 63)
)
INHERITS (t_net_audit)
WITH (OIDS=FALSE);
ALTER TABLE t_net_audit_63 OWNER TO gms_user;

CREATE TABLE t_net_audit_64
(
-- 继承:   id integer NOT NULL DEFAULT,
-- 继承:   dev_id character varying(64),
-- 继承:   ftime_start bigint,
-- 继承:   ftime_end bigint,
-- 继承:   if_mobite smallint,
-- 继承:   service_group integer,
-- 继承:   serivce_type integer,
-- 继承:   protol_id integer,
-- 继承:   up_bytes bigint,
-- 继承:   down_bytes bigint,
-- 继承:   userip character varying(16),
-- 继承:   user_id character varying(32),
-- 继承:   hash_id integer,
  CONSTRAINT hash_net_id_64 CHECK (hash_id = 64)
)
INHERITS (t_net_audit)
WITH (OIDS=FALSE);
ALTER TABLE t_net_audit_64 OWNER TO gms_user;

CREATE TABLE t_net_audit_65
(
-- 继承:   id integer NOT NULL DEFAULT,
-- 继承:   dev_id character varying(64),
-- 继承:   ftime_start bigint,
-- 继承:   ftime_end bigint,
-- 继承:   if_mobite smallint,
-- 继承:   service_group integer,
-- 继承:   serivce_type integer,
-- 继承:   protol_id integer,
-- 继承:   up_bytes bigint,
-- 继承:   down_bytes bigint,
-- 继承:   userip character varying(16),
-- 继承:   user_id character varying(32),
-- 继承:   hash_id integer,
  CONSTRAINT hash_net_id_65 CHECK (hash_id = 65)
)
INHERITS (t_net_audit)
WITH (OIDS=FALSE);
ALTER TABLE t_net_audit_65 OWNER TO gms_user;

CREATE TABLE t_net_audit_66
(
-- 继承:   id integer NOT NULL DEFAULT,
-- 继承:   dev_id character varying(64),
-- 继承:   ftime_start bigint,
-- 继承:   ftime_end bigint,
-- 继承:   if_mobite smallint,
-- 继承:   service_group integer,
-- 继承:   serivce_type integer,
-- 继承:   protol_id integer,
-- 继承:   up_bytes bigint,
-- 继承:   down_bytes bigint,
-- 继承:   userip character varying(16),
-- 继承:   user_id character varying(32),
-- 继承:   hash_id integer,
  CONSTRAINT hash_net_id_66 CHECK (hash_id = 66)
)
INHERITS (t_net_audit)
WITH (OIDS=FALSE);
ALTER TABLE t_net_audit_66 OWNER TO gms_user;

CREATE TABLE t_net_audit_67
(
-- 继承:   id integer NOT NULL DEFAULT,
-- 继承:   dev_id character varying(64),
-- 继承:   ftime_start bigint,
-- 继承:   ftime_end bigint,
-- 继承:   if_mobite smallint,
-- 继承:   service_group integer,
-- 继承:   serivce_type integer,
-- 继承:   protol_id integer,
-- 继承:   up_bytes bigint,
-- 继承:   down_bytes bigint,
-- 继承:   userip character varying(16),
-- 继承:   user_id character varying(32),
-- 继承:   hash_id integer,
  CONSTRAINT hash_net_id_67 CHECK (hash_id = 67)
)
INHERITS (t_net_audit)
WITH (OIDS=FALSE);
ALTER TABLE t_net_audit_67 OWNER TO gms_user;

CREATE TABLE t_net_audit_68
(
-- 继承:   id integer NOT NULL DEFAULT,
-- 继承:   dev_id character varying(64),
-- 继承:   ftime_start bigint,
-- 继承:   ftime_end bigint,
-- 继承:   if_mobite smallint,
-- 继承:   service_group integer,
-- 继承:   serivce_type integer,
-- 继承:   protol_id integer,
-- 继承:   up_bytes bigint,
-- 继承:   down_bytes bigint,
-- 继承:   userip character varying(16),
-- 继承:   user_id character varying(32),
-- 继承:   hash_id integer,
  CONSTRAINT hash_net_id_68 CHECK (hash_id = 68)
)
INHERITS (t_net_audit)
WITH (OIDS=FALSE);
ALTER TABLE t_net_audit_68 OWNER TO gms_user;

CREATE TABLE t_net_audit_69
(
-- 继承:   id integer NOT NULL DEFAULT,
-- 继承:   dev_id character varying(64),
-- 继承:   ftime_start bigint,
-- 继承:   ftime_end bigint,
-- 继承:   if_mobite smallint,
-- 继承:   service_group integer,
-- 继承:   serivce_type integer,
-- 继承:   protol_id integer,
-- 继承:   up_bytes bigint,
-- 继承:   down_bytes bigint,
-- 继承:   userip character varying(16),
-- 继承:   user_id character varying(32),
-- 继承:   hash_id integer,
  CONSTRAINT hash_net_id_69 CHECK (hash_id = 69)
)
INHERITS (t_net_audit)
WITH (OIDS=FALSE);
ALTER TABLE t_net_audit_69 OWNER TO gms_user;

CREATE TABLE t_net_audit_70
(
-- 继承:   id integer NOT NULL DEFAULT,
-- 继承:   dev_id character varying(64),
-- 继承:   ftime_start bigint,
-- 继承:   ftime_end bigint,
-- 继承:   if_mobite smallint,
-- 继承:   service_group integer,
-- 继承:   serivce_type integer,
-- 继承:   protol_id integer,
-- 继承:   up_bytes bigint,
-- 继承:   down_bytes bigint,
-- 继承:   userip character varying(16),
-- 继承:   user_id character varying(32),
-- 继承:   hash_id integer,
  CONSTRAINT hash_net_id_70 CHECK (hash_id = 70)
)
INHERITS (t_net_audit)
WITH (OIDS=FALSE);
ALTER TABLE t_net_audit_70 OWNER TO gms_user;

CREATE TABLE t_net_audit_71
(
-- 继承:   id integer NOT NULL DEFAULT,
-- 继承:   dev_id character varying(64),
-- 继承:   ftime_start bigint,
-- 继承:   ftime_end bigint,
-- 继承:   if_mobite smallint,
-- 继承:   service_group integer,
-- 继承:   serivce_type integer,
-- 继承:   protol_id integer,
-- 继承:   up_bytes bigint,
-- 继承:   down_bytes bigint,
-- 继承:   userip character varying(16),
-- 继承:   user_id character varying(32),
-- 继承:   hash_id integer,
  CONSTRAINT hash_net_id_71 CHECK (hash_id = 71)
)
INHERITS (t_net_audit)
WITH (OIDS=FALSE);
ALTER TABLE t_net_audit_71 OWNER TO gms_user;

CREATE TABLE t_net_audit_72
(
-- 继承:   id integer NOT NULL DEFAULT,
-- 继承:   dev_id character varying(64),
-- 继承:   ftime_start bigint,
-- 继承:   ftime_end bigint,
-- 继承:   if_mobite smallint,
-- 继承:   service_group integer,
-- 继承:   serivce_type integer,
-- 继承:   protol_id integer,
-- 继承:   up_bytes bigint,
-- 继承:   down_bytes bigint,
-- 继承:   userip character varying(16),
-- 继承:   user_id character varying(32),
-- 继承:   hash_id integer,
  CONSTRAINT hash_net_id_72 CHECK (hash_id = 72)
)
INHERITS (t_net_audit)
WITH (OIDS=FALSE);
ALTER TABLE t_net_audit_72 OWNER TO gms_user;

CREATE TABLE t_net_audit_73
(
-- 继承:   id integer NOT NULL DEFAULT,
-- 继承:   dev_id character varying(64),
-- 继承:   ftime_start bigint,
-- 继承:   ftime_end bigint,
-- 继承:   if_mobite smallint,
-- 继承:   service_group integer,
-- 继承:   serivce_type integer,
-- 继承:   protol_id integer,
-- 继承:   up_bytes bigint,
-- 继承:   down_bytes bigint,
-- 继承:   userip character varying(16),
-- 继承:   user_id character varying(32),
-- 继承:   hash_id integer,
  CONSTRAINT hash_net_id_73 CHECK (hash_id = 73)
)
INHERITS (t_net_audit)
WITH (OIDS=FALSE);
ALTER TABLE t_net_audit_73 OWNER TO gms_user;

CREATE TABLE t_net_audit_74
(
-- 继承:   id integer NOT NULL DEFAULT,
-- 继承:   dev_id character varying(64),
-- 继承:   ftime_start bigint,
-- 继承:   ftime_end bigint,
-- 继承:   if_mobite smallint,
-- 继承:   service_group integer,
-- 继承:   serivce_type integer,
-- 继承:   protol_id integer,
-- 继承:   up_bytes bigint,
-- 继承:   down_bytes bigint,
-- 继承:   userip character varying(16),
-- 继承:   user_id character varying(32),
-- 继承:   hash_id integer,
  CONSTRAINT hash_net_id_74 CHECK (hash_id = 74)
)
INHERITS (t_net_audit)
WITH (OIDS=FALSE);
ALTER TABLE t_net_audit_74 OWNER TO gms_user;

CREATE TABLE t_net_audit_75
(
-- 继承:   id integer NOT NULL DEFAULT,
-- 继承:   dev_id character varying(64),
-- 继承:   ftime_start bigint,
-- 继承:   ftime_end bigint,
-- 继承:   if_mobite smallint,
-- 继承:   service_group integer,
-- 继承:   serivce_type integer,
-- 继承:   protol_id integer,
-- 继承:   up_bytes bigint,
-- 继承:   down_bytes bigint,
-- 继承:   userip character varying(16),
-- 继承:   user_id character varying(32),
-- 继承:   hash_id integer,
  CONSTRAINT hash_net_id_75 CHECK (hash_id = 75)
)
INHERITS (t_net_audit)
WITH (OIDS=FALSE);
ALTER TABLE t_net_audit_75 OWNER TO gms_user;

CREATE TABLE t_net_audit_76
(
-- 继承:   id integer NOT NULL DEFAULT,
-- 继承:   dev_id character varying(64),
-- 继承:   ftime_start bigint,
-- 继承:   ftime_end bigint,
-- 继承:   if_mobite smallint,
-- 继承:   service_group integer,
-- 继承:   serivce_type integer,
-- 继承:   protol_id integer,
-- 继承:   up_bytes bigint,
-- 继承:   down_bytes bigint,
-- 继承:   userip character varying(16),
-- 继承:   user_id character varying(32),
-- 继承:   hash_id integer,
  CONSTRAINT hash_net_id_76 CHECK (hash_id = 76)
)
INHERITS (t_net_audit)
WITH (OIDS=FALSE);
ALTER TABLE t_net_audit_76 OWNER TO gms_user;

CREATE TABLE t_net_audit_77
(
-- 继承:   id integer NOT NULL DEFAULT,
-- 继承:   dev_id character varying(64),
-- 继承:   ftime_start bigint,
-- 继承:   ftime_end bigint,
-- 继承:   if_mobite smallint,
-- 继承:   service_group integer,
-- 继承:   serivce_type integer,
-- 继承:   protol_id integer,
-- 继承:   up_bytes bigint,
-- 继承:   down_bytes bigint,
-- 继承:   userip character varying(16),
-- 继承:   user_id character varying(32),
-- 继承:   hash_id integer,
  CONSTRAINT hash_net_id_77 CHECK (hash_id = 77)
)
INHERITS (t_net_audit)
WITH (OIDS=FALSE);
ALTER TABLE t_net_audit_77 OWNER TO gms_user;

CREATE TABLE t_net_audit_78
(
-- 继承:   id integer NOT NULL DEFAULT,
-- 继承:   dev_id character varying(64),
-- 继承:   ftime_start bigint,
-- 继承:   ftime_end bigint,
-- 继承:   if_mobite smallint,
-- 继承:   service_group integer,
-- 继承:   serivce_type integer,
-- 继承:   protol_id integer,
-- 继承:   up_bytes bigint,
-- 继承:   down_bytes bigint,
-- 继承:   userip character varying(16),
-- 继承:   user_id character varying(32),
-- 继承:   hash_id integer,
  CONSTRAINT hash_net_id_78 CHECK (hash_id = 78)
)
INHERITS (t_net_audit)
WITH (OIDS=FALSE);
ALTER TABLE t_net_audit_78 OWNER TO gms_user;

CREATE TABLE t_net_audit_79
(
-- 继承:   id integer NOT NULL DEFAULT,
-- 继承:   dev_id character varying(64),
-- 继承:   ftime_start bigint,
-- 继承:   ftime_end bigint,
-- 继承:   if_mobite smallint,
-- 继承:   service_group integer,
-- 继承:   serivce_type integer,
-- 继承:   protol_id integer,
-- 继承:   up_bytes bigint,
-- 继承:   down_bytes bigint,
-- 继承:   userip character varying(16),
-- 继承:   user_id character varying(32),
-- 继承:   hash_id integer,
  CONSTRAINT hash_net_id_79 CHECK (hash_id = 79)
)
INHERITS (t_net_audit)
WITH (OIDS=FALSE);
ALTER TABLE t_net_audit_79 OWNER TO gms_user;


CREATE TABLE t_net_audit_80
(
-- 继承:   id integer NOT NULL DEFAULT,
-- 继承:   dev_id character varying(64),
-- 继承:   ftime_start bigint,
-- 继承:   ftime_end bigint,
-- 继承:   if_mobite smallint,
-- 继承:   service_group integer,
-- 继承:   serivce_type integer,
-- 继承:   protol_id integer,
-- 继承:   up_bytes bigint,
-- 继承:   down_bytes bigint,
-- 继承:   userip character varying(16),
-- 继承:   user_id character varying(32),
-- 继承:   hash_id integer,
  CONSTRAINT hash_net_id_80 CHECK (hash_id = 80)
)
INHERITS (t_net_audit)
WITH (OIDS=FALSE);
ALTER TABLE t_net_audit_80 OWNER TO gms_user;

CREATE TABLE t_net_audit_81
(
-- 继承:   id integer NOT NULL DEFAULT,
-- 继承:   dev_id character varying(64),
-- 继承:   ftime_start bigint,
-- 继承:   ftime_end bigint,
-- 继承:   if_mobite smallint,
-- 继承:   service_group integer,
-- 继承:   serivce_type integer,
-- 继承:   protol_id integer,
-- 继承:   up_bytes bigint,
-- 继承:   down_bytes bigint,
-- 继承:   userip character varying(16),
-- 继承:   user_id character varying(32),
-- 继承:   hash_id integer,
  CONSTRAINT hash_net_id_81 CHECK (hash_id = 81)
)
INHERITS (t_net_audit)
WITH (OIDS=FALSE);
ALTER TABLE t_net_audit_81 OWNER TO gms_user;

CREATE TABLE t_net_audit_82
(
-- 继承:   id integer NOT NULL DEFAULT,
-- 继承:   dev_id character varying(64),
-- 继承:   ftime_start bigint,
-- 继承:   ftime_end bigint,
-- 继承:   if_mobite smallint,
-- 继承:   service_group integer,
-- 继承:   serivce_type integer,
-- 继承:   protol_id integer,
-- 继承:   up_bytes bigint,
-- 继承:   down_bytes bigint,
-- 继承:   userip character varying(16),
-- 继承:   user_id character varying(32),
-- 继承:   hash_id integer,
  CONSTRAINT hash_net_id_82 CHECK (hash_id = 82)
)
INHERITS (t_net_audit)
WITH (OIDS=FALSE);
ALTER TABLE t_net_audit_82 OWNER TO gms_user;

CREATE TABLE t_net_audit_83
(
-- 继承:   id integer NOT NULL DEFAULT,
-- 继承:   dev_id character varying(64),
-- 继承:   ftime_start bigint,
-- 继承:   ftime_end bigint,
-- 继承:   if_mobite smallint,
-- 继承:   service_group integer,
-- 继承:   serivce_type integer,
-- 继承:   protol_id integer,
-- 继承:   up_bytes bigint,
-- 继承:   down_bytes bigint,
-- 继承:   userip character varying(16),
-- 继承:   user_id character varying(32),
-- 继承:   hash_id integer,
  CONSTRAINT hash_net_id_83 CHECK (hash_id = 83)
)
INHERITS (t_net_audit)
WITH (OIDS=FALSE);
ALTER TABLE t_net_audit_83 OWNER TO gms_user;

CREATE TABLE t_net_audit_84
(
-- 继承:   id integer NOT NULL DEFAULT,
-- 继承:   dev_id character varying(64),
-- 继承:   ftime_start bigint,
-- 继承:   ftime_end bigint,
-- 继承:   if_mobite smallint,
-- 继承:   service_group integer,
-- 继承:   serivce_type integer,
-- 继承:   protol_id integer,
-- 继承:   up_bytes bigint,
-- 继承:   down_bytes bigint,
-- 继承:   userip character varying(16),
-- 继承:   user_id character varying(32),
-- 继承:   hash_id integer,
  CONSTRAINT hash_net_id_84 CHECK (hash_id = 84)
)
INHERITS (t_net_audit)
WITH (OIDS=FALSE);
ALTER TABLE t_net_audit_84 OWNER TO gms_user;

CREATE TABLE t_net_audit_85
(
-- 继承:   id integer NOT NULL DEFAULT,
-- 继承:   dev_id character varying(64),
-- 继承:   ftime_start bigint,
-- 继承:   ftime_end bigint,
-- 继承:   if_mobite smallint,
-- 继承:   service_group integer,
-- 继承:   serivce_type integer,
-- 继承:   protol_id integer,
-- 继承:   up_bytes bigint,
-- 继承:   down_bytes bigint,
-- 继承:   userip character varying(16),
-- 继承:   user_id character varying(32),
-- 继承:   hash_id integer,
  CONSTRAINT hash_net_id_85 CHECK (hash_id = 85)
)
INHERITS (t_net_audit)
WITH (OIDS=FALSE);
ALTER TABLE t_net_audit_85 OWNER TO gms_user;

CREATE TABLE t_net_audit_86
(
-- 继承:   id integer NOT NULL DEFAULT,
-- 继承:   dev_id character varying(64),
-- 继承:   ftime_start bigint,
-- 继承:   ftime_end bigint,
-- 继承:   if_mobite smallint,
-- 继承:   service_group integer,
-- 继承:   serivce_type integer,
-- 继承:   protol_id integer,
-- 继承:   up_bytes bigint,
-- 继承:   down_bytes bigint,
-- 继承:   userip character varying(16),
-- 继承:   user_id character varying(32),
-- 继承:   hash_id integer,
  CONSTRAINT hash_net_id_86 CHECK (hash_id = 86)
)
INHERITS (t_net_audit)
WITH (OIDS=FALSE);
ALTER TABLE t_net_audit_86 OWNER TO gms_user;

CREATE TABLE t_net_audit_87
(
-- 继承:   id integer NOT NULL DEFAULT,
-- 继承:   dev_id character varying(64),
-- 继承:   ftime_start bigint,
-- 继承:   ftime_end bigint,
-- 继承:   if_mobite smallint,
-- 继承:   service_group integer,
-- 继承:   serivce_type integer,
-- 继承:   protol_id integer,
-- 继承:   up_bytes bigint,
-- 继承:   down_bytes bigint,
-- 继承:   userip character varying(16),
-- 继承:   user_id character varying(32),
-- 继承:   hash_id integer,
  CONSTRAINT hash_net_id_87 CHECK (hash_id = 87)
)
INHERITS (t_net_audit)
WITH (OIDS=FALSE);
ALTER TABLE t_net_audit_87 OWNER TO gms_user;

CREATE TABLE t_net_audit_88
(
-- 继承:   id integer NOT NULL DEFAULT,
-- 继承:   dev_id character varying(64),
-- 继承:   ftime_start bigint,
-- 继承:   ftime_end bigint,
-- 继承:   if_mobite smallint,
-- 继承:   service_group integer,
-- 继承:   serivce_type integer,
-- 继承:   protol_id integer,
-- 继承:   up_bytes bigint,
-- 继承:   down_bytes bigint,
-- 继承:   userip character varying(16),
-- 继承:   user_id character varying(32),
-- 继承:   hash_id integer,
  CONSTRAINT hash_net_id_88 CHECK (hash_id = 88)
)
INHERITS (t_net_audit)
WITH (OIDS=FALSE);
ALTER TABLE t_net_audit_88 OWNER TO gms_user;

CREATE TABLE t_net_audit_89
(
-- 继承:   id integer NOT NULL DEFAULT,
-- 继承:   dev_id character varying(64),
-- 继承:   ftime_start bigint,
-- 继承:   ftime_end bigint,
-- 继承:   if_mobite smallint,
-- 继承:   service_group integer,
-- 继承:   serivce_type integer,
-- 继承:   protol_id integer,
-- 继承:   up_bytes bigint,
-- 继承:   down_bytes bigint,
-- 继承:   userip character varying(16),
-- 继承:   user_id character varying(32),
-- 继承:   hash_id integer,
  CONSTRAINT hash_net_id_89 CHECK (hash_id = 89)
)
INHERITS (t_net_audit)
WITH (OIDS=FALSE);
ALTER TABLE t_net_audit_89 OWNER TO gms_user;


CREATE TABLE t_net_audit_90
(
-- 继承:   id integer NOT NULL DEFAULT,
-- 继承:   dev_id character varying(64),
-- 继承:   ftime_start bigint,
-- 继承:   ftime_end bigint,
-- 继承:   if_mobite smallint,
-- 继承:   service_group integer,
-- 继承:   serivce_type integer,
-- 继承:   protol_id integer,
-- 继承:   up_bytes bigint,
-- 继承:   down_bytes bigint,
-- 继承:   userip character varying(16),
-- 继承:   user_id character varying(32),
-- 继承:   hash_id integer,
  CONSTRAINT hash_net_id_90 CHECK (hash_id = 90)
)
INHERITS (t_net_audit)
WITH (OIDS=FALSE);
ALTER TABLE t_net_audit_90 OWNER TO gms_user;

CREATE TABLE t_net_audit_91
(
-- 继承:   id integer NOT NULL DEFAULT,
-- 继承:   dev_id character varying(64),
-- 继承:   ftime_start bigint,
-- 继承:   ftime_end bigint,
-- 继承:   if_mobite smallint,
-- 继承:   service_group integer,
-- 继承:   serivce_type integer,
-- 继承:   protol_id integer,
-- 继承:   up_bytes bigint,
-- 继承:   down_bytes bigint,
-- 继承:   userip character varying(16),
-- 继承:   user_id character varying(32),
-- 继承:   hash_id integer,
  CONSTRAINT hash_net_id_91 CHECK (hash_id = 91)
)
INHERITS (t_net_audit)
WITH (OIDS=FALSE);
ALTER TABLE t_net_audit_91 OWNER TO gms_user;

CREATE TABLE t_net_audit_92
(
-- 继承:   id integer NOT NULL DEFAULT,
-- 继承:   dev_id character varying(64),
-- 继承:   ftime_start bigint,
-- 继承:   ftime_end bigint,
-- 继承:   if_mobite smallint,
-- 继承:   service_group integer,
-- 继承:   serivce_type integer,
-- 继承:   protol_id integer,
-- 继承:   up_bytes bigint,
-- 继承:   down_bytes bigint,
-- 继承:   userip character varying(16),
-- 继承:   user_id character varying(32),
-- 继承:   hash_id integer,
  CONSTRAINT hash_net_id_92 CHECK (hash_id = 92)
)
INHERITS (t_net_audit)
WITH (OIDS=FALSE);
ALTER TABLE t_net_audit_92 OWNER TO gms_user;

CREATE TABLE t_net_audit_93
(
-- 继承:   id integer NOT NULL DEFAULT,
-- 继承:   dev_id character varying(64),
-- 继承:   ftime_start bigint,
-- 继承:   ftime_end bigint,
-- 继承:   if_mobite smallint,
-- 继承:   service_group integer,
-- 继承:   serivce_type integer,
-- 继承:   protol_id integer,
-- 继承:   up_bytes bigint,
-- 继承:   down_bytes bigint,
-- 继承:   userip character varying(16),
-- 继承:   user_id character varying(32),
-- 继承:   hash_id integer,
  CONSTRAINT hash_net_id_93 CHECK (hash_id = 93)
)
INHERITS (t_net_audit)
WITH (OIDS=FALSE);
ALTER TABLE t_net_audit_93 OWNER TO gms_user;

CREATE TABLE t_net_audit_94
(
-- 继承:   id integer NOT NULL DEFAULT,
-- 继承:   dev_id character varying(64),
-- 继承:   ftime_start bigint,
-- 继承:   ftime_end bigint,
-- 继承:   if_mobite smallint,
-- 继承:   service_group integer,
-- 继承:   serivce_type integer,
-- 继承:   protol_id integer,
-- 继承:   up_bytes bigint,
-- 继承:   down_bytes bigint,
-- 继承:   userip character varying(16),
-- 继承:   user_id character varying(32),
-- 继承:   hash_id integer,
  CONSTRAINT hash_net_id_94 CHECK (hash_id = 94)
)
INHERITS (t_net_audit)
WITH (OIDS=FALSE);
ALTER TABLE t_net_audit_94 OWNER TO gms_user;

CREATE TABLE t_net_audit_95
(
-- 继承:   id integer NOT NULL DEFAULT,
-- 继承:   dev_id character varying(64),
-- 继承:   ftime_start bigint,
-- 继承:   ftime_end bigint,
-- 继承:   if_mobite smallint,
-- 继承:   service_group integer,
-- 继承:   serivce_type integer,
-- 继承:   protol_id integer,
-- 继承:   up_bytes bigint,
-- 继承:   down_bytes bigint,
-- 继承:   userip character varying(16),
-- 继承:   user_id character varying(32),
-- 继承:   hash_id integer,
  CONSTRAINT hash_net_id_95 CHECK (hash_id = 95)
)
INHERITS (t_net_audit)
WITH (OIDS=FALSE);
ALTER TABLE t_net_audit_95 OWNER TO gms_user;

CREATE TABLE t_net_audit_96
(
-- 继承:   id integer NOT NULL DEFAULT,
-- 继承:   dev_id character varying(64),
-- 继承:   ftime_start bigint,
-- 继承:   ftime_end bigint,
-- 继承:   if_mobite smallint,
-- 继承:   service_group integer,
-- 继承:   serivce_type integer,
-- 继承:   protol_id integer,
-- 继承:   up_bytes bigint,
-- 继承:   down_bytes bigint,
-- 继承:   userip character varying(16),
-- 继承:   user_id character varying(32),
-- 继承:   hash_id integer,
  CONSTRAINT hash_net_id_96 CHECK (hash_id = 96)
)
INHERITS (t_net_audit)
WITH (OIDS=FALSE);
ALTER TABLE t_net_audit_96 OWNER TO gms_user;

CREATE TABLE t_net_audit_97
(
-- 继承:   id integer NOT NULL DEFAULT,
-- 继承:   dev_id character varying(64),
-- 继承:   ftime_start bigint,
-- 继承:   ftime_end bigint,
-- 继承:   if_mobite smallint,
-- 继承:   service_group integer,
-- 继承:   serivce_type integer,
-- 继承:   protol_id integer,
-- 继承:   up_bytes bigint,
-- 继承:   down_bytes bigint,
-- 继承:   userip character varying(16),
-- 继承:   user_id character varying(32),
-- 继承:   hash_id integer,
  CONSTRAINT hash_net_id_97 CHECK (hash_id = 97)
)
INHERITS (t_net_audit)
WITH (OIDS=FALSE);
ALTER TABLE t_net_audit_97 OWNER TO gms_user;

CREATE TABLE t_net_audit_98
(
-- 继承:   id integer NOT NULL DEFAULT,
-- 继承:   dev_id character varying(64),
-- 继承:   ftime_start bigint,
-- 继承:   ftime_end bigint,
-- 继承:   if_mobite smallint,
-- 继承:   service_group integer,
-- 继承:   serivce_type integer,
-- 继承:   protol_id integer,
-- 继承:   up_bytes bigint,
-- 继承:   down_bytes bigint,
-- 继承:   userip character varying(16),
-- 继承:   user_id character varying(32),
-- 继承:   hash_id integer,
  CONSTRAINT hash_net_id_98 CHECK (hash_id = 98)
)
INHERITS (t_net_audit)
WITH (OIDS=FALSE);
ALTER TABLE t_net_audit_98 OWNER TO gms_user;

CREATE TABLE t_net_audit_99
(
-- 继承:   id integer NOT NULL DEFAULT,
-- 继承:   dev_id character varying(64),
-- 继承:   ftime_start bigint,
-- 继承:   ftime_end bigint,
-- 继承:   if_mobite smallint,
-- 继承:   service_group integer,
-- 继承:   serivce_type integer,
-- 继承:   protol_id integer,
-- 继承:   up_bytes bigint,
-- 继承:   down_bytes bigint,
-- 继承:   userip character varying(16),
-- 继承:   user_id character varying(32),
-- 继承:   hash_id integer,
  CONSTRAINT hash_net_id_99 CHECK (hash_id = 99)
)
INHERITS (t_net_audit)
WITH (OIDS=FALSE);
ALTER TABLE t_net_audit_99 OWNER TO gms_user;


CREATE TABLE t_net_audit_100
(
-- 继承:   id integer NOT NULL DEFAULT,
-- 继承:   dev_id character varying(64),
-- 继承:   ftime_start bigint,
-- 继承:   ftime_end bigint,
-- 继承:   if_mobite smallint,
-- 继承:   service_group integer,
-- 继承:   serivce_type integer,
-- 继承:   protol_id integer,
-- 继承:   up_bytes bigint,
-- 继承:   down_bytes bigint,
-- 继承:   userip character varying(16),
-- 继承:   user_id character varying(32),
-- 继承:   hash_id integer,
  CONSTRAINT hash_net_id_100 CHECK (hash_id = 100)
)
INHERITS (t_net_audit)
WITH (OIDS=FALSE);
ALTER TABLE t_net_audit_100 OWNER TO gms_user;

CREATE TABLE t_net_audit_101
(
-- 继承:   id integer NOT NULL DEFAULT,
-- 继承:   dev_id character varying(64),
-- 继承:   ftime_start bigint,
-- 继承:   ftime_end bigint,
-- 继承:   if_mobite smallint,
-- 继承:   service_group integer,
-- 继承:   serivce_type integer,
-- 继承:   protol_id integer,
-- 继承:   up_bytes bigint,
-- 继承:   down_bytes bigint,
-- 继承:   userip character varying(16),
-- 继承:   user_id character varying(32),
-- 继承:   hash_id integer,
  CONSTRAINT hash_net_id_101 CHECK (hash_id = 101)
)
INHERITS (t_net_audit)
WITH (OIDS=FALSE);
ALTER TABLE t_net_audit_101 OWNER TO gms_user;

CREATE TABLE t_net_audit_102
(
-- 继承:   id integer NOT NULL DEFAULT,
-- 继承:   dev_id character varying(64),
-- 继承:   ftime_start bigint,
-- 继承:   ftime_end bigint,
-- 继承:   if_mobite smallint,
-- 继承:   service_group integer,
-- 继承:   serivce_type integer,
-- 继承:   protol_id integer,
-- 继承:   up_bytes bigint,
-- 继承:   down_bytes bigint,
-- 继承:   userip character varying(16),
-- 继承:   user_id character varying(32),
-- 继承:   hash_id integer,
  CONSTRAINT hash_net_id_102 CHECK (hash_id = 102)
)
INHERITS (t_net_audit)
WITH (OIDS=FALSE);
ALTER TABLE t_net_audit_102 OWNER TO gms_user;

CREATE TABLE t_net_audit_103
(
-- 继承:   id integer NOT NULL DEFAULT,
-- 继承:   dev_id character varying(64),
-- 继承:   ftime_start bigint,
-- 继承:   ftime_end bigint,
-- 继承:   if_mobite smallint,
-- 继承:   service_group integer,
-- 继承:   serivce_type integer,
-- 继承:   protol_id integer,
-- 继承:   up_bytes bigint,
-- 继承:   down_bytes bigint,
-- 继承:   userip character varying(16),
-- 继承:   user_id character varying(32),
-- 继承:   hash_id integer,
  CONSTRAINT hash_net_id_103 CHECK (hash_id = 103)
)
INHERITS (t_net_audit)
WITH (OIDS=FALSE);
ALTER TABLE t_net_audit_103 OWNER TO gms_user;

CREATE TABLE t_net_audit_104
(
-- 继承:   id integer NOT NULL DEFAULT,
-- 继承:   dev_id character varying(64),
-- 继承:   ftime_start bigint,
-- 继承:   ftime_end bigint,
-- 继承:   if_mobite smallint,
-- 继承:   service_group integer,
-- 继承:   serivce_type integer,
-- 继承:   protol_id integer,
-- 继承:   up_bytes bigint,
-- 继承:   down_bytes bigint,
-- 继承:   userip character varying(16),
-- 继承:   user_id character varying(32),
-- 继承:   hash_id integer,
  CONSTRAINT hash_net_id_104 CHECK (hash_id = 104)
)
INHERITS (t_net_audit)
WITH (OIDS=FALSE);
ALTER TABLE t_net_audit_104 OWNER TO gms_user;

CREATE TABLE t_net_audit_105
(
-- 继承:   id integer NOT NULL DEFAULT,
-- 继承:   dev_id character varying(64),
-- 继承:   ftime_start bigint,
-- 继承:   ftime_end bigint,
-- 继承:   if_mobite smallint,
-- 继承:   service_group integer,
-- 继承:   serivce_type integer,
-- 继承:   protol_id integer,
-- 继承:   up_bytes bigint,
-- 继承:   down_bytes bigint,
-- 继承:   userip character varying(16),
-- 继承:   user_id character varying(32),
-- 继承:   hash_id integer,
  CONSTRAINT hash_net_id_105 CHECK (hash_id = 105)
)
INHERITS (t_net_audit)
WITH (OIDS=FALSE);
ALTER TABLE t_net_audit_105 OWNER TO gms_user;

CREATE TABLE t_net_audit_106
(
-- 继承:   id integer NOT NULL DEFAULT,
-- 继承:   dev_id character varying(64),
-- 继承:   ftime_start bigint,
-- 继承:   ftime_end bigint,
-- 继承:   if_mobite smallint,
-- 继承:   service_group integer,
-- 继承:   serivce_type integer,
-- 继承:   protol_id integer,
-- 继承:   up_bytes bigint,
-- 继承:   down_bytes bigint,
-- 继承:   userip character varying(16),
-- 继承:   user_id character varying(32),
-- 继承:   hash_id integer,
  CONSTRAINT hash_net_id_106 CHECK (hash_id = 106)
)
INHERITS (t_net_audit)
WITH (OIDS=FALSE);
ALTER TABLE t_net_audit_106 OWNER TO gms_user;

CREATE TABLE t_net_audit_107
(
-- 继承:   id integer NOT NULL DEFAULT,
-- 继承:   dev_id character varying(64),
-- 继承:   ftime_start bigint,
-- 继承:   ftime_end bigint,
-- 继承:   if_mobite smallint,
-- 继承:   service_group integer,
-- 继承:   serivce_type integer,
-- 继承:   protol_id integer,
-- 继承:   up_bytes bigint,
-- 继承:   down_bytes bigint,
-- 继承:   userip character varying(16),
-- 继承:   user_id character varying(32),
-- 继承:   hash_id integer,
  CONSTRAINT hash_net_id_107 CHECK (hash_id = 107)
)
INHERITS (t_net_audit)
WITH (OIDS=FALSE);
ALTER TABLE t_net_audit_107 OWNER TO gms_user;

CREATE TABLE t_net_audit_108
(
-- 继承:   id integer NOT NULL DEFAULT,
-- 继承:   dev_id character varying(64),
-- 继承:   ftime_start bigint,
-- 继承:   ftime_end bigint,
-- 继承:   if_mobite smallint,
-- 继承:   service_group integer,
-- 继承:   serivce_type integer,
-- 继承:   protol_id integer,
-- 继承:   up_bytes bigint,
-- 继承:   down_bytes bigint,
-- 继承:   userip character varying(16),
-- 继承:   user_id character varying(32),
-- 继承:   hash_id integer,
  CONSTRAINT hash_net_id_108 CHECK (hash_id = 108)
)
INHERITS (t_net_audit)
WITH (OIDS=FALSE);
ALTER TABLE t_net_audit_108 OWNER TO gms_user;

CREATE TABLE t_net_audit_109
(
-- 继承:   id integer NOT NULL DEFAULT,
-- 继承:   dev_id character varying(64),
-- 继承:   ftime_start bigint,
-- 继承:   ftime_end bigint,
-- 继承:   if_mobite smallint,
-- 继承:   service_group integer,
-- 继承:   serivce_type integer,
-- 继承:   protol_id integer,
-- 继承:   up_bytes bigint,
-- 继承:   down_bytes bigint,
-- 继承:   userip character varying(16),
-- 继承:   user_id character varying(32),
-- 继承:   hash_id integer,
  CONSTRAINT hash_net_id_109 CHECK (hash_id = 109)
)
INHERITS (t_net_audit)
WITH (OIDS=FALSE);
ALTER TABLE t_net_audit_109 OWNER TO gms_user;

CREATE TABLE t_net_audit_110
(
-- 继承:   id integer NOT NULL DEFAULT,
-- 继承:   dev_id character varying(64),
-- 继承:   ftime_start bigint,
-- 继承:   ftime_end bigint,
-- 继承:   if_mobite smallint,
-- 继承:   service_group integer,
-- 继承:   serivce_type integer,
-- 继承:   protol_id integer,
-- 继承:   up_bytes bigint,
-- 继承:   down_bytes bigint,
-- 继承:   userip character varying(16),
-- 继承:   user_id character varying(32),
-- 继承:   hash_id integer,
  CONSTRAINT hash_net_id_110 CHECK (hash_id = 110)
)
INHERITS (t_net_audit)
WITH (OIDS=FALSE);
ALTER TABLE t_net_audit_110 OWNER TO gms_user;

CREATE TABLE t_net_audit_111
(
-- 继承:   id integer NOT NULL DEFAULT,
-- 继承:   dev_id character varying(64),
-- 继承:   ftime_start bigint,
-- 继承:   ftime_end bigint,
-- 继承:   if_mobite smallint,
-- 继承:   service_group integer,
-- 继承:   serivce_type integer,
-- 继承:   protol_id integer,
-- 继承:   up_bytes bigint,
-- 继承:   down_bytes bigint,
-- 继承:   userip character varying(16),
-- 继承:   user_id character varying(32),
-- 继承:   hash_id integer,
  CONSTRAINT hash_net_id_111 CHECK (hash_id = 111)
)
INHERITS (t_net_audit)
WITH (OIDS=FALSE);
ALTER TABLE t_net_audit_111 OWNER TO gms_user;

CREATE TABLE t_net_audit_112
(
-- 继承:   id integer NOT NULL DEFAULT,
-- 继承:   dev_id character varying(64),
-- 继承:   ftime_start bigint,
-- 继承:   ftime_end bigint,
-- 继承:   if_mobite smallint,
-- 继承:   service_group integer,
-- 继承:   serivce_type integer,
-- 继承:   protol_id integer,
-- 继承:   up_bytes bigint,
-- 继承:   down_bytes bigint,
-- 继承:   userip character varying(16),
-- 继承:   user_id character varying(32),
-- 继承:   hash_id integer,
  CONSTRAINT hash_net_id_112 CHECK (hash_id = 112)
)
INHERITS (t_net_audit)
WITH (OIDS=FALSE);
ALTER TABLE t_net_audit_112 OWNER TO gms_user;

CREATE TABLE t_net_audit_113
(
-- 继承:   id integer NOT NULL DEFAULT,
-- 继承:   dev_id character varying(64),
-- 继承:   ftime_start bigint,
-- 继承:   ftime_end bigint,
-- 继承:   if_mobite smallint,
-- 继承:   service_group integer,
-- 继承:   serivce_type integer,
-- 继承:   protol_id integer,
-- 继承:   up_bytes bigint,
-- 继承:   down_bytes bigint,
-- 继承:   userip character varying(16),
-- 继承:   user_id character varying(32),
-- 继承:   hash_id integer,
  CONSTRAINT hash_net_id_113 CHECK (hash_id = 113)
)
INHERITS (t_net_audit)
WITH (OIDS=FALSE);
ALTER TABLE t_net_audit_113 OWNER TO gms_user;

CREATE TABLE t_net_audit_114
(
-- 继承:   id integer NOT NULL DEFAULT,
-- 继承:   dev_id character varying(64),
-- 继承:   ftime_start bigint,
-- 继承:   ftime_end bigint,
-- 继承:   if_mobite smallint,
-- 继承:   service_group integer,
-- 继承:   serivce_type integer,
-- 继承:   protol_id integer,
-- 继承:   up_bytes bigint,
-- 继承:   down_bytes bigint,
-- 继承:   userip character varying(16),
-- 继承:   user_id character varying(32),
-- 继承:   hash_id integer,
  CONSTRAINT hash_net_id_114 CHECK (hash_id = 114)
)
INHERITS (t_net_audit)
WITH (OIDS=FALSE);
ALTER TABLE t_net_audit_114 OWNER TO gms_user;

CREATE TABLE t_net_audit_115
(
-- 继承:   id integer NOT NULL DEFAULT,
-- 继承:   dev_id character varying(64),
-- 继承:   ftime_start bigint,
-- 继承:   ftime_end bigint,
-- 继承:   if_mobite smallint,
-- 继承:   service_group integer,
-- 继承:   serivce_type integer,
-- 继承:   protol_id integer,
-- 继承:   up_bytes bigint,
-- 继承:   down_bytes bigint,
-- 继承:   userip character varying(16),
-- 继承:   user_id character varying(32),
-- 继承:   hash_id integer,
  CONSTRAINT hash_net_id_115 CHECK (hash_id = 115)
)
INHERITS (t_net_audit)
WITH (OIDS=FALSE);
ALTER TABLE t_net_audit_115 OWNER TO gms_user;

CREATE TABLE t_net_audit_116
(
-- 继承:   id integer NOT NULL DEFAULT,
-- 继承:   dev_id character varying(64),
-- 继承:   ftime_start bigint,
-- 继承:   ftime_end bigint,
-- 继承:   if_mobite smallint,
-- 继承:   service_group integer,
-- 继承:   serivce_type integer,
-- 继承:   protol_id integer,
-- 继承:   up_bytes bigint,
-- 继承:   down_bytes bigint,
-- 继承:   userip character varying(16),
-- 继承:   user_id character varying(32),
-- 继承:   hash_id integer,
  CONSTRAINT hash_net_id_116 CHECK (hash_id = 116)
)
INHERITS (t_net_audit)
WITH (OIDS=FALSE);
ALTER TABLE t_net_audit_116 OWNER TO gms_user;

CREATE TABLE t_net_audit_117
(
-- 继承:   id integer NOT NULL DEFAULT,
-- 继承:   dev_id character varying(64),
-- 继承:   ftime_start bigint,
-- 继承:   ftime_end bigint,
-- 继承:   if_mobite smallint,
-- 继承:   service_group integer,
-- 继承:   serivce_type integer,
-- 继承:   protol_id integer,
-- 继承:   up_bytes bigint,
-- 继承:   down_bytes bigint,
-- 继承:   userip character varying(16),
-- 继承:   user_id character varying(32),
-- 继承:   hash_id integer,
  CONSTRAINT hash_net_id_117 CHECK (hash_id = 117)
)
INHERITS (t_net_audit)
WITH (OIDS=FALSE);
ALTER TABLE t_net_audit_117 OWNER TO gms_user;

CREATE TABLE t_net_audit_118
(
-- 继承:   id integer NOT NULL DEFAULT,
-- 继承:   dev_id character varying(64),
-- 继承:   ftime_start bigint,
-- 继承:   ftime_end bigint,
-- 继承:   if_mobite smallint,
-- 继承:   service_group integer,
-- 继承:   serivce_type integer,
-- 继承:   protol_id integer,
-- 继承:   up_bytes bigint,
-- 继承:   down_bytes bigint,
-- 继承:   userip character varying(16),
-- 继承:   user_id character varying(32),
-- 继承:   hash_id integer,
  CONSTRAINT hash_net_id_118 CHECK (hash_id = 118)
)
INHERITS (t_net_audit)
WITH (OIDS=FALSE);
ALTER TABLE t_net_audit_118 OWNER TO gms_user;

CREATE TABLE t_net_audit_119
(
-- 继承:   id integer NOT NULL DEFAULT,
-- 继承:   dev_id character varying(64),
-- 继承:   ftime_start bigint,
-- 继承:   ftime_end bigint,
-- 继承:   if_mobite smallint,
-- 继承:   service_group integer,
-- 继承:   serivce_type integer,
-- 继承:   protol_id integer,
-- 继承:   up_bytes bigint,
-- 继承:   down_bytes bigint,
-- 继承:   userip character varying(16),
-- 继承:   user_id character varying(32),
-- 继承:   hash_id integer,
  CONSTRAINT hash_net_id_119 CHECK (hash_id = 119)
)
INHERITS (t_net_audit)
WITH (OIDS=FALSE);
ALTER TABLE t_net_audit_119 OWNER TO gms_user;

CREATE TABLE t_net_audit_120
(
-- 继承:   id integer NOT NULL DEFAULT,
-- 继承:   dev_id character varying(64),
-- 继承:   ftime_start bigint,
-- 继承:   ftime_end bigint,
-- 继承:   if_mobite smallint,
-- 继承:   service_group integer,
-- 继承:   serivce_type integer,
-- 继承:   protol_id integer,
-- 继承:   up_bytes bigint,
-- 继承:   down_bytes bigint,
-- 继承:   userip character varying(16),
-- 继承:   user_id character varying(32),
-- 继承:   hash_id integer,
  CONSTRAINT hash_net_id_120 CHECK (hash_id = 120)
)
INHERITS (t_net_audit)
WITH (OIDS=FALSE);
ALTER TABLE t_net_audit_120 OWNER TO gms_user;

CREATE TABLE t_net_audit_121
(
-- 继承:   id integer NOT NULL DEFAULT,
-- 继承:   dev_id character varying(64),
-- 继承:   ftime_start bigint,
-- 继承:   ftime_end bigint,
-- 继承:   if_mobite smallint,
-- 继承:   service_group integer,
-- 继承:   serivce_type integer,
-- 继承:   protol_id integer,
-- 继承:   up_bytes bigint,
-- 继承:   down_bytes bigint,
-- 继承:   userip character varying(16),
-- 继承:   user_id character varying(32),
-- 继承:   hash_id integer,
  CONSTRAINT hash_net_id_121 CHECK (hash_id = 121)
)
INHERITS (t_net_audit)
WITH (OIDS=FALSE);
ALTER TABLE t_net_audit_121 OWNER TO gms_user;

CREATE TABLE t_net_audit_122
(
-- 继承:   id integer NOT NULL DEFAULT,
-- 继承:   dev_id character varying(64),
-- 继承:   ftime_start bigint,
-- 继承:   ftime_end bigint,
-- 继承:   if_mobite smallint,
-- 继承:   service_group integer,
-- 继承:   serivce_type integer,
-- 继承:   protol_id integer,
-- 继承:   up_bytes bigint,
-- 继承:   down_bytes bigint,
-- 继承:   userip character varying(16),
-- 继承:   user_id character varying(32),
-- 继承:   hash_id integer,
  CONSTRAINT hash_net_id_122 CHECK (hash_id = 122)
)
INHERITS (t_net_audit)
WITH (OIDS=FALSE);
ALTER TABLE t_net_audit_122 OWNER TO gms_user;

CREATE TABLE t_net_audit_123
(
-- 继承:   id integer NOT NULL DEFAULT,
-- 继承:   dev_id character varying(64),
-- 继承:   ftime_start bigint,
-- 继承:   ftime_end bigint,
-- 继承:   if_mobite smallint,
-- 继承:   service_group integer,
-- 继承:   serivce_type integer,
-- 继承:   protol_id integer,
-- 继承:   up_bytes bigint,
-- 继承:   down_bytes bigint,
-- 继承:   userip character varying(16),
-- 继承:   user_id character varying(32),
-- 继承:   hash_id integer,
  CONSTRAINT hash_net_id_123 CHECK (hash_id = 123)
)
INHERITS (t_net_audit)
WITH (OIDS=FALSE);
ALTER TABLE t_net_audit_123 OWNER TO gms_user;

CREATE TABLE t_net_audit_124
(
-- 继承:   id integer NOT NULL DEFAULT,
-- 继承:   dev_id character varying(64),
-- 继承:   ftime_start bigint,
-- 继承:   ftime_end bigint,
-- 继承:   if_mobite smallint,
-- 继承:   service_group integer,
-- 继承:   serivce_type integer,
-- 继承:   protol_id integer,
-- 继承:   up_bytes bigint,
-- 继承:   down_bytes bigint,
-- 继承:   userip character varying(16),
-- 继承:   user_id character varying(32),
-- 继承:   hash_id integer,
  CONSTRAINT hash_net_id_124 CHECK (hash_id = 124)
)
INHERITS (t_net_audit)
WITH (OIDS=FALSE);
ALTER TABLE t_net_audit_124 OWNER TO gms_user;

CREATE TABLE t_net_audit_125
(
-- 继承:   id integer NOT NULL DEFAULT,
-- 继承:   dev_id character varying(64),
-- 继承:   ftime_start bigint,
-- 继承:   ftime_end bigint,
-- 继承:   if_mobite smallint,
-- 继承:   service_group integer,
-- 继承:   serivce_type integer,
-- 继承:   protol_id integer,
-- 继承:   up_bytes bigint,
-- 继承:   down_bytes bigint,
-- 继承:   userip character varying(16),
-- 继承:   user_id character varying(32),
-- 继承:   hash_id integer,
  CONSTRAINT hash_net_id_125 CHECK (hash_id = 125)
)
INHERITS (t_net_audit)
WITH (OIDS=FALSE);
ALTER TABLE t_net_audit_125 OWNER TO gms_user;

CREATE TABLE t_net_audit_126
(
-- 继承:   id integer NOT NULL DEFAULT,
-- 继承:   dev_id character varying(64),
-- 继承:   ftime_start bigint,
-- 继承:   ftime_end bigint,
-- 继承:   if_mobite smallint,
-- 继承:   service_group integer,
-- 继承:   serivce_type integer,
-- 继承:   protol_id integer,
-- 继承:   up_bytes bigint,
-- 继承:   down_bytes bigint,
-- 继承:   userip character varying(16),
-- 继承:   user_id character varying(32),
-- 继承:   hash_id integer,
  CONSTRAINT hash_net_id_126 CHECK (hash_id = 126)
)
INHERITS (t_net_audit)
WITH (OIDS=FALSE);
ALTER TABLE t_net_audit_126 OWNER TO gms_user;

CREATE TABLE t_net_audit_127
(
-- 继承:   id integer NOT NULL DEFAULT,
-- 继承:   dev_id character varying(64),
-- 继承:   ftime_start bigint,
-- 继承:   ftime_end bigint,
-- 继承:   if_mobite smallint,
-- 继承:   service_group integer,
-- 继承:   serivce_type integer,
-- 继承:   protol_id integer,
-- 继承:   up_bytes bigint,
-- 继承:   down_bytes bigint,
-- 继承:   userip character varying(16),
-- 继承:   user_id character varying(32),
-- 继承:   hash_id integer,
  CONSTRAINT hash_net_id_127 CHECK (hash_id = 127)
)
INHERITS (t_net_audit)
WITH (OIDS=FALSE);
ALTER TABLE t_net_audit_127 OWNER TO gms_user;
CREATE INDEX kp_net_audit_ip_0 ON t_net_audit_0 USING btree (userip);
CREATE INDEX kp_net_audit_ip_1 ON t_net_audit_1 USING btree (userip);
CREATE INDEX kp_net_audit_ip_2 ON t_net_audit_2 USING btree (userip);
CREATE INDEX kp_net_audit_ip_3 ON t_net_audit_3 USING btree (userip);
CREATE INDEX kp_net_audit_ip_4 ON t_net_audit_4 USING btree (userip);
CREATE INDEX kp_net_audit_ip_5 ON t_net_audit_5 USING btree (userip);
CREATE INDEX kp_net_audit_ip_6 ON t_net_audit_6 USING btree (userip);
CREATE INDEX kp_net_audit_ip_7 ON t_net_audit_7 USING btree (userip);
CREATE INDEX kp_net_audit_ip_8 ON t_net_audit_8 USING btree (userip);
CREATE INDEX kp_net_audit_ip_9 ON t_net_audit_9 USING btree (userip);
CREATE INDEX kp_net_audit_ip_10 ON t_net_audit_10 USING btree (userip);
CREATE INDEX kp_net_audit_ip_11 ON t_net_audit_11 USING btree (userip);
CREATE INDEX kp_net_audit_ip_12 ON t_net_audit_12 USING btree (userip);
CREATE INDEX kp_net_audit_ip_13 ON t_net_audit_13 USING btree (userip);
CREATE INDEX kp_net_audit_ip_14 ON t_net_audit_14 USING btree (userip);
CREATE INDEX kp_net_audit_ip_15 ON t_net_audit_15 USING btree (userip);
CREATE INDEX kp_net_audit_ip_16 ON t_net_audit_16 USING btree (userip);
CREATE INDEX kp_net_audit_ip_17 ON t_net_audit_17 USING btree (userip);
CREATE INDEX kp_net_audit_ip_18 ON t_net_audit_18 USING btree (userip);
CREATE INDEX kp_net_audit_ip_19 ON t_net_audit_19 USING btree (userip);
CREATE INDEX kp_net_audit_ip_20 ON t_net_audit_20 USING btree (userip);
CREATE INDEX kp_net_audit_ip_21 ON t_net_audit_21 USING btree (userip);
CREATE INDEX kp_net_audit_ip_22 ON t_net_audit_22 USING btree (userip);
CREATE INDEX kp_net_audit_ip_23 ON t_net_audit_23 USING btree (userip);
CREATE INDEX kp_net_audit_ip_24 ON t_net_audit_24 USING btree (userip);
CREATE INDEX kp_net_audit_ip_25 ON t_net_audit_25 USING btree (userip);
CREATE INDEX kp_net_audit_ip_26 ON t_net_audit_26 USING btree (userip);
CREATE INDEX kp_net_audit_ip_27 ON t_net_audit_27 USING btree (userip);
CREATE INDEX kp_net_audit_ip_28 ON t_net_audit_28 USING btree (userip);
CREATE INDEX kp_net_audit_ip_29 ON t_net_audit_29 USING btree (userip);
CREATE INDEX kp_net_audit_ip_30 ON t_net_audit_30 USING btree (userip);
CREATE INDEX kp_net_audit_ip_31 ON t_net_audit_31 USING btree (userip);
CREATE INDEX kp_net_audit_ip_32 ON t_net_audit_32 USING btree (userip);
CREATE INDEX kp_net_audit_ip_33 ON t_net_audit_33 USING btree (userip);
CREATE INDEX kp_net_audit_ip_34 ON t_net_audit_34 USING btree (userip);
CREATE INDEX kp_net_audit_ip_35 ON t_net_audit_35 USING btree (userip);
CREATE INDEX kp_net_audit_ip_36 ON t_net_audit_36 USING btree (userip);
CREATE INDEX kp_net_audit_ip_37 ON t_net_audit_37 USING btree (userip);
CREATE INDEX kp_net_audit_ip_38 ON t_net_audit_38 USING btree (userip);
CREATE INDEX kp_net_audit_ip_39 ON t_net_audit_39 USING btree (userip);
CREATE INDEX kp_net_audit_ip_40 ON t_net_audit_40 USING btree (userip);
CREATE INDEX kp_net_audit_ip_41 ON t_net_audit_41 USING btree (userip);
CREATE INDEX kp_net_audit_ip_42 ON t_net_audit_42 USING btree (userip);
CREATE INDEX kp_net_audit_ip_43 ON t_net_audit_43 USING btree (userip);
CREATE INDEX kp_net_audit_ip_44 ON t_net_audit_44 USING btree (userip);
CREATE INDEX kp_net_audit_ip_45 ON t_net_audit_45 USING btree (userip);
CREATE INDEX kp_net_audit_ip_46 ON t_net_audit_46 USING btree (userip);
CREATE INDEX kp_net_audit_ip_47 ON t_net_audit_47 USING btree (userip);
CREATE INDEX kp_net_audit_ip_48 ON t_net_audit_48 USING btree (userip);
CREATE INDEX kp_net_audit_ip_49 ON t_net_audit_49 USING btree (userip);
CREATE INDEX kp_net_audit_ip_50 ON t_net_audit_50 USING btree (userip);
CREATE INDEX kp_net_audit_ip_51 ON t_net_audit_51 USING btree (userip);
CREATE INDEX kp_net_audit_ip_52 ON t_net_audit_52 USING btree (userip);
CREATE INDEX kp_net_audit_ip_53 ON t_net_audit_53 USING btree (userip);
CREATE INDEX kp_net_audit_ip_54 ON t_net_audit_54 USING btree (userip);
CREATE INDEX kp_net_audit_ip_55 ON t_net_audit_55 USING btree (userip);
CREATE INDEX kp_net_audit_ip_56 ON t_net_audit_56 USING btree (userip);
CREATE INDEX kp_net_audit_ip_57 ON t_net_audit_57 USING btree (userip);
CREATE INDEX kp_net_audit_ip_58 ON t_net_audit_58 USING btree (userip);
CREATE INDEX kp_net_audit_ip_59 ON t_net_audit_59 USING btree (userip);
CREATE INDEX kp_net_audit_ip_60 ON t_net_audit_60 USING btree (userip);
CREATE INDEX kp_net_audit_ip_61 ON t_net_audit_61 USING btree (userip);
CREATE INDEX kp_net_audit_ip_62 ON t_net_audit_62 USING btree (userip);
CREATE INDEX kp_net_audit_ip_63 ON t_net_audit_63 USING btree (userip);
CREATE INDEX kp_net_audit_ip_64 ON t_net_audit_64 USING btree (userip);
CREATE INDEX kp_net_audit_ip_65 ON t_net_audit_65 USING btree (userip);
CREATE INDEX kp_net_audit_ip_66 ON t_net_audit_66 USING btree (userip);
CREATE INDEX kp_net_audit_ip_67 ON t_net_audit_67 USING btree (userip);
CREATE INDEX kp_net_audit_ip_68 ON t_net_audit_68 USING btree (userip);
CREATE INDEX kp_net_audit_ip_69 ON t_net_audit_69 USING btree (userip);
CREATE INDEX kp_net_audit_ip_70 ON t_net_audit_70 USING btree (userip);
CREATE INDEX kp_net_audit_ip_71 ON t_net_audit_71 USING btree (userip);
CREATE INDEX kp_net_audit_ip_72 ON t_net_audit_72 USING btree (userip);
CREATE INDEX kp_net_audit_ip_73 ON t_net_audit_73 USING btree (userip);
CREATE INDEX kp_net_audit_ip_74 ON t_net_audit_74 USING btree (userip);
CREATE INDEX kp_net_audit_ip_75 ON t_net_audit_75 USING btree (userip);
CREATE INDEX kp_net_audit_ip_76 ON t_net_audit_76 USING btree (userip);
CREATE INDEX kp_net_audit_ip_77 ON t_net_audit_77 USING btree (userip);
CREATE INDEX kp_net_audit_ip_78 ON t_net_audit_78 USING btree (userip);
CREATE INDEX kp_net_audit_ip_79 ON t_net_audit_79 USING btree (userip);
CREATE INDEX kp_net_audit_ip_80 ON t_net_audit_80 USING btree (userip);
CREATE INDEX kp_net_audit_ip_81 ON t_net_audit_81 USING btree (userip);
CREATE INDEX kp_net_audit_ip_82 ON t_net_audit_82 USING btree (userip);
CREATE INDEX kp_net_audit_ip_83 ON t_net_audit_83 USING btree (userip);
CREATE INDEX kp_net_audit_ip_84 ON t_net_audit_84 USING btree (userip);
CREATE INDEX kp_net_audit_ip_85 ON t_net_audit_85 USING btree (userip);
CREATE INDEX kp_net_audit_ip_86 ON t_net_audit_86 USING btree (userip);
CREATE INDEX kp_net_audit_ip_87 ON t_net_audit_87 USING btree (userip);
CREATE INDEX kp_net_audit_ip_88 ON t_net_audit_88 USING btree (userip);
CREATE INDEX kp_net_audit_ip_89 ON t_net_audit_89 USING btree (userip);
CREATE INDEX kp_net_audit_ip_90 ON t_net_audit_90 USING btree (userip);
CREATE INDEX kp_net_audit_ip_91 ON t_net_audit_91 USING btree (userip);
CREATE INDEX kp_net_audit_ip_92 ON t_net_audit_92 USING btree (userip);
CREATE INDEX kp_net_audit_ip_93 ON t_net_audit_93 USING btree (userip);
CREATE INDEX kp_net_audit_ip_94 ON t_net_audit_94 USING btree (userip);
CREATE INDEX kp_net_audit_ip_95 ON t_net_audit_95 USING btree (userip);
CREATE INDEX kp_net_audit_ip_96 ON t_net_audit_96 USING btree (userip);
CREATE INDEX kp_net_audit_ip_97 ON t_net_audit_97 USING btree (userip);
CREATE INDEX kp_net_audit_ip_98 ON t_net_audit_98 USING btree (userip);
CREATE INDEX kp_net_audit_ip_99 ON t_net_audit_99 USING btree (userip);
CREATE INDEX kp_net_audit_ip_100 ON t_net_audit_100 USING btree (userip);
CREATE INDEX kp_net_audit_ip_101 ON t_net_audit_101 USING btree (userip);
CREATE INDEX kp_net_audit_ip_102 ON t_net_audit_102 USING btree (userip);
CREATE INDEX kp_net_audit_ip_103 ON t_net_audit_103 USING btree (userip);
CREATE INDEX kp_net_audit_ip_104 ON t_net_audit_104 USING btree (userip);
CREATE INDEX kp_net_audit_ip_105 ON t_net_audit_105 USING btree (userip);
CREATE INDEX kp_net_audit_ip_106 ON t_net_audit_106 USING btree (userip);
CREATE INDEX kp_net_audit_ip_107 ON t_net_audit_107 USING btree (userip);
CREATE INDEX kp_net_audit_ip_108 ON t_net_audit_108 USING btree (userip);
CREATE INDEX kp_net_audit_ip_109 ON t_net_audit_109 USING btree (userip);
CREATE INDEX kp_net_audit_ip_110 ON t_net_audit_110 USING btree (userip);
CREATE INDEX kp_net_audit_ip_111 ON t_net_audit_111 USING btree (userip);
CREATE INDEX kp_net_audit_ip_112 ON t_net_audit_112 USING btree (userip);
CREATE INDEX kp_net_audit_ip_113 ON t_net_audit_113 USING btree (userip);
CREATE INDEX kp_net_audit_ip_114 ON t_net_audit_114 USING btree (userip);
CREATE INDEX kp_net_audit_ip_115 ON t_net_audit_115 USING btree (userip);
CREATE INDEX kp_net_audit_ip_116 ON t_net_audit_116 USING btree (userip);
CREATE INDEX kp_net_audit_ip_117 ON t_net_audit_117 USING btree (userip);
CREATE INDEX kp_net_audit_ip_118 ON t_net_audit_118 USING btree (userip);
CREATE INDEX kp_net_audit_ip_119 ON t_net_audit_119 USING btree (userip);
CREATE INDEX kp_net_audit_ip_120 ON t_net_audit_120 USING btree (userip);
CREATE INDEX kp_net_audit_ip_121 ON t_net_audit_121 USING btree (userip);
CREATE INDEX kp_net_audit_ip_122 ON t_net_audit_122 USING btree (userip);
CREATE INDEX kp_net_audit_ip_123 ON t_net_audit_123 USING btree (userip);
CREATE INDEX kp_net_audit_ip_124 ON t_net_audit_124 USING btree (userip);
CREATE INDEX kp_net_audit_ip_125 ON t_net_audit_125 USING btree (userip);
CREATE INDEX kp_net_audit_ip_126 ON t_net_audit_126 USING btree (userip);
CREATE INDEX kp_net_audit_ip_127 ON t_net_audit_127 USING btree (userip);


E2







































