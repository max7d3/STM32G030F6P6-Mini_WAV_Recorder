#ifndef _PWR_H_
#define _PWR_H_

uint8_t show_battery_status;
uint32_t batter_voltage;

void PWR_ShowBatStatus(void);
void PWR_UpdateBatLevel(void);

void PWR_EnterSleep(int compatibility);

#endif /* _PWR_H_ */