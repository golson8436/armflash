/*!\file  CThreadDispatcher.h  Thread handling - unused at the moment
 *
 *	This file is part of the armflash (arm flashing utility)
 *  package.
 *
 *  Copyright (c) 2008-2009 by Gabriel Zabusek <gabriel.zabusek@gmail.com>
 *
 *	This program is free software; you can redistribute it and/or
 *	modify it under the terms of the GNU General Public License
 *	as published by the Free Software Foundation; either version
 *	2 of the License, or (at your option) any later version.
 */

#ifndef CTHREAD_DISPATCHER_H
#define CTHREAD_DISPATCHER_H

#include <pthread.h>
#include <tools/CFlashData.h>

#define MAX_THREADS 32

class CThreadDispatcher
{
private:
    pthread_t rgThreadID[MAX_THREADS];
    unsigned int nCurrentFreeThread;

    static void start_thread(void *pData)
    {
        reinterpret_cast<CThreadDispatcher *>(pData)->do_work();
    }

    void do_work()
    {
        //do the actual work here
    }

public:
    CThreadDispatcher();
    ~CThreadDispatcher();

    bool StartThread( SFlashData stFlashData );

};

#endif
