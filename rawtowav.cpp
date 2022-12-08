#include <cerrno>
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>

#define INPUT_FILE_DIR "/media/rafael-user/HDD2/Common/AudioFiles/RAW/tmp/audioDSP.raw"
#define OUTPUT_FILE_DIR "/media/rafael-user/HDD2/Common/AudioFiles/WAVE/GNU-Linux-DSP/audio.wav"

#define BITS_PER_SAMPLE 16
#define NUMBER_OF_CHANNELS 2
#define SAMPLE_RATE 44100

#define BUFFER_SIZE_BYTES 1024

std::fstream input_file;
unsigned int input_file_size = 0;
unsigned int input_file_pos = 0;

std::fstream output_file;
unsigned int output_file_pos = 0;

char *bytebuffer = NULL;

bool read_proc(void)
{
	if(input_file_pos >= input_file_size) return false;

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

void write_output_file_header(void)
{
	char *header_info = (char*) malloc(44);
	unsigned short *pushort = NULL;
	unsigned int *puint = NULL;

	header_info[0] = 'R';
	header_info[1] = 'I';
	header_info[2] = 'F';
	header_info[3] = 'F';

	puint = (unsigned int*) &header_info[4];
	puint[0] = input_file_size + 36;

	header_info[8] = 'W';
	header_info[9] = 'A';
	header_info[10] = 'V';
	header_info[11] = 'E';

	header_info[12] = 'f';
	header_info[13] = 'm';
	header_info[14] = 't';
	header_info[15] = ' ';

	puint = (unsigned int*) &header_info[16];
	puint[0] = 16;

	pushort = (unsigned short*) &header_info[20];
	pushort[0] = 1;
	pushort[1] = NUMBER_OF_CHANNELS;

	puint = (unsigned int*) &header_info[24];
	puint[0] = SAMPLE_RATE;
	puint[1] = NUMBER_OF_CHANNELS*BITS_PER_SAMPLE*SAMPLE_RATE/8;

	pushort = (unsigned short*) &header_info[32];
	pushort[0] = NUMBER_OF_CHANNELS*BITS_PER_SAMPLE/8;
	pushort[1] = BITS_PER_SAMPLE;

	header_info[36] = 'd';
	header_info[37] = 'a';
	header_info[38] = 't';
	header_info[39] = 'a';

	puint = (unsigned int*) &header_info[40];
	puint[0] = input_file_size;

	output_file.seekg(0);
	output_file.write(header_info, 44);

	output_file_pos = 44;
	free(header_info);
	return;
}

bool open_input_file(void)
{
	input_file.open(INPUT_FILE_DIR, std::ios_base::in);
	if(input_file.is_open())
	{
		input_file.seekg(0, input_file.end);
		input_file_size = input_file.tellg();
		input_file_pos = 0;
		input_file.seekg(input_file_pos);
		return true;
	}

	return false;
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
	if(!open_input_file())
	{
		std::cout << "Error opening input file\nError code: " << errno << "\nTerminated\n";
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
	write_output_file_header();
	buffer_malloc();

	std::cout << "Running...\n";
	while(runtime_proc());
	std::cout << "Done\n";

	input_file.close();
	output_file.close();
	buffer_free();

	std::cout << "Terminated\n";
	return 0;
}
