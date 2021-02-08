#include <iostream>
#include <vector>
#include <complex>
#include <math.h>
#include <random>
#include <pthread.h>
#include <fstream>
using namespace std;

#define PRECISION 50000
#define NUM_THREADS 4
#define DEBUG 1
pair<double, int> kappas[4] = {{0.5, 1}, {1, 2}, {2, 3}, {4, 4}};

vector<double> t;
vector<double> delta_t;
vector<double> random_samples;
vector<double> walk;



void init(){
	t = vector<double>(PRECISION + 1, 0);
	for (size_t i = 1; i < PRECISION + 1; i++)
		t[i] = pow(((double) i / PRECISION), (double) 3/2);
	
	delta_t = vector<double>(PRECISION, 0);
	for (size_t i = 0; i < PRECISION; i++)
		delta_t[i] = t[i+1] - t[i];

	default_random_engine generator;
	normal_distribution<double> distrib(0.0, 1.0);
	random_samples = vector<double>(PRECISION);
	for (size_t i = 0; i < PRECISION; i++)
		random_samples[i] = distrib(generator); 

	walk = vector<double>(PRECISION+1, 0);
	for (size_t i = 1; i < walk.size(); i++)
		walk[i] = walk[i - 1] + random_samples[i - 1];
}

void* sle(void* arg){
	pair<double, int>* a = (pair<double, int>*) arg;
	double kappa = a->first;
	int thread_id = a->second;


	vector<double> delta(PRECISION);
	for (size_t i = 0; i < delta.size(); i++)
		delta[i] = random_samples[i]*sqrt(kappa*delta_t[i]);

	vector<double> alpha(PRECISION);
	for (size_t i = 0; i < alpha.size(); i++){
		double tmp = pow(delta[i], 2) / delta_t[i];
		double sign = delta[i]/abs(delta[i]);
		alpha[i] = 0.5 - sign/2 * sqrt(tmp/(16+tmp));
	}

	cout << "starting calculations\n";

	ofstream file;
	file.open("output_" + to_string(thread_id));
	if (!file.is_open()){
		cout << "thread_id " << thread_id << " failed to open file, exiting\n";
		return NULL;
	}

	for (size_t i = 1; i < PRECISION+1; i++){
		complex<double> z = 0;
		for (int j = i - 1; j >= 0; j--){
			z = pow((z+2*sqrt(delta_t[j]*(1-alpha[j])/alpha[j])), 1 - alpha[j]) * pow(z-2*sqrt(delta_t[j]*alpha[j]/(1-alpha[j])), alpha[j]);
		}
		file << z << endl;
#if DEBUG
		if (!(i % 1000))
			cout << "thread_id " << thread_id << "\t" << i << ":" << z << endl;
#endif
	}
	file.close();

	return NULL;
}

int main(){
	init();
	ofstream file;
	file.open("walk");
	if (!file.is_open()){
		cout << "failed to open/create walk file, exiting\n";
		return 1;
	}

	for (size_t i = 0; i < walk.size(); i++)
		file << walk[i] << endl;
	file.close();

	cout << "init done\n";
	pthread_t threads[NUM_THREADS];
	for (size_t i = 0; i < NUM_THREADS; i++)
		pthread_create(&threads[i], NULL, sle, (void*) &(kappas[i])); 

	for (size_t i = 0; i < NUM_THREADS; i++)
		pthread_join(threads[i], NULL);
}
