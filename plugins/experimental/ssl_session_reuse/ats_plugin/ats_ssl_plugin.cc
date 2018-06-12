#include <stdio.h>
#include <ts/ts.h>
#include <ts/apidefs.h>
#include <openssl/ssl.h>

#include "ssl_utils.h"
int SSL_session_callback(TSCont contp, TSEvent event, void *edata);
void
TSPluginInit(int argc, const char *argv[])
{
  (void)argc;
  TSPluginRegistrationInfo info;

  info.plugin_name   = (char *)("ats_session_reuse");
  info.vendor_name   = (char *)("ats");
  info.support_email = (char *)("ats-devel@oath.com");

#if (TS_VERSION_NUMBER >= 7000000)
  if (TSPluginRegister(&info) != TS_SUCCESS) {
    TSError("Plugin registration failed.");
  }
#else
  if (TSPluginRegister(TS_SDK_VERSION_3_0, &info) != TS_SUCCESS) {
    TSError("Plugin registration failed.");
  }
#endif
  if (argc < 2) {
    TSError("Must specify config file");
  } else if (!init_ssl_params(argv[1])) {
    init_subscriber();
    TSCont cont = TSContCreate(SSL_session_callback, NULL);
    TSHttpHookAdd(TS_SSL_SESSION_HOOK, cont);
  } else {
    TSError("init_ssl_params failed.");
  }
}
