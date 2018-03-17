/**
* xml_parser.h
* DESCRIPTION:
*	�ο�CEGUI�Ĳ��ִ���
*
* Copyright (C) 2007, www.soucheng.com
* By Wu Benqi<wubenqi@youcity.com>, 2008-12-16
*/


#ifndef SERVER_BASE_XML_XML_PARSER_H_
#define SERVER_BASE_XML_XML_PARSER_H_
#pragma once

#include <string>

namespace server {

class XMLHandler;
class XMLParser {
public:
  enum XmlParserType {
    EXPAT_XMLPARSER = 0,
    TINYXML_XMLPARSER = 1,
    MARKUPSTL_XMLPARSER = 2,
    RAPIDXML_XMLPARSER = 3
  };

  XMLParser(void);
  virtual ~XMLParser(void);

  bool Initialise(void);
  void Cleanup(void);
  virtual bool ParseXMLData(XMLHandler& handler, const std::string &xml_text) = 0;
  virtual bool ParseXMLData(XMLHandler& handler, const char* xml_text, size_t xml_text_size) = 0;
  virtual bool ParseXMLFile(XMLHandler& handler, const std::string& filename) = 0;
  const std::string& GetIdentifierString() const;

protected:
  virtual bool InitialiseImpl(void) = 0;
  virtual void CleanupImpl(void) = 0;
  std::string d_identifierString;

private:
  bool d_initialised;     //!< true if the parser module has been initialised,
};

}
#endif
