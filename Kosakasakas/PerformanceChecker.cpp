//
//  PerformanceChecker.cpp
//
//  関数速度計測用の簡易クラスです.
//
//  Created by Takahiro.Kosaka on 2015/12/15.
//  Copyright (c) 2015年 Takahiro.Kosaka. All rights reserved.
//

// File Dependencies
#include <thread>
#include "PerformanceChecker.h"

// Namespace Dependencies
using namespace Kosakasakas;

/**
   @brief コンストラクタ
*/
PerformanceChecker::PerformanceChecker()
{
    if( !Initialize() )
    {
        m_DebugInfo = "fail to initialize.";
    };
}

/**
   @brief デストラクタ
*/
PerformanceChecker::~PerformanceChecker()
{
}

bool    PerformanceChecker::Initialize()
{
    MeasureParam param;
    param.FrameUpdateCount  = 100; // デフォルトは100フレーム平均
    param.MeasureUpdateFunc = nullptr;
    param.EnableSleep       = false;
    param.SleepMSec         = 0.0f;
    SetMeasureParam(param);
    
    ResetTimer();
    
    return true;
}

float   PerformanceChecker::MeasurePerformance( const MeasureFunc& func, const int itrNum )
{
    int i = 0;
    const auto startTime    = std::chrono::system_clock::now();
    for (; i < itrNum; ++i)
    {
        func.m_Func();
    }
    const auto endTime      = std::chrono::system_clock::now();
    
    const auto timeSpan     = endTime - startTime;
    float   timeSpanMicro   = std::chrono::duration_cast<std::chrono::microseconds>(timeSpan).count();
    float   averageMicro    = timeSpanMicro / (float) itrNum;
    
    char buff[512];
    snprintf(buff, sizeof(buff), "[%s] spends %lf [μsec].", func.m_Label.c_str(), averageMicro);
    m_DebugInfo     = buff;
    
    return  averageMicro;
};

void    PerformanceChecker::GetDebugInfo( std::string& dstString )
{
    dstString = m_DebugInfo;
}

void    PerformanceChecker::Update()
{
    if  (m_FrameCount % m_MeasureParam.FrameUpdateCount == 0)
    {
        UpdateMeasureResult(m_MeasureResult);
        ResetTimer();
    }
    if (m_MeasureParam.EnableSleep) {
        SleepTimer();
    }
    ++m_FrameCount;
}

void    PerformanceChecker::SleepTimer()
{
    int microSec    = (int)(m_MeasureParam.SleepMSec * 1000.0f);
    std::this_thread::sleep_for(std::chrono::microseconds(microSec));
}

void    PerformanceChecker::ResetTimer()
{
    m_FrameCount        = 0;
    m_MeasureStartTime  = std::chrono::system_clock::now();
}

void    PerformanceChecker::SetMeasureParam(MeasureParam& param)
{
    m_MeasureParam.FrameUpdateCount     = param.FrameUpdateCount;
    m_MeasureParam.MeasureUpdateFunc    = param.MeasureUpdateFunc;
    m_MeasureParam.EnableSleep          = param.EnableSleep;
    m_MeasureParam.SleepMSec            = param.SleepMSec;
}

void    PerformanceChecker::GetCurrentMeasureResult( MeasueResult& dstResult )
{
    dstResult.FPS   = m_MeasureResult.FPS;
    dstResult.Msec  = m_MeasureResult.Msec;
}

void    PerformanceChecker::UpdateMeasureResult( MeasueResult& dstResult )
{
    auto endTime            = std::chrono::system_clock::now();
    auto timeSpan           = endTime - m_MeasureStartTime;
    float   timeSpanMicro   = std::chrono::duration_cast<std::chrono::microseconds>(timeSpan).count();
    if (m_MeasureParam.EnableSleep) {
        // わざとスリープしている分は計測から外す.
        timeSpanMicro       -= m_MeasureParam.SleepMSec * 1000.0f * m_FrameCount;
    }
    float   averageMicro    = timeSpanMicro / (float) m_MeasureParam.FrameUpdateCount;
    
    dstResult.Msec          = averageMicro / 1000.0f;
    dstResult.FPS           = 1000.0f * 1000.0f / averageMicro;
    
    char buff[512];
    snprintf(buff, sizeof(buff), "Update FPS %lf [frame], %lf [msec].", dstResult.FPS, dstResult.Msec);
    m_DebugInfo = buff;
    
    if (m_MeasureParam.MeasureUpdateFunc != nullptr)
    {
       m_MeasureParam.MeasureUpdateFunc();
    }
}


/* End Of File */
