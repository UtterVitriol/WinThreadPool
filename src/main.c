#include <Windows.h>
#include <stdio.h>

VOID CALLBACK
MyWorkCallback (PTP_CALLBACK_INSTANCE Instance, PVOID Parameter, PTP_WORK WORK)
{
    static int a = 0;
    Sleep(1000);
    printf("Hello from work: %d | %s\n", a, (PCHAR)Parameter);

    a++;
}

int
main ()
{
    BOOL                bRet            = FALSE;
    PTP_POOL            pool            = NULL;
    TP_CALLBACK_ENVIRON callbackEnviron = { 0 };
    PTP_CLEANUP_GROUP   cleanupGroup    = NULL;
    PTP_WORK            work            = NULL;

    InitializeThreadpoolEnvironment(&callbackEnviron);
    char * words[10] = {
        "Yeet",
        "Beans",
        "Skibity",
        "Chungus",
        "Deez",
        "one",
        "two",
        "three",
        "four",
        "five"
    };

    pool = CreateThreadpool(NULL);
    if (NULL == pool)
    {
        printf("CrateThreadPool: %lu\n", GetLastError());
        goto EXIT;
    }

    SetThreadpoolThreadMaximum(pool, 0);
    bRet = SetThreadpoolThreadMinimum(pool, 1);
    if (!bRet)
    {
        printf("SetTreadPoolMinimum: %lu\n", GetLastError());
        goto EXIT;
    }

    cleanupGroup = CreateThreadpoolCleanupGroup();
    if (!cleanupGroup)
    {
        printf("CreateThreadpoolCleanupGroup %lu\n", GetLastError());
        goto EXIT;
    }

    SetThreadpoolCallbackPool(&callbackEnviron, pool);
    SetThreadpoolCallbackCleanupGroup(&callbackEnviron, cleanupGroup, NULL);

    for (int x = 0; x < 10; x++)
    {
        work = CreateThreadpoolWork(MyWorkCallback, words[x], &callbackEnviron);
        if (!work)
        {
            printf("CreateThreadpoolWork: %lu\n", GetLastError());
            goto EXIT;
        }

        SubmitThreadpoolWork(work);
    }

    printf("here");

    CloseThreadpoolCleanupGroupMembers(cleanupGroup, FALSE, NULL);

EXIT:
    if (pool)
    {
        CloseThreadpoolCleanupGroupMembers(cleanupGroup, FALSE, NULL);
        CloseThreadpoolCleanupGroup(cleanupGroup);
        CloseThreadpool(pool);
    }

    return 0;
}
