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
	{ 0x7da8156e, "__kmalloc" },
	{ 0x8e9703d1, "mem_map" },
	{ 0x3de88ff0, "malloc_sizes" },
	{ 0x291d3e27, "remove_wait_queue" },
	{ 0x56179c5f, "mtrr_add" },
	{ 0x5d8fb1be, "__page_cache_release" },
	{ 0x1053c828, "ivtv_api" },
	{ 0xda02d67, "jiffies" },
	{ 0x144f9d0f, "ivtv_cards" },
	{ 0xcc9f14e6, "default_wake_function" },
	{ 0x1b7d4074, "printk" },
	{ 0x7a890c8, "fb_alloc_cmap" },
	{ 0xc7b381bc, "ivtv_vapi" },
	{ 0x78c07923, "rwsem_wake" },
	{ 0x92a9d639, "ivtv_set_irq_mask" },
	{ 0xeb17b334, "register_framebuffer" },
	{ 0x2f287f0d, "copy_to_user" },
	{ 0x707f93dd, "preempt_schedule" },
	{ 0x6a6f0027, "kmem_cache_alloc" },
	{ 0x14f67530, "ivtv_debug" },
	{ 0xc2b4e244, "get_user_pages" },
	{ 0x17d59d01, "schedule_timeout" },
	{ 0x6b2dc060, "dump_stack" },
	{ 0x799aca4, "local_bh_enable" },
	{ 0x70196ffb, "ivtv_cards_active" },
	{ 0xbce89f77, "__wake_up" },
	{ 0x8850362c, "add_wait_queue" },
	{ 0x37a0cba, "kfree" },
	{ 0x272d394e, "mtrr_del" },
	{ 0x60a4461c, "__up_wakeup" },
	{ 0x96b27088, "__down_failed" },
	{ 0xd6c963c, "copy_from_user" },
	{ 0x74f0e702, "rwsem_down_read_failed" },
	{ 0xb5eb2d49, "unregister_framebuffer" },
};

static const char __module_depends[]
__attribute_used__
__attribute__((section(".modinfo"))) =
"depends=ivtv";

