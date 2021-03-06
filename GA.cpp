//改进// GA.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <chrono>

#define POPSIZE 100		//population size
#define MAXGENS 500		//max number of generation
#define DIM 2			//dimension
//#define PXOVER 0.8		//probability of crossover
double PXOVER = 0.6;
//#define PMUTATION 0.001	//probability of mutation
double PMUTATION = 0.01;
#define MIN_1 -3.0
#define MAX_1 12.1
#define MIN_2 4.1
#define MAX_2 5.8
#define PI 3.1415926

int genertaion;
bool isUpdate = false;


double upper[DIM + 2];
double lower[DIM + 2];

double pop[POPSIZE + 5][DIM + 2];
double fitness[POPSIZE + 5];
double relative_fit[POPSIZE + 5];
double cumulative_fit[POPSIZE + 5];

double globalBest[DIM + 2];		//全局最优
double bestFitness;

double newpop[POPSIZE + 5][DIM + 2];
double newfitness[POPSIZE + 5];
double newrelative[POPSIZE + 5];
double newcumulative[POPSIZE + 5];


//declaration of procedures
void init(void);
double rand_doul(double, double);
double evaluate(double*);
void keep_the_best(void);
void elitist(void);
void select(void);
void crossover(void);
void Xover(int, int);
void swap(double*, double*);
void mutate(void);
void report(void);
double gauss();
int rand_int(int, int);
void championship_select();

//产生low 到 high 之间的随机数
double rand_doul(double low, double high)
{
	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	srand(seed);
	double val;
	val = ((double)(rand() % 1000) / 1000.0) * (high - low) + low;
	return val;
}

//产生low 到 high 之间的整数
int rand_int(int low, int high)
{
	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	srand(seed);
	int val = rand() % (high - low) + low;
	return val;
}

//高斯分布
double gauss()
{
	static double U, V;
	static int phase = 0;
	double z;
	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	srand(seed);

	if (phase == 0)
	{
		U = rand() / (RAND_MAX + 1.0);
		V = rand() / (RAND_MAX + 1.0);
		z = sqrt(-2.0 * log(U)) * sin(2.0 * PI * V);
	}
	else
	{
		z = sqrt(-2 * log(U)) * cos(2.0 * PI * V);
	}
	phase = 1 - phase;
	return z;
}

//计算函数
double evaluate(double* x)
{
	return 21.5 + x[1] * sin(4 * PI*x[1]) + x[2] * sin(20 * PI*x[2]);
}

//初始化
void init(void)
{
	//设置边界
	upper[1] = MAX_1;
	upper[2] = MAX_2;
	lower[1] = MIN_1;
	lower[2] = MIN_2;

	for (int i = 1; i <= POPSIZE; i++)
	{
		fitness[i] = relative_fit[i] = cumulative_fit[i] = 0;
		for (int j = 1; j <= DIM; j++)
		{
			pop[i][j] = rand_doul(lower[j], upper[j]);
		}
	}

	bestFitness = 0;
	for (int j = 1; j <= DIM; j++)
	{
		globalBest[j] = 0;
	}

}

//保存最优染色体
void keep_the_best()
{
	int cur_best = 0;			//the index of the best individual

	for (int men = 1; men <= POPSIZE; men++)
	{
		if (fitness[men] > bestFitness)
		{
			cur_best = men;
			bestFitness = fitness[men];
		}
	}

	for (int i = 1; i <= DIM; i++)
	{
		globalBest[i] = pop[cur_best][i];

	}

}

//精英更新
void elitist()
{
	double best, worst;
	int best_index, worst_index;

	best = fitness[1];
	worst = fitness[1];
	best_index = 1;
	worst_index = 1;

	for (int i = 1; i <= POPSIZE; i++)
	{
		if (fitness[i] > best)
		{
			best = fitness[i];
			best_index = i;
		}
		else if (fitness[i] < worst)
		{
			worst = fitness[i];
			worst_index = i;
		}
	}

	if (best >= bestFitness)
	{
		isUpdate = true;
		for (int i = 1; i <= DIM; i++)
		{
			globalBest[i] = pop[best_index][i];
		}
		bestFitness = best;
	}
	else
	{
		isUpdate = false;
		for (int i = 1; i <= DIM; i++)
		{
			pop[worst_index][i] = globalBest[i];
		}
		fitness[worst_index] = bestFitness;
	}
}

//成比例选择
void select()
{
	double sum = 0.0;
	double p = 0.0;
	for (int mem = 1; mem <= POPSIZE; mem++)
	{
		sum += fitness[mem];
	}

	//计算relative_fit
	for (int mem = 1; mem <= POPSIZE; mem++)
	{
		relative_fit[mem] = fitness[mem] / sum;
	}

	//计算cumulative_fit
	cumulative_fit[1] = relative_fit[1];
	for (int mem = 2; mem <= POPSIZE; mem++)
	{
		cumulative_fit[mem] = cumulative_fit[mem - 1] + relative_fit[mem];
	}

	//选择生存下来的
	for (int i = 1; i <= POPSIZE; i++)
	{
		p = rand() % 1000 / 1000.0;
		if (p < cumulative_fit[0])
		{
			for (int k = 1; k <= DIM; k++)
			{
				newpop[i][k] = pop[1][k];
			}
			newfitness[i] = fitness[1];
			newcumulative[i] = cumulative_fit[1];
			newrelative[i] = relative_fit[1];
		}

		else
		{
			for (int j = 1; j < POPSIZE; j++)
			{
				if (p >= cumulative_fit[j] && p < cumulative_fit[j + 1])
				{
					for (int k = 1; k <= DIM; k++)
					{
						newpop[i][k] = pop[j + 1][k];
					}
					newfitness[i] = fitness[j + 1];
					newcumulative[i] = cumulative_fit[j + 1];
					newrelative[i] = relative_fit[j + 1];
				}
			}
		}
	}

	//更新
	for (int i = 1; i <= POPSIZE; i++)
	{
		for (int k = 1; k <= DIM; k++)
		{
			pop[i][k] = newpop[i][k];
		}
		fitness[i] = newfitness[i];
		cumulative_fit[i] = newcumulative[i];
		relative_fit[i] = newrelative[i];
	}

}

//锦标赛选择法
void championship_select()
{
	int cnt = 0;
	while (cnt <= POPSIZE)
	{
		int p1 = rand_int(1, POPSIZE);
		int p2 = rand_int(1, POPSIZE);

		if (p1 == p2) continue;
		p1 = fitness[p1] > fitness[p2] ? p1 : p2;

		cnt++;
		for (int k = 1; k <= DIM; k++)
		{
			newpop[cnt][k] = pop[p1][k];
		}
		newfitness[cnt] = fitness[p1];
		newcumulative[cnt] = cumulative_fit[p1];
		newrelative[cnt] = relative_fit[p1];

	}

	for (int i = 1; i <= POPSIZE; i++)
	{
		for (int k = 1; k <= DIM; k++)
		{
			pop[i][k] = newpop[i][k];
		}
		fitness[i] = newfitness[i];
		cumulative_fit[i] = newcumulative[i];
		relative_fit[i] = newrelative[i];
	}

}

//交配
void crossover()
{
	double p;
	int first = 0;
	int one;
	for (int mem = 1; mem < POPSIZE; mem++)
	{
		p = rand() % 1000 / 1000.0;
		if (p < PXOVER)
		{
			first++;
			if (first % 2 == 0)	Xover(one, mem);
			else	one = mem;
		}
	}
}

void Xover(int one, int two)
{
	//单点交配

	int point;
	if (DIM > 1)
	{
		if (DIM == 2)
			point = 1;
		else point = (rand() % (DIM - 1)) + 1;

		for (int i = 1; i <= point; i++)
		{
			swap(&pop[one][i], &pop[two][i]);
		}
	}
	/*
	//随机多点交配
	int length = rand_int(DIM / 6, DIM / 4);

	for (int j = 1; j <= DIM; j += length)
	{
	for (int i = 1; i <= length; i++)
	{
	swap(&pop[one][j + i], &pop[two][j + i]);
	}
	}
	*/
}

void swap(double *x, double *y)
{
	double temp;
	temp = *x;
	*x = *y;
	*y = temp;

}

//变异
void mutate()
{
	double p = 0.0;
	double add = 0.0;
	for (int i = 1; i <= POPSIZE; i++)
	{
		for (int j = 1; j <= DIM; j++)
		{
			p = rand() % 1000 / 1000.0;
			if (p < PMUTATION)
			{
				//pop[i][j] = randval(lower[j], upper[j]);
				while (1)
				{
					add = gauss();
					if ((pop[i][j] + add >= lower[j]) && (pop[i][j] + add <= upper[j]))
					{
						pop[i][j] += add;
						break;
					}
				}
			}


		}
	}
}

void report()
{

	double best_val;            /* best population fitness */
	double avg;                 /* avg population fitness */
	double stddev;              /* std. deviation of population fitness */
	double sum_square;          /* sum of square for std. calc */
	double square_sum;          /* square of sum for std. calc */
	double sum;                 /* total population fitness */

	sum = 0.0;
	sum_square = 0.0;

	for (int i = 0; i < POPSIZE; i++)
	{
		sum += fitness[i];
		sum_square += fitness[i] * fitness[i];
	}

	avg = sum / (double)POPSIZE;
	square_sum = avg * avg * POPSIZE;
	stddev = sqrt((sum_square - square_sum) / (POPSIZE - 1));
	best_val = bestFitness;
}

int main()
{
	int t = 1;
	double sum = 0.0;
	while (t <= 30)
	{
		unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
		srand(seed);
		genertaion = 0;
		init();
		for (int i = 1; i <= POPSIZE; i++)
		{
			fitness[i] = evaluate(pop[i]);
		}
		keep_the_best();
		while (genertaion < MAXGENS)
		{
			srand((unsigned)time(NULL));
			genertaion++;
			//select();
			championship_select();
			crossover();
			mutate();
			report();

			for (int i = 1; i <= POPSIZE; i++)
			{
				fitness[i] = evaluate(pop[i]);
			}
			//精英更新
			elitist();

			//自适应交配概率
			if ((!isUpdate) && (PXOVER < 0.9) && (PMUTATION <0.1))
			{
				//如果没有提升,那么交配概率进一步提升
				PXOVER += 0.01;
				PMUTATION += 0.001;
			}
			/*
			//当前最优显示
			for (int i = 1; i <= DIM; i++)
			{
				printf("%f  ", globalBest[i]);
			}
			printf("\n");
			printf("BestFitness:  %f \n\n", bestFitness);
			*/
		}

		//迭代结束，输出结果
		printf("\n Finally \n");
		printf("The PXOVER: %f   The PMUTATION: %f \n", PXOVER, PMUTATION);
		for (int i = 1; i <= DIM; i++)
		{
			printf("%f  ", globalBest[i]);
		}
		printf("\n");
		printf("BestFitness:   %f\n ", bestFitness);
		
		
		sum += bestFitness;
		t++;
	}

	printf("\n\n改进后的遗传算法测试30次之后\n");
	printf("平均值为： %f\n", sum / 30);

	printf("\n The End \n");
	return 0;
}

