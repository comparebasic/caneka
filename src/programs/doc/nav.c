#include <external.h>
#include <caneka.h>
#include <doc_module.h>

void Doc_GenNav(NodeObj *config, Span *files, WwwNav *nav){
    MemCh *m = config->m;

    NodeObj *out = Inst_ByPath(config,
        Sv(m, "out"), NULL, SPAN_OP_GET, NULL);

    NodeObj *src = Inst_ByPath(config, IoPath(m, "/in/src"), NULL, SPAN_OP_GET, NULL);

    StrVec *inDir = IoUtil_AbsVec(m, Inst_Att(src, K(m, "dir")));

    StrVec *rootPath = IoPath(m, "/");

    StrVec *outDir = IoUtil_AbsVec(m, Inst_Att(out, K(m, "dir")));
    Table *coordTbl = Inst_Att(nav, K(m, "coords"));

    Iter it;
    Iter_Init(&it, files);
    while((Iter_Next(&it) & END) == 0){
        StrVec *file = Iter_Get(&it);

        Str *last = Span_Get(file->p, file->p->max_idx);
        StrVec *route = StrVec_Copy(m, rootPath);
        StrVec_AddVec(route, StrVec_SubVec(m,
            file,
            Path_FlagIdx(file, (MORE|NOOP)),
            Path_FlagIdx(file, (LAST))-1));

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
            WwwNav *item = (WwwNav *)a;
            StrVec *name = Seel_Get(item, K(m, "name"));
            if(Empty(name)){
                void *ar[] = {name, it2d->coord, it2d->path, NULL};
                Out("^c.Nav Item @ \\@@/@^0\n", ar);

                Str *s = Span_Get(it2d->path->p, it2d->path->p->max_idx);
                if(s == NULL){
                    continue;
                }

                s = Str_Clone(m, s);
                Str_ToTitle(m, s);
                name = StrVec_From(m, s);
                Seel_Set(item, S(m, "name"), name);
                StrVec *url = StrVec_Copy(m, it2d->path);
                IoUtil_AddVec(m, url, IoPath(m, "index"));
                Seel_Set(item, S(m, "url"), url);
                Table_Set(coordTbl, name, it2d->coord);

                StrVec *txt = StrVec_Copy(m, it2d->path);

                StrVec *fpath = StrVec_Copy(m, inDir);
                IoUtil_AddVec(m, fpath, it2d->path);
                IoUtil_AddVec(m, fpath, IoPath(m, "doc.txt"));
                Inst_SetAtt(item, K(m, "fpath"), fpath);

                StrVec *outPath = StrVec_Make(m);
                IoUtil_AddVec(m, outPath, it2d->path);
                IoUtil_AddVec(m, outPath, IoPath(m, "index.html"));
                Inst_SetAtt(item, K(m, "out-path"), outPath);

                StrVec *display = StrVec_Copy(m, it2d->path);
                if(display->p->nvalues > 1){
                    StrVec_Pop(display);
                }
                IoUtil_AddVec(m, display, name);
                Path_SwapSep(display,
                    Str_Ref(m, (byte *)".", 1, 2, MORE), MORE);
                Inst_SetAtt(item, K(m, "display-path"), outPath);

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
