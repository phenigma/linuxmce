#ifndef __PENDING_GL_EFFECTS_H__
#define __PENDING_GL_EFFECTS_H__

struct PendingGLEffects //inline
{
	int m_nOnScreenTransitionEffectID;
	int m_nOffScreenTransitionEffectID;
	int m_nOnSelectWithChangeEffectID;

	PendingGLEffects() 
	{
		Reset();
	}

	void Reset()
	{
		m_nOnScreenTransitionEffectID = 0;
		m_nOffScreenTransitionEffectID = 0;
		m_nOnSelectWithChangeEffectID = 0;
	}
};

#endif
