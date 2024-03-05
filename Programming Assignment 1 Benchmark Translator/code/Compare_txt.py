with open("108501023_PA1.cpp") as file1, open("PA1_v6.cpp") as file2:
    diff = set(file1).difference(file2)

if diff:
    with open("output.txt", "w") as output:
        output.write("".join(diff))
        print("Differences written to output.txt")
else:
    print("No differences found")