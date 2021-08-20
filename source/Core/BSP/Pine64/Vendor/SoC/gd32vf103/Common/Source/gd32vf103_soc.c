#include "nuclei_sdk_soc.h"

static uint32_t get_timer_freq() { return SOC_TIMER_FREQ; }

uint32_t measure_cpu_freq(uint32_t n) {
  uint32_t start_mcycle, delta_mcycle;
  uint32_t start_mtime, delta_mtime;
  uint32_t mtime_freq = get_timer_freq();

  // Don't start measuruing until we see an mtime tick
  uint32_t tmp = (uint32_t)SysTimer_GetLoadValue();
  do {
    start_mtime  = (uint32_t)SysTimer_GetLoadValue();
    start_mcycle = __RV_CSR_READ(CSR_MCYCLE);
  } while (start_mtime == tmp);

  do {
    delta_mtime  = (uint32_t)SysTimer_GetLoadValue() - start_mtime;
    delta_mcycle = __RV_CSR_READ(CSR_MCYCLE) - start_mcycle;
  } while (delta_mtime < n);

  return (delta_mcycle / delta_mtime) * mtime_freq + ((delta_mcycle % delta_mtime) * mtime_freq) / delta_mtime;
}

uint32_t get_cpu_freq() {
  uint32_t cpu_freq;

  // warm up
  measure_cpu_freq(1);
  // measure for real
  cpu_freq = measure_cpu_freq(100);

  return cpu_freq;
}

/**
 * \brief      delay a time in milliseconds
 * \details
 *             provide API for delay
 * \param[in]  count: count in milliseconds
 * \remarks
 */
void delay_1ms(uint32_t count) {
  uint64_t end_mtime;
  uint64_t delay_ticks = ((SOC_TIMER_FREQ / 4) * (uint64_t)count) / 1000;

  end_mtime = SysTimer_GetLoadValue() + delay_ticks;

  do {
    asm("nop");
  } while (SysTimer_GetLoadValue() < end_mtime);
}
