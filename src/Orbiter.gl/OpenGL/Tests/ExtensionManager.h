#ifndef ExtensionManager_H_
#define ExtensionManager_H_

class ExtensionManager
{
	float CleanRed, CleanGreen, CleanBlue;
public:
	ExtensionManager(void);
	virtual ~ExtensionManager(void);
	
	virtual void Flip();
	virtual void SetClearColor(float Red, float Green, float Blue);
	virtual void ClearScreen(bool CleanColor = true, bool CleanZBuffer = true);
	virtual void EnableZBuffer(bool Enable);
	
};


#endif
