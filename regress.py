import numpy as np
from sklearn.linear_model import LinearRegression

import csv

num_packets = []
data_size = []
t_CPU2Device = []
t_Device = []
t_Device2CPU = []
with open('profile_multiple.csv') as csv_file:
    csv_reader = csv.reader(csv_file, delimiter=',')
    line_count = 0
    for row in csv_reader:
        if line_count == 0:
            line_count = line_count + 1
        else:
            # print(f'\t{row[0]} works in the {row[1]} department, and was born in {row[2]}.')
            num_packets.append(int(row[0]))
            data_size.append(int(row[1]))
            t_CPU2Device.append(int(row[2]))
            t_Device.append(int(row[3]))
            t_Device2CPU.append(int(row[4]))
            line_count = line_count + 1

# Linear regression
# print(num_packets)
arr1 = np.asarray(num_packets)
arr2 = np.asarray(data_size)
print(arr1.reshape((-1,1)))
x = np.concatenate((arr1.reshape((-1, 1)), arr2.reshape((-1, 1))), axis = 1)
print(x)
y = np.asarray(t_CPU2Device)
model = LinearRegression().fit(x, y)
r_sq = model.score(x, y)
print("Result for CPU to Device data transfer")
print(f"coefficient of determination: {r_sq}")
print(f"intercept: {model.intercept_}")
print(f"coefficients: {model.coef_}")

y = np.asarray(t_Device2CPU)
model = LinearRegression().fit(x, y)
r_sq = model.score(x, y)
print("Result for Device to CPU data transfer")
print(f"coefficient of determination: {r_sq}")
print(f"intercept: {model.intercept_}")
print(f"coefficients: {model.coef_}")


y = np.asarray(t_Device)
model = LinearRegression().fit(x, y)
r_sq = model.score(x, y)
print("Result for Device execution")
print(f"coefficient of determination: {r_sq}")
print(f"intercept: {model.intercept_}")
print(f"coefficients: {model.coef_}")
