#pragma once

#include <windows.h>
#include <cstdint>

namespace serial_communication {

    enum class CommResult {
        Error, Timeout, NotRecieved, Success
    };

    class SerialWrapper
    {
        HANDLE m_com_port = NULL; // �V���A���ʐM�f�o�C�X�̃n���h��

    public:
        // �w�肵���V���A���f�o�C�X���J��
        SerialWrapper(uint16_t com_num);

        // �J���Ă����V���A���f�o�C�X�����
        ~SerialWrapper();

        void set_timeout(uint32_t write_msec, uint32_t read_msec);

        // �V���A���ʐM�̏����ݒ�
        // bps: bit / sec
        void begin(uint32_t bps = 9600);

        // 1byte�̃f�[�^�𑗐M
        CommResult write(char byte);

        // ����byte�̃f�[�^����x�ɑ��M
        CommResult write_bytes(const char buf[], size_t len);

        // ��M�o�b�t�@�ɗ��܂���byte����Ԃ�
        int available();

        // ��M�o�b�t�@����1byte�ǂݍ���
        CommResult read(char buf[], size_t len);

        // ��M�o�b�t�@���畡��byte�ǂݍ���
        CommResult read_bytes(char buf[], size_t len);
    };

} // serial_communication

