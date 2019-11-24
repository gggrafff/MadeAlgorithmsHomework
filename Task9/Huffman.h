#pragma once
typedef unsigned char byte;

struct IInputStream {
	// Возвращает false, если поток закончился
	bool Read(byte& value) = 0;
};

struct IOutputStream {
	void Write(byte value) = 0;
};
