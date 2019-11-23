//compression

#include "Huffman.h"

void Encode(IInputStream& original, IOutputStream& compressed)
{
	copyStream(original, compressed);
}

void Decode(IInputStream& compressed, IOutputStream& original)
{
	copyStream(compressed, original);
}

