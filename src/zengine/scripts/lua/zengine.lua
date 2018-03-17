-- Copyright (C) 2012 by wubenqi
-- Distributable under the terms of either the Apache License (Version 2.0) or 
-- the GNU Lesser General Public License, as specified in the COPYING file.
--
-- By: wubenqi<wubenqi@gmail.com>
--

------------------------------------------------------------------------------------------
-- �������ʼ��
function MainInitialize()
  LOG("MainInitialize")  
  return 0
end

-- ���������
function MainDestroy()
  LOG("MainDestroy")
  return 0
end

------------------------------------------------------------------------------------------
-- �̴߳���������
function OnThreadInitialize(zcontext)
  LOG("OnThreadInitialize ")
    
  zcontext:SetTimer(10 ,10)
  return 0
end

function OnThreadDestroy(zcontext)
  LOG("OnThreadDestroy ")
  return 0
end

------------------------------------------------------------------------------------------
-- ��ʱ��
function OnTimer(zcontext, timer_id, tm)
  LOG("OnTimer: ".. timer_id)
  return 0
end

------------------------------------------------------------------------------------------
-- �첽��Ϣ
function OnTaskDataReceived(zcontext, task_data)
  LOG("OnTaskDataReceived: " .. task_data)
  return 0
end

------------------------------------------------------------------------------------------
-- �������ӽ���
function OnNewConnection(ih)
  --zcontext = ih:GetContext()
  LOG("OnNewConnection")
  --zcontext:PostTaskToMainThread("1111")
  return 0
end

-- ���յ���������
function OnDataReceived(ih, message_type, io_buffer)
  LOG("OnTCPPacketReceived")

  return 0
end

-- �������ӹر�
function OnConnectionClosed(ih)
  LOG("OnConnectionClosed")  
  return 0
end
