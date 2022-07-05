#include "return_codes.h"

#include <math.h>
#include <stdio.h>

void changeToMax(size_t startFrom, float** A, size_t n, float* B)
{
	_Bool found = 0;
	float max = 0;
	size_t maxI;
	for (size_t i = startFrom; i < n; i++)
	{
		if (fabs(A[i][startFrom]) >= max)
		{
			max = fabs(A[i][startFrom]);
			maxI = i;
		}
	}
	for (size_t i = 0; i < n; i++)
	{
		float t = A[startFrom][i];
		A[startFrom][i] = A[maxI][i];
		A[maxI][i] = t;
	}
	float t = B[startFrom];
	B[startFrom] = B[maxI];
	B[maxI] = t;
}

int main(int argc, char* argv[])
{
	// Считываем всю необходимую информацию

	if (argc != 3)
	{
		printf("Wrong amount of arguments");
		return ERROR_INVALID_DATA;
	}
	FILE* in = fopen(argv[1], "r");
	if (in == NULL)
	{
		printf("Couldn't find input file");
		return ERROR_FILE_NOT_FOUND;
	}
	FILE* out = fopen(argv[2], "w");
	if (out == NULL)
	{
		fclose(in);
		printf("Couldn't find/create output file");
		return ERROR_FILE_NOT_FOUND;
	}
	size_t n;
	fscanf(in, "%i", &n);
	float** A = malloc(n * sizeof(float*));
	if (A == NULL)
	{
		fclose(in);
		fclose(out);
		printf("Couldn't allocate memory");
		return ERROR_NOT_ENOUGH_MEMORY;
	}
	for (size_t i = 0; i < n; i++)
	{
		A[i] = malloc(n * sizeof(float));
		if (A[i] == NULL)
		{
			fclose(in);
			fclose(out);
			for (size_t j = 0; j < i; j++)
			{
				free(A[j]);
			}
			free(A);
			printf("Couldn't allocate memory");
			return ERROR_NOT_ENOUGH_MEMORY;
		}
	}
	float* B = malloc(n * sizeof(float));
	if (B == NULL)
	{
		fclose(in);
		fclose(out);
		for (size_t i = 0; i < n; i++)
		{
			free(A[i]);
		}
		free(A);
		printf("Couldn't allocate memory");
		return ERROR_NOT_ENOUGH_MEMORY;
	}
	for (size_t i = 0; i < n; i++)
	{
		for (size_t j = 0; j <= n; j++)
		{
			float number;
			fscanf(in, "%g", &number);
			if (j < n)
			{
				A[i][j] = number;
			}
			else
			{
				B[i] = number;
			}
		}
	}

	//Приводим к трапециевидной форме

	for (size_t i = 0; i < n; i++)
	{
		changeToMax(i, A, n, B);
		float element = A[i][i];
		size_t shift = 0;
		while (i + shift + 1 < n && element == 0)
		{
			shift++;
			element = A[i][i + shift];
		}
		if (element != 0)
		{
			for (size_t j = i + 1; j < n; j++)
			{
				float coef = -A[j][i + shift] / element;
				for (size_t k = i; k < n; k++)
				{
					A[j][k] += coef * A[i][k];
					if (fabs(A[j][k]) < 1e-5)
					{
						A[j][k] = 0;
					}
				}
				B[j] += coef * B[i];
				if (fabs(B[j]) < 1e-5)
				{
					B[j] = 0;
				}
			}
		}
	}

	//Ищем решение

	int ans = 0;

	//-1 - нет решений
	// 0 - 1 решение
	// 1 - много решений

	for (size_t i = 0; i < n; i++)
	{
		_Bool isZero = 1;
		for (size_t j = 0; j < n; j++)
		{
			if (A[i][j] != 0)
			{
				isZero = 0;
			}
		}
		if (isZero == 1)
		{
			ans = 1;
			if (B[i] != 0)
			{
				ans = -1;
				break;
			}
		}
	}
	if (ans == 1)
	{
		fprintf(out, "many solutions");
	}
	else if (ans == -1)
	{
		fprintf(out, "no solution");
	}
	else
	{
		for (size_t i = n; i--;)
		{
			B[i] /= A[i][i];
			A[i][i] = 1;
			for (size_t j = i; j--;)
			{
				float coef = -A[j][i];
				B[j] += B[i] * coef;
				A[j][i] = 0;
			}
		}
		for (size_t i = 0; i < n; i++)
		{
			if (fabs(B[i]) < 1e-5)
			{
				B[i] = 0;
			}
			fprintf(out, "%g\n", B[i]);
		}
	}

	// Все очищаем, закрываем

	fclose(in);
	fclose(out);
	for (size_t i = 0; i < n; i++)
	{
		free(A[i]);
	}
	free(A);
	free(B);
	return ERROR_SUCCESS;
}
