#ifndef __PID_CONTROL_H__
#define __PID_CONTROL_H__

#include <stdlib.h>
#include <cmath>

#define AC_PID_FILT_HZ_DEFAULT  20.0f   // default input filter frequency
#define AC_PID_FILT_HZ_MIN      0.01f   // minimum input filter frequency

/// @class	AC_PID
/// @brief	Copter PID control class
class PID {
public:

    // Constructor for PID
    PID(float initial_p, float initial_i, float initial_d, float initial_imax, float initial_filt_hz, float dt, float initial_ff = 0);

    // set_dt - set time step in seconds
    void        set_dt(float dt);

    void        set_input_filter_all(float input);
    void        set_input_filter_d(float input);

    // get_pid - get results from pid controller
    float       get_pid();
    float       get_pi();
    float       get_p();
    float       get_i();
    float       get_d();
    float       get_ff(float requested_rate);
    
    // reset_I - reset the integrator
    void        reset_I();

    // reset_filter - input filter will be reset to the next value provided to set_input()
    void        reset_filter() { _reset_filter = true; }

    /// operator function call for easy initialisation
    void operator() (float p, float i, float d, float imaxval, float input_filt_hz, float dt, float ffval = 0);

    float       get_filt_alpha() const;

    // set accessors
    void        kP(const float v) { _kp = v; }
    void        kI(const float v) { _ki = v; }
    void        kD(const float v) { _kd = v; }
    void        imax(const float v) { _imax = fabsf(v); }
    void        ff(const float v) { _ff = v; }

    float       get_integrator() const { return _integrator; }
    void        set_integrator(float i) { _integrator = i; }

protected:

    // parameters
    float        _kp;
    float        _ki;
    float        _kd;
    float        _imax;
    float        _filt_hz;                   // PID Input filter frequency in Hz
    float        _ff;

    // internal variables
    float           _dt;                    // timestep in seconds
    float           _integrator;            // integrator value
    float           _input;                 // last input for derivative
    float           _derivative;            // last derivative for low-pass filter

    bool            _reset_filter;
};

#endif