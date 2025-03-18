#include <scwx/qt/manager/thread_manager.hpp>
#include <scwx/util/logger.hpp>

#include <execution>
#include <mutex>

#include <boost/unordered/unordered_flat_map.hpp>
#include <QThread>

namespace scwx
{
namespace qt
{
namespace manager
{

static const std::string logPrefix_ = "scwx::qt::manager::thread_manager";
static const auto        logger_    = scwx::util::Logger::Create(logPrefix_);

class ThreadManager::Impl
{
public:
   explicit Impl() {}
   ~Impl() {}

   std::mutex mutex_ {};

   boost::unordered_flat_map<std::string, QThread*> threadMap_ {};
};

ThreadManager::ThreadManager() : p(std::make_unique<Impl>()) {}
ThreadManager::~ThreadManager() = default;

QThread* ThreadManager::thread(const std::string& id, bool autoStart)
{
   std::unique_lock lock {p->mutex_};
   QThread*         thread = nullptr;

   auto it = p->threadMap_.find(id);
   if (it != p->threadMap_.cend())
   {
      thread = it->second;
   }

   if (thread == nullptr)
   {
      logger_->debug("Creating thread: {}", id);

      thread = new QThread(this);
      p->threadMap_.insert_or_assign(id, thread);

      if (autoStart)
      {
         thread->start();
      }
   }

   return thread;
}

void ThreadManager::StopThreads()
{
   std::unique_lock lock {p->mutex_};

   logger_->debug("Stopping threads");

   std::for_each(std::execution::par_unseq,
                 p->threadMap_.begin(),
                 p->threadMap_.end(),
                 [](auto& thread)
                 {
                    logger_->trace("Stopping thread: {}", thread.first);

                    thread.second->quit();
                    if (!thread.second->wait(5000))
                    {
                       logger_->warn("Terminating thread: {}", thread.first);

                       thread.second->terminate();
                       thread.second->wait();
                    }

                    delete thread.second;
                 });

   p->threadMap_.clear();
}

ThreadManager& ThreadManager::Instance()
{
   static ThreadManager threadManager_ {};
   return threadManager_;
}

} // namespace manager
} // namespace qt
} // namespace scwx
