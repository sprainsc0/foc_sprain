#include <utils.h>
#include "pid.h"

// Constructor
PID::PID(float initial_p, float initial_i, float initial_d, float initial_imax, float initial_filt_hz, float dt, float initial_ff) :
    _dt(dt),
    _integrator(0.0f),
    _input(0.0f),
    _derivative(0.0f),
    _reset_filter(true)
{
    _kp = initial_p;
    _ki = initial_i;
    _kd = initial_d;
    _imax = fabsf(initial_imax);
    _filt_hz = fabsf(initial_filt_hz);
    _ff = initial_ff;
}

// set_dt - set time step in seconds
void PID::set_dt(float dt)
{
    // set dt and calculate the input filter alpha
    _dt = dt;
}

void PID::set_input_filter_all(float input)
{
    // reset input filter to value received
    if (_reset_filter) {
        _reset_filter = false;
        _input = input;
        _derivative = 0.0f;
    }

    // update filter and calculate derivative
    float input_filt_change = get_filt_alpha() * (input - _input);
    _input = _input + input_filt_change;
    if (_dt > 0.0f) {
        _derivative = input_filt_change / _dt;
    }
}

void PID::set_input_filter_d(float input)
{
    // don't process inf or NaN

    if(_reset_filter) {
        _reset_filter = false;
        _reset_filter = 0.0f;
    }

    // update filter and calculate derivative
    if (_dt > 0.0f) {
        float derivative = (input - _input) / _dt;
        _derivative = _derivative + get_filt_alpha() * (derivative-_derivative);
    }

    _input = input;
}

float PID::get_p()
{
    return _input * _kp;
}

float PID::get_i()
{
    if(!is_zero(_ki) && !is_zero(_dt)) {
        _integrator += ((float)_input * _ki) * _dt;
        if (_integrator < -_imax) {
            _integrator = -_imax;
        } else if (_integrator > _imax) {
            _integrator = _imax;
        }

        return _integrator;
    }
    return 0;
}

float PID::get_d()
{
    // derivative component
    return _kd * _derivative;
}

float PID::get_ff(float requested_rate)
{
    return (float)requested_rate * _ff;
}


float PID::get_pi()
{
    return get_p() + get_i();
}

float PID::get_pid()
{
    return get_p() + get_i() + get_d();
}

void PID::reset_I()
{
    _integrator = 0;
}

/// Overload the function call operator to permit easy initialisation
void PID::operator() (float p, float i, float d, float imaxval, float input_filt_hz, float dt, float ffval)
{
    _kp = p;
    _ki = i;
    _kd = d;
    _imax = fabsf(imaxval);
    _filt_hz = input_filt_hz;
    _dt = dt;
    _ff = ffval;
}

// calc_filt_alpha - recalculate the input filter alpha
float PID::get_filt_alpha() const
{
    if (is_zero(_filt_hz)) {
        return 1.0f;
    }

    // calculate alpha
    float rc = 1/(M_2PI*_filt_hz);
    return _dt / (_dt + rc);
}
