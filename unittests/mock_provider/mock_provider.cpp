/* Copyright (c) 2007 Eric Scott Albright
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#include <glib.h>
#include <string>
#include "mock_provider.h"

ENCHANT_PLUGIN_DECLARE("mock")

static void
mock_provider_dispose(EnchantProvider *me)
{
    g_free(me);
}

static const char *
mock_provider_identify (EnchantProvider *)
{
	return "mock";
}

static const char *
mock_provider_describe (EnchantProvider *)
{
	return "Mock Provider";
}

static ConfigureHook _hook;


extern "C" {

ENCHANT_MODULE_EXPORT(void)
set_configure(ConfigureHook hook){
    _hook = hook;
}


ENCHANT_MODULE_EXPORT(EnchantProvider *) 
init_enchant_provider(void)
{
    bool hasIdentify = true;
    bool hasDescribe = true;
#if defined(_WIN32)
    std::wstring null_provider(L"null_provider.dll");
    std::wstring null_identify(L"null_identify.dll");
    std::wstring null_describe(L"null_describe.dll");
    WCHAR szFilename[MAX_PATH];
    DWORD cFilename = GetModuleFileName((HMODULE)s_hModule, (LPWSTR) &szFilename, sizeof(szFilename));
    if(cFilename > null_provider.size()){
        if(std::wstring(&szFilename[cFilename-null_provider.size()]) == null_provider){
            return NULL;
        }
    }
    if(cFilename > null_identify.size()){
        if(std::wstring(&szFilename[cFilename-null_identify.size()]) == null_identify){
            hasIdentify = false;
        }
    }
    if(cFilename > null_describe.size()){
        if(std::wstring(&szFilename[cFilename-null_describe.size()]) == null_describe){
            hasDescribe = false;
        }
    }

#endif

    EnchantProvider *provider;
	
    provider = g_new0(EnchantProvider, 1);
    provider->dispose = mock_provider_dispose; //although this is technically optional, it will result in a memory leak 
    provider->request_dict = NULL;
    provider->dispose_dict = NULL;
    provider->identify = hasIdentify ? mock_provider_identify : NULL; // this is required or module won't load
    provider->describe = hasDescribe ? mock_provider_describe : NULL; // this is required or module won't load
    provider->list_dicts = NULL;
    provider->dictionary_exists = NULL;
    provider->free_string_list = NULL;

    return provider;
}

ENCHANT_MODULE_EXPORT(void)
configure_enchant_provider(EnchantProvider * me, const char *dir_name)
{
    if(_hook){
        _hook(me, dir_name);
    }
}

}