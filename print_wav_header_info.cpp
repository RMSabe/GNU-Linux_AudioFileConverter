//Auxiliary Tool: This code is used (if needed) to view the playback parameters from a wave (.wav) audio file.

#include <cerrno>
#include <fstream>
#include <iostream>
#include <string>

std::fstream wav_file;
std::string file_dir = "";
unsigned int file_size = 0;

bool compare_signature(const char *auth, const char *byte_buf, unsigned int byte_offset)
{
	if(byte_buf[byte_offset] != auth[0]) return false;
	if(byte_buf[byte_offset + 1] != auth[1]) return false;
	if(byte_buf[byte_offset + 2] != auth[2]) return false;
	if(byte_buf[byte_offset + 3] != auth[3]) return false;

	return true;
}

void print_header_info(void)
{
	char *header_info = (char*) malloc(1024);
	unsigned short *pushort = NULL;
	unsigned int *puint = NULL;

	wav_file.seekg(0);
	wav_file.read(header_info, 1024);
	unsigned int byte_pos = 0;

	std::cout << "Chunk Signature: " << header_info[byte_pos] << header_info[byte_pos + 1] << header_info[byte_pos + 2] << header_info[byte_pos + 3] << std::endl;
	puint = (unsigned int*) &header_info[byte_pos + 4];
	std::cout << "Chunk Size: " << puint[0] << " bytes\n";
	std::cout << "Format Signature: " << header_info[byte_pos + 8] << header_info[byte_pos + 9] << header_info[byte_pos + 10] << header_info[byte_pos + 11] << std::endl;
	byte_pos += 12;

	while(!compare_signature("fmt ", header_info, byte_pos))
	{
		if(byte_pos >= 1016)
		{
			std::cout << "Error occurred\n";
			return;
		}

		puint = (unsigned int*) &header_info[byte_pos + 4];
		byte_pos += puint[0] + 8;
		std::cout << "Subchunk Skipped\n";
	}

	std::cout << "Subchunk Signature: " << header_info[byte_pos] << header_info[byte_pos + 1] << header_info[byte_pos + 2] << header_info[byte_pos + 3] << std::endl;
	puint = (unsigned int*) &header_info[byte_pos + 4];
	std::cout << "Subchunk Size: " << puint[0] << " bytes\n";

	pushort = (unsigned short*) &header_info[byte_pos + 8];
	std::cout << "Encoding Type: " << pushort[0] << std::endl;
	std::cout << "Number Of Channels: " << pushort[1] << std::endl;

	puint = (unsigned int*) &header_info[byte_pos + 12];
	std::cout << "Sample Rate: " << puint[0] << " Hz\n";
	std::cout << "Bytes Per Second: " << puint[1] << std::endl;

	pushort = (unsigned short*) &header_info[byte_pos + 20];
	std::cout << "Block Size: " << pushort[0] << " bytes\n";
	std::cout << "Sample Size: " << pushort[1] << " bits\n";

	puint = (unsigned int*) &header_info[byte_pos + 4];
	byte_pos += (puint[0] + 8);

	while(!compare_signature("data", header_info, byte_pos))
	{
		if(byte_pos >= 1016)
		{
			std::cout << "Error occurred\n";
			return;
		}

		puint = (unsigned int*) &header_info[byte_pos + 4];
		byte_pos += puint[0] + 8;
		std::cout << "Subchunk Skipped\n";
	}

	std::cout << "Subchunk Signature: " << header_info[byte_pos] << header_info[byte_pos + 1] << header_info[byte_pos + 2] << header_info[byte_pos + 3] << std::endl;
	puint = (unsigned int*) &header_info[byte_pos + 4];
	std::cout << "Subchunk Size: " << puint[0] << " bytes\n";

	free(header_info);
	return;
}

bool open_file(void)
{
	wav_file.open(file_dir.c_str(), std::ios_base::in);
	if(wav_file.is_open())
	{
		wav_file.seekg(0, wav_file.end);
		file_size = wav_file.tellg();
		return true;
	}

	return false;
}

bool file_is_valid(void)
{
	unsigned int dir_len = file_dir.length();
	const char *dir = file_dir.c_str();
	if(compare_signature(".wav", dir, (dir_len - 4))) return true;
	if(compare_signature(".WAV", dir, (dir_len - 4))) return true;

	return false;
}

int main(int argc, char **argv)
{
	if(argc < 2)
	{
		std::cout << "Error: missing arguments\nTerminated\n";
		return 0;
	}

	file_dir = argv[1];
	if(!open_file())
	{
		std::cout << "Error opening file\nError code: " << errno << "\nTerminated\n";
		return 0;
	}
	if(!file_is_valid())
	{
		std::cout << "Error: file is invalid\nTerminated\n";
		return 0;
	}

	std::cout << "File is open\n";
	std::cout << "File Directory: " << file_dir.c_str() << std::endl;
	std::cout << "File Size: " << file_size << " bytes\n\n";
	print_header_info();

	wav_file.close();
	std::cout << "Terminated\n";
	return 0;
}
