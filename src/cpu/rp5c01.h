#ifndef RP5C01_H
#define RP5C01_H

/**********************************************************************

    Ricoh RP5C01(A) Real Time Clock With Internal RAM emulation

    Copyright MESS Team.
    Visit http://mamedev.org for licensing and usage restrictions.

**********************************************************************
                            _____   _____
                   _CS   1 |*    \_/     | 18  Vcc
                    CS   2 |             | 17  OSCOUT
                   ADJ   3 |             | 16  OSCIN
                    A0   4 |   RP5C01    | 15  _ALARM
                    A1   5 |   RP5C01A   | 14  D3
                    A2   6 |   RF5C01A   | 13  D2
                    A3   7 |   TC8521    | 12  D1
                   _RD   8 |             | 11  D0
                   GND   9 |_____________| 10  _WR

**********************************************************************/

#if 0

#include <QObject>

class CPObject;


//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

class CRP5C01:public QObject {
public:
    const char*	GetClassName(){ return("CRP5C01");}
    // construction/destruction
//    CRP5C01(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);
    CRP5C01(CPObject *parent);
    virtual ~CRP5C01();

    bool	init(void);						//initialize
    bool	exit(void);						//end
    void	Reset(void);
    bool	step(void);

    void	Load_Internal(FILE *ffile);
    void	save_internal(FILE *file);

protected:
    // device-level overrides
    virtual void device_config_complete();
    virtual void device_start();
    virtual void device_reset();
    virtual void device_timer(emu_timer &timer, device_timer_id id, int param, void *ptr);

    // device_rtc_interface overrides
    virtual bool rtc_feature_leap_year() { return true; }
    virtual void rtc_clock_updated(int year, int month, int day, int day_of_week, int hour, int minute, int second);

private:
    inline void set_alarm_line();
    inline int read_counter(int counter);
    inline void write_counter(int counter, int value);
    inline void check_alarm();

    static const device_timer_id TIMER_CLOCK = 0;
    static const device_timer_id TIMER_16HZ = 1;

    devcb_resolved_write_line	m_out_alarm_func;

    quint8 m_reg[2][13];			// clock registers
    quint8 m_ram[13];			// RAM

    quint8 m_mode;				// mode register
    quint8 m_reset;				// reset register
    int m_alarm;				// alarm output
    int m_alarm_on;				// alarm condition
    int m_1hz;					// 1 Hz condition
    int m_16hz;					// 16 Hz condition

    // timers
    emu_timer *m_clock_timer;
    emu_timer *m_16hz_timer;
};

#endif

#endif // RP5C01_H
