//------------------------------------------------------------------------------
//
//  FILE:           TimeMeasure.cpp
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


#include "StdAfx.h"
#include "TimeMeasure.h"


//------------------------------------------
//              Constructor
//------------------------------------------
CTimeMeasure::CTimeMeasure()
{
    QueryPerformanceFrequency((LARGE_INTEGER *) &m_CounterFrequency);
    StartTime();
    m_StopTime = m_StartTime;
}



//------------------------------------------
//              StartTime
//------------------------------------------
// Usefull to measure a single event
//------------------------------------------
void CTimeMeasure::StartTime()
{
    QueryPerformanceCounter((LARGE_INTEGER *) &m_StartTime);
}


//------------------------------------------
//              StopTime
//------------------------------------------
// Usefull to measure a single event
//------------------------------------------
void CTimeMeasure::StopTime()
{
    QueryPerformanceCounter((LARGE_INTEGER *) &m_StopTime);
}


//------------------------------------------
//              TagTime
//------------------------------------------
// Usefull to measure a periodic event
//------------------------------------------
void CTimeMeasure::TagTime()
{
    m_StartTime = m_StopTime;
    QueryPerformanceCounter((LARGE_INTEGER *) &m_StopTime);
}



//------------------------------------------
//              ComputeTime
//------------------------------------------
float CTimeMeasure::ComputeTime()
{
float fReturnedValue;
LONGLONG diffTime;


    if (m_StopTime > m_StartTime)
    {
        // Compute difference between start and stop
        diffTime = m_StopTime - m_StartTime;

        // Return time in seconds
        fReturnedValue = (float) diffTime / (float) m_CounterFrequency;
    }
    else
    {
        fReturnedValue = 0.0;
    }

    return (fReturnedValue);

}
