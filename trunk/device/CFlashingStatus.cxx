/*!\file  CFlashingStatus.cxx  Handling the flashing progress
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

#include <device/CFlashingStatus.h>

CFlashingStatus::CFlashingStatus()
{
    m_nWorkDone  = 0;
    m_nWorkTotal = 0;
}

CFlashingStatus::~CFlashingStatus()
{
}

void 
CFlashingStatus::AddMessage(string strMessage)
{
    m_clMessageQueue.push_back( strMessage );
}

string 
CFlashingStatus::PopMessage()
{
    string to_ret = m_clMessageQueue.front();
    m_clMessageQueue.pop_front();
    return to_ret;
}

void 
CFlashingStatus::SetTotalWork(int nWorkTotal)
{
    m_nWorkTotal = nWorkTotal;
}

int  
CFlashingStatus::GetTotalWork() const
{
    return m_nWorkTotal;
}

void 
CFlashingStatus::SetWorkDone(int nWorkDone)
{
    m_nWorkDone = nWorkDone;
}

int  
CFlashingStatus::GetWorkDone() const
{
    return m_nWorkDone;
}

deque<string> 
CFlashingStatus::GetMessageQueue()
{
    return m_clMessageQueue;
}

