DROP DATABASE IF EXISTS mytest;
CREATE DATABASE mytest;

USE mytest;

DROP TABLE IF EXISTS user;
CREATE TABLE user(
	  id                        INT UNSIGNED AUTO_INCREMENT COMMENT '主键，自增长',
	  cellphone                 VARCHAR(16) COMMENT '手机号',
	  nickname                  VARCHAR(64) COMMENT '昵称',
	  UNIQUE KEY (cellphone),
	  PRIMARY KEY (id)
) ENGINE=InnoDB AUTO_INCREMENT=1 COMMENT='用户信息表';

INSERT INTO user(cellphone,nickname) values("18201032326","good");
INSERT INTO user(cellphone,nickname)  values("18801032327","well");
