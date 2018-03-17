// Copyright (C) 2014 by wubenqi
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// By: wubenqi<wubenqi@gmail.com>
//

#ifndef NETENGINE_TEAMTALK_TEAMTALK_PACKET_H_
#define NETENGINE_TEAMTALK_TEAMTALK_PACKET_H_

#include <stdint.h>

#include "base/memory/ref_counted.h"

//���մ������ݰ�
class TeamTalkPacket :
  public base::RefCountedThreadSafe<TeamTalkPacket> {
public:
  static const uint32_t kMaxPacketSize = 64 * 1024 * 1024; // 64MB
  static const uint32_t kPacketHeaderSize = 12;
  static const uint16_t kPacketVersion = 1;
  static const uint16_t kReservedValue = 0;

  // Packet();
  // ע�⣺��body_len,packet_len = body_len+kPacketHeaderSize
  TeamTalkPacket(uint32_t message_type, uint16_t reserved, uint32_t body_len);
  virtual ~TeamTalkPacket();

  static const scoped_refptr<TeamTalkPacket>& GetEmptyPacket();

  // �����������ݰ�ͷ
  // ����ֵ��
  //  -1�� ���ɳ������ݰ����ȳ���kMaxPacketSize
  //  0 �� ���ɳɹ��������ݰ���
  //  1 �� ���ɳɹ���δ�����ݰ���
  static inline int GeneratePacketHeader(uint8_t* data, uint32_t message_type,
    uint16_t reserved, uint32_t body_len) {
    if (body_len > kMaxPacketSize) {
      return -1;
    }
    uint32_t data_len = body_len + kPacketHeaderSize;
    data[0] = (data_len >> 24) & 0xFF;
    data[1] = (data_len >> 16) & 0xFF;
    data[2] = (data_len >> 8) & 0xFF;
    data[3] = data_len & 0xFF;

    data[4] = (message_type >> 24) & 0xFF;
    data[5] = (message_type >> 16) & 0xFF;
    data[6] = (message_type >> 8) & 0xFF;
    data[7] = message_type & 0xFF;

    data[8] = (kPacketVersion >> 8) & 0xFF;
    data[9] = kPacketVersion & 0xFF;

    data[10] = (reserved >> 8) & 0xFF;
    data[11] = reserved & 0xFF;

    return 0;
  }

  // �����������ݰ�ͷ
  // ����ֵ��
  //  -1�� ������������汾�Ų��Ի������ݰ����ȳ���kMaxPacketSize
  //  0 �� �����ɹ��������ݰ���
  //  1 �� �����ɹ���δ�����ݰ���
  static inline int ParsePacketHeader(const uint8_t* head_data, uint32_t* message_type,
    uint16_t* reserved, uint32_t* body_len) {
    uint16_t version;
    // uint16_t reserved;
    uint32_t packet_len;

    packet_len = ((head_data[0] & 0xFF) << 24)
      | ((head_data[1] & 0xFF) << 16)
      | ((head_data[2] & 0xFF) << 8)
      | (head_data[3] & 0xFF);
    *message_type = ((head_data[4] & 0xFF) << 24)
      | ((head_data[5] & 0xFF) << 16)
      | ((head_data[6] & 0xFF) << 8)
      | (head_data[7] & 0xFF);
    version = head_data[8] << 8 | head_data[9];
    *reserved = head_data[10] << 8 | head_data[11];

    if (version != kPacketVersion ||
      packet_len > kMaxPacketSize ||
      packet_len < kPacketHeaderSize) {
      return -1;
    }
    else if (packet_len == kPacketHeaderSize) {
      *body_len = 0;
      return 1;
    }
    *body_len = packet_len - kPacketHeaderSize;
    return 0;
  }

  inline uint32_t GetRawdataLength() const {
    return ((data_[0] & 0xFF) << 24) |
      ((data_[1] & 0xFF) << 16) |
      ((data_[2] & 0xFF) << 8) |
      (data_[3] & 0xFF
        );
  }

  inline const uint8_t* GetRawdataConstBuffer() const {
    return data_;
  }

  inline uint32_t GetMessageType() const {
    return ((data_[4] & 0xFF) << 24) |
      ((data_[5] & 0xFF) << 16) |
      ((data_[6] & 0xFF) << 8) |
      (data_[7] & 0xFF
        );
  }

  inline uint16_t GetVersion() const {
    return (uint16_t)data_[8] << 8 | data_[9];
  }

  inline uint16_t GetReserved() const {
    return (uint16_t)data_[10] << 8 | data_[11];
  }

  inline const void* GetBodyData() const {
    return data_ + kPacketHeaderSize;
  }

  inline uint8_t* GetBodyData() {
    return data_ + kPacketHeaderSize;
  }

  inline uint8_t* GetBodyMutableData() {
    return data_ + kPacketHeaderSize;
  }

  inline uint32_t GetBodyLength() const {
    return GetRawdataLength() - kPacketHeaderSize;
  }

  inline uint8_t* GetHeadMutableBuffer() {
    return data_;
  }

  inline uint32_t GetHeaderLength() const {
    return kPacketHeaderSize;
  }

  TeamTalkPacket* Clone() const;

protected:
  // friend class OutPacketStream;
  // �˹��캯��ֻ����OutByteStream��ʹ��,�����ط�ʹ�û������
  // TeamTalkPacket(uint16_t cmd_type, const void* data, uint32_t data_len);
  TeamTalkPacket(const void* data, uint32_t data_len);

  uint8_t*	data_;

  DISALLOW_COPY_AND_ASSIGN(TeamTalkPacket);

};

typedef scoped_refptr<TeamTalkPacket> TeamTalkPacketPtr;

#endif	//
