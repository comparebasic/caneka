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

    return;
}
