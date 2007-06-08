#ifndef MasterMix_H
#define MasterMix_H

#include <alsa/asoundlib.h>

class MasterMix
{
	private:
		snd_mixer_t * m_MixerHandle;
		snd_mixer_elem_t * m_MixerElem;

		long m_VolMin, m_VolMax;
		bool m_bUsable;

		void GetMixerHandle();
		void GetMasterMixerElem();
		void GetVolumeLimits();
		void CloseMixerHandle();

	public:
		MasterMix();
		~MasterMix();

		long GetVolMin();
		long GetVolMax();
		long GetVolume();
		int GetVolumePercent();
		int IsOn();

		void SetVolume(long Volume);
		void SetVolumePercent(int Percent);
		void SetOn(int On);
};

#endif /*MasterMix_H*/
