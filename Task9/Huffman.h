#pragma once
typedef unsigned char byte;

struct IInputStream {
	// ���������� false, ���� ����� ����������
	bool Read(byte& value) = 0;
};

struct IOutputStream {
	void Write(byte value) = 0;
};
