#include <mpi.h>
#include <iostream>
#include <cmath>

void mpiTrapeziumPiCalc(int argc, char *argv[]);

int main(int argc, char *argv[]) {
	mpiTrapeziumPiCalc(argc, argv);
	return 0;
}

double f(double _val) {
	return 4.0 / (1 + _val * _val);
}

double intevalTrapeziumPiCalc(int _rank, int _mpiSize, unsigned int _n) {
	double result = 0;
	for (int i = _rank; i < _n; i+= _mpiSize)
		result += f(i / static_cast<double>(_n)) + f((i + 1) / static_cast<double>(_n));
	return result;
}

double errorApproximation(unsigned int _n) {
	return (2.0 / (3.0 * _n * _n));
}

void mpiTrapeziumPiCalc(int argc, char *argv[])
{
	int32_t size = 0;
	int32_t rank = 0;
	uint32_t n = 0;
	double	pi = 0;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);	// Get #processes
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);	// Get current rank

	MPI_Barrier(MPI_COMM_WORLD);
	if (rank == 0) {
		std::cout << "Enter number of intervals (n) " << std::endl;
		std::cin >> n;
	}

	MPI_Barrier(MPI_COMM_WORLD);
	double startTime = MPI_Wtime();
	MPI_Bcast(&n, /*count*/1, MPI_INT, /*root*/0, MPI_COMM_WORLD);
	double intervalResult = intevalTrapeziumPiCalc(rank, size, n);
	MPI_Reduce(&intervalResult, &pi, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
	double endTime = MPI_Wtime();

	if (rank == 0)
	{
		pi /= 2.0 * n;
		double realError = std::abs(M_PI - pi);
		double approximatedError  errorApproximation(n);
		std::cout << "Trapezium Pi = " << pi 
				<< " with n = " << n 
				<< " intervals" << std::endl;
		std::cout << "Real Error = " << realError  std::endl;
		std::cout << "Approximated Error = " << approximatedError << std::endl;
		std::cout << "WTime difference = " << endTime - startTime << std::endl;
	}

	MPI_Finalize();
}