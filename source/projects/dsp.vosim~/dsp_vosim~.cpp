/**
    @file
    dsp.vosim~: daisysp band limited oscillator
*/
#include "vosim.h"
#include <cstdlib>

#include "ext.h"
#include "ext_obex.h"
#include "z_dsp.h"


typedef struct _mdsp {
    t_pxobject ob;              // the object itself (t_pxobject in MSP instead of t_object)
    daisysp::VosimOscillator* osc;        // daisy band limited osc object
    double freq;                // Set carrier frequency in Hz.
    double form1_freq;          // Set formant 1 frequency in Hz.
    double form2_freq;          // Set formant 2 frequency in Hz.
    double shape;               // Shape to set. Works -1 to 1
} t_mdsp;


// method prototypes
void *mdsp_new(t_symbol *s, long argc, t_atom *argv);
void mdsp_free(t_mdsp *x);
void mdsp_assist(t_mdsp *x, void *b, long m, long a, char *s);
void mdsp_bang(t_mdsp *x);
void mdsp_anything(t_mdsp* x, t_symbol* s, long argc, t_atom* argv);
void mdsp_int(t_mdsp *x, long i);
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

    t_class *c = class_new("dsp.vosim~", (method)mdsp_new, (method)mdsp_free, (long)sizeof(t_mdsp), 0L, A_GIMME, 0);

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
        dsp_setup((t_pxobject *)x, 1);  // MSP inlets: arg is # of signal inlets and is REQUIRED!

        outlet_new(x, "signal");        // signal outlet (note "signal" rather than NULL)

        x->osc = new daisysp::VosimOscillator;
        x->freq = 100.0;
        x->form1_freq = 0.5;
        x->form2_freq = 0.5;
        x->shape = 0.0;
    }
    return (x);
}


void mdsp_free(t_mdsp *x)
{
    delete x->osc;
    dsp_free((t_pxobject *)x);
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
    if (s != gensym("") && argc > 0) {
        if (s == gensym("freq")) {
            x->freq = atom_getfloat(argv);
        }
        else if (s == gensym("form1_freq")) {
            x->form1_freq = atom_getfloat(argv);
        }
        else if (s == gensym("form2_freq")) {
            x->form2_freq = atom_getfloat(argv);
        }
        else if (s == gensym("shape")) {
            x->shape = atom_getfloat(argv);
        }
    }
}


void mdsp_dsp64(t_mdsp *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags)
{
    // post("sample rate: %f", samplerate);
    // post("maxvectorsize: %d", maxvectorsize);

    x->osc->Init(samplerate);

    object_method(dsp64, gensym("dsp_add64"), x, mdsp_perform64, 0, NULL);
}


void mdsp_perform64(t_mdsp *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long sampleframes, long flags, void *userparam)
{
    t_double *inL = ins[0];     // we get audio for each inlet of the object from the **ins argument
    t_double *outL = outs[0];   // we get audio for each outlet of the object from the **outs argument
    int n = sampleframes;       // n = 64
    x->osc->SetFreq(x->freq);
    x->osc->SetForm1Freq(x->form1_freq);
    x->osc->SetForm2Freq(x->form2_freq);
    x->osc->SetShape(x->shape);

    while (n--) {
        *outL++ = x->osc->Process();
    }
}
