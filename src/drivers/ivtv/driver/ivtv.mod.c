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
	{ 0x1a1a4f09, "__request_region" },
	{ 0xab89da95, "pci_bus_read_config_byte" },
	{ 0x7da8156e, "__kmalloc" },
	{ 0x98a9b87b, "__mod_timer" },
	{ 0x8e9703d1, "mem_map" },
	{ 0x89b301d4, "param_get_int" },
	{ 0x8d4e5f0d, "del_timer" },
	{ 0x9efed5af, "iomem_resource" },
	{ 0x778024df, "autoremove_wake_function" },
	{ 0x99f4cdf6, "video_device_release" },
	{ 0x997565e9, "video_usercopy" },
	{ 0x3de88ff0, "malloc_sizes" },
	{ 0x291d3e27, "remove_wait_queue" },
	{ 0x5d8fb1be, "__page_cache_release" },
	{ 0x7e4f89c3, "interruptible_sleep_on" },
	{ 0x17bb8430, "schedule_work" },
	{ 0x806d5133, "param_array_get" },
	{ 0xa03d6a57, "__get_user_4" },
	{ 0x3cbe760d, "queue_work" },
	{ 0xbc240b27, "filp_close" },
	{ 0x98bd6f46, "param_set_int" },
	{ 0x1d26aa98, "sprintf" },
	{ 0x89cef6fb, "param_array_set" },
	{ 0xda02d67, "jiffies" },
	{ 0xb0cc6c2f, "pci_find_slot" },
	{ 0xe0a35f53, "__create_workqueue" },
	{ 0xcc9f14e6, "default_wake_function" },
	{ 0x6bc05e9e, "video_register_device" },
	{ 0xfef06edf, "vfs_read" },
	{ 0xfd032465, "pci_set_dma_mask" },
	{ 0x4167e82d, "video_device_alloc" },
	{ 0x1b7d4074, "printk" },
	{ 0x824266cd, "video_unregister_device" },
	{ 0x78c07923, "rwsem_wake" },
	{ 0x5568be43, "lock_kernel" },
	{ 0xdc76cbcb, "param_set_bool" },
	{ 0x2f287f0d, "copy_to_user" },
	{ 0xbcb5f945, "i2c_bit_del_bus" },
	{ 0xacab2c6f, "destroy_workqueue" },
	{ 0xd7a0ee9c, "poll_freewait" },
	{ 0x707f93dd, "preempt_schedule" },
	{ 0x791f1bdb, "poll_initwait" },
	{ 0x1d98cf38, "flush_workqueue" },
	{ 0xb9d6fac3, "flush_signals" },
	{ 0x5dfe8f1a, "unlock_kernel" },
	{ 0x6a6f0027, "kmem_cache_alloc" },
	{ 0x3762cb6e, "ioremap_nocache" },
	{ 0xb30c1fa3, "pci_bus_read_config_word" },
	{ 0xc2b4e244, "get_user_pages" },
	{ 0x26e96637, "request_irq" },
	{ 0x17d59d01, "schedule_timeout" },
	{ 0x4292364c, "schedule" },
	{ 0x6b2dc060, "dump_stack" },
	{ 0x799aca4, "local_bh_enable" },
	{ 0xd49501d4, "__release_region" },
	{ 0x41699189, "pci_unregister_driver" },
	{ 0xbce89f77, "__wake_up" },
	{ 0xf1963415, "pci_bus_write_config_byte" },
	{ 0x8850362c, "add_wait_queue" },
	{ 0x37a0cba, "kfree" },
	{ 0x932da67e, "kill_proc" },
	{ 0x2e60bace, "memcpy" },
	{ 0x98adfde2, "request_module" },
	{ 0x801678, "flush_scheduled_work" },
	{ 0x9e4dfa23, "prepare_to_wait" },
	{ 0x407a28d8, "interruptible_sleep_on_timeout" },
	{ 0xedc03953, "iounmap" },
	{ 0x96df2102, "set_user_nice" },
	{ 0xca83d19d, "finish_wait" },
	{ 0x7e9ebb05, "kernel_thread" },
	{ 0x60a4461c, "__up_wakeup" },
	{ 0x5611e4ec, "param_get_bool" },
	{ 0xadc25933, "i2c_bit_add_bus" },
	{ 0x25da070, "snprintf" },
	{ 0x96b27088, "__down_failed" },
	{ 0x9edc833b, "pci_enable_device" },
	{ 0xd6c963c, "copy_from_user" },
	{ 0xdc43a9c8, "daemonize" },
	{ 0x74f0e702, "rwsem_down_read_failed" },
	{ 0xf20dabd8, "free_irq" },
	{ 0xfd8db0d6, "pci_register_driver" },
	{ 0x4ae99b3, "filp_open" },
};

static const char __module_depends[]
__attribute_used__
__attribute__((section(".modinfo"))) =
"depends=videodev,i2c-algo-bit";

