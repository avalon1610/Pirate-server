#ifndef _MY_LIST_H_
#define _MY_LIST_H_

#define true 1
#define false 0
#define bool int

#define STATIC static

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned long ULONG_PTR,*PULONG_PTR;

#define CONTAINING_RECORD(address,type,field) ((type *)(\
                                            (char *)(address)-\
                                            (ULONG_PTR)(&((type *)0)->field)))
typedef struct _LIST_ENTRY {
    struct _LIST_ENTRY *Flink;
    struct _LIST_ENTRY *Blink;
} LIST_ENTRY,*PLIST_ENTRY;

STATIC
inline void InitializeListHead(PLIST_ENTRY ListHead)
{
    ListHead->Flink = ListHead->Blink = ListHead;
}

STATIC
inline bool IsListEmpty(const LIST_ENTRY *ListHead)
{
    return (bool)(ListHead->Flink == ListHead);
}

STATIC
inline bool RemoveEntryList(PLIST_ENTRY Entry)
{
    PLIST_ENTRY Blink;
    PLIST_ENTRY Flink;

    Flink = Entry->Flink;
    Blink = Entry->Blink;
    Blink->Flink = Flink;
    Flink->Blink = Blink;
    return (bool)(Flink == Blink);
}

STATIC
inline PLIST_ENTRY RemoveHeadList(PLIST_ENTRY ListHead)
{
    PLIST_ENTRY Flink;
    PLIST_ENTRY Entry;

    Entry = ListHead->Flink;
    Flink = Entry->Flink;
    ListHead->Flink = Flink;
    Flink->Blink = ListHead;
    return Entry;
}

STATIC
inline PLIST_ENTRY RemoveTailList(PLIST_ENTRY ListHead)
{
    PLIST_ENTRY Blink;
    PLIST_ENTRY Entry;

    Entry = ListHead->Blink;
    Blink = Entry->Blink;
    ListHead->Blink = Blink;
    Blink->Flink = ListHead;
    return Entry;
}

STATIC
inline void InsertTailList(PLIST_ENTRY ListHead,PLIST_ENTRY Entry)
{
    PLIST_ENTRY Blink;

    Blink = ListHead->Blink;
    Entry->Flink = ListHead;
    Entry->Blink = Blink;
    Blink->Flink = Entry;
    ListHead->Blink = Entry;
}

STATIC
inline void InsertHeadList(PLIST_ENTRY ListHead,PLIST_ENTRY Entry)
{
    PLIST_ENTRY Flink;

    Flink = ListHead->Flink;
    Entry->Flink = Flink;
    Entry->Blink = ListHead;
    Flink->Blink = Entry;
    ListHead->Flink = Entry;
}

STATIC
inline void AppendTailList(PLIST_ENTRY ListHead,PLIST_ENTRY ListToAppend)
{
    PLIST_ENTRY ListEnd = ListHead->Blink;

    ListHead->Blink->Flink = ListToAppend;
    ListHead->Blink = ListToAppend->Blink;
    ListToAppend->Blink->Flink = ListHead;
    ListToAppend->Blink = ListEnd;
}

#ifdef __cplusplus
}
#endif

#endif
