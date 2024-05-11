import sys
if len(sys.argv) < 4:
    print("ERROR: Invalid length. 3 arguments needed (input_file output_file var_name)")
    exit
try:
    in_file = open(sys.argv[1], "rb")
except:
    print("ERROR: Input file does not exist!")
    exit

out_file = open(sys.argv[2], "a+")
varname = sys.argv[3]
data = in_file.read()

out_file.write("\nchar " + varname + "[" + str(len(data)) + "]" + " = {" + hex(data[0]))
for dat in data[1:]:
    out_file.write(", ")
    out_file.write(hex(dat))
out_file.write("};");