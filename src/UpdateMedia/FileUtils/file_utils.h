#ifndef __FILE_UTILS_H__
#define __FILE_UTILS_H__

#include <string>
#include <map>
#include <list>

namespace UpdateMediaFileUtils
{
	bool IsSymlink(std::string sFilePath);
	std::string SymlinkSource(std::string sFilePath);
	int MountedDeviceForFile(std::string sFilePath);
	int PlutoDeviceForFile(std::string sFilePath, const std::map<int, int>& mapMountedDevices);
	void MapMountedDevicesToPlutoDevices(const std::list<std::string>& listFiles, std::map<int, int>& mapMountedDevices);
}

#endif //__FILE_UTILS_H__
