#include "file_utils.h"
#include "../../PlutoUtils/FileUtils.h"
#include "../../DCE/Logger.h"

#include <iostream>
using namespace std;
using namespace UpdateMediaFileUtils;

namespace DCE
{
	Logger *g_pPlutoLogger = NULL;
}

using namespace DCE;

int main(int argc, char *argv[])
{

	if(argc != 2)
	{
		cout << "Usage: read_symlink filename" << endl;
		return 1;
	}

	string sFilePath(argv[1]);
	if(IsSymlink(sFilePath))
		cout << "symlink points to: " << SymlinkSource(sFilePath) << endl;
	else
		cout << "not a symlink" << endl;
	

	cout << "fs device : " << MountedDeviceForFile(sFilePath) << endl;
	
	map<int, int> mapMountedDevices;
	list<string> listFiles;
	FileUtils::FindFiles(listFiles, "/mnt/device", "", false, true);

	MapMountedDevicesToPlutoDevices(listFiles, mapMountedDevices);

	cout << "pluto device: " << PlutoDeviceForFile(sFilePath, mapMountedDevices) << endl;

	return 0;
}
