/* 
This is a fomula to take parts of the has for the coordinates of different levels of the
has table, each represented by a span with different dimensions
*/
static byte getTieredHash(hash, level){
    return (byte) (((hash >> (level*4)) & TABLE_MOD);
}
