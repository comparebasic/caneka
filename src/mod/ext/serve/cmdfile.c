#include <external.h>
#include <caneka.h>

void *CmdFile_SourceMake(MemCh *m, Abstract *key, HandlerDef *def){
    Buff *bf = Ifc(m, key, TYPE_BUFF);
    CmdFile *cmd = (CmdFile *)MemCh_AllocOf(m, sizeof(CmdFile), TYPE_CMD_FILE);
    cmd->type.of = TYPE_CMD_FILE;
    cmd->in = bf;
    Iter_Init(&cmd->it, Span_Make(m));
    cmd->rbl = NlParser_Make(m, &cmd->it, Cursor_Make(m, bf->v));
    cmd->rbl->type.state |= DEBUG;
    return cmd;
}

void CmdFile_HandleInput(MemCh *m, Req *req, Serve *srv){
    CmdFile *cmd = (CmdFile *)req->source;
    printf("HandleInput\n");
    fflush(stdout);

    if((cmd->rbl->type.state & ERROR) == 0){
        Buff_ReadAmount(cmd->in, SERVE_READ_SIZE);
        printf("Poll %d\n", poll(cmd->in->pfd, 1, 0));

        if((cmd->in->type.state & NOOP) == 0){
            cmd->rbl->curs->type.state &= ~END;
            Roebling_Run(cmd->rbl);
        }
    }

    if(cmd->rbl->type.state & (NOOP|ERROR)){
        cmd->rbl->type.state &= ~(NOOP|ERROR);
    }

    if((cmd->it.type.state & END) == 0){
        while((Iter_Next(&cmd->it) & END) == 0){
            void *ar[] = {
                I32_Wrapped(m, cmd->it.idx),
                Iter_Get(&cmd->it),
                cmd->rbl,
                NULL
            };
            Out("^g.Cmd recieved $: @^0\n   @", ar);
        }
    }
}

HandlerDef *CmdFile_DefMake(MemCh *m, Span *steps, Node *ext, Abstract *key, Node *config){
    Buff *bf = Ifc(m, key, TYPE_BUFF);

    HandlerDef *def = HandlerDef_Make(m);

    def->extra = (SourceMakerFunc)CmdFile_SourceMake;

    def->route = Span_Make(m);
    Span_Add(def->route, Func_Wrapped(m, CmdFile_HandleInput, RECV_FLAGS));
    def->ext = ext;

    return def;
}
