-- Copyright (C) 2012 by wubenqi
-- Distributable under the terms of either the Apache License (Version 2.0) or 
-- the GNU Lesser General Public License, as specified in the COPYING file.
--
-- By: wubenqi<wubenqi@gmail.com>
--


-- cache��������Ϣ���Ͷ���
zcache_message_types = {
  SERVER_DATA_REQ = 1,     -- ��Ϸ�߼�����������ʱ����ȫ������
  SERVER_DATA_REP = 2,     -- ȫ���������󷵻�
  
  ACCOUNT_DATA_REQ = 3,    -- �����ʺ���Ϣ
  ACCOUNT_DATA_REP = 4,    -- �����ʺ�����

  PLAYER_INFO_REQ = 5,     -- ���������Ϣ��ͨ������Ҫ��ϵ���б�����˵��������������Ҫ��ʾʱ����
  PLAYER_INFO_REP = 6,     -- ���������Ϣ
  
  PLAYER_DATA_REQ = 7,     -- ���������Ϸ����
  PLAYER_DATA_REP = 8,     -- ���������Ϸ����
  
  PLAYER_ATTRS_DATA_REQ = 9,     -- ��������������ݣ�ͨ���ǲ鿴���������ϸ��Ϣ�򾺼������������ս��ʱ������
  PLAYER_ATTRS_DATA_REP = 10,     -- ���������������
}
