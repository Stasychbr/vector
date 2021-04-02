#include <iostream>
#include <limits>
#include "../include/IVector.h"

using namespace std;

int main() {
    double data1[] = { 1, 2, 3 };
    double data2[] = { -1, -2, -3 };
    IVector* vec1 = IVector::createVector(3, data1);
    IVector* vec2 = IVector::createVector(3, data2);
    IVector* bufVec;
    ILogger* logger = ILogger::createLogger();
    function<void(double)> print = [](double num) { std::cout << num << ' '; };
    IVector::setLogger(logger);

    cout << "vec1: ";
    vec1->foreach(print);
    cout << "\nvec2: ";
    vec2->foreach(print);

    cout << endl;

    bufVec = IVector::sub(vec1, vec2);
    cout << "\nvec1 - vec2 = ";
    bufVec->foreach(print);
    delete bufVec;

    cout << "\n(vec1,vec2) = " << IVector::dot(vec1, vec2) << endl;

    cout << "\n||vec1||_1 = " << vec1->norm(IVector::NORM::FIRST) << endl;
    cout << "||vec2||_inf = " << vec2->norm(IVector::NORM::CHEBYSHEV) << endl;

    cout << endl;
    cout << "Setting coord 1 in vec2 to 16" << endl;
    vec2->setCord(1, 16);
    cout << "Setting coord 5 in vec1 to 0" << endl;
    vec1->setCord(5, 0);

    cout << "vec1: ";
    vec1->foreach(print);
    cout << "\nvec2: ";
    vec2->foreach(print);

    cout << endl;
    cout << "\nvec1 memory allocated: " << vec1->sizeAllocated() << " bytes" << endl;
    cout << "Copying vec2 to vec1" << endl;
    IVector::copyInstance(vec1, vec2);
    cout << "vec1: ";
    vec1->foreach(print);
    cout << "\nvec2: ";
    vec2->foreach(print);

    cout << endl;
    delete vec1;
    delete vec2;

    data1[0] = numeric_limits<double>::infinity();
    cout << "Trying to construct a vector using array { " << data1[0] << " , " << data1[1] << " , " << data1[2] << " }" << endl;
    vec1 = IVector::createVector(3, data1);

    cout << "\nIVector::createVector() returned " << vec1 << endl;
    cout << "\nTest is finished" << endl;

    delete logger;
    return 0;
}
