#pragma once
typedef unsigned char byte;

struct IInputStream {
	// Возвращает false, если поток закончился
	bool Read(byte& value);
};

struct IOutputStream {
	void Write(byte value);
};