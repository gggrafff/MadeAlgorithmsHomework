#pragma once
typedef unsigned char byte;

struct IInputStream {
	// ���������� false, ���� ����� ����������
	bool Read(byte& value);
};

struct IOutputStream {
	void Write(byte value);
};