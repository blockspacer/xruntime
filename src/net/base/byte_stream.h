// Copyright (C) 2012 by wubenqi
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// By: wubenqi<wubenqi@gmail.com>
//

#ifndef NET_BASE_BYTE_STREAM_H_
#define NET_BASE_BYTE_STREAM_H_

#include <stdint.h>

#include <string>

namespace net {

class ByteStream {
public:
  // Flag values
  // enum Mode { store = 0, load = 1}; // ���棨д�������س���������
  // ���ڴ洢�棨�ڴ�д��������ڴ��س����ڴ������
  /**
    * ��������д���ByteStream����,��д�������ֽ���
    * @param dataLen ���ݵ���󳤶�
    */
  ByteStream();

  /**
    * �������ڶ�����ByteStream����,�ٶ�pBuffer����dataLen���ȵ�����
    * �κζ�ByteStream����ĸ��Ľ�Ӱ�������pBuffer�е�����
    * @param pBuffer ����,Ӧȷ��������dataLen����
    * @param dataLen ���ݳ���
    */
  ByteStream(const void* buffer, uint32_t data_len);

  virtual ~ByteStream(void);

public:
  /**
    * ���ڲ�ָ�붨λ��ĳһλ��
    * @param pos ���ڲ�ָ�붨λ����λ��,��λ������ڿ�ʼλ��(0)
    * @return �ɹ����ص�ǰ�ڲ�ָ��ָ��λ��,���򷵻�-1
    */
  int32_t Seek(uint32_t pos) const;

  /**
    * �����ڲ�ָ����ָ��λ��
    * @return ���ɹ�����-1
    */
  uint32_t Tell(void) const;

  /**
    * ���������ֽ�
    * @param pos Ҫ�������ֽ���
    * @return �ɹ�����0
    */
  int32_t Skip(uint32_t pos) const;

  /**
    * �ж�operator << ��operator >>���� �Ƿ�ɹ�
    * @return �����ɹ�,�򷵻�true
    */
  bool Fail(void) const;

  /**
    * ���صײ�Ļ�����ָ��
    */
  const char* Data(void) const;
  char* MutableData(void);

  /**
    * ���صײ�����ݵĳ���
    */
  uint32_t Length(void) const;

  /**
    * ���������ֽ�
    * @param pdata ��Ŷ���������,Ӧ��ȷ����ָ�������ܹ�����dataLen���ֽ�
    * @param dataLen Ҫ�������ֽ���
    * @return �ɹ�����0
    */
  int32_t ReadRawData(void* data, uint32_t data_len) const;

  /**
    * д�뼸���ֽ�
    * @param pdata Ҫд�������,Ӧ��ȷ����ָ������ӵ��dataLen���ֽ�
    * @param dataLen Ҫд����ֽ���
    * @return �ɹ�����0
    */
  int32_t WriteRawData(const void* data, uint32_t data_len);

  /**
    * ����һ���ַ���(��'\0'��β)
    * @param pdata ��Ŷ������ַ���,Ӧ��ȷ����ָ�������ܹ�����dataLen���ֽ�
    * @param dataLen Ҫ�������ֽ���,������β�ַ�'\0'
    * @return �ɹ�����0
    */
  int32_t ReadString(char* s, uint32_t data_len) const;
  int32_t ReadString(std::string& str) const;
  const ByteStream& operator >> (std::string& str) const;

  /**
    * д��һ���ַ���(��'\0'��β)
    * @param pdata Ҫд����ַ���
    * @return �ɹ�����0
    */
  int32_t WriteString(const char* s);
  int32_t WriteString(const std::string& str);
  ByteStream& operator << (const std::string& str);

  /**
    * д��һ��bool����
    * @param b Ҫд���bool
    * @return ���ɹ�,��fail() == true
    */
  ByteStream& operator << (bool b);

  /**
    * д��һ��char��
    * @param ch Ҫд���ch
    * @return ���ɹ�,��fail() == true
    */
  ByteStream& operator << (char ch);

  /**
    * д��һ��uint8_t��
    * @param ch Ҫд���uint8_t
    * @return ���ɹ�,��fail() == true
    */
  ByteStream& operator << (uint8_t ch);

  /**
    * д��һ��int16_t��
    * @param s Ҫд���int16_t
    * @return ���ɹ�,��fail() == true
    */
  ByteStream& operator << (int16_t s);

  /**
    * д��һ��uint16_t��
    * @param s Ҫд���uint16_t
    * @return ���ɹ�,��fail() == true
    */
  ByteStream& operator << (uint16_t s);

  /**
    * д��һ��int32_t��
    * @param l Ҫд���int32_t
    * @return ���ɹ�,��fail() == true
    */
  ByteStream& operator << (int32_t l);

  /**
    * д��һ��uint32_t��
    * @param l Ҫд���uint32_t
    * @return ���ɹ�,��fail() == true
    */
  ByteStream& operator << (uint32_t l);

  /**
    * д��һ��int32_t
    * @param l ��Ŷ�����int32_t
    * @return ���ɹ�,��fail() == true
    */
  const ByteStream& operator >> (bool& b) const;

  /**
    * ����һ��char
    * @param ch ��Ŷ�����char
    * @return ���ɹ�,��fail() == true
    */
  const ByteStream& operator >> (char& ch) const;

  /**
    * ����һ��uint8_t
    * @param ch ��Ŷ�����uint8_t
    * @return ���ɹ�,��fail() == true
    */
  const ByteStream& operator >> (uint8_t& ch) const;

  /**
    * ����һ��int16_t
    * @param s ��Ŷ�����int16_t
    * @return ���ɹ�,��fail() == true
    */
  const ByteStream& operator >> (int16_t& s) const;

  /**
    * ����һ��uint16_t
    * @param s ��Ŷ�����uint16_t
    * @return ���ɹ�,��fail() == true
    */
  const ByteStream& operator >> (uint16_t& s) const;

  /**
    * ����һ��int32_t
    * @param l ��Ŷ�����int32_t
    * @return ���ɹ�,��fail() == true
    */
  const ByteStream& operator >> (int32_t& l) const;

  /**
    * ����һ��int32_t
    * @param l ��Ŷ�����int32_t
    * @return ���ɹ�,��fail() == true
    */
  const ByteStream& operator >> (uint32_t& l) const;

  operator bool() const {
    return 0 == failed_;
  }
  bool operator!() const {
    return 0 != failed_;
  }


protected:
  /** ��ֹ����������� */
  ByteStream(const ByteStream& s);
  /** ��ֹ��ֵ���� */
  ByteStream& operator= (const ByteStream& s);

protected:
  virtual void* Alloc(uint32_t dwNewLen);
  virtual void  Free();

protected:
  char* buffer_; /**< ʵ�����ڶ�д�Ļ����� */
  bool is_new_; /**< buffer_�Ƿ����ö�̬����� */
  uint32_t max_len_; /**< ����������󳤶� */
  uint32_t data_len_; /**< �������е����ݳ��� */
  mutable uint32_t current_pos_; /**< �������ж�д�ĵ�ǰλ�� */
  mutable uint32_t failed_; /**< ָʾ��д�����Ƿ�ɹ� */
};

} // namespace net
#endif // NET_BASE_BYTE_STREAM_H_

