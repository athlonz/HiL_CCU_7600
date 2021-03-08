#ifndef __INCrio_local_cfg_h
#define __INCrio_local_cfg_h

#ifdef _cplusplus
extern "C"
{
#endif
	
#include <spinlocklib.h>	


#define RIO_MAX_ROUTE_ENTRIES(size)	(size ? (1 << 16) : (1 << 8))

#ifndef USE_VXWORKS_LISTS

#undef LIST_HEAD

#define typeof __typeof__

#define offsetof1(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)
/**
 * container_of - cast a member of a structure out to the containing structure
 * @ptr:	the pointer to the member.
 * @type:	the type of the container struct this is embedded in.
 * @member:	the name of the member within the struct.
 *
 */
#define container_of(ptr, type, member) \
    ((type *)((char *)(ptr) - offsetof1(type,member)))



struct list_head
{
	struct list_head *prev;
	struct list_head *next;
};

/*
 * Simple doubly linked list implementation.
 *
 * Some of the internal functions ("__xxx") are useful when
 * manipulating whole lists rather than single entries, as
 * sometimes we already know the next/prev entries and we can
 * generate better code by using them directly rather than
 * using the generic single-entry routines.
 */

#define LIST_HEAD_INIT(name) { &(name), &(name) }

#define LIST_HEAD(name) \
	struct list_head name = LIST_HEAD_INIT(name)

static __inline__ void INIT_LIST_HEAD(struct list_head *list)
{
	list->next = list;
	list->prev = list;
}

#define rio_dev_g(n) list_entry(n, struct rio_dev, global_list)
#define rio_dev_f(n) list_entry(n, struct rio_dev, net_list)
#define	to_rio_dev(n) container_of(n, struct rio_dev, dev)
#define sw_to_rio_dev(n) container_of(n, struct rio_dev, rswitch[0])
#define	to_rio_mport(n) container_of(n, struct rio_mport, dev)

/*
 * Insert a new entry between two known consecutive entries.
 *
 * This is only for internal list manipulation where we know
 * the prev/next entries already!
 */
#ifndef CONFIG_DEBUG_LIST
static __inline__ void __list_add(struct list_head *new,
		struct list_head *prev,
		struct list_head *next)
{
	next->prev = new;
	new->next = next;
	new->prev = prev;
	prev->next = new;
}
#else
extern void __list_add(struct list_head *new,
		struct list_head *prev,
		struct list_head *next);
#endif

/**
 * list_add - add a new entry
 * @new: new entry to be added
 * @head: list head to add it after
 *
 * Insert a new entry after the specified head.
 * This is good for implementing stacks.
 */
static __inline__ void list_add(struct list_head *new, struct list_head *head)
{
	__list_add(new, head, head->next);
}

/**
 * list_add_tail - add a new entry
 * @new: new entry to be added
 * @head: list head to add it before
 *
 * Insert a new entry before the specified head.
 * This is useful for implementing queues.
 */
static __inline__ void list_add_tail(struct list_head *new, struct list_head *head)
{
	__list_add(new, head->prev, head);
}

/*
 * Delete a list entry by making the prev/next entries
 * point to each other.
 *
 * This is only for internal list manipulation where we know
 * the prev/next entries already!
 */
static __inline__ void __list_del(struct list_head * prev,
		struct list_head * next)
{
	next->prev = prev;
	prev->next = next;
}

/**
 * list_del - deletes entry from list.
 * @entry: the element to delete from the list.
 * Note: list_empty() on entry does not return true after this, the entry is
 * in an undefined state.
 */
#ifndef CONFIG_DEBUG_LIST
static __inline__ void __list_del_entry(struct list_head *entry)
{
	__list_del(entry->prev, entry->next);
}

static __inline__ void list_del(struct list_head *entry)
{
	__list_del(entry->prev, entry->next);
	entry->next = 0;
	entry->prev = 0;
}
#else
extern void __list_del_entry(struct list_head *entry);
extern void list_del(struct list_head *entry);
#endif

/**
 * list_replace - replace old entry by new one
 * @old : the element to be replaced
 * @new : the new element to insert
 *
 * If @old was empty, it will be overwritten.
 */
static __inline__ void list_replace(struct list_head *old,
		struct list_head *new)
{
	new->next = old->next;
	new->next->prev = new;
	new->prev = old->prev;
	new->prev->next = new;
}

static __inline__ void list_replace_init(struct list_head *old,
		struct list_head *new)
{
	list_replace(old, new);
	INIT_LIST_HEAD(old);
}

/**
 * list_del_init - deletes entry from list and reinitialize it.
 * @entry: the element to delete from the list.
 */
static __inline__ void list_del_init(struct list_head *entry)
{
	__list_del_entry(entry);
	INIT_LIST_HEAD(entry);
}

/**
 * list_move - delete from one list and add as another's head
 * @list: the entry to move
 * @head: the head that will precede our entry
 */
static __inline__ void list_move(struct list_head *list, struct list_head *head)
{
	__list_del_entry(list);
	list_add(list, head);
}

/**
 * list_move_tail - delete from one list and add as another's tail
 * @list: the entry to move
 * @head: the head that will follow our entry
 */
static __inline__ void list_move_tail(struct list_head *list,
		struct list_head *head)
{
	__list_del_entry(list);
	list_add_tail(list, head);
}

/**
 * list_is_last - tests whether @list is the last entry in list @head
 * @list: the entry to test
 * @head: the head of the list
 */
static __inline__ int list_is_last(const struct list_head *list,
		const struct list_head *head)
{
	return list->next == head;
}

/**
 * list_empty - tests whether a list is empty
 * @head: the list to test.
 */
static __inline__ int list_empty(const struct list_head *head)
{
	return head->next == head;
}

/**
 * list_empty_careful - tests whether a list is empty and not being modified
 * @head: the list to test
 *
 * Description:
 * tests whether a list is empty _and_ checks that no other CPU might be
 * in the process of modifying either member (next or prev)
 *
 * NOTE: using list_empty_careful() without synchronization
 * can only be safe if the only activity that can happen
 * to the list entry is list_del_init(). Eg. it cannot be used
 * if another CPU could re-list_add() it.
 */
static __inline__ int list_empty_careful(const struct list_head *head)
{
	struct list_head *next = head->next;
	return (next == head) && (next == head->prev);
}

/**
 * list_rotate_left - rotate the list to the left
 * @head: the head of the list
 */
static __inline__ void list_rotate_left(struct list_head *head)
{
	struct list_head *first;

	if (!list_empty(head))
	{
		first = head->next;
		list_move_tail(first, head);
	}
}

/**
 * list_is_singular - tests whether a list has just one entry.
 * @head: the list to test.
 */
static __inline__ int list_is_singular(const struct list_head *head)
{
	return !list_empty(head) && (head->next == head->prev);
}

static __inline__ void __list_cut_position(struct list_head *list,
		struct list_head *head, struct list_head *entry)
{
	struct list_head *new_first = entry->next;
	list->next = head->next;
	list->next->prev = list;
	list->prev = entry;
	entry->next = list;
	head->next = new_first;
	new_first->prev = head;
}

/**
 * list_cut_position - cut a list into two
 * @list: a new list to add all removed entries
 * @head: a list with entries
 * @entry: an entry within head, could be the head itself
 *	and if so we won't cut the list
 *
 * This helper moves the initial part of @head, up to and
 * including @entry, from @head to @list. You should
 * pass on @entry an element you know is on @head. @list
 * should be an empty list or a list you do not care about
 * losing its data.
 *
 */
static __inline__ void list_cut_position(struct list_head *list,
		struct list_head *head, struct list_head *entry)
{
	if (list_empty(head))
		return;
	if (list_is_singular(head) && (head->next != entry && head != entry))
		return;
	if (entry == head)
		INIT_LIST_HEAD(list);
	else
		__list_cut_position(list, head, entry);
}

static __inline__ void __list_splice(const struct list_head *list,
		struct list_head *prev, struct list_head *next)
{
	struct list_head *first = list->next;
	struct list_head *last = list->prev;

	first->prev = prev;
	prev->next = first;

	last->next = next;
	next->prev = last;
}
/**
 * list_splice - join two lists, this is designed for stacks
 * @list: the new list to add.
 * @head: the place to add it in the first list.
 */
static __inline__ void list_splice(const struct list_head *list,
		struct list_head *head)
{
	if (!list_empty(list))
		__list_splice(list, head, head->next);
}

/**
 * list_splice_tail - join two lists, each list being a queue
 * @list: the new list to add.
 * @head: the place to add it in the first list.
 */
static __inline__ void list_splice_tail(struct list_head *list,
		struct list_head *head)
{
	if (!list_empty(list))
		__list_splice(list, head->prev, head);
}
/**
 * list_splice_init - join two lists and reinitialise the emptied list.
 * @list: the new list to add.
 * @head: the place to add it in the first list.
 *
 * The list at @list is reinitialised
 */
static __inline__ void list_splice_init(struct list_head *list,
		struct list_head *head)
{
	if (!list_empty(list))
	{
		__list_splice(list, head, head->next);
		INIT_LIST_HEAD(list);
	}
}

/**
 * list_splice_tail_init - join two lists and reinitialise the emptied list
 * @list: the new list to add.
 * @head: the place to add it in the first list.
 *
 * Each of the lists is a queue.
 * The list at @list is reinitialised
 */
static __inline__ void list_splice_tail_init(struct list_head *list,
		struct list_head *head)
{
	if (!list_empty(list))
	{
		__list_splice(list, head->prev, head);
		INIT_LIST_HEAD(list);
	}
}

/**
 * list_entry - get the struct for this entry
 * @ptr:	the &struct list_head pointer.
 * @type:	the type of the struct this is embedded in.
 * @member:	the name of the list_struct within the struct.
 */
#define list_entry(ptr, type, member) \
	container_of(ptr, type, member)

/**
 * list_first_entry - get the first element from a list
 * @ptr:	the list head to take the element from.
 * @type:	the type of the struct this is embedded in.
 * @member:	the name of the list_struct within the struct.
 *
 * Note, that list is expected to be not empty.
 */
#define list_first_entry(ptr, type, member) \
	list_entry((ptr)->next, type, member)

/**
 * list_last_entry - get the last element from a list
 * @ptr:	the list head to take the element from.
 * @type:	the type of the struct this is embedded in.
 * @member:	the name of the list_struct within the struct.
 *
 * Note, that list is expected to be not empty.
 */
#define list_last_entry(ptr, type, member) \
	list_entry((ptr)->prev, type, member)

/**
 * list_first_entry_or_null - get the first element from a list
 * @ptr:	the list head to take the element from.
 * @type:	the type of the struct this is embedded in.
 * @member:	the name of the list_struct within the struct.
 *
 * Note that if the list is empty, it returns NULL.
 */
#define list_first_entry_or_null(ptr, type, member) \
	(!list_empty(ptr) ? list_first_entry(ptr, type, member) : NULL)

/**
 * list_next_entry - get the next element in list
 * @pos:	the type * to cursor
 * @member:	the name of the list_struct within the struct.
 */
#define list_next_entry(pos, member) \
	list_entry((pos)->member.next, typeof(*(pos)), member)

/**
 * list_prev_entry - get the prev element in list
 * @pos:	the type * to cursor
 * @member:	the name of the list_struct within the struct.
 */
#define list_prev_entry(pos, member) \
	list_entry((pos)->member.prev, typeof(*(pos)), member)

/**
 * list_for_each	-	iterate over a list
 * @pos:	the &struct list_head to use as a loop cursor.
 * @head:	the head for your list.
 */
#define list_for_each(pos, head) \
	for (pos = (head)->next; pos != (head); pos = pos->next)

/**
 * list_for_each_prev	-	iterate over a list backwards
 * @pos:	the &struct list_head to use as a loop cursor.
 * @head:	the head for your list.
 */
#define list_for_each_prev(pos, head) \
	for (pos = (head)->prev; pos != (head); pos = pos->prev)

/**
 * list_for_each_safe - iterate over a list safe against removal of list entry
 * @pos:	the &struct list_head to use as a loop cursor.
 * @n:		another &struct list_head to use as temporary storage
 * @head:	the head for your list.
 */
#define list_for_each_safe(pos, n, head) \
	for (pos = (head)->next, n = pos->next; pos != (head); \
		pos = n, n = pos->next)

/**
 * list_for_each_prev_safe - iterate over a list backwards safe against removal of list entry
 * @pos:	the &struct list_head to use as a loop cursor.
 * @n:		another &struct list_head to use as temporary storage
 * @head:	the head for your list.
 */
#define list_for_each_prev_safe(pos, n, head) \
	for (pos = (head)->prev, n = pos->prev; \
	     pos != (head); \
	     pos = n, n = pos->prev)

/**
 * list_for_each_entry	-	iterate over list of given type
 * @pos:	the type * to use as a loop cursor.
 * @head:	the head for your list.
 * @member:	the name of the list_struct within the struct.
 */
#define list_for_each_entry(pos, head, member)				\
	for (pos = list_first_entry(head, typeof(*pos), member);	\
	     &pos->member != (head);					\
	     pos = list_next_entry(pos, member))


/*
#define list_for_each_entry(pos, head, member) \
	for (pos = list_entry((head)->next, typeof(*pos), member); \
	     &pos->member != (head); \
	     pos = list_entry(pos->member.next, typeof(*pos), member))
*/


/**
 * list_for_each_entry_reverse - iterate backwards over list of given type.
 * @pos:	the type * to use as a loop cursor.
 * @head:	the head for your list.
 * @member:	the name of the list_struct within the struct.
 */
#define list_for_each_entry_reverse(pos, head, member)			\
	for (pos = list_last_entry(head, typeof(*pos), member);		\
	     &pos->member != (head); 					\
	     pos = list_prev_entry(pos, member))

/**
 * list_prepare_entry - prepare a pos entry for use in list_for_each_entry_continue()
 * @pos:	the type * to use as a start point
 * @head:	the head of the list
 * @member:	the name of the list_struct within the struct.
 *
 * Prepares a pos entry for use as a start point in list_for_each_entry_continue().
 */
#define list_prepare_entry(pos, head, member) \
	((pos) ? : list_entry(head, typeof(*pos), member))

/**
 * list_for_each_entry_continue - continue iteration over list of given type
 * @pos:	the type * to use as a loop cursor.
 * @head:	the head for your list.
 * @member:	the name of the list_struct within the struct.
 *
 * Continue to iterate over list of given type, continuing after
 * the current position.
 */
#define list_for_each_entry_continue(pos, head, member) 		\
	for (pos = list_next_entry(pos, member);			\
	     &pos->member != (head);					\
	     pos = list_next_entry(pos, member))

/**
 * list_for_each_entry_continue_reverse - iterate backwards from the given point
 * @pos:	the type * to use as a loop cursor.
 * @head:	the head for your list.
 * @member:	the name of the list_struct within the struct.
 *
 * Start to iterate over list of given type backwards, continuing after
 * the current position.
 */
#define list_for_each_entry_continue_reverse(pos, head, member)		\
	for (pos = list_prev_entry(pos, member);			\
	     &pos->member != (head);					\
	     pos = list_prev_entry(pos, member))

/**
 * list_for_each_entry_from - iterate over list of given type from the current point
 * @pos:	the type * to use as a loop cursor.
 * @head:	the head for your list.
 * @member:	the name of the list_struct within the struct.
 *
 * Iterate over list of given type, continuing from current position.
 */
#define list_for_each_entry_from(pos, head, member) 			\
	for (; &pos->member != (head);					\
	     pos = list_next_entry(pos, member))

/**
 * list_for_each_entry_safe - iterate over list of given type safe against removal of list entry
 * @pos:	the type * to use as a loop cursor.
 * @n:		another type * to use as temporary storage
 * @head:	the head for your list.
 * @member:	the name of the list_struct within the struct.
 */
#define list_for_each_entry_safe(pos, n, head, member)			\
	for (pos = list_first_entry(head, typeof(*pos), member),	\
		n = list_next_entry(pos, member);			\
	     &pos->member != (head); 					\
	     pos = n, n = list_next_entry(n, member))
/**
 * list_for_each_entry_safe_continue - continue list iteration safe against removal
 * @pos:	the type * to use as a loop cursor.
 * @n:		another type * to use as temporary storage
 * @head:	the head for your list.
 * @member:	the name of the list_struct within the struct.
 *
 * Iterate over list of given type, continuing after current point,
 * safe against removal of list entry.
 */
#define list_for_each_entry_safe_continue(pos, n, head, member) 		\
	for (pos = list_next_entry(pos, member), 				\
		n = list_next_entry(pos, member);				\
	     &pos->member != (head);						\
	     pos = n, n = list_next_entry(n, member))

/**
 * list_for_each_entry_safe_from - iterate over list from current point safe against removal
 * @pos:	the type * to use as a loop cursor.
 * @n:		another type * to use as temporary storage
 * @head:	the head for your list.
 * @member:	the name of the list_struct within the struct.
 *
 * Iterate over list of given type from current point, safe against
 * removal of list entry.
 */
#define list_for_each_entry_safe_from(pos, n, head, member) 			\
	for (n = list_next_entry(pos, member);					\
	     &pos->member != (head);						\
	     pos = n, n = list_next_entry(n, member))

/**
 * list_for_each_entry_safe_reverse - iterate backwards over list safe against removal
 * @pos:	the type * to use as a loop cursor.
 * @n:		another type * to use as temporary storage
 * @head:	the head for your list.
 * @member:	the name of the list_struct within the struct.
 *
 * Iterate backwards over list of given type, safe against removal
 * of list entry.
 */
#define list_for_each_entry_safe_reverse(pos, n, head, member)		\
	for (pos = list_last_entry(head, typeof(*pos), member),		\
		n = list_prev_entry(pos, member);			\
	     &pos->member != (head); 					\
	     pos = n, n = list_prev_entry(n, member))

/**
 * list_safe_reset_next - reset a stale list_for_each_entry_safe loop
 * @pos:	the loop cursor used in the list_for_each_entry_safe loop
 * @n:		temporary storage used in list_for_each_entry_safe
 * @member:	the name of the list_struct within the struct.
 *
 * list_safe_reset_next is not safe to use in general if the list may be
 * modified concurrently (eg. the lock is dropped in the loop body). An
 * exception to this is if the cursor element (pos) is pinned in the list,
 * and list_safe_reset_next is called after re-taking the lock and before
 * completing the current iteration of the loop body.
 */
#define list_safe_reset_next(pos, n, member)				\
	n = list_next_entry(pos, member)


#endif



/**
 * struct rio_switch - RIO switch info
 * @node: Node in global list of switches
 * @route_table: Copy of switch routing table
 * @port_ok: Status of each port (one bit per port) - OK=1 or UNINIT=0
 * @ops: pointer to switch-specific operations
 * @lock: lock to serialize operations updates
 * @nextdev: Array of per-port pointers to the next attached device
 */
struct rio_switch
{
	struct list_head node;
	UINT8 *route_table;
	UINT8 port_ok;
	struct rio_switch_ops *ops;
	spinlockIsr_t lock;
	struct rio_dev *nextdev[0];
};




/**
 * struct rio_dev - RIO device info
 * @global_list: Node in list of all RIO devices
 * @net_list: Node in list of RIO devices in a network
 * @net: Network this device is a part of
 * @do_enum: Enumeration flag
 * @did: Device ID
 * @vid: Vendor ID
 * @device_rev: Device revision
 * @asm_did: Assembly device ID
 * @asm_vid: Assembly vendor ID
 * @asm_rev: Assembly revision
 * @efptr: Extended feature pointer
 * @pef: Processing element features
 * @swpinfo: Switch port info
 * @src_ops: Source operation capabilities
 * @dst_ops: Destination operation capabilities
 * @comp_tag: RIO component tag
 * @phys_efptr: RIO device extended features pointer
 * @em_efptr: RIO Error Management features pointer
 * @dma_mask: Mask of bits of RIO address this device implements
 * @driver: Driver claiming this device
 * @dev: Device model device
 * @riores: RIO resources this device owns
 * @pwcback: port-write callback function for this device
 * @destid: Network destination ID (or associated destid for switch)
 * @hopcount: Hopcount to this device
 * @prev: Previous RIO device connected to the current one
 * @rswitch: struct rio_switch (if valid for this device)
 */
struct rio_dev {
	struct list_head global_list;	/* node in list of all RIO devices */
	struct list_head net_list;	/* node in per net list */
	struct rio_net *net;	/* RIO net this device resides in */
	BOOL do_enum;
	UINT16 did;
	UINT16 vid;
	UINT32 device_rev;
	UINT16 asm_did;
	UINT16 asm_vid;
	UINT16 asm_rev;
	UINT16 efptr;
	UINT32 pef;
	UINT32 swpinfo;
	UINT32 src_ops;
	UINT32 dst_ops;
	UINT32 comp_tag;
	UINT32 phys_efptr;
	UINT32 em_efptr;
	UINT64 dma_mask;
	struct rio_driver *driver;	/* RIO driver claiming this device */
#if 0	
	struct device dev;	/* LDM device structure */
	struct resource riores[RIO_MAX_DEV_RESOURCES];
	
	int (*pwcback) (struct rio_dev *rdev, union rio_pw_msg *msg, int step);
#endif	
	UINT16 destid;
	UINT8 hopcount;
	struct rio_dev *prev;
	struct rio_switch rswitch[0];	/* RIO switch info */
};


#define rio_dev_g(n) list_entry(n, struct rio_dev, global_list)

enum rio_phy_type {
	RIO_PHY_PARALLEL,
	RIO_PHY_SERIAL
};


/**
 * struct rio_mport - RIO master port info
 * @dbells: List of doorbell events
 * @node: Node in global list of master ports
 * @nnode: Node in network list of master ports
 * @iores: I/O mem resource that this master port interface owns
 * @riores: RIO resources that this master port interfaces owns
 * @inb_msg: RIO inbound message event descriptors
 * @outb_msg: RIO outbound message event descriptors
 * @host_deviceid: Host device ID associated with this master port
 * @ops: configuration space functions
 * @id: Port ID, unique among all ports
 * @index: Port index, unique among all port interfaces of the same type
 * @sys_size: RapidIO common transport system size
 * @phy_type: RapidIO phy type
 * @phys_efptr: RIO port extended features pointer
 * @name: Port name string
 * @priv: Master port private data
 * @dma: DMA device associated with mport
 * @nscan: RapidIO network enumeration/discovery operations
 */
struct rio_mport
{
	struct list_head dbells; /* list of doorbell events */
	struct list_head node; /* node in global list of ports */
	struct list_head nnode; /* node in net list of ports */
/*	//struct resource iores;
	//struct resource riores[RIO_MAX_MPORT_RESOURCES];
	//struct rio_msg inb_msg[RIO_MAX_MBOX];
	//struct rio_msg outb_msg[RIO_MAX_MBOX];
*/	int host_deviceid; /* Host device ID */
	struct rio_ops *ops; /* low-level architecture-dependent routines */
	unsigned char id; /* port ID, unique among all ports */
	unsigned char index; /* port index, unique among all port
	 interfaces of the same type */
	unsigned int sys_size; /* RapidIO common transport system size.
	 * 0 - Small size. 256 devices.
	 * 1 - Large size, 65536 devices.
	 */
	enum rio_phy_type phy_type; /* RapidIO phy type */
	UINT32 phys_efptr;
	unsigned char name[40];
	void *priv; /* Master port private data */
#ifdef CONFIG_RAPIDIO_DMA_ENGINE
	struct dma_device dma;
#endif
	struct rio_scan *nscan;
}rio_mport_t;

/**
 * struct rio_switch_ops - Per-switch operations
 * @owner: The module owner of this structure
 * @add_entry: Callback for switch-specific route add function
 * @get_entry: Callback for switch-specific route get function
 * @clr_table: Callback for switch-specific clear route table function
 * @set_domain: Callback for switch-specific domain setting function
 * @get_domain: Callback for switch-specific domain get function
 * @em_init: Callback for switch-specific error management init function
 * @em_handle: Callback for switch-specific error management handler function
 *
 * Defines the operations that are necessary to initialize/control
 * a particular RIO switch device.
 */
struct rio_switch_ops
{
	struct module *owner;
	int (*add_entry)(struct rio_mport *mport, UINT16 destid, UINT8 hopcount,
			UINT16 table, UINT16 route_destid, UINT8 route_port);
	int (*get_entry)(struct rio_mport *mport, UINT16 destid, UINT8 hopcount,
			UINT16 table, UINT16 route_destid, UINT8 *route_port);
	int
			(*clr_table)(struct rio_mport *mport, UINT16 destid, UINT8 hopcount,
					UINT16 table);
	int (*set_domain)(struct rio_mport *mport, UINT16 destid, UINT8 hopcount,
			UINT8 sw_domain);
	int (*get_domain)(struct rio_mport *mport, UINT16 destid, UINT8 hopcount,
			UINT8 *sw_domain);
	int (*em_init)(struct rio_dev *dev);
	int (*em_handle)(struct rio_dev *dev, UINT8 swport);
};


struct rio_id_table {
	UINT16 start;	/* logical minimal id */
	UINT32 max;	/* max number of IDs in table */
	spinlockIsr_t lock;
	unsigned long *table;
};

/**
 * struct rio_net - RIO network info
 * @node: Node in global list of RIO networks
 * @devices: List of devices in this network
 * @switches: List of switches in this netowrk
 * @mports: List of master ports accessing this network
 * @hport: Default port for accessing this network
 * @id: RIO network ID
 * @destid_table: destID allocation table
 */
struct rio_net {
	struct list_head node;	/* node in list of networks */
	struct list_head devices;	/* list of devices in this net */
	struct list_head switches;	/* list of switches in this net */
	struct list_head mports;	/* list of ports accessing net */
	struct rio_mport *hport;	/* primary port for accessing net */
	unsigned char id;	/* RIO network ID */
	struct rio_id_table destid_table;  /* destID allocation table */
};


/**************************************************************************/
typedef struct {
        UINT8   pll;     
        UINT8   portwidth;           
        UINT8   port;
        UINT8   resv;
        UINT8   lanes[4];       
} QUADRANT_TABLE;





int rio_local_config(struct rio_mport *mport, UINT32 ccsbar);
int rio_switch_port_speed_cfg(UINT8 hopcount, QUADRANT_TABLE port, int speed);
int rio_switch_port_auto_speed_cfg(UINT8 hopcount, QUADRANT_TABLE *Qcfg_ptr);
int rio_switch_port_set(int hopcount, int portnum, int speed);

#ifdef _cplusplus 
}
#endif

#endif
