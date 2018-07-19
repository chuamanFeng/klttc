//------------------------------------------------------------------------------
//
//  FILE:           TimeMeasure.h
//
//  BIRTH DATE:     December 2002
//
//  LANGUAGE:       C++
//
//  LIBRARY:        -MFC (Visual C++ V6.0)
//
//  OS:             Windows 2000 (Win32)
//
//  PLATFORM:       Intel Pentium (IA-32)
//
//  COMPANY:        Coreco Imaging
//
//------------------------------------------------------------------------------
// Implement time measurement functions with the CPU's high resolution timer.
//------------------------------------------------------------------------------


#ifndef _TIMEMEASURE_H_
#define _TIMEMEASURE_H_


#include "Afxmt.h"                  // Windows MFC for CEvent



class CTimeMeasure
{

public:

    CTimeMeasure(void);

    void StartTime(void);

    void StopTime(void);

    void TagTime(void);

    float GetTime(void)         { return ComputeTime() * 1.0f; }

    float GetTimeMilli(void)    { return ComputeTime() * 1000.0f; }

    float GetTimeMicro(void)    { return ComputeTime() * 1000000.0f; }

protected:

    float ComputeTime(void);

    LONGLONG m_CounterFrequency;
    LONGLONG m_StartTime;
    LONGLONG m_StopTime;
};

#endif	// _TIMEMEASURE_H_

