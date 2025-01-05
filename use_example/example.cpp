
#include "../log_master/bitlog.h"
#include <unistd.h>

void log_test(const std::string &name)
{
    log_master::Logger::ptr asynclogger = log_master::LoggerManager::getInstance().getLogger("ASYNCLOGGER");
    asynclogger->debug("%s--%d", "DEBUG", 666);
    asynclogger->info("%s--%d", "INFO", 666);
    asynclogger->warning("%s--%d", "WARNING", 666);
    asynclogger->error("%s--%d", "ERROR", 666);
    asynclogger->fatal("%s--%d", "FATAL", 666);

    size_t count = 0;
    while (count < 50000)
    {
        asynclogger->info("%s--%d", "INFO", count++);
        // usleep(500);
    }
}

int main()
{

    std::unique_ptr<log_master::LoggerBuilder> builder(new log_master::GlobalLoggerBuilder());
    builder->buildLoggerFormatter();//不输入参数或不调用则使用默认格式
    builder->buildLoggerLevel(log_master::Log_level::DEBUG);
    builder->buildLoggerName("ASYNCLOGGER");
    builder->buildLoggerSinks<log_master::StdoutLogSink>();
    builder->buildLoggerSinks<log_master::FileLogSink>("./test1/ASYNC.log");
    builder->buildLoggerSinks<log_master::RollByFileLogSink>("./test1/rollf-", 1024 * 1024);
    builder->buildLoggerSinks<log_master::RollByTimeLogSink>("./test1/rollt-", log_master::RollByTimeLogSink::GAP_SECOND);
    builder->buildLoggerType(log_master::LoggerType::LOGGER_ASYNC);

    log_master::Logger::ptr asynclogger = builder->build();

    log_test("ASYNCLOGGER");
    return 0;
}