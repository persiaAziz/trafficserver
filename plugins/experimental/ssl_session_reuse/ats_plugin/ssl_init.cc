#include <string>
#include "ssl_utils.h"
#include "config.h"
#include "common.h"

ssl_session_param ssl_param; // <- global containing all operational info
std::string conf_file;

int
init_subscriber()
{
  ssl_param.sub = new RedisSubscriber(conf_file);
  if ((!ssl_param.sub) || (!ssl_param.sub->is_good())) {
    TSError("Construct RedisSubscriber error.");
    return -1;
  }
  return 0;
}

int
init_ssl_params(const std::string &conf)
{
  conf_file = conf;
  if (Config::getSingleton().loadConfig(conf)) {
    Config::getSingleton().getValue("ssl_session", "AppName", ssl_param.app_name);
    Config::getSingleton().getValue("ssl_session", "ClusterName", ssl_param.cluster_name);
    Config::getSingleton().getValue("ssl_session", "KeyType", ssl_param.key_type);
    Config::getSingleton().getValue("ssl_session", "KeyVersion", ssl_param.key_version);
    Config::getSingleton().getValue("ssl_session", "KeyUpdateInterval", ssl_param.key_update_interval);
    Config::getSingleton().getValue("ssl_session", "EnableSessionIdCache", ssl_param.enable_sessionId_cache);
    Config::getSingleton().getValue("ssl_session", "SessionLifetime", ssl_param.session_lifetime);
    Config::getSingleton().getValue("ssl_session", "STEKMaster", ssl_param.stek_master);
    Config::getSingleton().getValue("ssl_session", "redis_auth_key_file", ssl_param.redis_auth_key_file);
  } else {
    return -1;
  }

  if (ssl_param.key_update_interval > STEK_MAX_LIFETIME) {
    ssl_param.key_update_interval = STEK_MAX_LIFETIME;
    TSDebug(PLUGIN, "KeyUpdateInterval too high, resetting session ticket key rotation to %d seconds",
            (int)ssl_param.key_update_interval);
  }

  TSDebug(PLUGIN, "init_ssl_params: I %s been configured to initially be stek_master",
          ((ssl_param.stek_master) ? "HAVE" : "HAVE NOT"));
  TSDebug(PLUGIN, "init_ssl_params: Rotation interval (ssl_param.key_update_interval)set to %d\n", ssl_param.key_update_interval);
  TSDebug(PLUGIN, "init_ssl_params: cluster_name set to %s", ssl_param.cluster_name.c_str());

  int ret = STEK_init_keys();
  if (ret < 0) {
    TSError("init keys failure. %s", conf.c_str());
    return -1;
  }

  if (!ssl_param.enable_sessionId_cache) {
    return 0;
  }

  ssl_param.pub = new RedisPublisher(conf);
  if ((!ssl_param.pub) || (!ssl_param.pub->is_good())) {
    TSError("Construct RedisPublisher error.");
    return -1;
  }
  return 0;
}

ssl_session_param::ssl_session_param() : enable_sessionId_cache(1), pub(NULL) {}

ssl_session_param::~ssl_session_param()
{
  // Let the publish object leak for now, we are shutting down anyway
  /*if (pub)
      delete pub; */
}
