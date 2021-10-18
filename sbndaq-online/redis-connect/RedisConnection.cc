#include <fstream>

#include "RedisConnection.h"

#include "messagefacility/MessageLogger/MessageLogger.h"

#include "../helpers/Utilities.h"

#include "../RedisTrace.h"


sbndaq::RedisConnection::RedisConnection(const fhicl::ParameterSet& pset) {
  fRedisPort = pset.get<unsigned>("port", 6379);
  fRedisHost = pset.get<std::string>("host", "localhost");
  // don't buffer by default (1). Set to (-1) to rely on calls to Flush().
  fMessageBufferSize = pset.get<int>("message_buffer_size", 1); 
  fNMessages = 0;

  fFailedConnection = false;
  
  fRedisPassword = pset.get<std::string>("password", "");
  if (fRedisPassword.size() == 0) {
    std::string redis_password_file = pset.get<std::string>("passfile", "");
    if (redis_password_file.size() > 0) {
      std::ifstream passfile(redis_password_file);
      if (passfile.good()) {
        passfile >> fRedisPassword;
      }
      else {
        mf::LogError("Redis Connection Service") << "Failed to topen password file";
        TLOG(REDIS_TRACE_LEVEL_ERR) << "Failed to open password file";
      }
    }
  }

  void *password_reply = NULL;
  fRedisContext = Connect2Redis(fRedisHost, fRedisPort, fRedisPassword, &password_reply);

  if (fRedisContext == NULL || fRedisContext->err) {
    if (fRedisContext) {
      mf::LogError("Redis Connection") << "Redis connection error reply: " << fRedisContext->errstr;
      TLOG(REDIS_TRACE_LEVEL_ERR) << "Redis connection error reply: " << fRedisContext->errstr;
    }
    else {
      mf::LogError("Redis Connection") << "Cannot allocate redis context.";
      TLOG(REDIS_TRACE_LEVEL_ERR) << "Cannot allocate redis context.";
    }
    fFailedConnection = true;
  }

  // check password setup if neccessary
  if (!fFailedConnection && fRedisPassword.size() > 0) {
    bool success = ProcessRedisReply(password_reply);
    if (!success) {
      fFailedConnection = true;
      TLOG(REDIS_TRACE_LEVEL_ERR) << "Redis authentication failed";
      mf::LogError("Redis Connection") << "Redis connection failed";
    }
    else {
      TLOG(REDIS_TRACE_LEVEL_MSG) << "Redis authentication succeeded";
      mf::LogInfo("Redis Connection") << "Redis authentication succeeded";
    }
  }
}

sbndaq::RedisConnection::~RedisConnection() {
  if (fRedisContext != NULL) {
    redisFree(fRedisContext);
  }
}

// TODO: implement
void sbndaq::RedisConnection::ProcessRedisReturn(int ) {}
// TODO: implement
bool sbndaq::RedisConnection::ProcessRedisReply(void *r) {
  if (r == NULL) {
    mf::LogError("Redis Connection") << "Redis connection NULL reply";
    TLOG(REDIS_TRACE_LEVEL_ERR) << "Redis connection NULL reply";
    return false;
  }

  bool success = true;
  redisReply *reply = (redisReply *)r;
  switch (reply->type) {
    case REDIS_REPLY_ERROR:
      mf::LogError("Redis Connection") << "Error reply status: " << reply->str;
      TLOG(REDIS_TRACE_LEVEL_ERR) << "Error reply status: " << reply->str;
      success = false;
      break;
    case REDIS_REPLY_STATUS:
      mf::LogDebug("Redis Connection") << "Message reply status: " << reply->str;
      TLOG(REDIS_TRACE_LEVEL_MSG) << "Message reply status: " << reply->str;
      break;
    case REDIS_REPLY_NIL:
      mf::LogDebug("Redis Connection") << "Nil reply";
      TLOG(REDIS_TRACE_LEVEL_MSG) << "Nil reply";
      break;
    case REDIS_REPLY_INTEGER:
      mf::LogDebug("Redis Connection") << "Message reply integer: " << reply->integer;
      TLOG(REDIS_TRACE_LEVEL_MSG) << "Message reply integer: " << reply->integer;
      break;
    case REDIS_REPLY_STRING:
      mf::LogDebug("Redis Connection") << "Message reply string: " << reply->str;
      TLOG(REDIS_TRACE_LEVEL_MSG) << "Message reply string: " << reply->str;
      break;
    case REDIS_REPLY_ARRAY:
      mf::LogDebug("Redis Connection") << "Message reply array";
      TLOG(REDIS_TRACE_LEVEL_MSG) << "Message reply array";
      break;
    default:
      mf::LogError("Redis Connection") << "Bad Redis reply type: " << reply->type;
      TLOG(REDIS_TRACE_LEVEL_ERR) << "Bad Redis reply type: " << reply->type;
      success = false;
      break;
  }
  freeReplyObject(reply);
  return success;
}
  
// NOTE: this command assumes that you have acquired the "fRedisLock" mutex
void sbndaq::RedisConnection::NewMessage() {
  fNMessages += 1;

  if (fMessageBufferSize >= 0 && fNMessages >= (unsigned)fMessageBufferSize) {
    DoFlush();
  }
}

void sbndaq::RedisConnection::Flush() {
  std::lock_guard<std::mutex> guard(fRedisLock);
  DoFlush();
}

// NOTE: this command assumes that you have acquired the "fRedisLock" mutex
void sbndaq::RedisConnection::DoFlush() {
  while (fNMessages != 0) {
    void *reply = NULL;
    redisGetReply(fRedisContext, &reply);
    bool success = ProcessRedisReply(reply);
    // TODO: what to do on failure?
    (void) success;
    fNMessages -=1;
  }
}

bool sbndaq::RedisConnection::CheckConnection() {
  if (fFailedConnection) {
    mf::LogWarning("Redis Connection") << "Attempting to send metric when connection failed.";
    TLOG(REDIS_TRACE_LEVEL_ERR) << "Attempting to send metric when connection failed.";
  }
  return !fFailedConnection;
}

void sbndaq::RedisConnection::Command(const char *fmt, ...) {
  if (!CheckConnection()) return;

  std::lock_guard<std::mutex> guard(fRedisLock);

  va_list argp;
  va_start(argp, fmt);
  redisvAppendCommand(fRedisContext, fmt, argp);
  va_end(argp);
  NewMessage();
}

void sbndaq::RedisConnection::CommandArgv(int argc, const char **argv, const size_t *argvlen) {
  if (!CheckConnection()) return;

  std::lock_guard<std::mutex> guard(fRedisLock);

  redisAppendCommandArgv(fRedisContext, argc, argv, argvlen);
  NewMessage();
}


