// Copyright 2020 The Chromium Embedded Framework Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CEF_LIBCEF_COMMON_APP_MANAGER_H_
#define CEF_LIBCEF_COMMON_APP_MANAGER_H_
#pragma once

#include <list>

#include "include/cef_app.h"
#include "include/cef_request_context.h"

#include "base/macros.h"
#include "build/build_config.h"
#include "content/public/common/content_client.h"

namespace blink {
class WebURLLoaderFactory;
}

class CefBrowserContext;
struct CefSchemeInfo;

// Exposes global application state in the main and render processes.
class CefAppManager {
 public:
  // Returns the singleton instance that is scoped to CEF lifespan.
  static CefAppManager* Get();

  // The following methods are available in both processes.

  virtual CefRefPtr<CefApp> GetApplication() = 0;
  virtual content::ContentClient* GetContentClient() = 0;

  // Custom scheme information will be registered first with all processes
  // (url/url_util.h) via ContentClient::AddAdditionalSchemes which calls
  // AddCustomScheme, and second with Blink (SchemeRegistry) via
  // ContentRendererClient::WebKitInitialized which calls GetCustomSchemes.
  void AddCustomScheme(CefSchemeInfo* scheme_info);
  bool HasCustomScheme(const std::string& scheme_name);

  using SchemeInfoList = std::list<CefSchemeInfo>;
  const SchemeInfoList* GetCustomSchemes();

  // Called from ContentClient::AddAdditionalSchemes.
  void AddAdditionalSchemes(content::ContentClient::Schemes* schemes);

  // The following methods are only available in the main (browser) process.

  // Called from CefRequestContextImpl.
  virtual CefRefPtr<CefRequestContext> GetGlobalRequestContext() = 0;
  virtual CefBrowserContext* CreateNewBrowserContext(
      const CefRequestContextSettings& settings) = 0;

#if defined(OS_WIN)
  // Returns the module name (usually libcef.dll).
  const wchar_t* GetResourceDllName();
#endif

  // The following methods are only available in the render process.

  // Returns a factory that only supports unintercepted http(s) and blob
  // requests. Used by CefRenderURLRequest.
  blink::WebURLLoaderFactory* GetDefaultURLLoaderFactory();

 protected:
  CefAppManager();
  virtual ~CefAppManager();

 private:
  // Custom schemes handled by the client.
  SchemeInfoList scheme_info_list_;
  bool scheme_info_list_locked_ = false;

  std::unique_ptr<blink::WebURLLoaderFactory> default_url_loader_factory_;

  DISALLOW_COPY_AND_ASSIGN(CefAppManager);
};

#endif  // CEF_LIBCEF_COMMON_APP_MANAGER_H_
