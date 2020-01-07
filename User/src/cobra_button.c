#include <cobra_button.h>
#include <cobra_console.h>

#if (LOG_BUTTON_LEVEL > LOG_LEVEL_NOT)
#define BUTTON_INFO(fm, ...) { \
		console_cmdline_clean(); \
		console("BUTTON  : " fm, ##__VA_ARGS__) \
		console_cmdline_restore(); \
	}
#else
#define BUTTON_INFO(fm, ...)
#endif

#if (LOG_BUTTON_LEVEL > LOG_LEVEL_INFO)
#define BUTTON_DEBUG(fm, ...) { \
		console_cmdline_clean(); \
		console("BUTTON  : " fm, ##__VA_ARGS__) \
		console_cmdline_restore(); \
	}
#else
#define BUTTON_DEBUG(fm, ...)
#endif

#define BUTTON_LOG(level, fm, ...) BUTTON_##level(fm, ##__VA_ARGS__)

/**********************************************************************************/
/* Button Interface Functions                                                     */
/* version : 2.0                                                                  */
/* review on 2019/12/21                                                           */
/**********************************************************************************/

int button_check_active(BUTTON_S *button)
{
    int interval = 0;

    /* shack process */
    if(button->state.dithering) {
        if((button->state.dithering == DITHERING_LAST_PRESS  && button->state.press) ||
           (button->state.dithering == DITHERING_LAST_LOOSEN && !button->state.press)) {
            button->state.duration++;
        }
        else {
            button->state.dithering = button->state.press ? DITHERING_LAST_PRESS : DITHERING_LAST_LOOSEN;
            button->state.duration = 0;
        }

        if(button->state.duration >= button->interval.dithering) {
            button->state.dithering = DITHERING_CLEAN;
            button->state.duration  = 0;
            if(!button->state.effective && button->state.press) {
                button->state.effective = ECT_PRESSED;
                button->state.active = 1;
				button->cont_count = 0;
            }
            else if(button->state.effective && !button->state.press) {
                button->state.effective = ECT_LOOSE;
				button->state.active = 1;
            }
        }
        return button->state.active;
    }

    switch(button->state.effective) {
    case ECT_LOOSE:
        if(button->state.press) {
            button->state.dithering = DITHERING_LAST_PRESS;
        }
        break;
    case ECT_PRESSED:
    case ECT_FOCUSED:
        if(!button->state.press) {
            button->state.dithering = DITHERING_LAST_LOOSEN;
            button->state.duration  = 0;
        }
        else {
            button->state.duration++;
            interval = button->state.effective == ECT_PRESSED ?
            			button->interval.long_press : button->interval.continuous;
            if(button->state.duration >= interval) {
				button->cont_count++;
				BUTTON_LOG(DEBUG, "active interval=%d cont_count=%d\n", interval, button->cont_count);
                button->state.duration  = 0;
                button->state.effective = ECT_FOCUSED;
                button->state.active    = 1;
            }
        }
        break;
    }
    return button->state.active;
}
