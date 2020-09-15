// Copyright (c) 2015 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef CEF_LIBCEF_BROWSER_EXTENSIONS_BROWSER_EXTENSIONS_UTIL_H_
#define CEF_LIBCEF_BROWSER_EXTENSIONS_BROWSER_EXTENSIONS_UTIL_H_

#include <vector>

#include "libcef/browser/browser_host_impl.h"

#include "url/gurl.h"

namespace content {
class BrowserContext;
class RenderViewHost;
class WebContents;
}  // namespace content

namespace extensions {

class Extension;

// Returns the full-page guest WebContents for the specified |owner|, if any.
content::WebContents* GetFullPageGuestForOwnerContents(
    content::WebContents* owner);

// Populates |guests| with all guest WebContents with the specified |owner|.
void GetAllGuestsForOwnerContents(content::WebContents* owner,
                                  std::vector<content::WebContents*>* guests);

// Returns the WebContents that owns the specified |guest|, if any.
content::WebContents* GetOwnerForGuestContents(content::WebContents* guest);

// Returns the CefBrowserHostImpl that owns the host identified by the specified
// routing IDs, if any. |is_guest_view| will be set to true if the IDs
// match a guest view associated with the returned browser instead of the
// browser itself.
CefRefPtr<CefBrowserHostImpl> GetOwnerBrowserForFrameRoute(
    int render_process_id,
    int render_routing_id,
    bool* is_guest_view);

// Returns the CefBrowserHostImpl that owns the specified |host|, if any.
// |is_guest_view| will be set to true if the host matches a guest view
// associated with the returned browser instead of the browser itself.
// TODO(cef): Delete the RVH variant once the remaining use case
// (via AlloyContentBrowserClient::OverrideWebkitPrefs) has been removed.
CefRefPtr<CefBrowserHostImpl> GetOwnerBrowserForHost(
    content::RenderViewHost* host,
    bool* is_guest_view);
CefRefPtr<CefBrowserHostImpl> GetOwnerBrowserForHost(
    content::RenderFrameHost* host,
    bool* is_guest_view);

// Returns the browser matching |tab_id| and |browser_context|. Returns false if
// |tab_id| is < 0 or a matching browser cannot be found within
// |browser_context|. Similar in concept to ExtensionTabUtil::GetTabById.
CefRefPtr<CefBrowserHostImpl> GetBrowserForTabId(
    int tab_id,
    content::BrowserContext* browser_context);

// Returns the extension associated with |url| in |profile|. Returns nullptr
// if the extension does not exist.
const Extension* GetExtensionForUrl(content::BrowserContext* browser_context,
                                    const GURL& url);

}  // namespace extensions

#endif  // CEF_LIBCEF_BROWSER_EXTENSIONS_BROWSER_EXTENSIONS_UTIL_H_
