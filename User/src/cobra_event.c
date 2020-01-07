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
			if(0 == pos->info.priority) {
				if(EV_STATUS_NORMAL == pos->info.status) {
					EVENT_LOG(DEBUG, "[%d]EV_STATUS_NORMAL: process, status=%d\n",
							pos->info.id, pos->info.status);
					gl_sys.event_process(pos);
					EVENT_LOG(DEBUG, "[%d]EV_STATUS_NORMAL: process end, status=%d\n",
							pos->info.id, pos->info.status);
					if(EV_STATUS_REQUEST != pos->info.status) { /* Status not upgraded to EV_TYPE_REQUEST */
						list_del(&pos->list);
						pos->info.active = CBA_FALSE;
					}
					return;
				}

				if(EV_STATUS_REQUEST == pos->info.status) {
					continue;
				}

				/* Note: for the same event, only one result event can be generated */
				/*       in either response or timeout                              */
				if(EV_STATUS_RESPONSE == pos->info.status) {
					EVENT_LOG(DEBUG, "[%d][%d]: EV_STATUS_RESPONSE\n", pos->info.id, pos->info.status);
				}
				else {
					EVENT_LOG(DEBUG, "[%d][%d]: EV_STATUS_TIMEOUT\n", pos->info.id, pos->info.status);
				}

				list_for_each_entry_safe(pos_req, tmp_req, &event_head.list, EVENT_S, list) {
					if(0 == pos_req->info.priority &&
							EV_STATUS_REQUEST == pos_req->info.status &&
							pos_req->info.id == pos->info.id) {
							EVENT_LOG(DEBUG, "[%d]status[%d]: found\n", pos->info.id, pos->info.status);
						gl_sys.event_process(pos);
						list_del(&pos_req->list);
						pos_req->info.active = CBA_FALSE;
						list_del(&pos->list);
						pos->info.active = CBA_FALSE;
						return;
					}
				}
				EVENT_LOG(DEBUG, "[%d]status[%d]: event ignored\n", pos->info.id, pos->info.status);
				list_del(&pos->list);
				pos->info.active = CBA_FALSE;

				return;
			}
			else {
				pos->info.priority--;
			}
		}
	}
}

void event_commit(EVENT_S *event)
{
	list_add_tail(&event->list, &event_head.list);
}

void event_init(void)
{
	INIT_LIST_HEAD(&event_head.list);
	EVENT_LOG(INFO, "%s ... OK\n", __func__);
}
