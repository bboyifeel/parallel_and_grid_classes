#include <mpi.h>
#include <iostream>
#include <bitset>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <chrono>

const int32_t 	kinput_size = 65536;

bool extract_bit(int32_t _n, int32_t _i)
{
	return (_n & (1 << _i) ? 1 : 0);
}

bool check_circuit(int32_t input)
{
	bool v[16];
	for (int i = 0; i < 16; i++)
		v[i] = extract_bit(input, i);
	return ((v[0] || v[1]) && (!v[1] || !v[3]) && (v[2] || v[3])
		&& (!v[3] || !v[4]) && (v[4] || !v[5]) && (v[5] || !v[6])
		&& (v[5] || v[6]) && (v[6] || !v[15]) && (v[7] || !v[8])
		&& (!v[7] || !v[13]) && (v[8] || v[9]) && (v[8] || !v[9])
		&& (!v[9] || !v[10]) && (v[9] || v[11]) && (v[10] || v[11])
		&& (v[12] || v[13]) && (v[13] || !v[14]) && (v[14] || v[15]));
}

void print_solution(int32_t _sol)
{
	std::cout << "found solution '0x" << std::hex << _sol << "' = " << std::bitset<16>(_sol) << " = " << std::dec << _sol;
}

class Timer
{
public:
	Timer() : beg_(clock_::now()) {}
	void reset() { beg_ = clock_::now(); }
	double elapsed() const {
		return std::chrono::duration_cast<second_>
			(clock_::now() - beg_).count();
	}

private:
	typedef std::chrono::high_resolution_clock clock_;
	typedef std::chrono::duration<double, std::ratio<1> > second_;
	std::chrono::time_point<clock_> beg_;
};


int main(int argc, char *argv[])
{
	Timer timer;

	int32_t size = 0;
	int32_t id = 0;
	int32_t local_count = 0;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);	// Get #processes
	MPI_Comm_rank(MPI_COMM_WORLD, &id);		// Get current rank

	for (int i = id; i < kinput_size; i += size)
	{
		if (check_circuit(i))
		{
			local_count++;
			std::cout << "[Node " << id << "] ";
			print_solution(i);
			std::cout << std::endl;
		}
	}
	std::cout << "[Node " << id << "] done!" << std::endl;
	double sec = timer.elapsed();

	int32_t global_count = 0;
	MPI_Reduce(&local_count, &global_count, 1, MPI_INT32_T, MPI_SUM, 0, MPI_COMM_WORLD);
	double minTime = 0;
	double avgTime = 0;
	double maxTime = 0;
	MPI_Reduce(&sec, &minTime, 1, MPI_DOUBLE, MPI_MIN, 0, MPI_COMM_WORLD);
	MPI_Reduce(&sec, &maxTime, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
	MPI_Reduce(&sec, &avgTime, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
	avgTime /= size;

	// Print the result
	std::cout << "[Node " << id << "] execution time = " << sec << " secs;" << std::endl;
	MPI_Barrier(MPI_COMM_WORLD);
	if (id == 0) {
		std::cout << "Total number of solutions = " << global_count << std::endl;
		std::cout << "Min execution time = " << minTime << " secs;" << std::endl;
		std::cout << "Max execution time = " << maxTime << " secs;" << std::endl;
		std::cout << "Avg execution time = " << avgTime << " secs;" << std::endl;
	}
	
	MPI_Finalize();

	return 0;
}