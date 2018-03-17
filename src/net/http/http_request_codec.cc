// Copyright (C) 2014 by wubenqi
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// By: wubenqi<wubenqi@gmail.com>
//

#include "net/http/http_request_codec.h"

#include "net/http/http_request_context.h"

namespace net {

int HttpRequestCodec::OnNewConnection(const IOHandlerPtr& ih) {
  ih->SetIOContext(new HttpRequestContext());
  delegate_->OnHttpConnection(ih);
  LOG(INFO) << "HttpRequestCodec::OnNewConnection()";
  return 0;
}

int HttpRequestCodec::OnDataReceived(const IOHandlerPtr& ih, IOBuffer* data,
  base::Time receive_time) {
  HttpRequestContext* ctx = reinterpret_cast<HttpRequestContext*>(ih->GetIOContext());
  int r = ctx->OnHttpDataParse(data);

  if (0 == r) {
    r = delegate_->OnHttpRequest(ih, ctx->http_request(), receive_time);
  }

  return r;
}

int HttpRequestCodec::OnConnectionClosed(const IOHandlerPtr& ih) {
  LOG(INFO) << "HttpRequestCodec::OnConnectionClosed()";
  delegate_->OnHttpConnectionClosed(ih);
  HttpRequestContext* ctx = reinterpret_cast<HttpRequestContext*>(ih->ReleaseIOContext());
  delete ctx;
  return 0;
}

void HttpRequestCodec::SendResponse(const IOHandlerPtr& ih, const HttpServerResponseInfo& response) {
  ih->SendInternal2(response.Serialize());
}

void HttpRequestCodec::Send404(const IOHandlerPtr& ih) {
  SendResponse(ih, HttpServerResponseInfo::CreateFor404());
}

void HttpRequestCodec::Send500(const IOHandlerPtr& ih, const std::string& message) {
  SendResponse(ih, HttpServerResponseInfo::CreateFor500(message));
}

}
