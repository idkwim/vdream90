#include <VThread>
#include <VEventHandler>
#include <VDebugNew>

#ifdef _MSC_VER
#pragma pack(push, 1)
typedef struct
{
  DWORD recType;
  char* name;
  DWORD threadID;
  DWORD flags;
} ThreadNameInfo;
#pragma pack (pop)
#endif // _MSC_VER

static void _setCurrentThreadName(const char* name) // for debugging
{
#if _MSC_VER
  ThreadNameInfo info;
  info.recType  = 0x1000;
  info.name     = (char*)name;
  info.threadID = 0xFFFFFFFF;
  info.flags    = 0;
  __try
  {
    RaiseException(0x406D1388, 0, sizeof(info) / sizeof(DWORD), (ULONG_PTR*)(&info));
  }
  __except(1)
  {
  }
#else
  Q_UNUSED(name)
#endif // _MSC_VER
}

// ----------------------------------------------------------------------------
// VQThread
// ----------------------------------------------------------------------------
VQThread::VQThread(void* owner) : QThread(NULL)
//VQThread::VQThread(void* owner) : QThread((QObject*)owner) // gilgil temp 2012.11.27
{
  this->owner = owner;
  // ----- gilgil temp 2012.08.16 -----
  /*
  {
    QThread* qthread = dynamic_cast<QThread*>(this);
    LOG_DEBUG("qthread=%p", qthread);
  }
  */
  // ----------------------------------
}

VQThread::~VQThread()
{
  // ----- gilgil temp 2012.08.16 -----
  /*
  {
    QThread* qthread = dynamic_cast<QThread*>(this);
    LOG_DEBUG(" qthread=%p", qthread);
  }
  */
  // ----------------------------------
}

void VQThread::_run()
{
  VThread* thread = (VThread*)owner;
  TRY
  {
    // msleep(1000); // to occur virtual function call error // gilgil temp 2011.10.09
    thread->run();
  } EXCEPT
  {
    throw std::exception();
  }
}

void VQThread::run()
{
  VThread* thread = (VThread*)owner;

  char threadName[vd::DEFAULT_BUF_SIZE];
  char className [vd::DEFAULT_BUF_SIZE];

  try
  {
    if (thread->name == "") thread->name = thread->className();
    strcpy_s(threadName, vd::DEFAULT_BUF_SIZE, qPrintable(thread->name));
    strcpy_s(className,  vd::DEFAULT_BUF_SIZE, qPrintable(thread->className()));
    _setCurrentThreadName((char*)threadName);

    thread->m_id = VThread::currentID();

    try
    {
      _run();
    }
    catch (...)
    {
      LOG_FATAL("%p oops1 exception threadName=%s className=%s tag=%d", thread, threadName, className, thread->tag);
    }

    bool freeThread = thread->freeOnTerminate;
    if (freeThread)
    {
      // ----- gilgil temp 2012.08.15 -----
      //delete thread;
      VDeleteObjectEvent* event = new VDeleteObjectEvent(thread);
      VEventHandler::instance().postEvent(event);
      // msleep(1000); // gilgil temp 2012.12.10
    }
  }
  catch(...)
  {
    LOG_FATAL("%p oops2 exception threadName=%s className=%s tag=%d", thread, threadName, className, thread->tag);
  }
}

// ----------------------------------------------------------------------------
// VThread
// ----------------------------------------------------------------------------
// VThread::VThread(void* owner) : VObject(owner) // gilgil temp 2012.09.18
VThread::VThread(void* owner) : VObject(NULL)
{
  this->owner = owner;
  // ----- gilgil temp 2012.08.16 -----
  /*
  {
    VThread* thread = dynamic_cast<VThread*>(this);
    LOG_DEBUG("thread=%p", thread);
  }
  */
  // ----------------------------------
  m_id            = 0; // m_id is determined in VQThread::run()
  m_qthread       = NULL;
  freeOnTerminate = false;
  threadPriority  = QThread::NormalPriority;
  VThreadMgr& mgr = VThreadMgr::instance();
  mgr.lock();
  mgr.add(this);
  mgr.unlock();
}

VThread::~VThread()
{
  // ----- gilgil temp 2012.08.16 -----
  /*
  {
    VThread* thread = dynamic_cast<VThread*>(this);
    LOG_DEBUG("thread=%p", thread);
  }
  */
  // ----------------------------------
  close(true);
  SAFE_DELETE(m_qthread);
  VThreadMgr& mgr = VThreadMgr::instance();
  mgr.lock();
  mgr.del(this);
  mgr.unlock();
}

bool VThread::open()
{
  VLock lock(m_cs);
  return VObject::open();
}

bool VThread::close()
{
  return close(true);
}

bool VThread::close(bool wait, VTimeout timeout)
{
  VLock lock(m_cs);

  // LOG_DEBUG("tag=%d state=%d", tag, (int)m_state); // gilgil temp 2011.10.10
  if (m_state == VState::Closed) // if already closed
  {
    return true;
  }

  // Even though m_state is already changed into Closing by other caller,
  // if wait is true, waitFor must be called.
  // So close method include 2 state conditions(Opened, Closing).
  if (m_state != VState::Opened && m_state != VState::Closing)
  {
    return false;
  }

  m_state = VState::Closing;
  if (wait)
  {
    bool res = this->wait(timeout);
    // set m_state info Closed even if waitfor return false so as to not call waitFor any more.
    m_state = VState::Closed;
    return res;
  }
  return true;
}

bool VThread::wait(VTimeout timeout)
{
  VLock lock(m_cs);

  {
    Qt::HANDLE calling_id = currentID();
    Qt::HANDLE closing_id = this->id();
    if (calling_id == closing_id)
    {
      LOG_WARN("*******************************************************************");
      LOG_WARN("self wait? id=0x%08X tag=%d state=%d", calling_id, tag, (int)m_state);
      LOG_WARN("*******************************************************************");
    }
  }

  bool res = true;
  // LOG_DEBUG("%s m_qthread=%p", qPrintable(name), m_qthread); // gilgil temp 2012.05.29
  if (m_qthread)
  {
    res = m_qthread->wait(timeout);
    if (!res)
    {
      LOG_ERROR("*******************************************************************************");
      SET_ERROR(VThreadError, qformat("thread(%s) timeout id=0x%08X tag=%d", qPrintable(name), id(), tag), VERR_TIMEOUT);
      LOG_ERROR("*******************************************************************************");
      return false;
    }
    SAFE_DELETE(m_qthread);
  }
  // LOG_DEBUG("%s res=%d",  qPrintable(name), res); // gilgil temp 2012.05.29
  return res;
}

bool VThread::doOpen()
{
  if (m_qthread) delete m_qthread;
  m_qthread = new VQThread(this);

  VThreadMgr& mgr = VThreadMgr::instance();
  mgr.lock();
  if (!mgr.suspending())
    m_qthread->start(threadPriority);
  mgr.unlock();
  return true;
}

bool VThread::doClose()
{
  return wait();
}

void VThread::run()
{
  LOG_ERROR("****************************************************");
  LOG_ERROR("%s : virtual function call error.", qPrintable(name));
  LOG_ERROR("'virtual void run()' must be implemented in descendant class of VThread.");
  LOG_ERROR("if descendant class has destructor, close() must be called explicitly in its destructor");
  LOG_ERROR("****************************************************");
}

void VThread::load(VXml xml)
{
  VObject::load(xml);
  freeOnTerminate = xml.getBool("freeOnTerminate", freeOnTerminate);
  threadPriority  = (QThread::Priority) xml.getInt("threadPriority", (int)threadPriority);
}

void VThread::save(VXml xml)
{
  VObject::save(xml);
  xml.setBool("freeOnTerminate", freeOnTerminate);
  xml.setInt("threadPriority", (int)threadPriority);
}

// ----------------------------------------------------------------------------
// VThreadMgr
// ----------------------------------------------------------------------------
VThreadMgr::VThreadMgr()
{
  VLock _lock(*this);
  m_suspending = false;
  clear(false);
}

VThreadMgr::~VThreadMgr()
{
  VLock _lock(*this);

  int _count = threadList.count();
  if (_count != 0)
  {
    LOG_FATAL("thread list count is not zero(%d)", _count);
  }
  clear(true);
}

void VThreadMgr::clear(bool checkLeak)
{
  VLock _lock(*this);

  if (checkLeak)
  {
    int _count = threadList.count();
    if (_count != 0)
    {
      LOG_FATAL("thread list count is not zero(%d). Call threadList.clear() before call clear", _count);
    }
  }
  threadList.clear();
}

void VThreadMgr::add(VThread* thread)
{
  VLock _lock(*this);

  threadList.push_back(thread);
}

void VThreadMgr::del(VThread* thread)
{
  VLock _lock(*this);

  int i = threadList.indexOf(thread);
  if (i == -1)
  {
    LOG_ERROR("can not find thread(%p)", thread);
    return;
  }
  threadList.removeAt(i);
}

void VThreadMgr::suspend()
{
  VLock _lock(*this);

  m_suspending = true;
}

void VThreadMgr::resume()
{
  VLock _lock(*this);

  int _count = threadList.count();
  for (int i = 0; i < _count; i++)
  {
    VThread* thread = threadList.at(i);

    if (thread == NULL) continue;
    // if (thread->active()) continue; // gilgil temp 2012.08.25

    VQThread* qthread = thread->m_qthread;
    if (qthread == NULL) continue;
    if (qthread->isRunning()) continue;
    if (qthread->isFinished()) continue;

    thread->m_qthread->start(thread->threadPriority);
  }
  m_suspending = false;
}

// ----------------------------------------------------------------------------
// VSimpleThread
// ----------------------------------------------------------------------------
VSimpleThread::VSimpleThread(VThreadFunc threadFunc, void* p, VRunnable* runnable)
{
  m_threadFunc = threadFunc;
  m_p          = p;
  m_runnable   = runnable;
}

VSimpleThread::~VSimpleThread()
{
  close();
}

bool VSimpleThread::open()
{
  if (name == "")
  {
    if (m_runnable != NULL)
    {
      try
      {
        name = CLASS_NAME(*m_runnable) + "::run";
      }
      catch(...)
      {
        name = "SimpleThread";
      }
    }
    else
    {
      name = CLASS_NAME(*this);
    }
  }
  return VThread::open();
}

void VSimpleThread::run()
{
  m_threadFunc(m_p);
}
