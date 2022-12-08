//Audio File Converter: wave (.wav) to headerless (.raw)

#include <cerrno>
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>

#define INPUT_FILE_DIR "/home/user/input_audio.wav"
#define OUTPUT_FILE_DIR "/home/user/output_audio.raw"

#define BUFFER_SIZE_BYTES 1024

std::fstream input_file;
unsigned int input_file_pos = 0;
unsigned int data_begin = 0;
unsigned int data_end = 0;

std::fstream output_file;
unsigned int output_file_pos = 0;

char *bytebuffer = NULL;

bool read_proc(void)
{
	if(input_file_pos >= data_end) return false;

	input_file.seekg(input_file_pos);
	input_file.read(bytebuffer, BUFFER_SIZE_BYTES);
	input_file_pos += BUFFER_SIZE_BYTES;
	return true;
}

void write_proc(void)
{
	output_file.seekg(output_file_pos);
	output_file.write(bytebuffer, BUFFER_SIZE_BYTES);
	output_file_pos += BUFFER_SIZE_BYTES;
	return;
}

bool runtime_proc(void)
{
	if(!read_proc()) return false;
	write_proc();
	return true;
}

void buffer_malloc(void)
{
	bytebuffer = (char*) malloc(BUFFER_SIZE_BYTES);
	memset(bytebuffer, 0, BUFFER_SIZE_BYTES);
	return;
}

void buffer_free(void)
{
	free(bytebuffer);
	return;
}

bool compare_signature(const char *auth, const char *bytebuf, unsigned int offset)
{
	if(bytebuf[offset] != auth[0]) return false;
	if(bytebuf[offset + 1] != auth[1]) return false;
	if(bytebuf[offset + 2] != auth[2]) return false;
	if(bytebuf[offset + 3] != auth[3]) return false;

	return true;
}

bool fetch_header_data(void)
{
	char *header_info = (char*) malloc(1024);
	unsigned int *puint = NULL;

	input_file.seekg(0);
	input_file.read(header_info, 1024);
	unsigned int bytepos = 0;

	if(!compare_signature("RIFF", header_info, bytepos)) return false;
	if(!compare_signature("WAVE", header_info, (bytepos + 8))) return false;

	bytepos += 12;

	while(!compare_signature("data", header_info, bytepos))
	{
		if(bytepos >= 1000) return false;

		puint = (unsigned int*) &header_info[bytepos + 4];
		bytepos += (puint[0] + 8);
	}

	puint = (unsigned int*) &header_info[bytepos + 4];
	data_begin = bytepos + 8;
	data_end = data_begin + puint[0];

	free(header_info);
	return true;
}

bool input_file_extension_is_valid(void)
{
	std::string dir = INPUT_FILE_DIR;
	unsigned int len = dir.length();

	if(compare_signature(".wav", dir.c_str(), (len - 4))) return true;
	if(compare_signature(".WAV", dir.c_str(), (len - 4))) return true;

	return false;
}

bool open_input_file(void)
{
	input_file.open(INPUT_FILE_DIR, std::ios_base::in);
	return input_file.is_open();
}

bool open_output_file(void)
{
	output_file.open(OUTPUT_FILE_DIR, (std::ios_base::in | std::ios_base::out));
	return output_file.is_open();
}

void create_output_file(void)
{
	std::string cmd_line = "";

	if(open_output_file())
	{
		output_file.close();
		cmd_line = "rm ";
		cmd_line += OUTPUT_FILE_DIR;
		system(cmd_line.c_str());
	}

	cmd_line = "touch ";
	cmd_line += OUTPUT_FILE_DIR;
	system(cmd_line.c_str());
	return;
}

int main(int argc, char **argv)
{
	if(!input_file_extension_is_valid())
	{
		std::cout << "Error: input file is not a WAVE file\nTerminated\n";
		return 0;
	}

	if(!open_input_file())
	{
		std::cout << "Error opening input file\nError code: " << errno << "\nTerminated\n";
		return 0;
	}

	if(!fetch_header_data())
	{
		input_file.close();
		std::cout << "Error collecting wav header data\nTerminated\n";
		return 0;
	}

	create_output_file();
	if(!open_output_file())
	{
		input_file.close();
		std::cout << "Error opening output file\nError code: " << errno << "\nTerminated\n";
		return 0;
	}

	std::cout << "Started\n";

	buffer_malloc();
	input_file_pos = data_begin;
	output_file_pos = 0;

	std::cout << "Running...\n";
	while(runtime_proc());
	std::cout << "Done\n";

	input_file.close();
	output_file.close();
	buffer_free();

	std::cout << "Terminated\n";
	return 0;
}
