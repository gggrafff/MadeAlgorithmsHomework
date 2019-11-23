//compression
#include <stdio.h>
#include <stdlib.h>

#define HEX_DUMP_WIDTH 16
#define	MAX_DIR_LENGTH 512

#define NORM_UINT16(a) ( (((a) & 0x00ff)<<8) | (((a)>>8) & 0x00ff) )
#define NORM_UINT32(a) ( (((a) & 0x000000ff)<<24) | (((a) & 0x0000ff00)<<8) | (((a) & 0x00ff0000)>>8) | (((a) & 0xff000000)>>24) )

#define LZF_UNCOMPRESSED	0x414C454D
#define LZF_COMPRESSED		0x75465A4C

#pragma pack(1)
typedef struct _LZF_HEADER
{
	unsigned int CompSize;
	unsigned int RawSize;
	unsigned int CompType;
	unsigned int CRC;
}LZF_HEADER, *PLZF_HEADER;
#pragma pack()

namespace custom_algorithms
{
class LZXPRESSARCHIVER_API LzxpressArchiver
{
public:
	LzxpressArchiver();
	~LzxpressArchiver();
	static bool DumpToFile(void* hFile, unsigned char *in_buf, unsigned in_buf_len);
	static bool WriteNewString(void* hFile);

	static int is_LZF_RTF(const unsigned char *data, int len);		//! Проверяет, является ли буфер FLZ_архивом (если да, то возвращает размер разархивированных данных...или -1)
	static int is_LZF_RTF_wo_crc32(const unsigned char *data, int len);		//! Проверяет, является ли буфер FLZ_архивом, без проверки контрольной суммы crc32 (если да, то возвращает размер разархивированных данных...или -1)
	static int LZF_Uncompress(const unsigned char *in, int in_len, unsigned char *out, int out_len);	//! Возвращает количество разархивированных байт или -1, если не уложились в выходной буфер
	static int LZF_RTF_Uncompress(const unsigned char *in, int in_len, unsigned char *out, int out_len);	//! Возвращает количество разархивированных байт или -1, если не уложились в выходной буфер
	static int LZF_RTF_Uncompress_wo_crc32(const unsigned char *in, int in_len, unsigned char *out, int out_len);	//! Возвращает количество разархивированных байт или -1, если не уложились в выходной буфер

	static int LZ_Uncompress(const unsigned char * in, int in_len, unsigned char *out, int out_len);	//! Возвращает количество разархивированных байт или -1, если не уложились в выходной буфер

	static unsigned int CRC32(unsigned char* input, int length);		//! Вычисляет CRC32
private:
	static void dumpMem(unsigned char* ptr, unsigned length, char *buffer);
	static int DecodeMetadata(const unsigned char *p_metadata, short *offset, unsigned int *len);


	static unsigned char m_use_nibble_count;
	static unsigned char m_nibble_byte;//! Байт используемый для временного хранения байта длины, если длина > 10

};
unsigned char LzxpressArchiver::m_use_nibble_count = 0;
unsigned char LzxpressArchiver::m_nibble_byte = 0;

LzxpressArchiver::LzxpressArchiver()
= default;

LzxpressArchiver::~LzxpressArchiver()
= default;

void LzxpressArchiver::dumpMem(unsigned char* ptr, unsigned length, char *buffer)
{
	unsigned bytesToGo = length;

	while (bytesToGo)
	{
		const unsigned cOutput = bytesToGo >= HEX_DUMP_WIDTH ? HEX_DUMP_WIDTH : bytesToGo;

		char* buffPtr = buffer;

		buffPtr += sprintf_s(buffPtr, length, "%08X:  ", length - bytesToGo);
		char* buffPtr2 = buffPtr + (HEX_DUMP_WIDTH * 3) + 1;

		for (unsigned i = 0; i < HEX_DUMP_WIDTH; i++)
		{
			if (i >= cOutput)
			{
				// On last line.  Pad with spaces
				*buffPtr++ = ' ';
				*buffPtr++ = ' ';
				*buffPtr++ = ' ';
			}
			else
			{
				const unsigned char value = *(ptr + i);

				if (value < 0x10)
				{
					*buffPtr++ = '0';
					_itoa_s(value, buffPtr++, length,16);
				}
				else
				{
					_itoa_s(value, buffPtr, length, 16);
					buffPtr += 2;
				}

				*buffPtr++ = ' ';
				*buffPtr2++ = isprint(value) ? value : '.';
			}

			if (i == (HEX_DUMP_WIDTH / 2) - 1)
				*buffPtr++ = ' ';
		}

		*buffPtr2 = 0x0D;
		*(buffPtr2 + 1) = 0x0A;
		buffer = buffPtr2 + 2;
		bytesToGo -= cOutput;
		ptr += HEX_DUMP_WIDTH;
	}
}

bool LzxpressArchiver::DumpToFile(void* hFile, unsigned char *in_buf, unsigned in_buf_len)
{
	unsigned long written;

	const auto out_buf = static_cast<char *>(malloc(in_buf_len * 10 + 0x50));
	memset(out_buf, 0, in_buf_len * 10 + 0x50);

	dumpMem(static_cast<unsigned char*>(in_buf), in_buf_len, out_buf);

	const unsigned out_buf_len = strlen(out_buf);

	WriteFile(hFile, static_cast<char *>(out_buf), out_buf_len, &written, nullptr);
	WriteNewString(hFile);

	free(out_buf);
	return true;
}

bool LzxpressArchiver::WriteNewString(void* hFile)
{
	unsigned long BytesWritten;
	return WriteFile(hFile, "\r\n", 2, &BytesWritten, nullptr);
}

//! Считывает с p_metadata - метаданные и в offset и len кладет результирующие параметры...
//! возвращает количество прочитанных байт из входного потока
int LzxpressArchiver::DecodeMetadata(const unsigned char *p_metadata, short *offset, unsigned int *len)
{
	*offset = (*reinterpret_cast<const unsigned short *>(p_metadata)) >> 3;
	*offset ^= 0xFFFF;

	unsigned int readed = 0;
	unsigned int tmp_len = (*reinterpret_cast<const unsigned short *>(p_metadata)) & 7;
	readed += 2;

	if (tmp_len == 7)
	{
		if (m_use_nibble_count % 2 == 0)
		{
			m_nibble_byte = *(p_metadata + readed);
			++readed;
		}
		++m_use_nibble_count;


		const unsigned char nibble_byte_len = (m_nibble_byte & 0x0F);
		m_nibble_byte >>= 4;

		if (nibble_byte_len == 0x0F)
		{
			const unsigned char next_byte = *(p_metadata + readed);
			++readed;

			if (next_byte == 0xFF)
			{
				tmp_len = *reinterpret_cast<const unsigned short*>(p_metadata + readed);
				tmp_len += 3;
				*len = tmp_len;
				readed += 2;
				return readed;
			}
			tmp_len += next_byte;
		}

		tmp_len += nibble_byte_len;

	}
	tmp_len += 3;
	*len = tmp_len;
	return readed;
}

int LzxpressArchiver::LZ_Uncompress(const unsigned char* in, int in_len, unsigned char *out, int out_len)
{
	unsigned int in_index = 0;
	unsigned int out_index = 0;

	m_use_nibble_count = 0;
	m_nibble_byte = 0;

	if (out_len <= 0)
		return -1;

	while (in_index < in_len)
	{
		const unsigned int bitmask = *reinterpret_cast<const unsigned int *>(in + in_index);
		in_index += 4;
		if (in_index > in_len)
			return out_index;

		for (int j = 32;j > 0;j--)
		{
			if (bitmask >> (j - 1) & 1)
			{//! Если стоит бит, то это Metadata
				short offset;
				unsigned int len;

				in_index += DecodeMetadata(in + in_index, &offset, &len);
				if (in_index > in_len)
					return out_index;

				const unsigned short tmp_offset = offset ^ 0xFFFF;
				if (tmp_offset > out_index)
				{
					return -1;
                    //continue;
				}

				for (int k = 0;k < len;k++)
				{
                    if (out_index > out_len)
                        return -1;
                        //return out_index;

					*(out + out_index) = *(out + out_index + offset);
					++out_index;
				}

			}
			else
			{
				if (out_index > out_len)
					return -1;

				*(out + out_index) = *(in + in_index);
				++out_index;
				++in_index;

				if (in_index > in_len)
					return out_index;
			}
		}
	}

	return out_index;
}


#define DICTIONARY_SIZE		4096

int LzxpressArchiver::LZF_Uncompress(const unsigned char *in, int in_len, unsigned char *out, int out_len)
{
	unsigned char dictionary[DICTIONARY_SIZE];
	unsigned char RTF_Dictionary[207] = {
		0x7B, 0x5C, 0x72, 0x74, 0x66, 0x31, 0x5C, 0x61, 0x6E, 0x73, 0x69, 0x5C, 0x6D, 0x61, 0x63, 0x5C,
		0x64, 0x65, 0x66, 0x66, 0x30, 0x5C, 0x64, 0x65, 0x66, 0x74, 0x61, 0x62, 0x37, 0x32, 0x30, 0x7B,
		0x5C, 0x66, 0x6F, 0x6E, 0x74, 0x74, 0x62, 0x6C, 0x3B, 0x7D, 0x7B, 0x5C, 0x66, 0x30, 0x5C, 0x66,
		0x6E, 0x69, 0x6C, 0x20, 0x5C, 0x66, 0x72, 0x6F, 0x6D, 0x61, 0x6E, 0x20, 0x5C, 0x66, 0x73, 0x77,
		0x69, 0x73, 0x73, 0x20, 0x5C, 0x66, 0x6D, 0x6F, 0x64, 0x65, 0x72, 0x6E, 0x20, 0x5C, 0x66, 0x73,
		0x63, 0x72, 0x69, 0x70, 0x74, 0x20, 0x5C, 0x66, 0x64, 0x65, 0x63, 0x6F, 0x72, 0x20, 0x4D, 0x53,
		0x20, 0x53, 0x61, 0x6E, 0x73, 0x20, 0x53, 0x65, 0x72, 0x69, 0x66, 0x53, 0x79, 0x6D, 0x62, 0x6F,
		0x6C, 0x41, 0x72, 0x69, 0x61, 0x6C, 0x54, 0x69, 0x6D, 0x65, 0x73, 0x20, 0x4E, 0x65, 0x77, 0x20,
		0x52, 0x6F, 0x6D, 0x61, 0x6E, 0x43, 0x6F, 0x75, 0x72, 0x69, 0x65, 0x72, 0x7B, 0x5C, 0x63, 0x6F,
		0x6C, 0x6F, 0x72, 0x74, 0x62, 0x6C, 0x5C, 0x72, 0x65, 0x64, 0x30, 0x5C, 0x67, 0x72, 0x65, 0x65,
		0x6E, 0x30, 0x5C, 0x62, 0x6C, 0x75, 0x65, 0x30, 0x0D, 0x0A, 0x5C, 0x70, 0x61, 0x72, 0x20, 0x5C,
		0x70, 0x61, 0x72, 0x64, 0x5C, 0x70, 0x6C, 0x61, 0x69, 0x6E, 0x5C, 0x66, 0x30, 0x5C, 0x66, 0x73,
		0x32, 0x30, 0x5C, 0x62, 0x5C, 0x69, 0x5C, 0x75, 0x5C, 0x74, 0x61, 0x62, 0x5C, 0x74, 0x78,
	};

	memset(dictionary, 0, DICTIONARY_SIZE);
	memcpy(dictionary, RTF_Dictionary, 207);

	int in_index = 0;
	int out_index = 0;
	int dictionary_ptr = 207;

	if (out_len <= 0)
		return -1;

	while (in_index < in_len)
	{
		unsigned char ControlByte = *(in + in_index);
		++in_index;
		if (in_index >= in_len)
			return out_index;

		for (int i = 0;i < 8;i++)
		{
			if (ControlByte & 1)
			{
				const unsigned short dictionary_reference = NORM_UINT16(*(unsigned short*)(in + in_index));
				in_index += 2;
				if (in_index >= in_len)
					return out_index;

				const unsigned short offset = dictionary_reference >> 4;
				unsigned char length = dictionary_reference & 0x000F;
				length += 2;
				for (int k = 0; k < length; k++)
				{
					if (out_index >= out_len)
						return out_index;

					if (offset + k < DICTIONARY_SIZE)
						*(out + out_index) = *(dictionary + offset + k);
					else
						*(out + out_index) = *(dictionary + offset + k - DICTIONARY_SIZE);

					if (dictionary_ptr >= DICTIONARY_SIZE)
						dictionary_ptr = 0;

					*(dictionary + dictionary_ptr) = *(out + out_index);

					++dictionary_ptr;
					if (dictionary_ptr >= DICTIONARY_SIZE)
						dictionary_ptr = 0;

					++out_index;
				}
			}
			else
			{
				if (out_index >= out_len)
					return out_index;

				*(out + out_index) = *(in + in_index);
				*(dictionary + dictionary_ptr) = *(out + out_index);
				++dictionary_ptr;
				if (dictionary_ptr >= DICTIONARY_SIZE)
					dictionary_ptr = 0;

				++out_index;
				++in_index;
				if (in_index >= in_len)
					return out_index;

			}
			ControlByte = ControlByte >> 1;
		}

	}

	return out_index;
}


int LzxpressArchiver::LZF_RTF_Uncompress(const unsigned char *in, int in_len, unsigned char *out, int out_len)
{
	int RawLen = is_LZF_RTF(in, in_len);
	if ((RawLen == -1) || (RawLen < out_len))
		return -1;

	const PLZF_HEADER p_header = (const PLZF_HEADER)(in);
    const unsigned char* p_contents = in + sizeof(LZF_HEADER);

	if (p_header->CompType == LZF_COMPRESSED)
	{
		RawLen = LZF_Uncompress(p_contents, p_header->CompSize - 12, out, out_len);
	}
	else if (p_header->CompType == LZF_UNCOMPRESSED)
	{
		memcpy(out, p_contents, RawLen);
	}
	else
		return -1;


	return RawLen;
}

int LzxpressArchiver::LZF_RTF_Uncompress_wo_crc32(const unsigned char *in, int in_len, unsigned char *out, int out_len)
{
	int RawLen = is_LZF_RTF_wo_crc32(in, in_len);
	if ((RawLen == -1) || (RawLen < out_len))
		return -1;

	const PLZF_HEADER p_header = (const PLZF_HEADER)(in);
    const unsigned char* p_contents = in + sizeof(LZF_HEADER);

	if (p_header->CompType == LZF_COMPRESSED)
	{
		RawLen = LZF_Uncompress(p_contents, p_header->CompSize - 12, out, out_len);
	}
	else if (p_header->CompType == LZF_UNCOMPRESSED)
	{
		memcpy(out, p_contents, RawLen);
	}
	else
		return -1;


	return RawLen;
}

int LzxpressArchiver::is_LZF_RTF(const unsigned char *data, int len)
{
	if (len < sizeof(LZF_HEADER))	return -1;

	const PLZF_HEADER p_header = (const PLZF_HEADER)(data);
	//unsigned char* p_contents = data + sizeof(LZF_HEADER);

	if (p_header->CompSize > len - 4)	return -1;		//! CompSize - размер следующих данных - включая 3 поля заголовка

	if ((p_header->CompType == LZF_UNCOMPRESSED) && (p_header->CRC == 0))
	{
		return p_header->RawSize;
	}
	if (p_header->CompType == LZF_COMPRESSED)
	{
		const unsigned int crc = CRC32(reinterpret_cast<unsigned char*>(p_header) + sizeof(LZF_HEADER), p_header->CompSize - 12);

		if (crc == p_header->CRC)
			return p_header->RawSize;
		return -1;
	}
	return -1;
}

int LzxpressArchiver::is_LZF_RTF_wo_crc32(const unsigned char *data, int len)
{
	if (len < sizeof(LZF_HEADER))	return -1;

	const PLZF_HEADER p_header = (const PLZF_HEADER)(data);
	//unsigned char* p_contents = data + sizeof(LZF_HEADER);

	if (p_header->CompSize > len - 4)	return -1;		//! CompSize - размер следующих данных - включая 3 поля заголовка

	if ((p_header->CompType == LZF_UNCOMPRESSED) && (p_header->CRC == 0))
	{
		return p_header->RawSize;
	}
	if (p_header->CompType == LZF_COMPRESSED)
	{
		//! Контрольная сумма не проверяется
		return p_header->RawSize;
	}
	return -1;
}

unsigned int LzxpressArchiver::CRC32(unsigned char* input, int length)
{
	const unsigned int TABLE[256] = {
		0x00000000, 0x77073096, 0xEE0E612C, 0x990951BA, 0x076DC419, 0x706AF48F,
		0xE963A535, 0x9E6495A3, 0x0EDB8832, 0x79DCB8A4, 0xE0D5E91E, 0x97D2D988,
		0x09B64C2B, 0x7EB17CBD, 0xE7B82D07, 0x90BF1D91, 0x1DB71064, 0x6AB020F2,
		0xF3B97148, 0x84BE41DE, 0x1ADAD47D, 0x6DDDE4EB, 0xF4D4B551, 0x83D385C7,
		0x136C9856, 0x646BA8C0, 0xFD62F97A, 0x8A65C9EC, 0x14015C4F, 0x63066CD9,
		0xFA0F3D63, 0x8D080DF5, 0x3B6E20C8, 0x4C69105E, 0xD56041E4, 0xA2677172,
		0x3C03E4D1, 0x4B04D447, 0xD20D85FD, 0xA50AB56B, 0x35B5A8FA, 0x42B2986C,
		0xDBBBC9D6, 0xACBCF940, 0x32D86CE3, 0x45DF5C75, 0xDCD60DCF, 0xABD13D59,
		0x26D930AC, 0x51DE003A, 0xC8D75180, 0xBFD06116, 0x21B4F4B5, 0x56B3C423,
		0xCFBA9599, 0xB8BDA50F, 0x2802B89E, 0x5F058808, 0xC60CD9B2, 0xB10BE924,
		0x2F6F7C87, 0x58684C11, 0xC1611DAB, 0xB6662D3D, 0x76DC4190, 0x01DB7106,
		0x98D220BC, 0xEFD5102A, 0x71B18589, 0x06B6B51F, 0x9FBFE4A5, 0xE8B8D433,
		0x7807C9A2, 0x0F00F934, 0x9609A88E, 0xE10E9818, 0x7F6A0DBB, 0x086D3D2D,
		0x91646C97, 0xE6635C01, 0x6B6B51F4, 0x1C6C6162, 0x856530D8, 0xF262004E,
		0x6C0695ED, 0x1B01A57B, 0x8208F4C1, 0xF50FC457, 0x65B0D9C6, 0x12B7E950,
		0x8BBEB8EA, 0xFCB9887C, 0x62DD1DDF, 0x15DA2D49, 0x8CD37CF3, 0xFBD44C65,
		0x4DB26158, 0x3AB551CE, 0xA3BC0074, 0xD4BB30E2, 0x4ADFA541, 0x3DD895D7,
		0xA4D1C46D, 0xD3D6F4FB, 0x4369E96A, 0x346ED9FC, 0xAD678846, 0xDA60B8D0,
		0x44042D73, 0x33031DE5, 0xAA0A4C5F, 0xDD0D7CC9, 0x5005713C, 0x270241AA,
		0xBE0B1010, 0xC90C2086, 0x5768B525, 0x206F85B3, 0xB966D409, 0xCE61E49F,
		0x5EDEF90E, 0x29D9C998, 0xB0D09822, 0xC7D7A8B4, 0x59B33D17, 0x2EB40D81,
		0xB7BD5C3B, 0xC0BA6CAD, 0xEDB88320, 0x9ABFB3B6, 0x03B6E20C, 0x74B1D29A,
		0xEAD54739, 0x9DD277AF, 0x04DB2615, 0x73DC1683, 0xE3630B12, 0x94643B84,
		0x0D6D6A3E, 0x7A6A5AA8, 0xE40ECF0B, 0x9309FF9D, 0x0A00AE27, 0x7D079EB1,
		0xF00F9344, 0x8708A3D2, 0x1E01F268, 0x6906C2FE, 0xF762575D, 0x806567CB,
		0x196C3671, 0x6E6B06E7, 0xFED41B76, 0x89D32BE0, 0x10DA7A5A, 0x67DD4ACC,
		0xF9B9DF6F, 0x8EBEEFF9, 0x17B7BE43, 0x60B08ED5, 0xD6D6A3E8, 0xA1D1937E,
		0x38D8C2C4, 0x4FDFF252, 0xD1BB67F1, 0xA6BC5767, 0x3FB506DD, 0x48B2364B,
		0xD80D2BDA, 0xAF0A1B4C, 0x36034AF6, 0x41047A60, 0xDF60EFC3, 0xA867DF55,
		0x316E8EEF, 0x4669BE79, 0xCB61B38C, 0xBC66831A, 0x256FD2A0, 0x5268E236,
		0xCC0C7795, 0xBB0B4703, 0x220216B9, 0x5505262F, 0xC5BA3BBE, 0xB2BD0B28,
		0x2BB45A92, 0x5CB36A04, 0xC2D7FFA7, 0xB5D0CF31, 0x2CD99E8B, 0x5BDEAE1D,
		0x9B64C2B0, 0xEC63F226, 0x756AA39C, 0x026D930A, 0x9C0906A9, 0xEB0E363F,
		0x72076785, 0x05005713, 0x95BF4A82, 0xE2B87A14, 0x7BB12BAE, 0x0CB61B38,
		0x92D28E9B, 0xE5D5BE0D, 0x7CDCEFB7, 0x0BDBDF21, 0x86D3D2D4, 0xF1D4E242,
		0x68DDB3F8, 0x1FDA836E, 0x81BE16CD, 0xF6B9265B, 0x6FB077E1, 0x18B74777,
		0x88085AE6, 0xFF0F6A70, 0x66063BCA, 0x11010B5C, 0x8F659EFF, 0xF862AE69,
		0x616BFFD3, 0x166CCF45, 0xA00AE278, 0xD70DD2EE, 0x4E048354, 0x3903B3C2,
		0xA7672661, 0xD06016F7, 0x4969474D, 0x3E6E77DB, 0xAED16A4A, 0xD9D65ADC,
		0x40DF0B66, 0x37D83BF0, 0xA9BCAE53, 0xDEBB9EC5, 0x47B2CF7F, 0x30B5FFE9,
		0xBDBDF21C, 0xCABAC28A, 0x53B39330, 0x24B4A3A6, 0xBAD03605, 0xCDD70693,
		0x54DE5729, 0x23D967BF, 0xB3667A2E, 0xC4614AB8, 0x5D681B02, 0x2A6F2B94,
		0xB40BBE37, 0xC30C8EA1, 0x5A05DF1B, 0x2D02EF8D };

	unsigned int tmp = 0;
	while (length >= 16)
	{
		tmp = TABLE[(tmp ^ input[0]) & 0xFF] ^ (tmp >> 8);
		tmp = TABLE[(tmp ^ input[1]) & 0xFF] ^ (tmp >> 8);
		tmp = TABLE[(tmp ^ input[2]) & 0xFF] ^ (tmp >> 8);
		tmp = TABLE[(tmp ^ input[3]) & 0xFF] ^ (tmp >> 8);
		tmp = TABLE[(tmp ^ input[4]) & 0xFF] ^ (tmp >> 8);
		tmp = TABLE[(tmp ^ input[5]) & 0xFF] ^ (tmp >> 8);
		tmp = TABLE[(tmp ^ input[6]) & 0xFF] ^ (tmp >> 8);
		tmp = TABLE[(tmp ^ input[7]) & 0xFF] ^ (tmp >> 8);
		tmp = TABLE[(tmp ^ input[8]) & 0xFF] ^ (tmp >> 8);
		tmp = TABLE[(tmp ^ input[9]) & 0xFF] ^ (tmp >> 8);
		tmp = TABLE[(tmp ^ input[10]) & 0xFF] ^ (tmp >> 8);
		tmp = TABLE[(tmp ^ input[11]) & 0xFF] ^ (tmp >> 8);
		tmp = TABLE[(tmp ^ input[12]) & 0xFF] ^ (tmp >> 8);
		tmp = TABLE[(tmp ^ input[13]) & 0xFF] ^ (tmp >> 8);
		tmp = TABLE[(tmp ^ input[14]) & 0xFF] ^ (tmp >> 8);
		tmp = TABLE[(tmp ^ input[15]) & 0xFF] ^ (tmp >> 8);
		input += 16;
		length -= 16;
	}

	for (unsigned int j = 0; j != length; j++)
		tmp = TABLE[(tmp ^ input[j]) & 0xFF] ^ (tmp >> 8);

	return tmp;
}
}