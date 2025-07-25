#include <external.h>
#include <caneka.h>

static void match_Reset(Match *mt){
    mt->pat.curDef = mt->pat.startTermDef = mt->pat.startDef;
    mt->remaining = 0;
}

static boolean charMatched(word c, PatCharDef *def){
    boolean matched = FALSE;
    matched = (c >= def->from && c <= def->to);

    if((def->flags & PAT_INVERT) != 0 && (def->flags & PAT_KO) == 0){
        matched = !matched;
    }
    return matched;
}

static void match_NextTerm(Match *mt){
    while(
            (mt->pat.curDef < mt->pat.endDef) &&
            (mt->pat.curDef->flags & PAT_TERM) == 0){
        mt->pat.curDef++;
    }
    if((mt->pat.curDef->flags & PAT_TERM) != 0){
        mt->pat.curDef++;
    }
}

static void match_StartOfTerm(Match *mt){
    if(mt->pat.curDef > mt->pat.startDef){
        PatCharDef *prev = mt->pat.curDef;
        prev--;
        while(
                (prev->flags & PAT_TERM) == 0 &&
                (mt->pat.curDef > mt->pat.startDef)){
            if(prev == mt->pat.startDef){
                mt->pat.curDef = mt->pat.startDef;
                break;
            }else{
                mt->pat.curDef--;
            }
            prev--;
        }
    }
}

static void match_EndOfKoTerm(Match *mt){
    while(
            (mt->pat.curDef < mt->pat.endDef) &&
            ((mt->pat.curDef->flags & PAT_KO) != 0) && 
            ((mt->pat.curDef->flags & PAT_KO_TERM) == 0)){
        mt->pat.curDef++;
    }
}

static void match_NextKoTerm(Match *mt){
    match_EndOfKoTerm(mt);
    if((mt->pat.curDef->flags & PAT_KO_TERM) != 0){
        mt->pat.curDef++;
    }
}

static void addCount(MemCh *m, Match *mt, word flags, i32 length){
    if(mt->snip.type.state == ZERO){
        mt->snip.type.state = flags;
    }

    if((mt->snip.type.state & flags) == flags){
        mt->snip.length += length;
    }else{
        SnipSpan_Add(mt->backlog, &mt->snip);
        mt->snip.type.state = flags;
        mt->snip.length = length;
    }

    return;
}

status Match_StartOver(Match *mt){
    match_Reset(mt);
    mt->type.state = mt->type.state & MATCH_SEARCH;
    mt->snip.type.state = ZERO;
    mt->snip.length = 0;
    return Span_Wipe(mt->backlog);
}

status Match_AddBoundrySnip(MemCh *m, Match *mt){
    if(mt->snip.length > 0 || mt->backlog->nvalues > 0){
        addCount(m, mt, SNIP_STR_BOUNDRY, 0);
        return SUCCESS;
    }
    return NOOP;
}

status Match_ResolveOverlay(Match *mt, i32 length){
    SnipSpan_Add(mt->backlog, &mt->snip);
    return SnipSpan_Remove(mt->backlog, length);
}

status Match_Feed(MemCh *m, Match *mt, byte c){
    if((mt->type.state & NOOP) != 0){
        return mt->type.state;
    }
    boolean matched = FALSE;
    PatCharDef *def;

    boolean unclaimed = FALSE;
    while(mt->pat.curDef < mt->pat.endDef){
        def = mt->pat.curDef;

        matched = charMatched(c, def);
        if(mt->type.state & DEBUG){
            Str *yn = Str_CstrRef(m, (matched ? "Y":"N"));
            Str *yncolor = Str_Ref(m, (byte *)(matched ? "DE.":"de."), 3, 4,
                STRING_FMT_ANSI);
            Str *cs = Str_Ref(m, (byte *)&c, 1, 1, DEBUG);
            Abstract *args[] = {
                (Abstract *)yncolor,
                (Abstract *)yn, 
                (Abstract *)cs,
                (Abstract *)mt,
                NULL
            };
            Out("^p.Match-Feed $$/^D.$^de. @^0\n", args);
        }

        if(matched){
            unclaimed = FALSE;
            if((def->flags & (PAT_KO|PAT_INVERT)) == (PAT_KO|PAT_INVERT)){
                mt->type.state |= MATCH_KO_INVERT;
            }else if((def->flags & PAT_DROPOUT) != 0){
                mt->type.state |= NOOP;
                return mt->type.state;
            }else if(def->flags & PAT_KO){
                if(mt->type.state & MATCH_KO_INVERT){
                    mt->type.state &= ~MATCH_KO_INVERT;
                    mt->pat.curDef++;
                    continue;
                }else{
                    mt->type.state |= MATCH_KO;
                    if(def->flags & PAT_LEAVE){
                        mt->type.state |= MATCH_LEAVE;
                    }
                    if((def->flags & PAT_INVERT_CAPTURE) && 
                        (def->flags & PAT_CONSUME) == 0){
                        addCount(m, mt, SNIP_UNCLAIMED, 1);
                    }else{
                        addCount(m, mt, SNIP_GAP, 1);
                    }

                    match_EndOfKoTerm(mt);
                    PatCharDef *nextDef = mt->pat.curDef+1;

                    if((nextDef->flags & PAT_KO) == 0){
                        if(mt->type.state & MATCH_LEAVE){
                            goto miss;
                            break;
                        }
                        match_NextTerm(mt);
                        break;
                    }else{
                        break;
                    }
                }
            }

            if(mt->type.state & MATCH_KO){
                mt->type.state &= ~MATCH_KO;
                mt->snip.type.state = SNIP_CONTENT;
            }

            if(def >= mt->pat.lastTermDef){
                mt->type.state |= (PROCESSING|MATCH_LAST_TERM);
            }else{
                mt->type.state &= ~MATCH_LAST_TERM;
                mt->type.state |= PROCESSING;
            }

            word snipFlag = ZERO;
            if((def->flags & PAT_LEAVE) != 0){
                snipFlag = SNIP_UNCLAIMED;
            }else if( (def->flags & (PAT_INVERT_CAPTURE|PAT_INVERT)) == (PAT_INVERT_CAPTURE|PAT_INVERT)){
                /* no increment if it's an invert and no capture */;
                snipFlag = SNIP_UNCLAIMED;
            }else if((def->flags & PAT_CONSUME) != 0){
                snipFlag = SNIP_GAP;
            }else if((def->flags & PAT_INVERT_CAPTURE) != 0){
                snipFlag = SNIP_UNCLAIMED;
            }else{
                snipFlag = SNIP_CONTENT;
            }
            addCount(m, mt, snipFlag, 1);
            
            if((def->flags & (PAT_ANY|PAT_MANY)) != 0 || 
                    ((def->flags & PAT_COUNT) != 0 && (--mt->remaining) > 0)){
                match_StartOfTerm(mt);
                mt->type.state |= MATCH_TERM_FOUND;
            }else if(def->flags & PAT_INVERT){
                mt->pat.curDef++;
                continue;
            }else{
                match_NextTerm(mt);
                mt->type.state &= ~MATCH_TERM_FOUND;
            }

            break;
        }else{ /* matched is FALSE */
            if((mt->type.state & MATCH_KO) != 0){
                unclaimed = TRUE;
                if((def->flags & PAT_KO) == 0){
                    mt->snip.type.state &= ~NOOP;
                    mt->snip.type.state = SNIP_CONTENT;
                    if((mt->type.state & MATCH_LEAVE) != 0){
                        goto miss;
                        break;
                    }
                    mt->type.state &= ~(PROCESSING|MATCH_KO|MATCH_LAST_TERM);
                    mt->pat.curDef++;
                    continue;
                }else{
                    mt->pat.curDef++;
                    continue;
                }
            }else if((def->flags & (PAT_KO|PAT_OPTIONAL|PAT_ANY)) != 0){
                unclaimed = TRUE;
                mt->pat.curDef++;
                continue;
            }else if((def->flags & (PAT_MANY)) != 0){
                unclaimed = TRUE;
                if((def->flags & PAT_TERM) != 0){
                    if((mt->type.state & MATCH_TERM_FOUND) != 0){
                        match_NextTerm(mt);
                        continue;
                    }else{
                        goto miss;
                        break;
                    }
                }else{
                    mt->pat.curDef++;
                    continue;
                }
            }else if((def->flags & PAT_TERM) == 0){
                if((def->flags & PAT_INVERT) == 0){
                    mt->pat.curDef++;
                    continue;
                }
            }
miss:
            mt->type.state &= ~(PROCESSING|MATCH_LAST_TERM);
            if((mt->type.state & MATCH_SEARCH) != 0){
                match_Reset(mt);
                addCount(m, mt, SNIP_SKIPPED, 1);
            }else{
                mt->type.state |= NOOP;
            }
            break;
        }
    }

    if(mt->pat.curDef == mt->pat.endDef){
        if(unclaimed){
            addCount(m, mt, SNIP_UNCLAIMED, 1);
        }
        SnipSpan_Add(mt->backlog, &mt->snip);
        mt->snip.length = 0;
        if(SnipSpan_Total(mt->backlog, SNIP_CONTENT) == 0 &&
                (mt->type.state & MATCH_ACCEPT_EMPTY) == 0){
            mt->type.state = NOOP;
        }else{
            mt->type.state = (mt->type.state | SUCCESS) & ~(PROCESSING|MATCH_LAST_TERM);
        }
    }

    if(mt->type.state & DEBUG){
        Str *cs = Str_Ref(m, (byte *)&c, 1, 1, DEBUG);
        Abstract *args[] = {
            (Abstract *)cs,
            (Abstract *)mt,
            NULL
        };
        Out("^c.Match-Feed ^D.$^d. @^0\n", args);
    }

    return mt->type.state;
}

status Match_FeedStrVec(MemCh *m, Match *mt, StrVec *v, i32 offset){
    if((mt->type.state & NOOP) != 0){
        return mt->type.state;
    }

    i32 pos = 0;
    Iter it;
    Iter_Init(&it, v->p);
    offset = StrVec_FfIter(&it, offset);
    do {
        Str *s = (Str *)it.value;
        i64 i = offset;
        for(; i < s->length; i++){
            byte b = s->bytes[i];
            Match_Feed(m, mt, (word)b);
            if((mt->type.state & SUCCESS) != 0){
                break;
            }
        }
        offset -= (i-1);
    } while((Iter_Next(&it) & END) == 0);

    return mt->type.state;
}

status Match_FeedEnd(MemCh *m, Match *mt){
    Match_Feed(m, mt, 0);
    return mt->type.state;
}

status Match_SetCount(Match *mt, i32 count){
    PatCharDef *def = mt->pat.startDef;
    while(def != mt->pat.endDef){
        if(def->flags & PAT_COUNT){
            mt->remaining = count;
            PatCountDef *countDef = (PatCountDef *)def;
            countDef->count = count;
            return SUCCESS;
        }
    }
    return ERROR;
}

Match *Match_Make(MemCh *m, PatCharDef *def, Span *backlog){
    Match *mt = (Match *)MemCh_Alloc(m, sizeof(Match));
    mt->type.of = TYPE_PATMATCH;
    mt->pat.startDef = mt->pat.curDef = mt->pat.lastTermDef = def;
    i32 count = 0;
    boolean termStart = FALSE;
    while(def->flags != PAT_END){
       def++;
       if(++count > PAT_CHAR_MAX_LENGTH){
            Fatal(FUNCNAME, FILENAME, LINENUMBER,
                "PatCharDef: PAT_END not found before max", NULL);
       }
       if(termStart && def->flags != PAT_END){
            mt->pat.lastTermDef = def;
            termStart = FALSE;
       }
       if(def->flags & PAT_TERM){
            termStart = TRUE;
       }
    }
    if(def->flags == PAT_END){
        mt->pat.endDef = def;
    }else{
        Fatal(FUNCNAME, FILENAME, LINENUMBER,
            "PatCharDef: end not found", NULL);
    }
    mt->remaining = -1;
    if(backlog == NULL){
        backlog = Span_Make(m);
    }
    mt->backlog = backlog;
    mt->snip.type.of = TYPE_SNIP;

    return mt;
}
