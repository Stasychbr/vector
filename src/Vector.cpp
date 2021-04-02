#include <cstdio>
#include <string.h>
#include <cmath>
#include <stdint.h>
#include <limits>
#include "Vector.h"

using namespace std;

ILogger* Vector::_logger = nullptr;

Vector* Vector::createVector(size_t dim, double const* const& pData) {
    size_t size = sizeof(Vector) + dim * sizeof(double);
    void* mem = malloc(size);
    if (!mem) {
        return nullptr;
    }
    Vector* vector = new (mem) Vector(dim);
    double* vecData = (double*)vector->getData();
    for (size_t i = 0; i < dim; i++) {
#ifndef FAST_MATH
        if (isnan(pData[i]) || isinf(pData[i])) {
            free(mem);
            return nullptr;
        }
#endif
        vecData[i] = pData[i];
    }
    return vector;
}

inline double* Vector::getDataArray() {
    return (double*)((uint8_t*)this + sizeof(Vector));
}

IVector* Vector::clone() const {
    return Vector::createVector(_dim, getData());
}

double const* Vector::getData() const {
    return (double const*)((uint8_t*)this + sizeof(Vector));
}

RC Vector::setLogger(ILogger* const logger) {
    Vector::_logger = logger;
    return RC::SUCCESS;
}

RC Vector::getCord(size_t index, double& val) const {
#ifndef FAST_MATH
    if (index >= _dim) {
        return RC::INDEX_OUT_OF_BOUND;
    }
#endif
    val = *(getData() + index);
    return RC::SUCCESS;
}

RC Vector::setCord(size_t index, double val) {
#ifndef FAST_MATH
    if (index >= _dim) {
        if (_logger) {
            _logger->warning(RC::INDEX_OUT_OF_BOUND);
        }
        return RC::INDEX_OUT_OF_BOUND;
    }
    if (isnan(val) || isinf(val)) {
        if (_logger) {
            _logger->warning(RC::INVALID_ARGUMENT);
        }
        return RC::INVALID_ARGUMENT;
    }
#endif
    *(getDataArray() + index) = val;
    return RC::SUCCESS;
}

RC Vector::scale(double multiplier) {
#ifndef FAST_MATH
    double buf;
    if (isnan(multiplier) || isinf(multiplier)) {
        if (_logger) {
            _logger->warning(RC::INVALID_ARGUMENT);
        }
        return RC::INVALID_ARGUMENT;
    }
#endif
    double* data = (double*)getData();
    
    for (size_t i = 0; i < _dim; i++) {
#ifdef FAST_MATH
        data[i] *= multiplier;
#else
        buf = data[i] * multiplier;
        if (isinf(buf)) {
            return RC::INVALID_ARGUMENT;
        }
        data[i] = buf;
#endif
    }
    return RC::SUCCESS;
}

size_t Vector::getDim() const {
    return _dim;
}

RC Vector::inc(IVector const* const& op) {
#ifndef FAST_MATH
    if (_dim != op->getDim()) {
        if (_logger) {
            _logger->warning(RC::MISMATCHING_DIMENSIONS);
        }
        return RC::MISMATCHING_DIMENSIONS;
    }
#endif
    double* data = (double*)getData();
    const double* opData = op->getData();
    for (size_t i = 0; i < _dim; i++) {
        data[i] += opData[i];
    }
    return RC::SUCCESS;
}

RC Vector::dec(IVector const* const& op) {
#ifndef FAST_MATH
    if (_dim != op->getDim()) {
        if (_logger) {
            _logger->warning(RC::MISMATCHING_DIMENSIONS);
        }
        return RC::MISMATCHING_DIMENSIONS;
    }
#endif
    double* data = getDataArray();
    const double* opData = op->getData();
    for (size_t i = 0; i < _dim; i++) {
        data[i] -= opData[i];
    }
    return RC::SUCCESS;
}

double Vector::norm(NORM n) const {
    double res = 0;
    const double* data = getData();
    switch (n) {
    case NORM::FIRST:
        for (size_t i = 0; i < _dim; i++) {
            res += fabs(data[i]);
        }
        break;
    case NORM::SECOND:
        for (size_t i = 0; i < _dim; i++) {
            res += pow(data[i], 2);
        }
        res = sqrt(res);
        break;
    case NORM::CHEBYSHEV:
        res = 0;
        for (size_t i = 0; i < _dim; i++) {
            res = fmax(res, fabs(data[i]));
        }
        break;
    default:
        if (_logger) {
            _logger->warning(RC::INVALID_ARGUMENT);
        }
        return NAN;
    }
#ifndef FAST_MATH
    if (isinf(res)) {
        if (_logger) {
            _logger->warning(RC::INFINITY_OVERFLOW);
        }
        return NAN;
    }
#endif
    return res;
}

RC Vector::applyFunction(const std::function<double(double)>& fun) {
    double* data = (double*)getData();
    double funcRes;
    for (size_t i = 0; i < _dim; i++) {
        funcRes = fun(data[i]);
#ifndef FAST_MATH
        if (isnan(funcRes) || isinf(funcRes)) {
            if (_logger) {
                _logger->warning(RC::INVALID_ARGUMENT);
            }
            return RC::INVALID_ARGUMENT;
        }
#endif
        data[i] = funcRes;
    }
    return RC::SUCCESS;
}

RC Vector::foreach(const std::function<void(double)>& fun) const {
    const double* data = getData();
    for (size_t i = 0; i < _dim; i++) {
        fun(data[i]);
    }
    return RC::SUCCESS;
}

size_t Vector::sizeAllocated() const {
    return sizeof(Vector) + _dim * sizeof(double);
}

Vector::~Vector() {

}

Vector::Vector(size_t dim) {
    _dim = dim;
}

IVector* IVector::createVector(size_t dim, double const* const& ptr_data) {
    return (IVector*)Vector::createVector(dim, ptr_data);
}

RC IVector::copyInstance(IVector* const dest, IVector const* const& src) {
#ifndef FAST_MATH
    if (dest->sizeAllocated() != src->sizeAllocated()) {
        return RC::MISMATCHING_DIMENSIONS;
    }
#endif
    memcpy(dest, src, src->sizeAllocated());
    return RC::SUCCESS;
}

RC IVector::moveInstance(IVector* const dest, IVector*& src) {
    RC copyCode = copyInstance(dest, src);
    if (copyCode != RC::SUCCESS) {
        return copyCode;
    }
    delete src;
    src = nullptr;
    return RC::SUCCESS;
}

RC IVector::setLogger(ILogger* const logger) {
    return Vector::setLogger(logger);
}

IVector* IVector::add(IVector const* const& op1, IVector const* const& op2) {
#ifndef FAST_MATH
    if (op1->getDim() != op2->getDim()) {
        // we need to update the interface to use logger here 
        /* if (_logger) {
            _logger->warning(RC::MISMATCHING_DIMENSIONS);
        } */
        return nullptr;
    }
#endif
    IVector* res = createVector(op1->getDim(), op1->getData());
    if (!res) {
        return nullptr;
    }
    res->inc(op2);
    return res;
}

IVector* IVector::sub(IVector const* const& op1, IVector const* const& op2) {
#ifndef FAST_MATH
    if (op1->getDim() != op2->getDim()) {
        // we need to update the interface to use logger here 
        /* if (_logger) {
            _logger->warning(RC::MISMATCHING_DIMENSIONS);
        } */
        return nullptr;
    }
#endif
    size_t dim = op1->getDim();
    IVector* res = createVector(dim, op1->getData());
    if (!res) {
        return nullptr;
    }
    double* data = (double*)res->getData();
    const double* opData = op2->getData();
    for (size_t i = 0; i < dim; i++) {
        data[i] -= opData[i];
    }
    return res;

}

double IVector::dot(IVector const* const& op1, IVector const* const& op2) {
#ifndef FAST_MATH
    if (op1->getDim() != op2->getDim()) {
        // we need to update the interface to use logger here 
        /* if (_logger) {
            _logger->warning(RC::MISMATCHING_DIMENSIONS);
        } */
        return NAN;
    }
#endif
    size_t dim = op1->getDim();
    double res = 0;
    const double* data1 = op1->getData();
    const double* data2 = op2->getData();
    for (size_t i = 0; i < dim; i++) {
        res += data1[i] * data2[i];
    }
#ifndef FAST_MATH
    if (isinf(res)) {
        // we need to update the interface to use logger here 
        /* if (_logger) {
            _logger->warning(RC::INFINITY_OVERFLOW);
        } */
        return NAN;
    }
#endif
    return res;
}

bool IVector::equals(IVector const* const& op1, IVector const* const& op2, NORM n, double tol) {
    IVector* diff = op1->clone();
#ifndef FAST_MATH
    if (!diff) {
        // we need to update the interface to use logger here 
        /* if (_logger) {
            _logger->warning(RC::ALLOCATION_ERROR);
        } */
        return false;
    }
#endif
    diff->dec(op2);
    double diffNorm = diff->norm(n);
#ifndef FAST_MATH
    if (isnan(diffNorm)) {
        delete diff;
        // we need to update the interface to use logger here 
        /* if (_logger) {
            _logger->warning(RC::NOT_NUMBER);
        } */
        return false;
    }
#endif
    delete diff;
    return diffNorm < tol;
}

IVector::~IVector() = default;
