// Copyright (c) 2020 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.
//
// ---------------------------------------------------------------------------
//
// This file was generated by the CEF translator tool. If making changes by
// hand only do so within the body of existing method and function
// implementations. See the translator.README.txt file in the tools directory
// for more information.
//
// $hash=1e8cc9fc4bad94fa2d42eb1fef1e5445f59c1033$
//

#ifndef CEF_LIBCEF_DLL_CPPTOC_STREAM_WRITER_CPPTOC_H_
#define CEF_LIBCEF_DLL_CPPTOC_STREAM_WRITER_CPPTOC_H_
#pragma once

#if !defined(BUILDING_CEF_SHARED)
#error This file can be included DLL-side only
#endif

#include "include/capi/cef_stream_capi.h"
#include "include/cef_stream.h"
#include "libcef_dll/cpptoc/cpptoc_ref_counted.h"

// Wrap a C++ class with a C structure.
// This class may be instantiated and accessed DLL-side only.
class CefStreamWriterCppToC : public CefCppToCRefCounted<CefStreamWriterCppToC,
                                                         CefStreamWriter,
                                                         cef_stream_writer_t> {
 public:
  CefStreamWriterCppToC();
  virtual ~CefStreamWriterCppToC();
};

#endif  // CEF_LIBCEF_DLL_CPPTOC_STREAM_WRITER_CPPTOC_H_
