#include <external.h>
#include <caneka.h>

static void EncPair_Print(Abstract *a, cls type, char *msg, int color, boolean extended){
    EncPair *pair = (EncPair *)as(a, TYPE_ENC_PAIR);
    printf("\x1b[%dm%sEnc<%s %s:",
        color, msg, State_ToChars(pair->type.state), pair->keyId->bytes);
    String *enc = String_Present(DebugM, (Abstract *)pair->enc);
    while(enc != NULL){
        printf("%s", enc->bytes);
        enc = String_Next(enc);
    }
    printf("->");
    String *dec = String_Present(DebugM, (Abstract *)pair->dec);
    while(dec != NULL){
        printf("%s", dec->bytes);
        dec = String_Next(dec);
    }
    printf(">\x1b[0m");
}
