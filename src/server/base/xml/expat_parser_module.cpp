/**
 * expat_parser_module.cpp
 * DESCRIPTION:
 *	�ο�CEGUI�Ĳ��ִ���
 *
 * Copyright (C) 2007, www.soucheng.com
 * By Wu Benqi<wubenqi@youcity.com>, 2008-12-16
 */

#include "expat_parser.h"
#include "expat_parser_module.h"

base::XMLParser* createParser(void) {
    return new base::ExpatParser();
}

void destroyParser(base::XMLParser* parser) {
    delete parser;
}
