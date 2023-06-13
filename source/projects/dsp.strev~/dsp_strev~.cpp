/**
    @file
    dsp.osc~: mdsp sine for Max
*/
#include "reverbsc.h"
#include <cstdlib>

#include "ext.h"
#include "ext_obex.h"
#include "z_dsp.h"


enum {
    // FEEDBACK is assigned to default left inlet
    LP_FREQ = 1, 
    MAX_INLET_INDEX // -> maximum number of inlets (0-based)
};


// struct to represent the object's state
typedef struct _mdsp {
    t_pxobject ob;              // the object itself (t_pxobject in MSP instead of t_object)
    daisysp::ReverbSc* rev;     // daisy rev object
    double feedback;            // controls the internal dampening filter's cutoff frequency.
                                // param freq - low pass frequency. range: 0.0 to sample_rate / 2
    double lp_freq;             // Sets the amplitude of the waveform.
    long m_in;                  // space for the inlet number used by all the proxies
    void *inlets[MAX_INLET_INDEX];
} t_mdsp;


// method prototypes
void *mdsp_new(t_symbol *s, long argc, t_atom *argv);
void mdsp_free(t_mdsp *x);
void mdsp_assist(t_mdsp *x, void *b, long m, long a, char *s);
void mdsp_bang(t_mdsp *x);
void mdsp_anything(t_mdsp* x, t_symbol* s, long argc, t_atom* argv);
void mdsp_float(t_mdsp *x, double f);
void mdsp_dsp64(t_mdsp *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags);
void mdsp_perform64(t_mdsp *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long sampleframes, long flags, void *userparam);


// global class pointer variable
static t_class *mdsp_class = NULL;


//-----------------------------------------------------------------------------------------------

void ext_main(void *r)
{
    // object initialization, note the use of dsp_free for the freemethod, which is required
    // unless you need to free allocated memory, in which case you should call dsp_free from
    // your custom free function.

    t_class *c = class_new("dsp.strev~", (method)mdsp_new, (method)mdsp_free, (long)sizeof(t_mdsp), 0L, A_GIMME, 0);

    class_addmethod(c, (method)mdsp_float,    "float",    A_FLOAT,   0);
    class_addmethod(c, (method)mdsp_anything, "anything", A_GIMME,   0);
    class_addmethod(c, (method)mdsp_bang,     "bang",                0);
    class_addmethod(c, (method)mdsp_dsp64,    "dsp64",    A_CANT,    0);
    class_addmethod(c, (method)mdsp_assist,   "assist",   A_CANT,    0);

    class_dspinit(c);
    class_register(CLASS_BOX, c);
    mdsp_class = c;
}

void *mdsp_new(t_symbol *s, long argc, t_atom *argv)
{
    t_mdsp *x = (t_mdsp *)object_alloc(mdsp_class);

    if (x) {
        dsp_setup((t_pxobject *)x, 2);  // MSP inlets: arg is # of signal inlets and is REQUIRED!
        // use 0 if you don't need signal inlets

        outlet_new(x, "signal");        // signal outlet (note "signal" rather than NULL)
        outlet_new(x, "signal");        // signal outlet (note "signal" rather than NULL)
        

        for(int i = (MAX_INLET_INDEX - 1); i > 0; i--) {
            x->inlets[i] = proxy_new((t_object *)x, i, &x->m_in);
        }

        x->rev = new daisysp::ReverbSc;
        x->feedback = 100.0;
        x->lp_freq = 0.5;
    }
    return (x);
}


void mdsp_free(t_mdsp *x)
{
    delete x->rev;
    dsp_free((t_pxobject *)x);
    for(int i = (MAX_INLET_INDEX - 1); i > 0; i--) {
        object_free(x->inlets[i]);
    }

}


void mdsp_assist(t_mdsp *x, void *b, long m, long a, char *s)
{
    // FIXME: assign to inlets
    if (m == ASSIST_INLET) { //inlet
        sprintf(s, "I am inlet %ld", a);
    }
    else {  // outlet
        sprintf(s, "I am outlet %ld", a);
    }
}

void mdsp_bang(t_mdsp *x)
{
    post("bang");
}

void mdsp_anything(t_mdsp* x, t_symbol* s, long argc, t_atom* argv)
{

    if (s != gensym("")) {
        post("symbol: %s", s->s_name);
    }
}


void mdsp_float(t_mdsp *x, double f)
{
    switch (proxy_getinlet((t_object *)x)) {
        case 0:
            x->feedback = f;
            break;
        case 1:
            x->lp_freq = f;
            break;
    }
}

void mdsp_dsp64(t_mdsp *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags)
{
    post("sample rate: %f", samplerate);
    post("maxvectorsize: %d", maxvectorsize);

    x->rev->Init(samplerate);

    object_method(dsp64, gensym("dsp_add64"), x, mdsp_perform64, 0, NULL);
}


void mdsp_perform64(t_mdsp *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long sampleframes, long flags, void *userparam)
{
    t_double *inL = ins[0];     // we get audio for each inlet of the object from the **ins argument
    t_double *inR = ins[1];     // we get audio for each inlet of the object from the **ins argument
    t_double *outL = outs[0];   // we get audio for each outlet of the object from the **outs argument
    t_double *outR = outs[1];   // we get audio for each outlet of the object from the **outs argument

    int n = sampleframes;       // n = 64
    x->rev->SetFeedback(x->feedback);
    x->rev->SetLpFreq(x->lp_freq);
    float in_left;
    float in_right;
    float out_left;
    float out_right;

    // while (n--) {
    //     *outL++ = *inL++;
    //     *outR++ = *inR++;
    // }

    while (n--) {
        in_left = *inL++;
        in_right = *inR++;
        x->rev->Process(in_left, in_right, &out_left, &out_right);
        *outL++ = (t_double)out_left;
        *outR++ = (t_double)out_right;
        // x->rev->Process(const float &in1, const float &in2, float *out1, float *out2);
        // *outL++ = x->rev->Process();
    }
}
