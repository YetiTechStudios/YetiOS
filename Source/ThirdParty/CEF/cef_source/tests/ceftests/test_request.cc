// Copyright (c) 2020 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "tests/ceftests/test_request.h"

#include <algorithm>

#include "include/cef_stream.h"
#include "include/cef_urlrequest.h"
#include "include/wrapper/cef_closure_task.h"
#include "include/wrapper/cef_helpers.h"
#include "include/wrapper/cef_stream_resource_handler.h"

namespace test_request {

namespace {

// Implementation of CefURLRequestClient that stores response information.
class RequestClient : public CefURLRequestClient, public State {
 public:
  RequestClient(const bool has_credentials,
                const std::string& username,
                const std::string& password,
                const RequestDoneCallback& done_callback)
      : has_credentials_(has_credentials),
        username_(username),
        password_(password),
        done_callback_(done_callback) {
    DCHECK(!done_callback_.is_null());
  }

  void OnUploadProgress(CefRefPtr<CefURLRequest> request,
                        int64 current,
                        int64 total) override {
    upload_progress_ct_++;
    upload_total_ = total;
  }

  void OnDownloadProgress(CefRefPtr<CefURLRequest> request,
                          int64 current,
                          int64 total) override {
    response_ = request->GetResponse();
    DCHECK(response_.get());
    DCHECK(response_->IsReadOnly());
    download_progress_ct_++;
    download_total_ = total;
  }

  void OnDownloadData(CefRefPtr<CefURLRequest> request,
                      const void* data,
                      size_t data_length) override {
    response_ = request->GetResponse();
    DCHECK(response_.get());
    DCHECK(response_->IsReadOnly());
    download_data_ct_++;
    download_data_ += std::string(static_cast<const char*>(data), data_length);
  }

  bool GetAuthCredentials(bool isProxy,
                          const CefString& host,
                          int port,
                          const CefString& realm,
                          const CefString& scheme,
                          CefRefPtr<CefAuthCallback> callback) override {
    auth_credentials_ct_++;
    if (has_credentials_) {
      callback->Continue(username_, password_);
      return true;
    }
    return false;
  }

  void OnRequestComplete(CefRefPtr<CefURLRequest> request) override {
    request_complete_ct_++;

    request_ = request->GetRequest();
    DCHECK(request_->IsReadOnly());
    status_ = request->GetRequestStatus();
    error_code_ = request->GetRequestError();
    response_was_cached_ = request->ResponseWasCached();
    response_ = request->GetResponse();
    if (response_) {
      DCHECK(response_->IsReadOnly());
    }

    done_callback_.Run(*this);
  }

 private:
  const bool has_credentials_;
  const std::string username_;
  const std::string password_;
  const RequestDoneCallback done_callback_;

  IMPLEMENT_REFCOUNTING(RequestClient);
};

}  // namespace

void Send(const SendConfig& config, const RequestDoneCallback& callback) {
  DCHECK(config.request_);
  CefRefPtr<RequestClient> client = new RequestClient(
      config.has_credentials_, config.username_, config.password_, callback);
  if (config.frame_) {
    config.frame_->CreateURLRequest(config.request_, client.get());
  } else {
    CefURLRequest::Create(config.request_, client.get(),
                          config.request_context_);
  }
}

std::string GetPathURL(const std::string& url) {
  const size_t index1 = url.find('?');
  const size_t index2 = url.find('#');
  size_t index = std::string::npos;
  if (index1 != std::string::npos && index2 != std::string::npos) {
    index = std::min(index1, index2);
  } else if (index1 != std::string::npos) {
    index = index1;
  } else if (index2 != std::string::npos) {
    index = index2;
  }
  if (index != std::string::npos) {
    return url.substr(0, index);
  }
  return url;
}

CefRefPtr<CefResourceHandler> CreateResourceHandler(
    CefRefPtr<CefResponse> response,
    const std::string& response_data) {
  CefRefPtr<CefStreamReader> stream;
  if (!response_data.empty()) {
    stream = CefStreamReader::CreateForData(
        static_cast<void*>(const_cast<char*>(response_data.c_str())),
        response_data.length());
  }

  CefResponse::HeaderMap headerMap;
  response->GetHeaderMap(headerMap);

  return new CefStreamResourceHandler(
      response->GetStatus(), response->GetStatusText(), response->GetMimeType(),
      headerMap, stream);
}

}  // namespace test_request
