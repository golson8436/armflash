/*!\file  CFlashingStatus.h  Handling of the flashing progress info
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

#ifndef CFLASHINGSTATUS_H
#define CFLASHINGSTATUS_H

#include <string>
#include <deque>
using namespace std;

class CFlashingStatus{
private:
    int m_nWorkTotal;
    int m_nWorkDone;
    deque<string> m_clMessageQueue;

public:
    CFlashingStatus();
    ~CFlashingStatus();

    void AddMessage(string strMessage);
    string PopMessage();
    void SetTotalWork(int nWorkTotal);
    int  GetTotalWork() const;
    void SetWorkDone(int nWorkDone);
    int  GetWorkDone() const;
    deque<string> GetMessageQueue();
};

#endif
