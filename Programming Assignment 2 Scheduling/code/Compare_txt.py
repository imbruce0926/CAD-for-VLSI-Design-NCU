with open("output2.txt") as file1, open("B.txt") as file2:
    diff = set(file1).difference(file2)

if diff:
    with open("output.txt", "w") as output:
        output.write("".join(diff))
        print("Differences written to output.txt")
else:
    print("No differences found")