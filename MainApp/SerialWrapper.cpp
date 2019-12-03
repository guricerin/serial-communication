#include <tchar.h>
#include <iostream>
#include <cassert>
#include <algorithm>
#include "SerialWrapper.h"

using namespace serial_communication;

SerialWrapper::SerialWrapper(uint16_t com_num) {
    constexpr auto len = 16;
    TCHAR com[len] = { '\0' };
    //constexpr auto len = sizeof(com) / sizeof(com[0]);

    // WARNING: 0番以上のCOMポート名は _T("\\\\.\\COM10") としないと認識してくれないらしい。
    // http://www.salutesound.com/~oss_winapi232.html
    _stprintf_s(com, len, _T("COM%d"), com_num);

    // COMポートを開く
    m_com_port = CreateFile(
        com, 
        GENERIC_READ | GENERIC_WRITE, 
        0, 
        NULL, 
        OPEN_EXISTING, 
        0, 
        NULL
    );

    if (m_com_port == INVALID_HANDLE_VALUE) {
        std::cout << "com open failed..." << "\n";
        exit(1);
    }
}

SerialWrapper::~SerialWrapper() {
    // COMポートを閉じる
    CloseHandle(m_com_port);
}

void SerialWrapper::set_timeout(uint32_t write_msec, uint32_t read_msec) {
    COMMTIMEOUTS timeouts;
    // 読み込みタイムアウト秒数 = (ReadTotalTimeoutMultiplier * 読み込みバイト数) + ReadTotalTimeoutConstant
    // 書き込みタイムアウト秒数 = (WriteTotalTimeoutMultiplier * 書き込みバイト数) + WriteTotalTimeoutConstant
    // http://donnk.com/Nmura/soft/help015.html

    // 単位はすべてmsec
    timeouts.WriteTotalTimeoutMultiplier = write_msec;
    timeouts.WriteTotalTimeoutConstant = 0;
    timeouts.ReadTotalTimeoutMultiplier = 0;
    timeouts.ReadTotalTimeoutConstant = read_msec;

    SetCommTimeouts(m_com_port, &timeouts);
}

void SerialWrapper::begin(uint32_t bps)
{
    DCB dcb; // シリアルポートの構成情報が入る構造体
    const auto res = GetCommState(m_com_port, &dcb); // 現在の設定値を読み込み

    dcb.BaudRate = static_cast<DWORD>(bps); // 速度
    dcb.ByteSize = 8; // データ長
    dcb.Parity = NOPARITY; // パリティ
    dcb.StopBits = ONESTOPBIT; // ストップビット長
    dcb.fOutxCtsFlow = FALSE; // 送信時CTSフロー
    dcb.fRtsControl = RTS_CONTROL_ENABLE; // RTSフロー

    // 変更した設定値を書き込み
    const auto res2 = SetCommState(m_com_port, &dcb);
}

CommResult SerialWrapper::write(char byte)
{
    char* send_data = &byte; // 送信する1byte
    DWORD actual_len; // 実際に送信したbyte数

    // ポートへ送信
    const auto res = WriteFile(m_com_port, send_data, 1, &actual_len, NULL);

    if (!res) {
        return CommResult::Error;
    }
    else if (actual_len < 1) {
        return CommResult::Timeout;
    }
    else {
        return CommResult::Success;
    }
}

CommResult SerialWrapper::write_bytes(const char buf[], size_t len)
{
    DWORD expect_len = static_cast<DWORD>(len); // 送信するbyte数
    DWORD actual_len; // 実際に送信したbyte数

    // ポートへ送信
    const auto res = WriteFile(m_com_port, buf, expect_len, &actual_len, NULL);

    if (!res) {
        return CommResult::Error;
    }
    else if (actual_len < 1) {
        return CommResult::Timeout;
    }
    else {
        return CommResult::Success;
    }
}

int SerialWrapper::available()
{
    //受信データ数を調べる
    DWORD errors;
    COMSTAT com_stat;
    const auto res = ClearCommError(m_com_port, &errors, &com_stat);

    const auto recieved_len = static_cast<int>(com_stat.cbInQue); // 受信したメッセージ長を取得する
    return recieved_len;
}

CommResult SerialWrapper::read(char buf[], size_t len)
{
    assert(len >= 2);

    //受信データがない場合は読み込まない
    if (available() < 1)
    {
        return CommResult::NotRecieved;
    }

    std::fill(buf, buf + len, '\0');
    DWORD actual_len; // 実際に受信したバイト数

    // データ受信
    const auto res = ReadFile(m_com_port, buf, 1, &actual_len, NULL);

    if (!res) {
        return CommResult::Error;
    }
    else if (actual_len < 1) {
        return CommResult::Timeout;
    }
    else {
        return CommResult::Success;
    }
}

CommResult SerialWrapper::read_bytes(char buf[], size_t len)
{
    assert(len >= 2);

    //受信データがない場合は読み込まない
    if (available() < 1)
    {
        return CommResult::NotRecieved;
    }

    std::fill(buf, buf + len, '\0');
    DWORD actual_len; // 実際に受信したバイト数

    // データ受信
    const auto res = ReadFile(m_com_port, buf, len, &actual_len, NULL);

    if (!res) {
        return CommResult::Error;
    }
    else if (actual_len < 1) {
        return CommResult::Timeout;
    }
    else {
        return CommResult::Success;
    }
}
