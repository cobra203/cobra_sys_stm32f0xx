#include <cobra_event.h>
#include <cobra_console.h>
#include <cobra_sys.h>

#if (LOG_EVENT_LEVEL > LOG_LEVEL_NOT)
#define EVENT_INFO(fm, ...) { \
	console_cmdline_clean(); \
	console("EVENT   : " fm, ##__VA_ARGS__) \
	console_cmdline_restore(); \
}
#else
#define EVENT_INFO(fm, ...)
#endif

#if (LOG_EVENT_LEVEL > LOG_LEVEL_INFO)
#define EVENT_DEBUG(fm, ...) { \
	console_cmdline_clean(); \
	console("EVENT   : " fm, ##__VA_ARGS__) \
	console_cmdline_restore(); \
}
#else
#define EVENT_DEBUG(fm, ...)
#endif

#define EVENT_LOG(level, fm, ...) EVENT_##level(fm, ##__VA_ARGS__)

static EVENT_S	event_head;

void event_monitor_handle(void)
{
	int i = 0;
	EVENT_S *pos, *tmp;
	EVENT_S *pos_req, *tmp_req;

	for(i = 0; i < PRIORITY_LEVEL_COUNT; i++) {
		list_for_each_entry_safe(pos, tmp, &event_head.list, EVENT_S, list) {
			if(0 == pos->status.priority) {
				if(EV_STATE_WAITFOR > pos->status.state) {
					EVENT_LOG(DEBUG, "[%d]EV_STATE_%s: process\n",
							pos->id, pos->status.state ? "REQUEST" : "NORMAL");
					gl_sys.event_process(pos);

					if(EV_STATE_NORMAL == pos->status.state) {
						EVENT_LOG(DEBUG, "[%d]EV_STATE_NORMAL: completed\n", pos->id);
						list_del(&pos->list);
						pos->status.active = CBA_FALSE;
					}
					else {
						pos->status.state = EV_STATE_WAITFOR;
						EVENT_LOG(DEBUG, "[%d]: state->EV_STATE_WAITFOR\n", pos->id);
					}
					return;
				}

				if(EV_STATE_WAITFOR == pos->status.state) {
					continue;
				}

				/* Note: for the same event, only one result event can be generated */
				/*       in either response or timeout                              */
				/*       EV_STATE_WAITFOR < pos->status.state                       */
				EVENT_LOG(DEBUG, "[%d]EV_STATE_%s: process\n",
						pos->id, EV_STATE_RESPONSE == pos->status.state ? "RESPONSE" : "TIMEOUT");

				list_for_each_entry_safe(pos_req, tmp_req, &event_head.list, EVENT_S, list) {
					if(EV_STATE_WAITFOR == pos_req->status.state &&
							pos_req->id == pos->id) {
							EVENT_LOG(DEBUG, "[%d]EV_STATE_WAITFOR: matched\n", pos->id);
						gl_sys.event_process(pos);
						EVENT_LOG(DEBUG, "[%d]EV_STATE_%s: completed\n",
								pos->id, EV_STATE_RESPONSE == pos->status.state ? "RESPONSE" : "TIMEOUT");
						list_del(&pos_req->list);
						pos_req->status.active = CBA_FALSE;
						list_del(&pos->list);
						pos->status.active = CBA_FALSE;
						return;
					}
				}
				EVENT_LOG(DEBUG, "[%d]EV_STATE_%s: not matched, ignored\n",
						pos->id, EV_STATE_RESPONSE == pos->status.state ? "RESPONSE" : "TIMEOUT");
				list_del(&pos->list);
				pos->status.active = CBA_FALSE;

				return;
			}
			else {
				pos->status.priority--;
			}
		}
	}
}

CBA_BOOL event_is_active(EVENT_S *event)
{
	return event->status.active;
}

void event_commit(EVENT_S *event, uint16_t id, uint8_t priority, uint8_t state, void *data)
{
	event->id = id;
	event->status.priority = priority;
	event->status.state = state;
	event->status.active = CBA_TRUE;
	event->data = data;
	list_add_tail(&event->list, &event_head.list);
}

void event_init(void)
{
	INIT_LIST_HEAD(&event_head.list);
	EVENT_LOG(INFO, "%s ... OK\n", __func__);
}
