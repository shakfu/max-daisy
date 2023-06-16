/**
    @file
    dsp.moog~: daisy_sp moog ladder filter
*/
#include "moogladder.h"
#include <cstdlib>

#include "ext.h"
#include "ext_obex.h"
#include "z_dsp.h"

#define N_CHANNELS 1

typedef struct _mxd {
    t_pxobject ob;                  // the object itself (t_pxobject in MSP instead of t_object)
    daisysp::MoogLadder* filter;    // daisy rev object
    double freq;                    // Sets the cutoff frequency in Hz
    double res;                     // Sets the resonance of the filter.
} t_mxd;


// method prototypes
void *mxd_new(t_symbol *s, long argc, t_atom *argv);
void mxd_free(t_mxd *x);
void mxd_assist(t_mxd *x, void *b, long m, long a, char *s);
void mxd_bang(t_mxd *x);
void mxd_anything(t_mxd* x, t_symbol* s, long argc, t_atom* argv);
void mxd_dsp64(t_mxd *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags);
void mxd_perform64(t_mxd *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long sampleframes, long flags, void *userparam);


// global class pointer variable
static t_class *mxd_class = NULL;


//-----------------------------------------------------------------------------------------------

void ext_main(void *r)
{
    t_class *c = class_new("dsp.moog~", (method)mxd_new, (method)mxd_free, (long)sizeof(t_mxd), 0L, A_GIMME, 0);

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
        dsp_setup((t_pxobject *)x, N_CHANNELS);

        outlet_new(x, "signal"); 
        
        x->filter = new daisysp::MoogLadder;
        x->freq = 100.0;
        x->res = 0.5;
    }
    return (x);
}


void mxd_free(t_mxd *x)
{
    delete x->filter;
    dsp_free((t_pxobject *)x);
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
    if (s != gensym("") && argc > 0) {
        if (s == gensym("freq")) {
            x->freq = atom_getfloat(argv);

        }
        else if (s == gensym("res")) {
            x->res = atom_getfloat(argv);
        }
    }
}



void mxd_dsp64(t_mxd *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags)
{
    // post("sample rate: %f", samplerate);
    // post("maxvectorsize: %d", maxvectorsize);

    x->filter->Init(samplerate);

    object_method(dsp64, gensym("dsp_add64"), x, mxd_perform64, 0, NULL);
}


void mxd_perform64(t_mxd *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long sampleframes, long flags, void *userparam)
{
    t_double *inL = ins[0];     // we get audio for each inlet of the object from the **ins argument
    t_double *outL = outs[0];   // we get audio for each outlet of the object from the **outs argument

    int n = sampleframes;       // n = 64
    x->filter->SetFreq(x->freq);
    x->filter->SetRes(x->res);
    float in_left;

    while (n--) {
        in_left = *inL++;
        *outL++ = (t_double)x->filter->Process(in_left);
    }
}
