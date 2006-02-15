
extern "C" {
#include "iaxclient.h"
#include "aec_cwrap.h"
#include <math.h>
}
#include "aec.h"


static float dB2q(float dB)
{
  /* Dezibel to Ratio */
  return powf(10.0f, dB / 20.0f);
}

static float q2dB(float q)
{
  /* Ratio to Dezibel */
  return 20.0f * log10f(q);
}



static void destroy( struct aec_nlms *aec)
{
	if (aec)
	{
		delete ((AEC *)aec->data);
		free (aec);
	}
}

static void process( struct aec_nlms *aec, short *in_out, short *ref, int samples)
{
	AEC *myaec = ((AEC *)aec->data);
	int i;
	for(i=0; i<samples; i++ )
		*(in_out++) = myaec->doAEC( *in_out, *ref++ , true );
}



static void setambient( struct aec_nlms *aec, float db)
{
	((AEC *)aec->data)->setambient( 32768*dB2q(db));
}


struct aec_nlms *aec_nlms_init(  )
{
	struct aec_nlms *aec= NULL;

	aec = ( struct aec_nlms * )calloc(sizeof(aec_nlms),1);
	if (aec)
	{
		aec->data = new AEC();	
		if (aec->data == NULL)
		{
			free(aec);
			aec = NULL;
		}
		else
		{
			aec->process = process;
			aec->destroy = destroy;
			aec->setambientdb = setambient;
		}
	}
	return aec;
}