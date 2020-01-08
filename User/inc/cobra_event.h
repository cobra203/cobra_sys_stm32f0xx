#ifndef _COBRA_EVENT_H_
#define _COBRA_EVENT_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include <cobra_common.h>
#include <cobra_list.h>

#define PRIORITY_LEVEL_COUNT	4

typedef enum event_state_e
{
	EV_STATE_NORMAL,
	EV_STATE_REQUEST,
	EV_STATE_WAITFOR,
	EV_STATE_RESPONSE,
	EV_STATE_TIMEOUT,
} EVENT_STATE_E;

typedef struct event_status_s
{
	uint8_t 	active			:1;
	uint8_t		state			:3;
	uint8_t		priority		:2;
	uint8_t						:2;
} EVENT_STATUS_S;

typedef struct event_s
{
	uint16_t		id;
	EVENT_STATUS_S	status;
	void			*data;
	LIST_S			list;
} EVENT_S;

CBA_BOOL event_is_active(EVENT_S *event);
void event_commit(EVENT_S *event, uint16_t id, uint8_t priority, uint8_t status, void *data);
void event_monitor_handle(void);
void event_init(void);


#ifdef __cplusplus
}
#endif

#endif /* _COBRA_EVENT_H_ */
