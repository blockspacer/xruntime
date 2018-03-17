// Copyright (C) 2012 by wubenqi
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// By: wubenqi<wubenqi@gmail.com>
//

#ifndef NET_ZNET_ZNET_PACKET_HEADER_H_
#define NET_ZNET_ZNET_PACKET_HEADER_H_
#pragma once

#include "base/logging.h"

namespace net {

class ZNetPacketHeader {
public:
  static const uint32_t kMaxPacketSize = 2 * 1024 * 1024; // 2MB
  static const uint32_t kPacketHeaderSize = 8;
  static const uint32_t kPacketVersion = 0;

  // �����������ݰ�ͷ
  // ����ֵ��
  //  -1�� ���ɳ������ݰ����ȳ���kMaxPacketSize
  //  0 �� ���ɳɹ��������ݰ���
  //  1 �� ���ɳɹ���δ�����ݰ���
  static inline int GeneratePacketHeader(uint8_t* data, uint16_t message_type,
    uint32_t body_len) {
    if (body_len > kMaxPacketSize) {
      return -1;
    }

    data[0] = (kPacketVersion >> 8) & 0xFF;
    data[1] = kPacketVersion & 0xFF;
    data[2] = (message_type >> 8) & 0xFF;
    data[3] = message_type & 0xFF;
    data[4] = (body_len >> 24) & 0xFF;
    data[5] = (body_len >> 16) & 0xFF;
    data[6] = (body_len >> 8) & 0xFF;
    data[7] = body_len & 0xFF;

    if (body_len > 0) {
      return 0;
    }
    else {
      return 1;
    }
  }

  // �����������ݰ�ͷ
  // ����ֵ��
  //  -1�� ������������汾�Ų��Ի������ݰ����ȳ���kMaxPacketSize
  //  0 �� �����ɹ��������ݰ���
  //  1 �� �����ɹ���δ�����ݰ���

  static inline int ParsePacketHeader(const uint8_t* data, uint16_t* message_type,
    uint32_t* body_len) {
    uint16_t version;
    version = (uint16_t)data[0] << 8 | data[1];
    *message_type = (uint16_t)data[2] << 8 | data[3];
    *body_len = ((data[4] & 0xFF) << 24)
      | ((data[5] & 0xFF) << 16)
      | ((data[6] & 0xFF) << 8)
      | (data[7] & 0xFF);
    if (/*version!=kPacketVersion
      || */*body_len > kMaxPacketSize) {
      return -1;
    }
    else if (*body_len == 0) {
      return 1;
    }
    return 0;
  }

private:
  DISALLOW_COPY_AND_ASSIGN(ZNetPacketHeader);
};

}

#endif	// NET_ZNET_ZNET_PACKET_HEADER_H_
