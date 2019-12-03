#pragma once

#include <windows.h>
#include <cstdint>

namespace serial_communication {

    enum class CommResult {
        Error, Timeout, NotRecieved, Success
    };

    class SerialWrapper
    {
        HANDLE m_com_port = NULL; // シリアル通信デバイスのハンドル

    public:
        // 指定したシリアルデバイスを開く
        SerialWrapper(uint16_t com_num);

        // 開いていたシリアルデバイスを閉じる
        ~SerialWrapper();

        void set_timeout(uint32_t write_msec, uint32_t read_msec);

        // シリアル通信の初期設定
        // bps: bit / sec
        void begin(uint32_t bps = 9600);

        // 1byteのデータを送信
        CommResult write(char byte);

        // 複数byteのデータを一度に送信
        CommResult write_bytes(const char buf[], size_t len);

        // 受信バッファに溜まったbyte数を返す
        int available();

        // 受信バッファから1byte読み込む
        CommResult read(char buf[], size_t len);

        // 受信バッファから複数byte読み込む
        CommResult read_bytes(char buf[], size_t len);
    };

} // serial_communication

