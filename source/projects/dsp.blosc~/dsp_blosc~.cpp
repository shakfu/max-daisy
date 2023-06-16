/**
    @file
    dsp.blosc~: daisysp band limited oscillator
*/
#include "blosc.h"
#include <cstdlib>

#include "ext.h"
#include "ext_obex.h"
#include "z_dsp.h"


enum {
    // FREQ is assigned to default left inlet
    AMP = 1, 
    PULSE_WIDTH,
    MAX_INLET_INDEX // -> maximum number of inlets (0-based)
};


// struct to represent the object's state
typedef struct _mxd {
    t_pxobject ob;              // the object itself (t_pxobject in MSP instead of t_object)
    daisysp::BlOsc* osc;        // daisy band limited osc object
    double freq;                // Float freq: Set oscillator frequency in Hz.
    double amp;                 // Float amp: Set oscillator amplitude, 0 to 1.
    int waveform;               // waveform: select between waveforms from enum. i.e. SetWaveform(BL_WAVEFORM_SAW); to set waveform to saw
    double pulse_width;         // Float pw: Set square osc pulsewidth, 0 to 1. (no thru 0 at the moment)
    long m_in;                  // space for the inlet number used by all the proxies
    void *inlets[MAX_INLET_INDEX];
    // t_outlet *outlet; 
} t_mxd;


// method prototypes
void *mxd_new(t_symbol *s, long argc, t_atom *argv);
void mxd_free(t_mxd *x);
void mxd_assist(t_mxd *x, void *b, long m, long a, char *s);
void mxd_bang(t_mxd *x);
void mxd_anything(t_mxd* x, t_symbol* s, long argc, t_atom* argv);
void mxd_float(t_mxd *x, double f);
void mxd_int(t_mxd *x, long i);
void mxd_dsp64(t_mxd *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags);
void mxd_perform64(t_mxd *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long sampleframes, long flags, void *userparam);


// global class pointer variable
static t_class *mxd_class = NULL;


//-----------------------------------------------------------------------------------------------

void ext_main(void *r)
{
    // object initialization, note the use of dsp_free for the freemethod, which is required
    // unless you need to free allocated memory, in which case you should call dsp_free from
    // your custom free function.

    t_class *c = class_new("dsp.blosc~", (method)mxd_new, (method)mxd_free, (long)sizeof(t_mxd), 0L, A_GIMME, 0);

    class_addmethod(c, (method)mxd_float,    "float",    A_FLOAT,   0);
    class_addmethod(c, (method)mxd_int,      "int",      A_DEFLONG, 0);    
    class_addmethod(c, (method)mxd_anything, "anything", A_GIMME,   0);
    class_addmethod(c, (method)mxd_bang,     "bang",                0);
    class_addmethod(c, (method)mxd_dsp64,    "dsp64",    A_CANT,    0);
    class_addmethod(c, (method)mxd_assist,   "assist",   A_CANT,    0);

    class_dspinit(c);
    class_register(CLASS_BOX, c);
    mxd_class = c;
}

void *mxd_new(t_symbol *s, long argc, t_atom *argv)
{
    t_mxd *x = (t_mxd *)object_alloc(mxd_class);

    if (x) {
        dsp_setup((t_pxobject *)x, 1);  // MSP inlets: arg is # of signal inlets and is REQUIRED!
        // use 0 if you don't need signal inlets

        // x->outlet = bangout(x);      // optional outlet to bang out at end of cycle
        outlet_new(x, "signal");        // signal outlet (note "signal" rather than NULL)
        

        for(int i = (MAX_INLET_INDEX - 1); i > 0; i--) {
            x->inlets[i] = proxy_new((t_object *)x, i, &x->m_in);
        }

        x->osc = new daisysp::BlOsc;
        x->freq = 100.0;
        x->amp = 0.5;
        x->waveform = daisysp::BlOsc::WAVE_TRIANGLE;
        x->pulse_width = 0.5;
    }
    return (x);
}


void mxd_free(t_mxd *x)
{
    delete x->osc;
    dsp_free((t_pxobject *)x);
    for(int i = (MAX_INLET_INDEX - 1); i > 0; i--) {
        object_free(x->inlets[i]);
    }

}


void mxd_assist(t_mxd *x, void *b, long m, long a, char *s)
{
    // FIXME: assign to inlets
    if (m == ASSIST_INLET) { //inlet
        sprintf(s, "I am inlet %ld", a);
    }
    else {  // outlet
        sprintf(s, "I am outlet %ld", a);
    }
}

void mxd_bang(t_mxd *x)
{
    post("bang");
}

void mxd_anything(t_mxd* x, t_symbol* s, long argc, t_atom* argv)
{

    if (s != gensym("")) {
        post("symbol: %s", s->s_name);
    }
}


void mxd_float(t_mxd *x, double f)
{
    switch (proxy_getinlet((t_object *)x)) {
        case 0:
            x->freq = f;
            break;
        case 1:
            x->amp = f;
            break;
        case 2:
            x->pulse_width = f;
            break;
    }
}

void mxd_int(t_mxd *x, long i)
{
    // post("long: %d", i);
    if (i < 5) {
        x->osc->SetWaveform(i);
    } else {
        error("out of range: 0 WAVE_TRIANGLE, 1 WAVE_SAW, 2, WAVE_SQUARE, 3 WAVE_OFF");
    }
}

void mxd_dsp64(t_mxd *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags)
{
    post("sample rate: %f", samplerate);
    post("maxvectorsize: %d", maxvectorsize);

    x->osc->Init(samplerate);
    x->osc->Reset();

    object_method(dsp64, gensym("dsp_add64"), x, mxd_perform64, 0, NULL);
}


void mxd_perform64(t_mxd *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long sampleframes, long flags, void *userparam)
{
    t_double *inL = ins[0];     // we get audio for each inlet of the object from the **ins argument
    t_double *outL = outs[0];   // we get audio for each outlet of the object from the **outs argument
    int n = sampleframes;       // n = 64
    x->osc->SetFreq(x->freq);
    x->osc->SetAmp(x->amp);
    x->osc->SetPw(x->pulse_width);

    while (n--) {
        *outL++ = x->osc->Process();
    }
}
