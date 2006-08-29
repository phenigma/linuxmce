#include "GaleryStateMachine.h"

GaleryStateMachine::GaleryStateMachine(void)
	: Status(STATUS_STARTUP)
{
}

GaleryStateMachine::~GaleryStateMachine(void)
{
}

int GaleryStateMachine::GetStatus(void)
{
	return Status;
}

void GaleryStateMachine::LoadedImages(void)
{
	Status = STATUS_LOADED_IMAGES;
}

void GaleryStateMachine::AnimateCurrentImage(void)
{
	Status = STATUS_ANIMATE_CURRENT_IMAGE;
}

void GaleryStateMachine::FadeImages(void)
{
	Status = STATUS_FADE_IMAGES;
}
