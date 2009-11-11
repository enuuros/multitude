#include "CameraDriver.hpp"

#ifdef WIN32
# include <Radiant/VideoCameraCMU.hpp>
#else
# include <Radiant/VideoCamera1394.hpp>
#endif

namespace Radiant
{

 CameraDriverFactory::CameraDriverFactory()
 {}

 CameraDriverFactory::~CameraDriverFactory()
 {
   for(DriverMap::iterator it = m_drivers.begin(); it != m_drivers.end(); it++)
     delete it->second;
 }

 VideoCamera * CameraDriverFactory::createCamera(const std::string & driverName)
 {
   CameraDriver * driver = getCameraDriver(driverName);
   if(driver)
     return driver->createCamera();

   return 0;
 }

 VideoCamera * CameraDriverFactory::createPreferredCamera()
 {
   CameraDriver * cd = getPreferredCameraDriver();
   if(cd) return cd->createCamera();

   return 0;
 }

 CameraDriver * CameraDriverFactory::getCameraDriver(const std::string & driverName)
 {
   // If the user has not registered any drivers, we register the defaults here once
   static bool once = true;
   if(once && m_drivers.empty()) {
#ifdef WIN32
     registerDriver(new CameraDriverCMU());
#else
     registerDriver(new CameraDriver1394());
#endif
     once = false;
   }

   DriverMap::iterator it = m_drivers.find(driverName);
   if(it != m_drivers.end())
     return it->second;

   return 0;
 }

 CameraDriver * CameraDriverFactory::getPreferredCameraDriver()
 {
   // If no preferences are set, use defaults
   if(m_preferredDrivers.empty())
     setDriverPreference("libdc,cmu,ptgrey");

   std::vector<VideoCamera::CameraInfo> cameras;

   for(StringUtils::StringList::iterator it = m_preferredDrivers.begin(); it != m_preferredDrivers.end(); it++) {

     CameraDriver * cd = getCameraDriver(*it);
     if(cd) {
       // Make sure there is at least one camera available using this driver
       if(cd->queryCameras(cameras) > 0) return cd;
     }
   }

   return 0;
 }

 void CameraDriverFactory::registerDriver(CameraDriver * driver)
 {
   m_drivers.insert(std::make_pair(driver->driverName(), driver));
 }

 void CameraDriverFactory::setDriverPreference(const std::string & pref)
 {
   m_preferredDrivers.clear();
   StringUtils::split(pref, ",", m_preferredDrivers, true);
 }

}
