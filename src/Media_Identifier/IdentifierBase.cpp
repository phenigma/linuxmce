/**
 * IdentifierBase - the base class for media identification
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com> - class structure
 * Author: phenigma <phenigma@hotmail.com> - functionality
 */

#include "IdentifierBase.h"
#include "PlutoUtils/FileUtils.h"
#include "DCE/Logger.h"

#include <sys/types.h>
#include <cdio/cdio.h>

using namespace std;
using namespace DCE;

/**
 * ctor - pass in file or device path to identify.
 */
IdentifierBase::IdentifierBase(string sPath)
{
  m_sPath = sPath;
}

/**
 * dtor
 */
IdentifierBase::~IdentifierBase()
{
}

bool IdentifierBase::Init()
{
  return true;
}

bool IdentifierBase::Identify()
{
  if (!FileUtils::FileExists(m_sPath))
    {
      LoggerWrapper::GetInstance()->Write(LV_WARNING,"IdentifierBase::Identify() - path %s does not exist. Bailing.",m_sPath.c_str());
      return false;
    }
  return true; // Only false if our sanity checks here fail.
}

string IdentifierBase::GetIdentifiedData()
{
  return "";
}

IdentifierBase::eIdentityType IdentifierBase::GetIdentityType()
{
  return NONE;
}
