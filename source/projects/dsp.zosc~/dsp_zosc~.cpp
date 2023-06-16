/**
    @file
    dsp.zosc~: daisysp Sinewave multiplied by and sync'ed to a carrier.
*/
#include "zoscillator.h"
#include <cstdlib>

#include "ext.h"
#include "ext_obex.h"
#include "z_dsp.h"


typedef struct _mxd {
    t_pxobject ob;              // the object itself (t_pxobject in MSP instead of t_object)
    daisysp::ZOscillator* osc;  // daisy zosc object
    double freq;                // Set carrier frequency in Hz.
    double formant_freq;        // Set formant frequency in Hz.
    double shape;               // Adjust the contour of the waveform. (0-1)
    double mode;                // Set the offset amount and phase shift. (-1 to 1)
} t_mxd;


// method prototypes
void *mxd_new(t_symbol *s, long argc, t_atom *argv);
void mxd_free(t_mxd *x);
void mxd_assist(t_mxd *x, void *b, long m, long a, char *s);
void mxd_bang(t_mxd *x);
void mxd_anything(t_mxd* x, t_symbol* s, long argc, t_atom* argv);
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

    t_class *c = class_new("dsp.zosc~", (method)mxd_new, (method)mxd_free, (long)sizeof(t_mxd), 0L, A_GIMME, 0);

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

        outlet_new(x, "signal");        // signal outlet (note "signal" rather than NULL)

        x->osc = new daisysp::ZOscillator;
        x->freq = 100.0;
        x->formant_freq = 0.5;
        x->shape = 0.0;
        x->mode = 0.0;

    }
    return (x);
}


void mxd_free(t_mxd *x)
{
    delete x->osc;
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
        else if (s == gensym("formant_freq")) {
            x->formant_freq = atom_getfloat(argv);
        }
        else if (s == gensym("shape")) {
            x->shape = atom_getfloat(argv);
        }
        else if (s == gensym("mode")) {
            x->mode = atom_getfloat(argv);
        }
    }
}


void mxd_dsp64(t_mxd *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags)
{
    // post("sample rate: %f", samplerate);
    // post("maxvectorsize: %d", maxvectorsize);

    x->osc->Init(samplerate);

    object_method(dsp64, gensym("dsp_add64"), x, mxd_perform64, 0, NULL);
}


void mxd_perform64(t_mxd *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long sampleframes, long flags, void *userparam)
{
    t_double *inL = ins[0];     // we get audio for each inlet of the object from the **ins argument
    t_double *outL = outs[0];   // we get audio for each outlet of the object from the **outs argument
    int n = sampleframes;       // n = 64
    x->osc->SetFreq(x->freq);
    x->osc->SetFormantFreq(x->formant_freq);
    x->osc->SetShape(x->shape);
    x->osc->SetMode(x->mode);

    while (n--) {
        *outL++ = x->osc->Process();
    }
}
