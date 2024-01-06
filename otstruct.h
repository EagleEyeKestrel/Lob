#ifndef LOB_OFSTRUCT_H
#define LOB_OFSTRUCT_H
struct OrderRaw {
    char fieldType;  // 0:order, 1:trade, 2:cancel
    int index;
    char securityID[16];
    int tradingDay;  // yyyymmdd
    int exchangeTime;  // HHMMSSsss
    double price;
    int volume;
    char direction;  // 0:bidorder, 1:askorder
    int askIndex;
    int bidIndex;
};
#endif



