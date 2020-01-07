#ifndef _COBRA_BUTTON_H_
#define _COBRA_BUTTON_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include <cobra_common.h>

typedef enum btn_effective_e
{
    ECT_LOOSE = 0,
    ECT_PRESSED,
    ECT_FOCUSED,
} BTN_EFFECTIVE_E;

typedef enum btn_dithering_e
{
	DITHERING_CLEAN,
    DITHERING_LAST_PRESS = 1,
    DITHERING_LAST_LOOSEN,
} BTN_DITHERING_E;

typedef struct btn_state_s
{
    uint16_t    effective   :2;
    uint16_t    dithering   :2;
    uint16_t    active      :1;
    uint16_t    press       :1;
    uint16_t    duration    :10;
} BTN_STATE_S;

typedef struct btn_interval_s
{
    uint8_t     dithering;	/* interval for dithering elimination */
    uint16_t    long_press;	/* interval for first into focused status */
	uint8_t     continuous;	/* interval for continuous press on focused status */
} BTN_INTERVAL_S;

typedef struct button_s
{
    BTN_STATE_S     state;
    BTN_INTERVAL_S  interval;
	uint16_t		cont_count;		/* count of the continuous press */
	uint8_t			detect_count;	/* count of need to detect from pin */
    int             (*check_active)(struct button_s *);
} BUTTON_S;

int button_check_active(BUTTON_S *button);

#ifdef __cplusplus
}
#endif

#endif /* _COBRA_BUTTON_H_ */
