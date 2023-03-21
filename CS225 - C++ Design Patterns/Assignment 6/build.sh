g++ -Wall -Werror -Wextra -Wconversion -pedantic -std=c++11 -o seq11 main.cpp -DN=0 -DCPP11
g++ -Wall -Werror -Wextra -Wconversion -pedantic -std=c++17 -o seq17 main.cpp -DN=0

echo "#### N=0 ####" > actual-output.txt
./seq11 >> actual-output.txt
./seq17 >> actual-output.txt

g++ -Wall -Werror -Wextra -Wconversion -pedantic -std=c++11 -o seq11 main.cpp -DN=6 -DCPP11
g++ -Wall -Werror -Wextra -Wconversion -pedantic -std=c++17 -o seq17 main.cpp -DN=6

echo "#### N=6 ####" >> actual-output.txt
./seq11 >> actual-output.txt
./seq17 >> actual-output.txt

g++ -Wall -Werror -Wextra -Wconversion -pedantic -std=c++11 -o seq11 main.cpp -DN=63 -DCPP11
g++ -Wall -Werror -Wextra -Wconversion -pedantic -std=c++17 -o seq17 main.cpp -DN=63

echo "#### N=63 ####" >> actual-output.txt
./seq11 >> actual-output.txt
./seq17 >> actual-output.txt