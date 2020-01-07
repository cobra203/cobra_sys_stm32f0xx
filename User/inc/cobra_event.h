#ifndef _COBRA_EVENT_H_
#define _COBRA_EVENT_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include <cobra_common.h>
#include <cobra_list.h>

#define PRIORITY_LEVEL_COUNT	4

typedef enum event_status_e
{
	EV_STATUS_NORMAL,
	EV_STATUS_REQUEST,
	EV_STATUS_RESPONSE,
	EV_STATUS_TIMEOUT,
} EVENT_STATUS_E;

typedef struct event_info_s
{
	uint32_t		status			:2;
	uint32_t		active			:1;
	uint32_t						:5;
	uint32_t		priority		:2;
	uint32_t						:6;
	uint32_t		id				:16;
} EVENT_INFO_S;

typedef struct event_s
{
	EVENT_INFO_S	info;
	void			*data;
	LIST_S			list;
} EVENT_S;

void event_commit(EVENT_S *event);
void event_monitor_handle(void);
void event_init(void);


#ifdef __cplusplus
}
#endif

#endif /* _COBRA_EVENT_H_ */
