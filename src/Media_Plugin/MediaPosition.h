#ifndef Media_Position_h
#define Media_Position_h

namespace DCE
{
	// When a media device can save and restore a position, it must create a derived class that stores the position information.
	// For a CD, this may be a track and timecount.  For a DVD it is more complicated, involving a lot of settings, like sub-title and audio, needed to restore this position/state
	// GetID must return a unique string identifying the format.  This way media devices can see the format based on GetID, and cast MediaPosition to the correct type, handling it accordingly
	class MediaPosition
	{
	public:
		/**
		* @brief A Media position should be able to be reset.
		*/
		virtual void Reset() = 0;

		/**
		* @brief A media position should be identified.
		*/
		virtual string GetID() = 0;
	};
}

#endif
