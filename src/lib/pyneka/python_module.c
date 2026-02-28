#include <external.h>
#include <caneka.h>

static PyObject *PynekaError = NULL;

static boolean _canekaInit = FALSE;

typedef struct pyneka {
    PyObject_HEAD
    MemCh *m;
    Task *rootTask;
} PynekaCtx;

static void PynekaCtx_dealloc(PyObject *op)
{
    PynekaCtx *self = (PynekaCtx *) op;
    Py_TYPE(self)->tp_free(self);
}

static PyObject *PynekaCtx_new(PyTypeObject *type,
        PyObject *args, PyObject *kwargs){
    PynekaCtx *pnk = (PynekaCtx *)type->tp_alloc(type, 0);
    if(pnk != NULL){
        if(!_canekaInit){
            if(MemBook_Make(NULL) == NULL){
                PyErr_SetString(PynekaError, "Pyneka MemBook is null");
                return NULL;
            }
            _canekaInit = TRUE;
        }

        pnk->m = MemCh_Make();
        if(pnk->m == NULL){
            PyErr_SetString(PynekaError, "Pyneka MemChapter is null");
            return NULL;
        }

        Caneka_Init(pnk->m);
        Inter_Init(pnk->m);
        Ansi_SetColor(FALSE);
    }
    return (PyObject *)pnk;
}

static int PynekaCtx_init(PyObject *op, PyObject *args, PyObject *kwds)
{
    PynekaCtx *self = (PynekaCtx *) op;


    DebugStack_Push(NULL, 0);
    return 0;
}

static PyObject *PynekaCtx_memstats(PyObject *op, PyObject *Py_UNUSED(dummy))
{
    PynekaCtx *self = (PynekaCtx *) op;

    self->m->level++;
    MemBookStats st;
    MemBook_GetStats(self->m, &st);
    Buff *bf = Buff_Make(self->m, ZERO);

    void *args[5];
    args[0] = Str_MemCount(bf->m, st.total * PAGE_SIZE);
    args[1] = I64_Wrapped(bf->m, st.total);
    args[2] = I64_Wrapped(bf->m, st.pageIdx);
    args[3] = I64_Wrapped(bf->m, PAGE_SIZE);
    args[4] = NULL;

    Fmt(bf, "MemStat<$ total/maxIdx=$/$ page-size=$b>", args);

    PyObject *response = PyUnicode_FromString(Ifc(bf->m, bf->v, TYPE_CSTR));

    self->m->level--;
    MemCh_FreeTemp(self->m);

    return response;
}

static PyMethodDef PynekaCtx_methods[] = {
    {"memstats", PynekaCtx_memstats, METH_NOARGS,
     "Return a string describing the memory usage of the Caneka instance referenced by the Python Object instance."
    },
    {NULL}
};

static PyTypeObject PynekaCtxType = {
    .ob_base = PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "pyneka.PynekaCtx",
    .tp_doc = "Pyneka is a Python Object which has a reference to a few pointers to Caneka memory objects and a pointer to a Caneka Task to use as a root Task.",
    .tp_basicsize = sizeof(PynekaCtx),
    .tp_itemsize = 0,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_new = PynekaCtx_new,
    .tp_init = PynekaCtx_init,
    .tp_dealloc = PynekaCtx_dealloc,
    .tp_methods = PynekaCtx_methods,
};

static int pyneka_module_exec(PyObject *m){
    if (PynekaError != NULL) {
        PyErr_SetString(PyExc_ImportError,
            "Error Pyneka initialize exec already called");
        return -1;
    }

    PynekaError = PyErr_NewException("pyneka.error", NULL, NULL);
    if (PyModule_AddObjectRef(m, "PynekaError", PynekaError) < 0) {
        return -1;
    }

    if(PyType_Ready(&PynekaCtxType) < 0){
        return -1;
    }

    if(PyModule_AddObjectRef(m, "PynekaCtx", (PyObject *)&PynekaCtxType) < 0){
        return -1;
    }

    return 0;
}

static PyModuleDef_Slot pyneka_module_slots[] = {
    {Py_mod_exec, pyneka_module_exec},
    {Py_mod_multiple_interpreters, Py_MOD_MULTIPLE_INTERPRETERS_NOT_SUPPORTED},
    {0, NULL}
};

static struct PyModuleDef pyneka_module = {
    .m_base = PyModuleDef_HEAD_INIT,
    .m_name = "pyneka",
    .m_doc = "Pyneka is a wrapper around a Caneka runtime instance",
    .m_size = 0,
    .m_slots = pyneka_module_slots,
};

PyMODINIT_FUNC PyInit_pyneka(void){
    return PyModuleDef_Init(&pyneka_module);
}
