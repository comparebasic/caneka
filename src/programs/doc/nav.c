#include <external.h>
#include <caneka.h>
#include <doc_module.h>

void Doc_GenNav(NodeObj *config, Span *files, WwwNav *nav){
    MemCh *m = config->m;

    NodeObj *out = Inst_ByPath(config,
        Sv(m, "out"), NULL, SPAN_OP_GET, NULL);

    StrVec *outDir = IoUtil_AbsVec(m, Inst_Att(out, K(m, "dir")));

    Iter it;
    Iter_Init(&it, files);
    while((Iter_Next(&it) & END) == 0){
        StrVec *file = Iter_Get(&it);

        Str *last = Span_Get(file->p, file->p->max_idx);
        StrVec *route = StrVec_SubVec(m,
            file,
            Path_FlagIdx(file, (MORE|NOOP)),
            Path_FlagIdx(file, (LAST))-1);

        StrVec *outPath = StrVec_SubVec(m,
            file,
            Path_FlagIdx(file, (MORE|NOOP))+1,
            Path_FlagIdx(file, (LAST)));

        StrVec *display = StrVec_Copy(m, outPath);
        StrVec_Pop(display);
        StrVec_Pop(display);

        Str *div = Span_Get(route->p, 0);
        div->type.state &= ~NOOP;

        Str *name = Str_Clone(m, Span_Get(route->p, route->p->max_idx)); 
        Str_ToTitle(m, name);
        WwwNav *item = WwwNav_Make(m, route, StrVec_From(m, name));

        Table *coords = Inst_Att(nav, K(m, "coords"));

        StrVec_Add(outPath, S(m, "html"));
        Inst_SetAtt(item, K(m, "out-path"), outPath);

        Inst_SetAtt(item, K(m, "fpath"), file);

        Str *sep = Str_Ref(m, (byte *)".", 1, 2, MORE);
        StrVec_Add(display, name);
        Path_SwapSep(display, sep, MORE); 

        Inst_SetAtt(item, K(m, "display-path"), display);

        WwwNav_Add(nav, item, coords);
    }

    Iter2d *it2d = Iter2d_Make(m, nav);
    for(;(Iter2d_State(it2d) & END) == 0; Iter2d_InstNext(it2d)){
        Abstract *a = Iter2d_Get(it2d);
        if(a->type.of == TYPE_WWW_NAV){
            WwwNav *nav = (WwwNav *)a;
            if(Empty(Seel_Get(nav, K(m, "name")))){
                void *ar[] = {nav, NULL};
                Out("^c.Nav Item @^0\n", ar);
            }
        }else{
            void *ar[] = {a, NULL};
            Out("^c.Non Nav Item @^0\n", ar);
        }

        /*
        route = StrVec_SubVec(m,
            file,
            Path_FlagIdx(file, (MORE|NOOP))+1,
            file->p->max_idx);

        outPath = StrVec_SubVec(m,
            file,
            Path_FlagIdx(file, (MORE|NOOP))+1,
            file->p->max_idx);
        IoUtil_AddVec(m, file, IoPath(m, "index.html"));
        */

    }

    return;
}
