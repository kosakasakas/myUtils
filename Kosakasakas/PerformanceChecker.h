//
//  PerformanceChecker.h
//
//  関数速度計測用の簡易クラスです.
//
//  Created by Takahiro.Kosaka on 2015/12/15.
//  Copyright (c) 2015年 Takahiro.Kosaka. All rights reserved.
//

#ifndef ___INCLUDE_PerformanceChecker_h___
#define ___INCLUDE_PerformanceChecker_h___

// File Dependencies
#include <functional>
#include <string>
#include <chrono>

// Function Definitions
namespace Kosakasakas {
/**
 @brief 関数速度計測用クラス
 
 関数の実行速度を計測するためのクラスです.
 シングルトンオブジェクトを提供します.
 */
class PerformanceChecker
{
private:
    PerformanceChecker();
    virtual ~PerformanceChecker();
    
public:
    PerformanceChecker(const PerformanceChecker&)               = delete;
    PerformanceChecker& operator=(const PerformanceChecker&)    = delete;
    PerformanceChecker(PerformanceChecker&&)                    = delete;
    PerformanceChecker& operator=(PerformanceChecker&&)         = delete;
    
    // シングルトンの取得
    static  PerformanceChecker& GetInstance() {
        static  PerformanceChecker inst;
        return inst;
    }
    
    //! 関数計測用オブジェクト
    struct MeasureFunc
    {
        //! 計測する関数
        std::function< void() > m_Func;
        //! 関数名などラベル
        std::string m_Label;
    };
    
    //! 計測パラメータオブジェクト
    struct  MeasureParam
    {
        //! 何フレーム平均で計測するか
        int     FrameUpdateCount;
        //! フレーム更新時に呼び出される関数
        std::function< void() >     MeasureUpdateFunc;
        //! 強制的にスリープさせるかどうか
        bool    EnableSleep;
        //! スリープさせる場合の秒数(msec). ここでスリープした秒数は計測には含まれません.
        float   SleepMSec;
    };
    
    //! 計測結果オブジェクト
    struct  MeasueResult
    {
        //! 1秒あたりの消費フレーム数
        float   FPS;
        //! 1フレーム辺りの消費msec
        float   Msec;
    };
    
protected:
    /**
     @brief 初期化関数
     
     もろもろの初期化をここで行います.
     @return    成功の可否
     */
    bool    Initialize();
    
public:
    /**
     @brief 実行時間の計測
     
     入力された関数オブジェクトの実行時間を計測します.
     計測はm_ItrNum回実施し、平均を取得します.
     計測の最小精度はμsecまでです.
     @param func    実行する関数オブジェクト
     @return        一実行あたりの平均計測時間[μsec]
     */
    float   MeasurePerformance( const MeasureFunc& func, const int itrNum = 100 );
    
    /**
     @brief デバッグ情報の取得
     
     直前の処理で発生したデバッグ情報を書き込みます.
     @param dstString   情報を書き込むStringオブジェクト
     */
    void    GetDebugInfo( std::string& dstString );
    
    /**
     @brief フレームレートの更新
     
     ゲームループのUpdate関数内でこの関数をコールしてください.
     内部でフレームレートの計測を行います.
     */
    void    Update();
    
    /**
     @brief 計測パラメータのセッタ
     
     メジャーオブジェクトを登録します.
     */
    void    SetMeasureParam( MeasureParam& param );
    
    /**
     @brief 現在の計測結果を取得
     
     m_MeasueParamにセットされたFrameUpdateCount数平均の計測結果を取得します.
     @param 出力MeasueResultオブジェクト
     */
    void    GetCurrentMeasureResult( MeasueResult& dstResult );

private:
    /**
     @brief 現在の計測結果を更新
     
     MeasureResultを更新します.
     @param 出力MeasueResultオブジェクト
     */
    void    UpdateMeasureResult( MeasueResult& dstResult );
    
    /**
     @brief タイマーをリセットします.
     
     各種タイマーのリセット
     */
    void    ResetTimer();
    
    /**
     @brief Update処理をスリープさせます
     
     Update処理をスリープします
     */
    void    SleepTimer();

private:
    //! デバッグ情報のバッファ
    std::string     m_DebugInfo;
    //! 処理したフレーム数
    int             m_FrameCount;
    //! FPS計測パラメータ
    MeasureParam    m_MeasureParam;
    //! 計測結果のキャッシュ
    MeasueResult    m_MeasureResult;
    //! 計測のスタート時間
    std::chrono::time_point<std::chrono::system_clock>  m_MeasureStartTime;
};
    
} // namespace Lift {

#endif /* defined(___INCLUDE_PerformanceChecker_h___) */
/* End Of File */

