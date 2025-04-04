#pragma once

#ifndef __AY8910_H__
#define __AY8910_H__

#include "devlegcy.h"

/*
AY-3-8910A: 2 I/O ports
AY-3-8912A: 1 I/O port
AY-3-8913A: 0 I/O port
AY-3-8914:  same as 8910 except for different register mapping and two bit envelope enable / volume field
AY8930: upper compatible with 8910.
In extended mode, it has higher resolution and duty ratio setting
YM2149: higher resolution, selectable clock divider
YM3439: same as 2149
YMZ284: 0 I/O port, different clock divider
YMZ294: 0 I/O port
*/

#define ALL_8910_CHANNELS -1

/* Internal resistance at Volume level 7. */

#define AY8910_INTERNAL_RESISTANCE  (356)
#define YM2149_INTERNAL_RESISTANCE  (353)

/*
 * Default values for resistor loads.
 * The macro should be used in AY8910interface if
 * the real values are unknown.
 */
#define AY8910_DEFAULT_LOADS        {1000, 1000, 1000}

/*
 * The following is used by all drivers not reviewed yet.
 * This will like the old behaviour, output between
 * 0 and 7FFF
 */
#define AY8910_LEGACY_OUTPUT        (0x01)

/*
 * Specifing the next define will simulate the special
 * cross channel mixing if outputs are tied together.
 * The driver will only provide one stream in this case.
 */
#define AY8910_SINGLE_OUTPUT        (0x02)

/*
 * The following define is the default behaviour.
 * Output level 0 is 0V and 7ffff corresponds to 5V.
 * Use this to specify that a discrete mixing stage
 * follows.
 */
#define AY8910_DISCRETE_OUTPUT      (0x04)

/*
 * The following define causes the driver to output
 * raw volume levels, i.e. 0 .. 15 and 0..31.
 * This is intended to be used in a subsequent
 * mixing modul (i.e. mpatrol ties 6 channels from
 * AY-3-8910 together). Do not use it now.
 */
/* TODO: implement mixing module */
#define AY8910_RAW_OUTPUT           (0x08)

/*
 * This define specifies the initial state of YM2149
 * pin 26 (SEL pin). By default it is set to high,
 * compatible with AY8910.
 */
/* TODO: make it controllable while it's running (used by any hw???) */
#define YM2149_PIN26_HIGH           (0x00) /* or N/C */
#define YM2149_PIN26_LOW            (0x10)


#define AY8910_INTERFACE(name) \
	const ay8910_interface (name) =

struct ay8910_interface
{
	int                 flags;          /* Flags */
	int                 res_load[3];    /* Load on channel in ohms */
	devcb_read8         portAread;
	devcb_read8         portBread;
	devcb_write8        portAwrite;
	devcb_write8        portBwrite;
};


void ay8910_set_volume(device_t *device,int channel,int volume);

DECLARE_READ8_DEVICE_HANDLER( ay8910_r );
DECLARE_WRITE8_DEVICE_HANDLER( ay8910_address_w );
DECLARE_WRITE8_DEVICE_HANDLER( ay8910_data_w );

/* /RES */
DECLARE_WRITE8_DEVICE_HANDLER( ay8910_reset_w );

/* use this when BC1 == A0; here, BC1=0 selects 'data' and BC1=1 selects 'latch address' */
DECLARE_WRITE8_DEVICE_HANDLER( ay8910_data_address_w );

/* use this when BC1 == !A0; here, BC1=0 selects 'latch address' and BC1=1 selects 'data' */
DECLARE_WRITE8_DEVICE_HANDLER( ay8910_address_data_w );

/* AY8914 handlers needed due to different register map */
DECLARE_READ8_DEVICE_HANDLER( ay8914_r );
DECLARE_WRITE8_DEVICE_HANDLER( ay8914_w );


/*********** An interface for SSG of YM2203 ***********/

void *ay8910_start_ym(void *infoptr, device_type chip_type, device_t *device, int clock, const ay8910_interface *intf);

void ay8910_stop_ym(void *chip);
void ay8910_reset_ym(void *chip);
void ay8910_set_clock_ym(void *chip, int clock);
void ay8910_write_ym(void *chip, int addr, int data);
int ay8910_read_ym(void *chip);

class ay8910_device : public device_t,
									public device_sound_interface
{
public:
	ay8910_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);
	ay8910_device(const machine_config &mconfig, device_type type, const char *name, const char *tag, device_t *owner, UINT32 clock);
	~ay8910_device() { global_free(m_token); }

	// access to legacy token
	void *token() const { assert(m_token != NULL); return m_token; }
protected:
	// device-level overrides
	virtual void device_config_complete();
	virtual void device_start();
	virtual void device_stop();
	virtual void device_reset();

	// sound stream update overrides
	virtual void sound_stream_update(sound_stream &stream, stream_sample_t **inputs, stream_sample_t **outputs, int samples);
private:
	// internal state
	void *m_token;
};

extern const device_type AY8910;

class ay8912_device : public ay8910_device
{
public:
	ay8912_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);

	// sound stream update overrides
	virtual void sound_stream_update(sound_stream &stream, stream_sample_t **inputs, stream_sample_t **outputs, int samples);
};

extern const device_type AY8912;

class ay8913_device : public ay8910_device
{
public:
	ay8913_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);

	// sound stream update overrides
	virtual void sound_stream_update(sound_stream &stream, stream_sample_t **inputs, stream_sample_t **outputs, int samples);
};

extern const device_type AY8913;

class ay8914_device : public ay8910_device
{
public:
	ay8914_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);

	// sound stream update overrides
	virtual void sound_stream_update(sound_stream &stream, stream_sample_t **inputs, stream_sample_t **outputs, int samples);
};

extern const device_type AY8914;

class ay8930_device : public ay8910_device
{
public:
	ay8930_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);

	// sound stream update overrides
	virtual void sound_stream_update(sound_stream &stream, stream_sample_t **inputs, stream_sample_t **outputs, int samples);
};

extern const device_type AY8930;

class ym2149_device : public ay8910_device
{
public:
	ym2149_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);
	ym2149_device(const machine_config &mconfig, device_type type, const char *name, const char *tag, device_t *owner, UINT32 clock);
protected:
	// device-level overrides
	virtual void device_start();

	// sound stream update overrides
	virtual void sound_stream_update(sound_stream &stream, stream_sample_t **inputs, stream_sample_t **outputs, int samples);
};

extern const device_type YM2149;

class ym3439_device : public ym2149_device
{
public:
	ym3439_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);

	// sound stream update overrides
	virtual void sound_stream_update(sound_stream &stream, stream_sample_t **inputs, stream_sample_t **outputs, int samples);
};

extern const device_type YM3439;

class ymz284_device : public ym2149_device
{
public:
	ymz284_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);

	// sound stream update overrides
	virtual void sound_stream_update(sound_stream &stream, stream_sample_t **inputs, stream_sample_t **outputs, int samples);
};

extern const device_type YMZ284;

class ymz294_device : public ym2149_device
{
public:
	ymz294_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);

	// sound stream update overrides
	virtual void sound_stream_update(sound_stream &stream, stream_sample_t **inputs, stream_sample_t **outputs, int samples);
};

extern const device_type YMZ294;


#endif /* __AY8910_H__ */
