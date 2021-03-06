
#include <mach/mt6573_reg_base.h>
//Register address define
#define MIC_BIAS_REG 0xF702F630
#define VMIC_CON1 0xF702F634
#define PMIC_RESERVE_CON2 0xF702FE88

#define ACCDET_RSTB ACCDET_BASE
#define ACCDET_CTRL ACCDET_BASE + 0x0004
#define ACCDET_STATE_SWCTRL ACCDET_BASE + 0x0008
#define ACCDET_PWM_WIDTH ACCDET_BASE + 0x000c

#define ACCDET_PWM_THRESH ACCDET_BASE + 0x0010
#define ACCDET_EN_DELAY_NUM ACCDET_BASE + 0x0024
#define ACCDET_PWM_IDLE_VALUE ACCDET_BASE + 0x0028
#define ACCDET_DEBOUNCE0 ACCDET_BASE + 0x002c
#define ACCDET_DEBOUNCE1 ACCDET_BASE + 0x0030
#define ACCDET_DEBOUNCE2 ACCDET_BASE + 0x0034
#define ACCDET_DEBOUNCE3 ACCDET_BASE + 0x0038


#define ACCDET_IRQ_STS ACCDET_BASE + 0x003c
#define ACCDET_CURR_IN ACCDET_BASE + 0x0040
#define ACCDET_SAMPLE_IN ACCDET_BASE + 0x0044
#define ACCDET_MEMORIZED_IN ACCDET_BASE + 0x0048


#define ACCDET_TV_START_LINE0 ACCDET_BASE + 0x0058
#define ACCDET_TV_END_LINE0 ACCDET_BASE + 0x005C
#define ACCDET_TV_START_LINE1 ACCDET_BASE + 0x0060
#define ACCDET_TV_END_LINE1 ACCDET_BASE + 0x0064
#define ACCDET_TV_PRE_LINE ACCDET_BASE + 0x0068
#define ACCDET_TV_START_PXL ACCDET_BASE + 0x006c
#define ACCDET_TV_END_PXL ACCDET_BASE + 0x0070
#define ACCDET_TV_EN_DELAY_NUM ACCDET_BASE + 0x0074
#define ACCDET_TV_DIV_RATE ACCDET_BASE + 0x0078

//Register value define
#define RSTB_BIT        (0<<0)
#define RSTB_FINISH_BIT (1<<0)
#define MIC_INIT_BIT (1<<1)
#define TV_INIT_BIT (1<<2)

#define IRQ_STATUS_BIT  (1<<0)
#define IRQ_CLR_BIT     (1<<8)
#define IRQ_CLR_FINISH  (0<<8)

#define ACCDET_ENABLE   (1<<0)
#define ACCDET_DISABLE  (0<<0)
#define ANASW_BIT (1<<0)
#define TV_DET_BIT (1<<1)
#define CMP_PWM_EN_BIT (1<<2)
#define VTH_PWM_EN_BIT (1<<3)
#define MBIAS_PWM_EN_BIT (1<<4)
#define ACCDET_SWCTRL_EN (MBIAS_PWM_EN_BIT | VTH_PWM_EN_BIT | CMP_PWM_EN_BIT | ANASW_BIT)

//PMIC reserve CON2 value define
#define MIC_FORCE_LOW   (1<<3)

#define VMIC_EN_BIT (1<<0)
#define VMIC_LOW_POWER_BIT (1<<2)

