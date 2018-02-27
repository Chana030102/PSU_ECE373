#include <linux/module.h>
#include <linux/types.h>
#include <linux/pci.h>

static int pci_test_probe(struct pci_dev *pdev, const struct pci_device_id *ent)
{
	/* this is where I'd map BAR's for access, save stuff off, etc. */
	printk(KERN_INFO "It's dangerous to go alone, take this with you.\n");

	/* 0 means success */
	return 0;
}

static void pci_test_remove(struct pci_dev *pdev)
{
	/* nothing for me to clean up here since I don't do anything in probe */
	printk(KERN_INFO "So long!!\n");
}

static DEFINE_PCI_DEVICE_TABLE(pci_test_tbl) = {
	{ PCI_DEVICE(0x10ec, 0x8168) },
	{ }, /* must have an empty at the end! */
};

static struct pci_driver test_pci_driver = {
	.name = "I see you",
	.id_table = pci_test_tbl,
	.probe = pci_test_probe,
	.remove = pci_test_remove,
};

static int __init pci_test_init(void)
{
	return pci_register_driver(&test_pci_driver);
}

static void __exit pci_test_exit(void)
{
	pci_unregister_driver(&test_pci_driver);
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("PJ Waskiewicz");
MODULE_VERSION("0.1");

module_init(pci_test_init);
module_exit(pci_test_exit);
