/*
 zns.onemess - like onebang, but for all messages
	
    Developed by Zachary Seldess, King Abdullah University of Science and Technology
*/

#include "ext.h" // standard Max include, always required
#include "ext_obex.h" // required for new style Max object
#include <math.h>

////////////////////////// object struct
typedef struct _onemess 
{
    t_object s_obj; // the object itself (must be first)
    long s_pass; // value to control outlet passing
    long m_in; // space for inlet number used by proxy
    void *m_proxy; // right inlet
    void *m_outlet1; // left outlet
    void *m_outlet2; // right outlet
} t_onemess;

///////////////////////// function prototypes
void *onemess_new(t_symbol *s, long argc, t_atom *argv);
void onemess_free(t_onemess *x);
void onemess_assist(t_onemess *x, void *b, long m, long a, char *s);

void onemess_int(t_onemess *x, long n);
void onemess_float(t_onemess *x, double f);
void onemess_list(t_onemess *x, t_symbol *s, long argc, t_atom *argv);
void onemess_anything(t_onemess *x, t_symbol *s, long argc, t_atom *argv);
void onemess_bang(t_onemess *x);

//////////////////////// global class pointer variable
void *onemess_class;


int C74_EXPORT main(void)
{	
    t_class *c;

    c = class_new("zns.onemess", (method)onemess_new, (method)onemess_free, (long)sizeof(t_onemess), 0L, A_GIMME, 0);

    class_addmethod(c, (method)onemess_int, "int", A_LONG, 0);
    class_addmethod(c, (method)onemess_float, "float", A_FLOAT, 0);
    class_addmethod(c, (method)onemess_list, "list", A_GIMME, 0);    
    class_addmethod(c, (method)onemess_anything, "anything", A_GIMME, 0);
    class_addmethod(c, (method)onemess_bang, "bang", 0);
    
    class_addmethod(c, (method)stdinletinfo, "inletinfo", A_CANT, 0); // all right inlets are cold
    class_addmethod(c, (method)onemess_assist, "assist", A_CANT, 0); // you CAN'T call this from the patcher

    class_register(CLASS_BOX, c); /* CLASS_NOBOX */
    onemess_class = c;
    
    post("zns.onemess - Copyright (c) 2023, Zachary Seldess. All rights reserved.");

    return 0;
}

void onemess_assist(t_onemess *x, void *b, long m, long a, char *s)
{
    if (m == ASSIST_INLET) { // inlet
        switch (a) {
            case 0:
                sprintf(s, "(bang/anything) Only first message gets through");
                break;
            case 1:
                sprintf(s, "(bang/anything) Set to let one message through");
                break;
            default:
                break;
        }
        //sprintf(s, "I am inlet %ld", a);
    } 
    else {	// outlet
        switch (a) {
            case 0:
                sprintf(s, "(bang/anything) First message");
                break;
            case 1:
                sprintf(s, "(bang/anything) All other messages");
                break;        
            default:
                break;
        } 
        //sprintf(s, "I am outlet %ld", a);
    }
}

void onemess_free(t_onemess *x)
{
    ;
}

void onemess_int(t_onemess *x, long n)
{
    switch (proxy_getinlet((t_object *)x)) {
        case 0:
            if (x->s_pass == 1) {
                outlet_int(x->m_outlet1, n);
                x->s_pass = 0;
            }
            else {
                outlet_int(x->m_outlet2, n);
            }
            break;
        case 1:
            if (x->s_pass == 0) {
                x->s_pass = 1;
            }
            break;
    }
}

void onemess_float(t_onemess *x, double f)
{
    switch (proxy_getinlet((t_object *)x)) {
        case 0:
            if (x->s_pass == 1) {
                outlet_float(x->m_outlet1, f);
                x->s_pass = 0;
            }
            else {
                outlet_float(x->m_outlet2, f);
            }
            break;
        case 1:
            if (x->s_pass == 0) {
                x->s_pass = 1;
            }
            break;
    }
}

void onemess_list(t_onemess *x, t_symbol *s, long argc, t_atom *argv)
{
    /*
    // report message
    long i;
    t_atom *ap;
    
    post("message selector is %s",s->s_name);
    post("there are %ld arguments",argc);
    // increment ap each time to get to the next atom
    for (i = 0, ap = argv; i < argc; i++, ap++) {
        switch (atom_gettype(ap)) {
            case A_LONG:
                post("%ld: %ld",i+1,atom_getlong(ap));
                break;
            case A_FLOAT:
                post("%ld: %.2f",i+1,atom_getfloat(ap));
                break;
            case A_SYM:
                post("%ld: %s",i+1, atom_getsym(ap)->s_name);
                break;
            default:
                post("%ld: unknown atom type (%ld)", i+1, atom_gettype(ap));
                break;
        }
    }
    */
    switch (proxy_getinlet((t_object *)x)) {
        case 0:
            if (x->s_pass == 1) {
                outlet_list(x->m_outlet1, s, argc, argv);
                x->s_pass = 0;
            }
            else {
                outlet_list(x->m_outlet2, s, argc, argv);
            }
            break;
        case 1:
            if (x->s_pass == 0) {
                x->s_pass = 1;
            }
            break;
    }
}

void onemess_anything(t_onemess *x, t_symbol *s, long argc, t_atom *argv)
{
    /*
    // report message
    long i;
    t_atom *ap;
    
    post("message selector is %s",s->s_name);
    post("there are %ld arguments",argc);
    // increment ap each time to get to the next atom
    for (i = 0, ap = argv; i < argc; i++, ap++) {
        switch (atom_gettype(ap)) {
            case A_LONG:
                post("%ld: %ld",i+1,atom_getlong(ap));
                break;
            case A_FLOAT:
                post("%ld: %.2f",i+1,atom_getfloat(ap));
                break;
            case A_SYM:
                post("%ld: %s",i+1, atom_getsym(ap)->s_name);
                break;
            default:
                post("%ld: unknown atom type (%ld)", i+1, atom_gettype(ap));
                break;
        }
    }
    */    
    switch (proxy_getinlet((t_object *)x)) {
        case 0:
            if (x->s_pass == 1) {
                outlet_anything(x->m_outlet1, s, argc, argv);
                x->s_pass = 0;
            }
            else {
                outlet_anything(x->m_outlet2, s, argc, argv);
            }
            break;
        case 1:
            if (x->s_pass == 0) {
                x->s_pass = 1;
            }
            break;
    }
}

void onemess_bang(t_onemess *x)
{
    switch (proxy_getinlet((t_object *)x)) {
        case 0:
            if (x->s_pass == 1) {
                outlet_bang(x->m_outlet1);
                x->s_pass = 0;
            }
            else {
                outlet_bang(x->m_outlet2);
            }
            break;
        case 1:
            if (x->s_pass == 0) {
                x->s_pass = 1;
            }
            break;
    }
}

void *onemess_new(t_symbol *s, long argc, t_atom *argv)
{
    t_onemess *x = NULL;
    //long i; 
	
    if ((x = (t_onemess *)object_alloc(onemess_class))) {
        x->m_proxy = proxy_new((t_object *)x, 1, &x->m_in); // proxy inlet
        x->m_outlet2 = outlet_new((t_object *)x, NULL); // right outlet
        x->m_outlet1 = outlet_new((t_object *)x, NULL); // left outlet
        /*
        object_post((t_object *)x, "a new %s object was instantiated: 0x%X", s->s_name, x);
        object_post((t_object *)x, "it has %ld arguments", argc);
        // report all provided arguments
        for (i = 0; i < argc; i++) {
            if ((argv + i)->a_type == A_LONG) {
                object_post((t_object *)x, "arg %ld: long (%ld)", i, atom_getlong(argv+i));
            } 
            else if ((argv + i)->a_type == A_FLOAT) {
                object_post((t_object *)x, "arg %ld: float (%f)", i, atom_getfloat(argv+i));
            } 
            else if ((argv + i)->a_type == A_SYM) {
                object_post((t_object *)x, "arg %ld: symbol (%s)", i, atom_getsym(argv+i)->s_name);
            }
            else {
                object_error((t_object *)x, "forbidden argument");
            }
        }
        */
        // set default object state
        x->s_pass = 0;
        
        // initialize with arg, if provided
        if (argc >= 1) {
            if ((argv)->a_type == A_LONG || (argv)->a_type == A_FLOAT) {
                if (atom_getlong(argv) != 0) {
                    x->s_pass = 1;
                }
                else {
                    x->s_pass = 0;
                }
            }
        }
    }
    return (x);
}
