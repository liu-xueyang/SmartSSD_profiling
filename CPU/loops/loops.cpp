// #include <iostream>

using namespace std;

#define N 5


extern "C" int main()
{
  int a[N] = {1, 2, 3, 4, 5};
  int b[N] = {1, 1, 1, 1, 1};
  int c[N*2];

  for (int i = 0; i < N; i++)
    c[i] = a[i] + b[i];

  for (int i = 0; i < N; i++)
    if (a[i] > 3) c[i]--;

  return 0;
}