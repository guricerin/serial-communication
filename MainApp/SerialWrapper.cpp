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

    // WARNING: 0�Ԉȏ��COM�|�[�g���� _T("\\\\.\\COM10") �Ƃ��Ȃ��ƔF�����Ă���Ȃ��炵���B
    // http://www.salutesound.com/~oss_winapi232.html
    _stprintf_s(com, len, _T("COM%d"), com_num);

    // COM�|�[�g���J��
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
    // COM�|�[�g�����
    CloseHandle(m_com_port);
}

void SerialWrapper::set_timeout(uint32_t write_msec, uint32_t read_msec) {
    COMMTIMEOUTS timeouts;
    // �ǂݍ��݃^�C���A�E�g�b�� = (ReadTotalTimeoutMultiplier * �ǂݍ��݃o�C�g��) + ReadTotalTimeoutConstant
    // �������݃^�C���A�E�g�b�� = (WriteTotalTimeoutMultiplier * �������݃o�C�g��) + WriteTotalTimeoutConstant
    // http://donnk.com/Nmura/soft/help015.html

    // �P�ʂ͂��ׂ�msec
    timeouts.WriteTotalTimeoutMultiplier = write_msec;
    timeouts.WriteTotalTimeoutConstant = 0;
    timeouts.ReadTotalTimeoutMultiplier = 0;
    timeouts.ReadTotalTimeoutConstant = read_msec;

    SetCommTimeouts(m_com_port, &timeouts);
}

void SerialWrapper::begin(uint32_t bps)
{
    DCB dcb; // �V���A���|�[�g�̍\����񂪓���\����
    const auto res = GetCommState(m_com_port, &dcb); // ���݂̐ݒ�l��ǂݍ���

    dcb.BaudRate = static_cast<DWORD>(bps); // ���x
    dcb.ByteSize = 8; // �f�[�^��
    dcb.Parity = NOPARITY; // �p���e�B
    dcb.StopBits = ONESTOPBIT; // �X�g�b�v�r�b�g��
    dcb.fOutxCtsFlow = FALSE; // ���M��CTS�t���[
    dcb.fRtsControl = RTS_CONTROL_ENABLE; // RTS�t���[

    // �ύX�����ݒ�l����������
    const auto res2 = SetCommState(m_com_port, &dcb);
}

CommResult SerialWrapper::write(char byte)
{
    char* send_data = &byte; // ���M����1byte
    DWORD actual_len; // ���ۂɑ��M����byte��

    // �|�[�g�֑��M
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
    DWORD expect_len = static_cast<DWORD>(len); // ���M����byte��
    DWORD actual_len; // ���ۂɑ��M����byte��

    // �|�[�g�֑��M
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
    //��M�f�[�^���𒲂ׂ�
    DWORD errors;
    COMSTAT com_stat;
    const auto res = ClearCommError(m_com_port, &errors, &com_stat);

    const auto recieved_len = static_cast<int>(com_stat.cbInQue); // ��M�������b�Z�[�W�����擾����
    return recieved_len;
}

CommResult SerialWrapper::read(char buf[], size_t len)
{
    assert(len >= 2);

    //��M�f�[�^���Ȃ��ꍇ�͓ǂݍ��܂Ȃ�
    if (available() < 1)
    {
        return CommResult::NotRecieved;
    }

    std::fill(buf, buf + len, '\0');
    DWORD actual_len; // ���ۂɎ�M�����o�C�g��

    // �f�[�^��M
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

    //��M�f�[�^���Ȃ��ꍇ�͓ǂݍ��܂Ȃ�
    if (available() < 1)
    {
        return CommResult::NotRecieved;
    }

    std::fill(buf, buf + len, '\0');
    DWORD actual_len; // ���ۂɎ�M�����o�C�g��

    // �f�[�^��M
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
