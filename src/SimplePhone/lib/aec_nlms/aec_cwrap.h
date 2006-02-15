
struct aec_nlms {
	void *data;
	void (*process)(struct aec_nlms *aec,short *in_out, short *ref, int samples);
	void (*setambientdb)(struct aec_nlms *aec, float db);
	void (*destroy)(struct aec_nlms *aec);
};

struct aec_nlms *aec_nlms_init(  );