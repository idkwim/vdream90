#include <VTcpClient>
#include <VDebugNew>

REGISTER_METACLASS(VTcpClient, VNet)

// ----------------------------------------------------------------------------
// VTcpClient
// ----------------------------------------------------------------------------
VTcpClient::VTcpClient(void* owner) : VNetClient(owner)
{
  tcpSession = new VTcpSession(this);
  m_ip       = 0;
  host       = "";
  port       = 0;
  localHost  = "";
  localPort  = 0;
}

VTcpClient::~VTcpClient()
{
  close();
  SAFE_DELETE(tcpSession);
}

bool VTcpClient::doOpen()
{
  if (port == 0)
  {
    SET_ERROR(VNetError, "port is zero", VERR_PORT_IS_ZERO);
    return false;
  }

  // ------------------------------------------------------------------------
  // socket
  // ------------------------------------------------------------------------
  tcpSession->handle = socket(AF_INET, SOCK_STREAM, 0);
  if (tcpSession->handle == INVALID_SOCKET)
  {
    SET_ERROR(VSocketError, "error in socket", WSAGetLastError());
    return false;
  }

  // ------------------------------------------------------------------------
  // bind
  // ------------------------------------------------------------------------
  tcpSession->addr.sin_family = AF_INET;
  tcpSession->addr.sin_port = htons(quint16(localPort));
  if (localHost == 0)
  {
    tcpSession->addr.sin_addr.s_addr = INADDR_ANY;
  } else
  {
    Ip ip = VNet::resolve(localHost);
    if (ip == 0)
    {
      SET_ERROR(VNetError, qformat("can not resolve host(%s)", qPrintable(localHost)), VERR_CAN_NOT_RESOLVE_HOST);
      return false;
    }
    tcpSession->addr.sin_addr.s_addr = htonl(ip);
  }
  memset(&tcpSession->addr.sin_zero, 0, sizeof(tcpSession->addr.sin_zero));

  int res = bind(tcpSession->handle, (SOCKADDR*)&tcpSession->addr, sizeof(tcpSession->addr));
  if (res == SOCKET_ERROR)
  {
    SET_ERROR(VSocketError, qformat("error in bind(%s:%d)", qPrintable(localHost), localPort), WSAGetLastError());
    return false;
  }

  // ------------------------------------------------------------------------
  // connect
  // ------------------------------------------------------------------------
  tcpSession->addr.sin_family = AF_INET;
  tcpSession->addr.sin_port = htons(quint16(port));
  if (host == "")
  {
    SET_ERROR(VNetError, "host is invalid", VERR_INVALID_HOST);
    return false;
  }
  m_ip = VNet::resolve(host);
  if (m_ip == 0)
  {
    SET_ERROR(VNetError, qformat("can not resolve host(%s)", qPrintable(host)), VERR_CAN_NOT_RESOLVE_HOST);
    return false;
  }
  tcpSession->addr.sin_addr.s_addr = htonl(m_ip);
  memset(tcpSession->addr.sin_zero, 0, sizeof(tcpSession->addr.sin_zero));
  res = ::connect(tcpSession->handle, (SOCKADDR*)&tcpSession->addr, sizeof(tcpSession->addr));
  if (res == SOCKET_ERROR)
  {
    SET_ERROR(VSocketError, qformat("error in connect(%s:%d)", qPrintable(host), port), WSAGetLastError());
    return false;
  }

  // ------------------------------------------------------------------------
  // ok
  // ------------------------------------------------------------------------
  tcpSession->onceWriteSize = this->onceWriteSize;
  if (!tcpSession->open()) return false;

  return true;
}

bool VTcpClient::doClose()
{
  tcpSession->close();
  return true;
}

int  VTcpClient::doRead(char* buf, int size)
{
  int res = tcpSession->read(buf, size);
  if (res == VERR_FAIL)
    error = tcpSession->error;
  return res;
}

int  VTcpClient::doWrite(char* buf, int size)
{
  int res = tcpSession->write(buf, size);
  if (res == VERR_FAIL)
    error = tcpSession->error;
  return res;
}

void VTcpClient::load(VXml xml)
{
  VNetClient::load(xml);
  host = xml.getStr("host", host);
  port = xml.getInt("port", port);
  localHost = xml.getStr("localHost", localHost);
  localPort = xml.getInt("localPort", localPort);
}

void VTcpClient::save(VXml xml)
{
  VNetClient::save(xml);
  xml.setStr("host", host);
  xml.setInt("port", port);
  if (localHost != "") xml.setStr("localHost", localHost);
  if (localPort != 0)  xml.setInt("localPort", localPort);
}

#ifdef QT_GUI_LIB
void VTcpClient::addOptionWidget(QLayout* layout)
{
  VNetClient::addOptionWidget(layout);
  VShowOption::addLineEdit(layout, "leHost",      "Host",       host);
  VShowOption::addLineEdit(layout, "lePort",      "Port",       QString::number(port));
  VShowOption::addLineEdit(layout, "leLocalHost", "Local Host", localHost);
  VShowOption::addLineEdit(layout, "leLocalPort", "Local Port", QString::number(localPort));
}

void VTcpClient::saveOption(QDialog* dialog)
{
  VNetClient::saveOption(dialog);
  host      = dialog->findChild<QLineEdit*>("leHost")->text();
  port      = dialog->findChild<QLineEdit*>("lePort")->text().toInt();
  localHost = dialog->findChild<QLineEdit*>("leLocalHost")->text();
  localPort = dialog->findChild<QLineEdit*>("leLocalPort")->text().toInt();
}
#endif // QT_GUI_LIB
