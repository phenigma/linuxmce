#ifndef GaleryStateMachine_H_
#define GaleryStateMachine_H_

#define STATUS_STARTUP 0
#define STATUS_LOADED_IMAGES 1
#define STATUS_ANIMATE_CURRENT_IMAGE 2
#define STATUS_FADE_IMAGES 3

class GaleryStateMachine
{
	int Status;
public:
	GaleryStateMachine(void);
	~GaleryStateMachine(void);
	int GetStatus(void);
	void LoadedImages(void);
	void AnimateCurrentImage(void);
	void FadeImages(void);
};

#endif 
