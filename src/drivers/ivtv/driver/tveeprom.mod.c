#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

#undef unix
struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
 .name = __stringify(KBUILD_MODNAME),
 .init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
 .exit = cleanup_module,
#endif
};

static const struct modversion_info ____versions[]
__attribute_used__
__attribute__((section("__versions"))) = {
	{ 0x8bf65fd8, "struct_module" },
	{ 0x7419190e, "i2c_del_driver" },
	{ 0x9c774762, "i2c_add_driver" },
	{ 0x57a37c89, "i2c_detach_client" },
	{ 0xdb0d9d84, "i2c_probe" },
	{ 0x37a0cba, "kfree" },
	{ 0x2f6be3c3, "i2c_attach_client" },
	{ 0x25da070, "snprintf" },
	{ 0x6a6f0027, "kmem_cache_alloc" },
	{ 0x3de88ff0, "malloc_sizes" },
	{ 0x48ea0584, "i2c_check_functionality" },
	{ 0x806d5133, "param_array_get" },
	{ 0x89cef6fb, "param_array_set" },
	{ 0x6483655c, "param_get_short" },
	{ 0x40046949, "param_set_short" },
	{ 0xead05966, "i2c_master_recv" },
	{ 0xbce2236f, "i2c_master_send" },
	{ 0x1b7d4074, "printk" },
	{ 0x89b301d4, "param_get_int" },
	{ 0x98bd6f46, "param_set_int" },
};

static const char __module_depends[]
__attribute_used__
__attribute__((section(".modinfo"))) =
"depends=i2c-core";

