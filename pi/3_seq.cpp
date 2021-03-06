#include <iostream>
#include <random>
#include <cmath>
#include <fstream>
#include <cstdio>

const uint64_t MAX_NB_RANDOM_POINT = 1000000000;

void runMonteCarloPiCalc();

int main(int argc, char* argv[]) {
	runMonteCarloPiCalc();
	return 0;
}

// remove file
void cleanUp(std::string filename) {
	if (std::remove(filename.c_str())!=0){
		std::cout << "Nothing to delete" << std::endl;
	}
	else {
		std::cout << filename << " has been successfully deleted" << std::endl;
	}
}

void runMonteCarloPiCalc() {
	int ncirc = 0;
	double r = 1.0;
	double pi, x, y;
	unsigned int iterSize = 10000;
	double epsilon = 0.1;

	do {
		std::cout << "Enter desired accuracy (format e=0.0001)" << std::endl;
		std::cin >> epsilon;
	} while (epsilon >= 1 || epsilon <= 0);

	std::random_device	rand_dev;
	std::default_random_engine generator(rand_dev());
	std::uniform_real_distribution<double> distribution(-1.0, 1.0);

	std::string inFileName = "insideOfCircle.dat";
	std::string outFileName = "outsideOfCircle.dat";
	cleanUp(inFileName);
	cleanUp(outFileName);
	std::ofstream insideOfCircle;
	insideOfCircle.open (inFileName);

	std::ofstream outsideOfCircle;
  	outsideOfCircle.open (outFileName);

	int iter = 0;
	double error = 1;

  	while (error > epsilon || (iterSize * iter) > MAX_NB_RANDOM_POINT) {
  		iter++;
		for (int i = 0; i < iterSize; i++)
		{
			x = distribution(generator);
			y = distribution(generator);
			if ((x * x + y * y) <= r * r) {
				ncirc++;
				insideOfCircle << x << " " << y << std::endl;
			}
			else {
				outsideOfCircle << x << " " << y << std::endl;
			}
		}
		pi = 4.0 * ((double) ncirc / ((double) iterSize * iter));
		error = std::abs(M_PI - pi);
	}

	insideOfCircle.close();
	outsideOfCircle.close();

	std::cout << "Number of iteration " << iter << std::endl;
	std::cout << "epsilon " << epsilon << std::endl; 
	printf("PI ~ %.20f\n", pi);
}

// gnuplot script
// set object 1 circle front at 0,0 size 1 fillcolor rgb "black" lw 1
// set xrange [-1:1]
// set yrange [-1:1]
// set size square
// plot "insideOfCircle.dat", "outsideOfCircle.dat"