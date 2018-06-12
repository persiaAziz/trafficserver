#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#include <string>
#include <vector>
#include <sstream>

const std::string cDefaultConfig("ats_ssl_session_reuse.xml");
const unsigned int cPubNumWorkerThreads(100);
const int cDefaultRedisPort(6379);
const std::string cDefaultRedisHost("localhost");
const std::string cDefaultRedisEndpoint("localhost:6379");
const unsigned int cDefaultRedisConnectTimeout(1000000);
const unsigned int cDefaultRedisPublishTries(5);
const unsigned int cDefaultRedisConnectTries(5);
const unsigned int cDefaultRedisRetryDelay(5000000);
const unsigned int cDefaultMaxQueuedMessages(1000);
const unsigned int cSubNumWorkerThreadsPerEndpoint(5);
const std::string cDefaultSubColoChannel("test.*");
const unsigned int cDefaultRedisMessageWaitTime(100000);
const unsigned int cDefaultMdbmCleanupTime(100);
const unsigned int cDefaultMdbmMaxSize(65536);
const unsigned int cDefaultMdbmPageSize(2048);
const int SUCCESS(0);
const int FAILURE(-1);

#endif
