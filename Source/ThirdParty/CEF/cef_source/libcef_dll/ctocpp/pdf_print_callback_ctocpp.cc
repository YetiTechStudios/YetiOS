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
// $hash=0b9a4766622f15290ae7328c08f1b28eb2d2275d$
//

#include "libcef_dll/ctocpp/pdf_print_callback_ctocpp.h"
#include "libcef_dll/shutdown_checker.h"

// VIRTUAL METHODS - Body may be edited by hand.

NO_SANITIZE("cfi-icall")
void CefPdfPrintCallbackCToCpp::OnPdfPrintFinished(const CefString& path,
                                                   bool ok) {
  shutdown_checker::AssertNotShutdown();

  cef_pdf_print_callback_t* _struct = GetStruct();
  if (CEF_MEMBER_MISSING(_struct, on_pdf_print_finished))
    return;

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: path; type: string_byref_const
  DCHECK(!path.empty());
  if (path.empty())
    return;

  // Execute
  _struct->on_pdf_print_finished(_struct, path.GetStruct(), ok);
}

// CONSTRUCTOR - Do not edit by hand.

CefPdfPrintCallbackCToCpp::CefPdfPrintCallbackCToCpp() {}

// DESTRUCTOR - Do not edit by hand.

CefPdfPrintCallbackCToCpp::~CefPdfPrintCallbackCToCpp() {
  shutdown_checker::AssertNotShutdown();
}

template <>
cef_pdf_print_callback_t* CefCToCppRefCounted<
    CefPdfPrintCallbackCToCpp,
    CefPdfPrintCallback,
    cef_pdf_print_callback_t>::UnwrapDerived(CefWrapperType type,
                                             CefPdfPrintCallback* c) {
  NOTREACHED() << "Unexpected class type: " << type;
  return nullptr;
}

template <>
CefWrapperType CefCToCppRefCounted<CefPdfPrintCallbackCToCpp,
                                   CefPdfPrintCallback,
                                   cef_pdf_print_callback_t>::kWrapperType =
    WT_PDF_PRINT_CALLBACK;
