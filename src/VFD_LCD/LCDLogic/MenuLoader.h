#ifndef __MENU_LOADER_H__
#define __MENU_LOADER_H__
//--------------------------------------------------------------------------------------------------------
class MenuHolder;
//--------------------------------------------------------------------------------------------------------
class MenuLoader 
{
public:

	MenuLoader();
	~MenuLoader();

	MenuHolder *Load(string sFilename);
	MenuHolder *Load(char *pData, size_t nSize);
};
//--------------------------------------------------------------------------------------------------------
#endif //__MENU_LOADER_H__
