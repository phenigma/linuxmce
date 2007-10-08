#include "libresolution.h"

#include <confuse.h>

// class ResolutionConf

cfg_opt_t ResolutionConf::cfgWorkaround[] =
{
	CFG_STR_LIST("driver", "{}", CFGF_NONE),
	CFG_STR("x11", "", CFGF_NONE),
	CFG_END()
};

cfg_opt_t ResolutionConf::cfgModeline[] =
{
	CFG_INT("refresh", 60, CFGF_NONE),
	CFG_BOOL("builtin", cfg_false, CFGF_NONE),
	CFG_SEC("workaround", ResolutionConf::cfgWorkaround, CFGF_MULTI),
	CFG_STR("x11", "", CFGF_NONE),
	CFG_END()
};

cfg_opt_t ResolutionConf::cfgResolution[] =
{
	CFG_STR("name", "-unspecified-", CFGF_NONE),
	CFG_INT("width", 0, CFGF_NONE),
	CFG_INT("height", 0, CFGF_NONE),
	CFG_STR("aspect_ratio", 0, CFGF_NONE),
	CFG_SEC("modeline", ResolutionConf::cfgModeline, CFGF_MULTI),
	CFG_BOOL("visible", cfg_false, CFGF_NONE),
	CFG_END()
};

cfg_opt_t ResolutionConf::cfgTop[] =
{
	CFG_SEC("resolution", ResolutionConf::cfgResolution, CFGF_MULTI),
	CFG_END()
};

ResolutionConf::ResolutionConf(string ConfigFilename)
{
	cfg = cfg_init(cfgTop, CFGF_NONE);
	cfg_parse(cfg, ConfigFilename.c_str());
}

ResolutionConf::~ResolutionConf()
{
	cfg_free(cfg);
}
