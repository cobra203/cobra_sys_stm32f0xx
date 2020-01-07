#ifndef _COBRA_LIST_H_
#define _COBRA_LIST_H_

#ifdef __cplusplus
extern "C" {
#endif

#define COBRA_LIST_INLINEx

typedef struct list_head {
	struct list_head *next, *prev;
} LIST_S;

#define LIST_HEAD_INIT(name) { &(name), &(name) }

#define LIST_HEAD(name) \
		struct list_head name = LIST_HEAD_INIT(name)

#ifdef COBRA_LIST_INLINE
static inline void INIT_LIST_HEAD(struct list_head *list)
{
	list->next = list;
	list->prev = list;
}

static inline int list_empty(const struct list_head *head)
{
	return head->next == head;
}

static inline int list_empty_careful(const struct list_head *head)
{
	struct list_head *next = head->next;
	return (next == head) && (next == head->prev);
}

static inline void __list_add(struct list_head *new,
                  struct list_head *prev,
                  struct list_head *next)
{
    next->prev = new;
    new->next = next;
    new->prev = prev;
    prev->next = new;
}

static inline void list_add(struct list_head *new, struct list_head *head)
{
    __list_add(new, head, head->next);
}

static inline void list_add_tail(struct list_head *new, struct list_head *head)
{
    __list_add(new, head->prev, head);
}

static inline void __list_del(struct list_head *prev, struct list_head *next)
{
    next->prev = prev;
    prev->next = next;
}

static inline void list_del(struct list_head *entry)
{
	__list_del(entry->prev, entry->next);
	entry->prev = NULL;
	entry->next = NULL;
}

#else
void INIT_LIST_HEAD(struct list_head *list);
int list_empty(const struct list_head *head);
int list_empty_careful(const struct list_head *head);
void list_add(struct list_head *new, struct list_head *head);
void list_add_tail(struct list_head *new, struct list_head *head);
void list_del(struct list_head *entry);

#endif

#define offsetof(TYPE, MEMBER)  ((size_t)&((TYPE *)0)->MEMBER)

#define container_of(ptr, type, member) ((type *)((char *)(ptr) - offsetof(type,member)))

#define list_entry(ptr, type, member) \
		container_of(ptr, type, member)

#define list_first_entry(ptr, type, member) \
		list_entry((ptr)->next, type, member)

#define list_last_entry(ptr, type, member) \
		list_entry((ptr)->prev, type, member)

#define list_next_entry(pos, type, member) \
		list_entry((pos)->member.next, type, member)

#define list_prev_entry(pos, type, member) \
		list_entry((pos)->member.prev, type, member)

#define list_for_each(pos, head) \
		for (pos = (head)->next; pos != (head); pos = pos->next)

#define list_for_each_safe(pos, n, head) \
		for (pos = (head)->next, n = pos->next; pos != (head); \
			 pos = n, n = pos->next)

#define list_for_each_prev_safe(pos, n, head) \
		for (pos = (head)->prev, n = pos->prev; \
			 pos != (head); \
			 pos = n, n = pos->prev)

#define list_for_each_prev(pos, head) \
		for (pos = (head)->prev; pos != (head); pos = pos->prev)

#define list_for_each_entry(pos, head, type, member) \
		for (pos = list_first_entry(head, type, member); \
			 &pos->member != (head); \
			 pos = list_next_entry(pos, type, member))

#define list_for_each_entry_reverse(pos, head, type, member) \
		for (pos = list_last_entry(head, type, member); \
			 &pos->member != (head); \
			 pos = list_prev_entry(pos, type, member))

#define list_for_each_entry_safe(pos, n, head, type, member) \
		for (pos = list_first_entry(head, type, member), \
			 n = list_next_entry(pos, type, member); \
			 &pos->member != (head); \
			 pos = n, n = list_next_entry(n, type, member))

#define list_for_each_entry_safe_reverse(pos, n, head, type, member) \
		for (pos = list_last_entry(head, type, member), \
			 n = list_prev_entry(pos, type, member); \
			 &pos->member != (head); \
			 pos = n, n = list_prev_entry(n, type, member))

#ifdef __cplusplus
}
#endif

#endif /* _COBRA_LIST_H_ */
