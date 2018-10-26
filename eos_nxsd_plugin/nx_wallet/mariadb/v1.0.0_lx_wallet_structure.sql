DROP DATABASE IF EXISTS nwdb_wallet;
CREATE DATABASE nwdb_wallet DEFAULT CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci;

USE nwdb_wallet;

DROP TABLE IF EXISTS nw_user;
CREATE TABLE nw_user(
  id							          INT UNSIGNED AUTO_INCREMENT COMMENT '主键，自增长',
  cellphone                 VARCHAR(16) COMMENT '手机号',
  nickname					        VARCHAR(64) COMMENT '昵称',
  invite_code					      VARCHAR(64) COMMENT '用户邀请码',
  up_user_id					      VARCHAR(64) COMMENT '上级用户id',
  user_status               TINYINT UNSIGNED DEFAULT 1 NOT NULL COMMENT '用户状态，非空，默认1（有效）;0：封号；1：有效；',
  wallet_id					        INT UNSIGNED NOT NULL  COMMENT '默认钱包ID',
  update_time               TIMESTAMP NOT NULL COMMENT '更新时间',
  create_time               TIMESTAMP NOT NULL COMMENT '创建时间',
  UNIQUE KEY (cellphone),
  PRIMARY KEY (id)
) ENGINE=InnoDB AUTO_INCREMENT=1001 COMMENT='用户信息表';

DROP TABLE IF EXISTS nw_device; 
CREATE TABLE nw_device(
  id							          INT UNSIGNED AUTO_INCREMENT COMMENT '主键，自增长',
  device_platform						TINYINT UNSIGNED NOT NULL COMMENT '设备平台，非空;1：android；2：ios；',
  user_id							      INT UNSIGNED NOT NULL COMMENT '用户ID 无则为0',
  device_uuid               VARCHAR(64) COMMENT '设备ID',
  wallet_id					        INT UNSIGNED NOT NULL  COMMENT '默认钱包ID',
  create_time               TIMESTAMP NOT NULL COMMENT '创建时间',
  PRIMARY KEY (id)
) ENGINE=InnoDB AUTO_INCREMENT=1001 COMMENT='设备表';

DROP TABLE IF EXISTS nw_invite_code_store;
CREATE TABLE nw_invite_code_store(
  id						      	    INT UNSIGNED AUTO_INCREMENT COMMENT '主键，自增长',
  invite_code               varchar(64) null comment '邀请码:',
  batch                     varchar(64) null comment '批次',
  activity_id							  INT UNSIGNED DEFAULT 0 COMMENT '关联活动ID,无则为0',
  type                      SMALLINT null comment '类型（1-活动，2-自主注册钱包）',
  status                    SMALLINT null comment '状态(1-初始 2-可用 3-已使用 4-失效)',
  expire_time               TIMESTAMP NOT NULL COMMENT '过期时间（预留）',
  update_time               TIMESTAMP NOT NULL COMMENT '更新时间',
  create_time               TIMESTAMP NOT NULL COMMENT '创建时间',
  PRIMARY KEY (id)
) ENGINE=InnoDB AUTO_INCREMENT=1001 COMMENT='邀请码库表';

DROP TABLE IF EXISTS nw_invite_code_buy;
CREATE TABLE nw_invite_code_buy(
  id						      	    INT UNSIGNED AUTO_INCREMENT COMMENT '主键，自增长',
  user_id							      INT UNSIGNED NOT NULL COMMENT '用户ID',
  device_id							    INT UNSIGNED NOT NULL COMMENT '设备ID',
  invite_code               varchar(64) null comment '邀请码:',
  type                      SMALLINT null comment '类型（1-EOS创建钱包邀请码）',
  status                    SMALLINT null comment '是否已经使用（1-是，2-否）',
  pay_status                SMALLINT null comment '是否支付（1-是，2-否）',
  pay_type                  SMALLINT null comment '支付类型（1-支付宝，2-微信，转账）',
  pay_money                 DECIMAL(30,10) NOT NULL COMMENT '支付金额',
  order_no                  varchar(64) null comment '订单id',
  channel_order_no          varchar(64) null comment '支付渠道订单id',
  update_time               TIMESTAMP NOT NULL COMMENT '更新时间',
  create_time               TIMESTAMP NOT NULL COMMENT '创建时间',
  PRIMARY KEY (id)
) ENGINE=InnoDB AUTO_INCREMENT=1001 COMMENT='邀请码购买表';

DROP TABLE IF EXISTS nw_wallet;
CREATE TABLE nw_wallet(
  id							    INT UNSIGNED AUTO_INCREMENT COMMENT '主键，自增长',
  invite_code               varchar(64) null comment '邀请码:',
  wallet_name					      VARCHAR(64) COMMENT '昵称',
  wallet_account_name	      VARCHAR(64) COMMENT '账户名 目前只有eos需要',
  wallet_type               TINYINT UNSIGNED NOT NULL COMMENT '钱包类型，非空;1：xbeans钱包；2：eos钱包；',
  public_key					      VARCHAR(1024) COMMENT '公钥',
  private_key               VARCHAR(1024) COMMENT '私钥',
  eos_pw                    VARCHAR(512) COMMENT 'eos账户密码',
  app_pw                    VARCHAR(64) COMMENT '钱包支付密码',
  update_time               TIMESTAMP NOT NULL COMMENT '更新时间',
  create_time               TIMESTAMP NOT NULL COMMENT '创建时间',
  PRIMARY KEY (id)
) ENGINE=InnoDB AUTO_INCREMENT=1001 COMMENT='钱包信息表';

DROP TABLE IF EXISTS nw_native_wallet;
CREATE TABLE nw_native_wallet(
  id							          INT UNSIGNED AUTO_INCREMENT COMMENT '主键，自增长',
  user_id							      INT UNSIGNED NOT NULL COMMENT '用户ID',
  device_id							    INT UNSIGNED NOT NULL COMMENT '设备ID',
  coin_type                 SMALLINT null comment '币的类型（1-LB）',
  coin_quantity             DECIMAL(30,10) NOT NULL COMMENT '币的数量',
  update_time               TIMESTAMP NOT NULL COMMENT '更新时间',
  create_time               TIMESTAMP NOT NULL COMMENT '创建时间',
  PRIMARY KEY (id)
) ENGINE=InnoDB AUTO_INCREMENT=1001 COMMENT='本地钱包';

DROP TABLE IF EXISTS nw_user_re_wallet;
CREATE TABLE nw_user_re_wallet(
  user_id							      INT UNSIGNED NOT NULL COMMENT '用户ID',
  wallet_id					        INT UNSIGNED NOT NULL  COMMENT '钱包ID',
  create_time               TIMESTAMP NOT NULL COMMENT '创建时间'
) ENGINE=InnoDB AUTO_INCREMENT=1001 COMMENT='用户钱包信息关联表';

DROP TABLE IF EXISTS nw_device_re_wallet;
CREATE TABLE nw_device_re_wallet(
  device_id							    INT UNSIGNED NOT NULL COMMENT '设备ID',
  wallet_id					        INT UNSIGNED NOT NULL  COMMENT '钱包ID',
  create_time               TIMESTAMP NOT NULL COMMENT '创建时间'
) ENGINE=InnoDB AUTO_INCREMENT=1001 COMMENT='设备钱包信息关联表';

DROP TABLE IF EXISTS nw_user_withdraw;
CREATE TABLE nw_user_withdraw(
  id                        INT UNSIGNED AUTO_INCREMENT COMMENT '主键，自增长',
  wallet_id					        INT UNSIGNED NOT NULL COMMENT '钱包id',
  coin_type                 SMALLINT UNSIGNED NOT NULL COMMENT '币种：1-LB',
  withdraw_coin             DECIMAL(30,10) NOT NULL COMMENT '提现币的个数',
  poundage                  DECIMAL(30,10) NOT NULL COMMENT '提现手续费？扣LB币还是人民币？',
  rmb                       DECIMAL(30,10) NULL COMMENT '人民币（元）',
  staus                     TINYINT UNSIGNED NOT NULL COMMENT '提现状态;1-提现中 2-提现成功 3-提现失败',
  reason                    VARCHAR(128) COMMENT '失败原因',
  update_time               TIMESTAMP NOT NULL COMMENT '更新时间',
  create_time               TIMESTAMP NOT NULL COMMENT '创建时间',
  PRIMARY KEY (id)
) ENGINE=InnoDB AUTO_INCREMENT=1001 COMMENT='提现申请表';


DROP TABLE IF EXISTS nw_wallet_trade;
CREATE TABLE nw_wallet_trade(
  id                        INT UNSIGNED AUTO_INCREMENT COMMENT '主键，自增长',
  wallet_id					        INT UNSIGNED NOT NULL COMMENT '钱包id',
  wallet_account_name	      VARCHAR(64) COMMENT '账户名 目前只有eos需要',
  wallet_type               SMALLINT UNSIGNED NOT NULL COMMENT '钱包类型，非空;1-xbeans钱包；2-eos钱包；',
  coin_type                 SMALLINT UNSIGNED NOT NULL COMMENT '币种；1-LB 2-XB；',
  from_wallet_address			  VARCHAR(64) COMMENT '来源钱包地址',
  to_wallet_address					VARCHAR(64) COMMENT '目标钱包地址',
  poundage_coin            DECIMAL(30,10) NOT NULL COMMENT '转账手续费金额',
  transfer_coin            DECIMAL(30,10) NULL COMMENT '转账金额',
  create_time               TIMESTAMP NOT NULL COMMENT '创建时间',
  PRIMARY KEY (id)
) ENGINE=InnoDB AUTO_INCREMENT=1001 COMMENT='钱包交易信息表';


DROP TABLE IF EXISTS nw_system_config;
CREATE TABLE nw_system_config(
  id							          INT UNSIGNED AUTO_INCREMENT COMMENT '主键，自增长',
  config_key                varchar(64) null comment '系统配置键值',
	config_name               varchar(100) null comment '系统配置名称',
	config_value              varchar(1024) null comment '系统配置值',
	status                    SMALLINT null comment '是否有效（1-有效，2-失效）',
	config_type               varchar(20) null comment 'config_value的类型(int-整型,string-字符串)',
  update_time               TIMESTAMP NOT NULL COMMENT '更新时间',
  create_time               TIMESTAMP NOT NULL COMMENT '创建时间',
  PRIMARY KEY (id)
) ENGINE=InnoDB AUTO_INCREMENT=1001 COMMENT='系统配置表';


DROP TABLE IF EXISTS nw_user_trader;
CREATE TABLE nw_user_trader(
  id							          INT UNSIGNED AUTO_INCREMENT COMMENT '主键，自增长',
  user_id							      INT UNSIGNED NOT NULL COMMENT '用户ID 无则为0',
  linkman_name              VARCHAR(64) COMMENT '联系人名称',
  update_time               TIMESTAMP NOT NULL COMMENT '更新时间',
  create_time               TIMESTAMP NOT NULL COMMENT '创建时间',
  PRIMARY KEY (id)
) ENGINE=InnoDB AUTO_INCREMENT=1001 COMMENT='用户交易人表';

DROP TABLE IF EXISTS nw_user_trader_wallet;
CREATE TABLE nw_user_trader_wallet(
  id							          INT UNSIGNED AUTO_INCREMENT COMMENT '主键，自增长',
  user_trader_id			      INT UNSIGNED NOT NULL COMMENT '用户交易者信息ID',
  address                   VARCHAR(1024) COMMENT '钱包地址',
  account                   VARCHAR(64) COMMENT '钱包名称：例如eos账号',
  wallet_type               SMALLINT UNSIGNED NOT NULL COMMENT '钱包类型，非空;1：xbeans钱包；2：eos钱包；',
  update_time               TIMESTAMP NOT NULL COMMENT '更新时间',
  create_time               TIMESTAMP NOT NULL COMMENT '创建时间',
  PRIMARY KEY (id)
) ENGINE=InnoDB AUTO_INCREMENT=1001 COMMENT='用户交易人钱包表';

DROP TABLE IF EXISTS nw_user_trader;
CREATE TABLE nw_user_trader(
  id							          INT UNSIGNED AUTO_INCREMENT COMMENT '主键，自增长',
  user_id							      INT UNSIGNED NOT NULL COMMENT '用户ID',
  config_key                VARCHAR(64) COMMENT '配置key',
  config_value              VARCHAR(64) COMMENT '配置内容',
  update_time               TIMESTAMP NOT NULL COMMENT '更新时间',
  create_time               TIMESTAMP NOT NULL COMMENT '创建时间',
  PRIMARY KEY (id)
) ENGINE=InnoDB AUTO_INCREMENT=1001 COMMENT='用户交易人表';

DROP TABLE IF EXISTS nw_exchange_market;
CREATE TABLE nw_exchange_market(
  id                        INT UNSIGNED AUTO_INCREMENT COMMENT '主键，自增长',
  user_id							      INT UNSIGNED NOT NULL COMMENT '用户ID',
  user_cellphone			      VARCHAR(16) COMMENT '用户手机号码（冗余字段）',
  original_coin             VARCHAR(64) COMMENT '原币',
  target_coin               VARCHAR(64) COMMENT '目标币',
  coin_num                  DECIMAL(30,10) COMMENT '原币兑换数量',
  exchange_price            DECIMAL(30,10) NULL COMMENT '原币兑换目标币价格：如：0.5代表1个原币兑换0.5个目标币',
  status                    SMALLINT null comment '兑换状态（1-未兑换，2-已兑换）',
  update_time               TIMESTAMP NOT NULL COMMENT '更新时间',
  create_time               TIMESTAMP NOT NULL COMMENT '创建时间',
  PRIMARY KEY (id)
) ENGINE=InnoDB AUTO_INCREMENT=1001 COMMENT='用户虚拟币兑换市场表';

DROP TABLE IF EXISTS nw_exchange_record;
CREATE TABLE nw_exchange_record(
  id                        INT UNSIGNED AUTO_INCREMENT COMMENT '主键，自增长',
  user_id							      INT UNSIGNED NOT NULL COMMENT '用户ID',
  exchange_market_id	      INT UNSIGNED NOT NULL COMMENT '用户ID',
  original_user_id          VARCHAR(64) COMMENT '原币用户id',
  original_coin             VARCHAR(64) COMMENT '原币',
  target_coin               VARCHAR(64) COMMENT '目标币',
  coin_num                  DECIMAL(30,10) COMMENT '原币兑换数量',
  exchange_price            DECIMAL(30,10) NULL COMMENT '原币兑换目标币价格：如：0.5代表1个原币兑换0.5个目标币',
  update_time               TIMESTAMP NOT NULL COMMENT '更新时间',
  create_time               TIMESTAMP NOT NULL COMMENT '创建时间',
  PRIMARY KEY (id)
) ENGINE=InnoDB AUTO_INCREMENT=1001 COMMENT='用户虚拟币兑换记录表';


